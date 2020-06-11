#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;


#define HLT     0x000001    // Halt clock
#define CLR     0x000002    // Clear microcode (start next instruction)
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

#define fetch_0 OutEn|OUT_1|AdrIn               // Move the PC to the MAR
#define fetch_1 PCpp|OutEn|OUT_0|IrIn|LdTmp     // Fetch opcode to IR
#define fetch_2 OutEn|OUT_0|MPg_0               // Fetch arguments

#define CtrlA IOp_2                 // Immediate move A
#define CtrlB IOp_2|IOp_0           // Immediate move B
#define CtrlAndi IOp_2|IOp_1|IOp_0  // Immediate Andi

#define StackMem MPg_1|MPg_0        // Stack memory page
#define StackOut OUT_1|OUT_0        // Output the Stack Pointer


// 4 bits flags + 4 bits opcode + 2 bits opcode extra + 3 bits timesteps
const int size = 16*16*4*8;
vector<unsigned int> content(size);

// Size of template: 16 (opcode) * 4 (opcode extra) * 8 (timesteps)
const int TEMPL_SIZE = 16*4*8;
const vector<unsigned int> TEMPLATE = {
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|LdReg|LdFlg,                      CLR,    0, 0, 0,        // 000000 - ALU OPERATIONS
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|LdReg|LdFlg,                      CLR,    0, 0, 0,        // 000001 - ALU OPERATIONS
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|LdReg|LdFlg,                      CLR,    0, 0, 0,        // 000010 - ALU OPERATIONS
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|LdReg|LdFlg,                      CLR,    0, 0, 0,        // 000011 - ALU OPERATIONS
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|IOp_0|LdFlg,        CLR,    0, 0, 0,        // 000100 - ADDI
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|IOp_0|LdFlg,        CLR,    0, 0, 0,        // 000101 - ADDI
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|IOp_0|LdFlg,        CLR,    0, 0, 0,        // 000110 - ADDI
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|IOp_0|LdFlg,        CLR,    0, 0, 0,        // 000111 - ADDI
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|LdFlg,                                    CLR,    0, 0, 0, 0,     // 001000 - CMP-OP
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2|IOp_1|LdFlg,                        CLR,    0, 0, 0, 0,     // 001001 - CMP-SUBI
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|CtrlAndi|LdFlg,                           CLR,    0, 0, 0, 0,     // 001010 - CMP-ANDI
    
    fetch_0,    fetch_1,    Inp|LdTmp|IOp_2,    0,  fetch_2|LdTmp|CtrlAndi|LdFlg,   CLR,    0, 0,           // 001011 - CMP-IN
    
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 001100 - MOVI
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 001101 - MOVI
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 001110 - MOVI
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 001111 - MOVI
    
    fetch_0,    fetch_1,    Inp|LdReg,  0,                                          CLR,    0, 0, 0,        // 010000 - IN
    fetch_0,    fetch_1,    Inp|LdReg,  0,                                          CLR,    0, 0, 0,        // 010001 - IN
    fetch_0,    fetch_1,    Inp|LdReg,  0,                                          CLR,    0, 0, 0,        // 010010 - IN
    fetch_0,    fetch_1,    Inp|LdReg,  0,                                          CLR,    0, 0, 0,        // 010011 - IN
    
    fetch_0,    fetch_1,    Ack,                                                    CLR,    0, 0, 0, 0,     // 010100 - IN-Ack
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|CtrlB|MPg_1|MemIn,                CLR,    0, 0, 0,        // 010101 - ST-Addr
    
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|CtrlB|AdrIn,  OutEn|CtrlA|MPg_1|MemIn,    CLR,    0, 0,   // 010110 - ST-Reg
    
    fetch_0,    fetch_1,    SPpp|IOp_2,     OutEn|StackOut|AdrIn,   OutEn|CtrlB|StackMem|MemIn,     CLR,    0, 0,   // 010111 - PUSH
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011000 - LD-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011001 - LD-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011010 - LD-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011011 - LD-Addr
    
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011100 - LD-Reg
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011101 - LD-Reg
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011110 - LD-Reg
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011111 - LD-Reg

    
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg|SPpp, CLR,    0, 0, 0,        // 100000 - POP
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg|SPpp, CLR,    0, 0, 0,        // 100001 - POP
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg|SPpp, CLR,    0, 0, 0,        // 100010 - POP
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg|SPpp, CLR,    0, 0, 0,        // 100011 - POP
    
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|LdReg,     OutEn|CtrlB|StackMem|MemIn,     CLR,    0,  0,          // 100100 - SWAP
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|LdReg,     OutEn|CtrlB|StackMem|MemIn,     CLR,    0,  0,          // 100101 - SWAP
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|LdReg,     OutEn|CtrlB|StackMem|MemIn,     CLR,    0,  0,          // 100110 - SWAP
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|LdReg,     OutEn|CtrlB|StackMem|MemIn,     CLR,    0,  0,          // 100111 - SWAP
    
    fetch_0,    fetch_1,    fetch_2|PcIn,                                           CLR,    0, 0, 0, 0,     // 101000 - J
    
    fetch_0,    fetch_1,    OutEn|CtrlB|PcIn,                                       CLR,    0, 0, 0, 0,     // 101001 - JR
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2|SPpp,   OutEn|StackOut|AdrIn,   OutEn|OUT_1|StackMem|MemIn, OutEn|CtrlB|PcIn,   CLR,    0,  // 101010 - CALL
    
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|OUT_0|StackMem|PcIn|SPpp, CLR,    0, 0, 0,        // 101011 - RET
    
    fetch_0,    fetch_1,    CLR,  /*   Will get changed by fetch_2|PcIn if     */   CLR,    0, 0, 0, 0,     // 101100 - JZ
    
    fetch_0,    fetch_1,    CLR,  /*   the flags are in the correct state.     */   CLR,    0, 0, 0, 0,     // 101101 - JNZ
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 101110 - JC
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 101111 - JNC
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110000 - JV
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110001 - JNV
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110010 - JN
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110011 - JP
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110100 - JSP
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110101 - JLEU
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110110 - JLT
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 110111 - JLE
    
    fetch_0,    fetch_1,    fetch_2|LcdIn,                                          CLR,    0, 0, 0, 0,     // 111000 - LCD-Com
    
    fetch_0,    fetch_1,    fetch_2|LcdIn|LcdDt,                                    CLR,    0, 0, 0, 0,     // 111001 - LCD-Imm
    
    fetch_0,    fetch_1,    OutEn|CtrlB|LcdIn|LcdDt,                                CLR,    0, 0, 0, 0,     // 111010 - LCD-Reg
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LcdIn|LcdDt,          CLR,    0, 0, 0,        // 111011 - LCD-Mem
    
    fetch_0,    fetch_1,    OutEn|CtrlB|DecIn,                                      CLR,    0, 0, 0, 0,     // 111100 - OUT-Reg
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|DecIn,                CLR,    0, 0, 0,        // 111101 - OUT-Mem
    
    fetch_0,    fetch_1,    HLT,                                                    0, 0, 0, 0, 0,          // 111110 - HLT
    
    fetch_0,    fetch_1,                                                            0, 0, 0, 0, 0, CLR      // 111111 - NOP
};


int enable_jmp(int flags, int opcode) {
    // Replace the first microinstruction (excluding fetch 0 and 1) from a given opcode with: fetch_2|PcIn
    int address = TEMPL_SIZE*flags + 8*opcode + 2;
    content[address] = fetch_2|PcIn;
}

void generate() {
    
    assert(TEMPLATE.size() == TEMPL_SIZE); // Check size
    
    // COPY TEMPLATE 16 TIMES:
    for (int i = 0; i < size; i++)
        content[i] = TEMPLATE[i % TEMPL_SIZE];
    
    
    // FLAG MODIFICATIONS: Enable jump instructions
    for (int flags = 0; flags < 16; flags++) {
        bool ZF = flags & 0b0001; // Zero flag
        bool CF = flags & 0b0010; // Carry flag
        bool VF = flags & 0b0100; // Overflow flag
        bool SF = flags & 0b1000; // Sign flag
        
        if (ZF) enable_jmp(flags, 0b101100); // JZ
        else enable_jmp(flags, 0b101101); // JNZ
        
        if (CF) enable_jmp(flags, 0b101110); // JC
        else enable_jmp(flags, 0b101111); // JNC
        
        if (VF) enable_jmp(flags, 0b110000); // JV
        else enable_jmp(flags, 0b110001); // JNV
        
        if (SF) enable_jmp(flags, 0b110010); // JN
        else enable_jmp(flags, 0b110011); // JP
        
        if (not SF and not ZF) enable_jmp(flags, 0b110100); // JSP
        
        if (ZF or CF) enable_jmp(flags, 0b110101); // JLEU
        
        if (VF xor SF) enable_jmp(flags, 0b110110); // JLT
        
        if ((VF xor SF) or ZF) enable_jmp(flags, 0b110111); // JLE
    }
}


void write_file(int filenum) {
    ofstream outputFile;
    
    cout << "Writing HEX file " << filenum << endl;
    outputFile.open ("output" + to_string(filenum) + ".hex");
    
    for (int i = 0; i < size/32; i++) {
        for (int j = 0; j < 32; j++)
            outputFile << setw(2) << setfill('0') << hex << ((content[32*i + j] >> 8*filenum)&0xFF) << ' ';
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
            outputFile << dec << ((content[32*i + j] >> 8*filenum)&0xFF);
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
