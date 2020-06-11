#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;


#define MOVE    0x04
#define ADD     0x4C
#define SUB     0x30
#define ADDC    0x4C    // 0x48 if carry bit is set
#define SUBB    0x30    // 0x34 if carry bit is set
#define AND     0xD8
#define OR      0xF0
#define NOT     0x80
#define XOR     0xB0
#define NAND    0xA0
#define NOR     0x88
#define XNOR    0xC8
#define SLL     0x64
#define SRL     0xFA
#define SRA     0xFE
#define ROL     0x60
#define MOVE_B  0xD0


// ImmOp (3 bits), Funct (4 bits)
const int BLOCK_SIZE = 8*16;

// Carry out (1 bit), carry flag (1 bit) 
const int TOTAL_SIZE = 2*2*BLOCK_SIZE;

const vector<unsigned char> BASE_TEMPLATE = {MOVE, ADD, SUB, ADDC, SUBB, AND, OR, NOT, XOR, NAND, NOR, XNOR, SLL, SRL, SRA, ROL};

void generate_from_template(vector<unsigned char>& content, int offset, const vector<unsigned char>& TEMPLATE) {
    
    assert(TEMPLATE.size() == 16); // Check size and offset
    assert(offset % 16 == 0);

    for (int i = offset; i < BLOCK_SIZE + offset; i++) {
        unsigned char c = TEMPLATE[i % 16];
        
        switch ((i % BLOCK_SIZE) / 16) {    // This is quite inefficient, but at this small scale it doesn't matter.
            case 0:
                content[i] = c;     // Copy template directly
                break;
            case 1:
                content[i] = TEMPLATE[1];   // ADD
                break;
            case 2:
                content[i] = 0;     // Unsed
                break;
            case 3:
                content[i] = 0;     // Unsed
                break;
            case 4:
                content[i] = TEMPLATE[0];   // MOVE
                break;
            case 5:
                content[i] = MOVE_B;  // Move second operand
                break;
            case 6:
                content[i] = TEMPLATE[2];   // SUB
                break;
            case 7:
                content[i] = TEMPLATE[5];   // AND
                break;
            default:
                assert(false);  // Unreachable case
        }
        
    }
}

void generate(vector<unsigned char>& content) {
    for (int i = 0; i < (TOTAL_SIZE / BLOCK_SIZE); i++) {
        
        vector<unsigned char> modified_template = BASE_TEMPLATE; // Copy template
        
        if (i & 0b01) {     // Carry flag is set
            modified_template[3] = 0x48;    // Modify ADDC
            modified_template[4] = 0x34;    // Modify SUBB
        }
        
        if (i & 0b10) {     // ALU generates carry out
            modified_template[2]++;     // SUB generated carry
            modified_template[4]++;     // SUBB generated carry
            modified_template[15] += 0b100; // ROL: add the carry back
            
        } else {            // ALU doesn't generate carry out
            modified_template[1]++;     // ADD generated carry
            modified_template[3]++;     // ADDC generated carry
            modified_template[12]++;    // SLL generated carry
            modified_template[15]++;    // ROL generated carry
        }
        
        generate_from_template(content, i*BLOCK_SIZE, modified_template);
    }
}

int main() {
    vector<unsigned char> content(TOTAL_SIZE);
    
    generate(content);
    
    
    ofstream outputFile;
    outputFile.open ("output.hex");
    cout << "Writing HEX file" << endl;
    for (int i = 0; i < TOTAL_SIZE/16; i++) {
        for (int j = 0; j < 16; j++)
            outputFile << setw(2) << setfill('0') << hex << int(content[16*i + j]) << ' ';
        outputFile << endl;
    }
    outputFile.close();

    outputFile.open ("eeprom_contents.h");
    cout << "Writing arduino format" << endl;
    outputFile << dec << "const PROGMEM int CONTENTS_SIZE = " << TOTAL_SIZE << ";" << endl;
    outputFile << "const PROGMEM byte EEPROM_CONTENTS[] = {";
    for (int i = 0; i < TOTAL_SIZE/32; i++) {
        outputFile << endl << "    ";
        for (int j = 0; j < 32; j++) {
            outputFile << dec << int(content[32*i + j]);
            if (j < 31 or i < TOTAL_SIZE/32 - 1) outputFile << ", ";
        }
    }
    outputFile << endl << "};" << endl;
    
    outputFile.close();
    cout << "Done." << endl;
}