#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;


// CONTROL SIGNALS
#define HLT     0x000001    // Halt clock
#define CLR     0x000002    // Clear microcode counter (start next instruction)
#define OutEn   0x000004    // Enable output
#define OUT_1   0x000008    // Output select (output to the bus)
#define OUT_0   0x000010    //     00 = ALU, 01 = Mem, 10 = PC, 11 = SP
#define IOp_2   0x000020    // Immediate operation
#define IOp_1   0x000040    //     000 = Use Funct, 001 = ADDI, 010 = SUBI, 011 = ANDI
#define IOp_0   0x000080    //     100 = A, 101 = b, 110 = Unused, 111 = Unused
#define MPg_1   0x000100    // Mem page
#define MPg_0   0x000200    //     00 = Program Op, 01 = Progr. Arg, 10 = Data, 11 = Stack
#define LdReg   0x000400    // Load Register (Rd)
#define LdTmp   0x000800    // Load temporary registers
#define LdFlg   0x001000    // Load flags
#define AdrIn   0x002000    // Memory adress register in
#define MemIn   0x004000    // Memory (RAM) in
#define IrIn    0x008000    // Instruction register in
#define PcIn    0x010000    // Program Counter in (Jump)
#define PCpp    0x020000    // Increment Program Counter
#define SPpp    0x040000    // Increment/decrement Stack Pointer
#define DecIn   0x080000    // Decimal display in (load output register)
#define LcdIn   0x100000    // LCD display in (enable LCD module)
#define LcdDt   0x200000    // LCD data
#define Inp     0x400000    // Get input (input register out)
#define Ack     0x800000    // Input acknowledge


// COMMONLY USED SIGNAL COMBINATIONS
#define fetch_op    OutEn|OUT_1|AdrIn, PCpp|OutEn|OUT_0|IrIn|LdTmp  // Move the PC to the MAR, then fetch opcode to IR
#define fetch_arg   OutEn|OUT_0|MPg_0                               // Fetch arguments

#define CtrlA       IOp_2               // Immediate move A
#define CtrlB       IOp_2|IOp_0         // Immediate move B
#define CtrlAndi    IOp_2|IOp_1|IOp_0   // Immediate Andi

#define StackMem    MPg_1|MPg_0         // Stack memory page
#define StackOut    OUT_1|OUT_0         // Output the Stack Pointer



// INSTRUCTIONS:
#define ALU_OP      fetch_op,   fetch_arg|LdTmp,  OutEn|LdReg|LdFlg|CLR,                    0, 0, 0, 0

#define ADDI        fetch_op,   fetch_arg|LdTmp|IOp_2,    OutEn|LdReg|IOp_0|LdFlg|CLR,      0, 0, 0, 0

#define CMP_OP      fetch_op,   fetch_arg|LdTmp|LdFlg|CLR,                                  0, 0, 0, 0, 0

#define CMP_SUBI    fetch_op,   fetch_arg|LdTmp|IOp_2|IOp_1|LdFlg|CLR,                      0, 0, 0, 0, 0

#define CMP_ANDI    fetch_op,   fetch_arg|LdTmp|CtrlAndi|LdFlg|CLR,                         0, 0, 0, 0, 0

#define CMP_IN      fetch_op,   Inp|LdTmp|IOp_2,    0,  fetch_arg|LdTmp|CtrlAndi|LdFlg|CLR, 0, 0, 0

#define MOVI        fetch_op,   fetch_arg|LdReg|CLR,                                        0, 0, 0, 0, 0

#define IN          fetch_op,   Inp|LdReg,  CLR,                                            0, 0, 0, 0

#define IN_Ack      fetch_op,   Ack|CLR,                                                    0, 0, 0, 0, 0

#define ST_A        fetch_op,   fetch_arg|AdrIn,  OutEn|CtrlB|MPg_1|MemIn|CLR,              0, 0, 0, 0

#define ST_R        fetch_op,   fetch_arg|LdTmp,  OutEn|CtrlB|AdrIn,  OutEn|CtrlA|MPg_1|MemIn|CLR,      0, 0, 0

#define PUSH        fetch_op,   SPpp|IOp_2,     OutEn|StackOut|AdrIn,   OutEn|CtrlB|StackMem|MemIn|CLR, 0, 0, 0

#define LD_A        fetch_op,   fetch_arg|AdrIn,  OutEn|OUT_0|MPg_1|LdReg|CLR,              0, 0, 0, 0

#define LD_R        fetch_op,   OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg|CLR,            0, 0, 0, 0

#define POP         fetch_op,   OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg|SPpp|CLR, 0, 0, 0, 0

#define SWAP        fetch_op,   OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|LdReg,     OutEn|CtrlB|StackMem|MemIn|CLR, 0, 0, 0

#define JMP         fetch_op,   fetch_arg|PcIn|CLR,                                         0, 0, 0, 0, 0

#define JR          fetch_op,   OutEn|CtrlB|PcIn|CLR,                                       0, 0, 0, 0, 0

#define CALL        fetch_op,   fetch_arg|LdTmp|IOp_2|SPpp,   OutEn|StackOut|AdrIn,   OutEn|OUT_1|StackMem|MemIn, OutEn|CtrlB|PcIn|CLR, 0, 0

#define RET         fetch_op,   OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|PcIn|SPpp|CLR, 0, 0, 0, 0

#define COND_JMP    fetch_op,   CLR,   0, 0, 0, 0, 0
                                // The CLR will get changed to fetch_arg|PcIn|CLR if the flags are in the correct state.

#define LCD_C       fetch_op,   fetch_arg|LcdIn|CLR,                                        0, 0, 0, 0, 0

#define LCD_I       fetch_op,   fetch_arg|LcdIn|LcdDt|CLR,                                  0, 0, 0, 0, 0

#define LCD_R       fetch_op,   OutEn|CtrlB|LcdIn|LcdDt|CLR,                                0, 0, 0, 0, 0

#define LCD_A       fetch_op,   fetch_arg|AdrIn,  OutEn|OUT_0|MPg_1|LcdIn|LcdDt|CLR,        0, 0, 0, 0

#define OUT_R       fetch_op,   OutEn|CtrlB|DecIn|CLR,                                      0, 0, 0, 0, 0

#define OUT_A       fetch_op,   fetch_arg|AdrIn,  OutEn|OUT_0|MPg_1|DecIn|CLR,              0, 0, 0, 0

#define HALT        fetch_op,   HLT,                                                        0, 0, 0, 0, 0

#define NOP         fetch_op,                                                               0, 0, 0, 0, 0, 0



// 4 bit flags + 4 bit opcode + 2 bit extra opcode + 3 bit timestep
const unsigned int size = 16*16*4*8;
vector<unsigned int> content(size);

// Size of template: 16 (opcode) * 4 (opcode extra) * 8 (timesteps)
const unsigned int TEMPL_SIZE = 16*4*8;
const vector<unsigned int> TEMPLATE = {
    ALU_OP, ALU_OP, ALU_OP, ALU_OP, // 0000XX - ALU OPERATIONS
    ADDI, ADDI, ADDI, ADDI,         // 0001XX - ADDI
    CMP_OP,                         // 001000 - CMP-OP
    CMP_SUBI,                       // 001001 - CMP-SUBI
    CMP_ANDI,                       // 001010 - CMP-ANDI
    CMP_IN,                         // 001011 - CMP-IN
    MOVI, MOVI, MOVI, MOVI,         // 0011XX - MOVI
    IN, IN, IN, IN,                 // 0100XX - IN
    IN_Ack,                         // 010100 - IN-Ack
    ST_A,                           // 010101 - ST-Addr
    ST_R,                           // 010110 - ST-Reg
    PUSH,                           // 010111 - PUSH
    LD_A, LD_A, LD_A, LD_A,         // 0110XX - LD-Addr
    LD_R, LD_R, LD_R, LD_R,         // 0111XX - LD-Reg
    POP, POP, POP, POP,             // 1000XX - POP
    SWAP, SWAP, SWAP, SWAP,         // 1001XX - SWAP
    JMP,                            // 101000 - J
    JR,                             // 101001 - JR
    CALL,                           // 101010 - CALL
    RET,                            // 101011 - RET
    COND_JMP,                       // 101100 - JZ
    COND_JMP,                       // 101101 - JNZ
    COND_JMP,                       // 101110 - JC
    COND_JMP,                       // 101111 - JNC
    COND_JMP,                       // 110000 - JV
    COND_JMP,                       // 110001 - JNV
    COND_JMP,                       // 110010 - JN
    COND_JMP,                       // 110011 - JP
    COND_JMP,                       // 110100 - JSP
    COND_JMP,                       // 110101 - JLEU
    COND_JMP,                       // 110110 - JLT
    COND_JMP,                       // 110111 - JLE
    LCD_C,                          // 111000 - LCD-Com
    LCD_I,                          // 111001 - LCD-Imm
    LCD_R,                          // 111010 - LCD-Reg
    LCD_A,                          // 111011 - LCD-Addr
    OUT_R,                          // 111100 - OUT-Reg
    OUT_A,                          // 111101 - OUT-Addr
    HALT,                           // 111110 - HLT
    NOP                             // 111111 - NOP
};


void enable_jmp(int flags, int opcode) {
    // Replace the first microinstruction (excluding the 2 fetch cycles) from a given opcode with: fetch_arg|PcIn|CLR
    int address = TEMPL_SIZE*flags + 8*opcode + 2;
    content[address] = fetch_arg | PcIn | CLR;
}

void generate() {
    
    assert(TEMPLATE.size() == TEMPL_SIZE); // Check size
    
    // COPY TEMPLATE 16 TIMES:
    for (int i = 0; i < size; i+=TEMPL_SIZE)
        for (int j = 0; j < TEMPL_SIZE; j++)
            content[i+j] = TEMPLATE[j];
    
    
    // FLAG MODIFICATIONS: Enable jump instructions
    for (int flags = 0; flags < 16; flags++) {
        bool ZF = flags & 0b0001; // Zero flag
        bool CF = flags & 0b0010; // Carry flag
        bool VF = flags & 0b0100; // Overflow flag
        bool SF = flags & 0b1000; // Sign flag
        
        if (ZF) enable_jmp(flags, 0b101100); // JZ
        else    enable_jmp(flags, 0b101101); // JNZ
        
        if (CF) enable_jmp(flags, 0b101110); // JC
        else    enable_jmp(flags, 0b101111); // JNC
        
        if (VF) enable_jmp(flags, 0b110000); // JV
        else    enable_jmp(flags, 0b110001); // JNV
        
        if (SF) enable_jmp(flags, 0b110010); // JN
        else    enable_jmp(flags, 0b110011); // JP
        
        if (not SF and not ZF) enable_jmp(flags, 0b110100); // JSP
        
        if (ZF or CF)  enable_jmp(flags, 0b110101); // JLEU
        
        if (VF xor SF) enable_jmp(flags, 0b110110); // JLT
        
        if ((VF xor SF) or ZF) enable_jmp(flags, 0b110111); // JLE
    }
}


void write_file(int filenum) {
    ofstream outputFile;
    
    cout << "Writing HEX file " << filenum << endl;
    outputFile.open("output" + to_string(filenum) + ".hex");
    
    for (int i = 0; i < size/32; i++) {
        for (int j = 0; j < 32; j++) {
            unsigned char output = content[32*i + j] >> (8*filenum);
            outputFile << setw(2) << setfill('0') << hex << int(output) << ' ';
        }
        outputFile << endl;
    }
    outputFile.close();
    
    
    cout << "Writing arduino file " << filenum << endl;
    outputFile.open ("eeprom_contents" + to_string(filenum) + ".h");
    
    outputFile << dec << "const PROGMEM int CONTENTS_SIZE = " << size << ";" << endl;
    outputFile << "const PROGMEM byte EEPROM_CONTENTS[] = {";
    for (int i = 0; i < size/32; i++) {
        outputFile << endl << "    ";
        for (int j = 0; j < 32; j++) {
            unsigned char output = content[32*i + j] >> (8*filenum);
            outputFile << dec << int(output);
            if (j < 31 or i < size/32 - 1) outputFile << ", ";
        }
    }
    outputFile << endl << "};" << endl;
    
    outputFile.close();
}

int main() {
    cout << "Generating file..." << endl;
    generate();
    
    for (int i = 0; i < 3; i++)
        write_file(i);
    
    cout << "File 0 contains HLT, CLR...,  File 1 contains MemPag, LdReg..., File 2 contains PcIn, PC++..." << endl;
    
    cout << "Done." << endl;
}
