#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;


#define HLT     0x010000    // Halt clock
#define CLR     0x020000    // Clear microcode (start next instruction)
#define OutEn   0x040000    // Enable output
#define OUT_1   0x080000    // Output select (output to the bus)
#define OUT_0   0x100000    //     00 = ALU, 01 = Mem, 10 = PC, 11 = SP
#define IOp_2   0x200000    // Immediate operation
#define IOp_1   0x400000    //     000 = Use Funct, 001 = ADDI, 010 = SUBI, 011 = ANDI
#define IOp_0   0x800000    //     100 = A, 101 = b, 110 = Unused, 111 = Unused
#define MPg_1   0x000100    // Mem page
#define MPg_0   0x000200    //     00 = Program Op, 01 = Progr. Arg, 10 = Data, 11 = Stack
#define LdReg   0x000400    // Load Register (Rd)
#define LdTmp   0x000800    // Load temporary registers
#define LdFlg   0x001000    // Load flags
#define AdrIn   0x002000    // Memory adress register in
#define MemIn   0x004000    // Memory (RAM) in
#define IrIn    0x008000    // Instruction register in
#define PcIn    0x000001    // Program Counter in (Jump)
#define PCpp    0x000002    // Increment Program Counter
#define SPpp    0x000004    // Increment/decrement Stack Pointer
#define DecIn   0x000008    // Decimal display in (load output register)
#define LcdIn   0x000010    // LCD display in (enable LCD module)
#define LcdDt   0x000020    // LCD data
#define Un_0    0x000040    // Unused
#define Un_1    0x000080    // Unused

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

// Size of tamplate: 16 (opcode) * 4 (opcode extra) * 8 (timesteps)
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
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|CtrlAndi|LdFlg,     CLR,    0, 0, 0,        // 001000 - ANDI
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|CtrlAndi|LdFlg,     CLR,    0, 0, 0,        // 001001 - ANDI
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|CtrlAndi|LdFlg,     CLR,    0, 0, 0,        // 001010 - ANDI
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    OutEn|LdReg|CtrlAndi|LdFlg,     CLR,    0, 0, 0,        // 001011 - ANDI
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|LdFlg,                                    CLR,    0, 0, 0, 0,     // 001100 - CMP-OP
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2|IOp_1|LdFlg,                        CLR,    0, 0, 0, 0,     // 001101 - CMP-SUBI
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|CtrlAndi|LdFlg,                           CLR,    0, 0, 0, 0,     // 001110 - CMP-ANDI
    
    fetch_0,    fetch_1,    fetch_2|LdTmp|IOp_2,    fetch_2|LdTmp|CtrlA|LdFlg,      CLR,    0, 0, 0,        // 001111 - CLF
    
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 010000 - LI
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 010001 - LI
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 010010 - LI
    fetch_0,    fetch_1,    fetch_2|LdReg,                                          CLR,    0, 0, 0, 0,     // 010011 - LI
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|CtrlB|MPg_1|MemIn,                CLR,    0, 0, 0,        // 010100 - ST-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|CtrlB|MPg_1|MemIn,                CLR,    0, 0, 0,        // 010101 - ST-Addr
    
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|CtrlB|AdrIn,  OutEn|CtrlA|MPg_1|MemIn,    CLR,    0, 0,   // 010110 - ST-Reg
    fetch_0,    fetch_1,    fetch_2|LdTmp,  OutEn|CtrlB|AdrIn,  OutEn|CtrlA|MPg_1|MemIn,    CLR,    0, 0,   // 010111 - ST-Reg
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011000 - LD-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011001 - LD-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011010 - LD-Addr
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,                CLR,    0, 0, 0,        // 011011 - LD-Addr
    
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011100 - LD-Reg
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011101 - LD-Reg
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011110 - LD-Reg
    fetch_0,    fetch_1,    OutEn|CtrlB|AdrIn,  OutEn|OUT_0|MPg_1|LdReg,            CLR,    0, 0, 0,        // 011111 - LD-Reg
    
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|SPpp|CtrlB|StackMem|MemIn,    CLR,    0, 0, 0,    // 100000 - PUSH
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|SPpp|CtrlB|StackMem|MemIn,    CLR,    0, 0, 0,    // 100001 - PUSH
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|SPpp|CtrlB|StackMem|MemIn,    CLR,    0, 0, 0,    // 100010 - PUSH
    fetch_0,    fetch_1,    OutEn|StackOut|AdrIn,   OutEn|SPpp|CtrlB|StackMem|MemIn,    CLR,    0, 0, 0,    // 100011 - PUSH
    
    fetch_0,    fetch_1,    SPpp,   OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg,  CLR,    0, 0,       // 100100 - POP
    fetch_0,    fetch_1,    SPpp,   OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg,  CLR,    0, 0,       // 100101 - POP
    fetch_0,    fetch_1,    SPpp,   OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg,  CLR,    0, 0,       // 100110 - POP
    fetch_0,    fetch_1,    SPpp,   OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|LdReg,  CLR,    0, 0,       // 100111 - POP
    
    fetch_0,    fetch_1,    fetch_2|PcIn,                                           CLR,    0, 0, 0, 0,     // 101000 - J
    
    fetch_0,    fetch_1,    OutEn|CtrlB|PcIn,                                       CLR,    0, 0, 0, 0,     // 101001 - JR
    
    fetch_0,    fetch_1,    fetch_2|IOp_2|LdTmp,    OutEn|StackOut|AdrIn,
                OutEn|OUT_1|StackMem|MemIn,     OutEn|SPpp|CtrlB|PcIn,              CLR,    0,              // 101010 - JAL
                
    fetch_0,    fetch_1,    SPpp,   OutEn|StackOut|AdrIn,  OutEn|OUT_0|StackMem|PcIn,   CLR,    0, 0,       // 101011 - RET
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 101100 - JZ
    
    fetch_0,    fetch_1,    CLR,                                                    CLR,    0, 0, 0, 0,     // 101101 - JNZ
    
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
    
    fetch_0,    fetch_1,    OutEn|CtrlB|DecIn,                                      CLR,    0, 0, 0, 0,     // 111100 - DEC-Reg
    
    fetch_0,    fetch_1,    fetch_2|AdrIn,  OutEn|OUT_0|MPg_1|DecIn,                CLR,    0, 0, 0,        // 111101 - DEC-Mem
    
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
    
    outputFile << "const PROGMEM uint8_t EEPROM_CONTENTS[] = {";
    for (int i = 0; i < size/32; i++) {
        outputFile << endl << "\t";
        for (int j = 0; j < 32; j++) {
            outputFile << dec << ((content[32*i + j] >> 8*filenum)&0xFF);
            if (j < 31 or i < size/32 - 1) outputFile << ", ";
        }
    }
    outputFile << endl << "}" << endl;
    
    outputFile.close();
}

int main() {
    cout << "Generating file" << endl;
    generate();
    
    write_file(0);
    write_file(1);
    write_file(2);
    
    cout << "WARNING: File 0 contains the least significant bits of the control word. File 2 contains HLT, CLR, ..." << endl;
    
    cout << "Done." << endl;
}
