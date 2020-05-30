#include "CESCA.cpu"

; Program for checking if all the conditional jumps work. The decimal display outputs the current state:
; - Program running: 0
; - All tests passed: 1
; - A test failed: 0xFF (255)

; WARNING: THE J AND JNZ INTRUCTIONS NEED TO BE CHECKED MANUALLY, SINCE THIS TEST DEPENDS ON THEM

#bank "data"
; Arguments to be tested:
args:       #d8 0, 0,     25, 25,   -1, -1,   0, 100,   5, 127,   10, -10,  0, -128,  -1, -128,  -128, 0,  -128, 1,  2, 1,     -2, -1
; Expected outputs:
outputs:    #d8 9, 0x55,  9, 0x55,  9, 0x55,  6, 0x67,  6, 0x67,  6, 0x5C,  6, 0xA4,  5, 0x58,   5, 0x63,  5, 0x9B,  5, 0x58,  6, 0x67

#bank "program"

; Main program
    LI R2, args
    LI R3, outputs

.loop:
    LD-Reg R0, R2   ; Load first argument
    ADDI R2, R2, 1
    LD-Reg R1, R2   ; Load second argument
    ADDI R2, R2, -1
    
    CALL test_cond0
    LD-Reg R1, R3   ; Load first output
    ADDI R3, R3, 1
    CMP-SUB R0, R1
    JNZ error
    
    LD-Reg R0, R2   ; Load first argument
    ADDI R2, R2, 1
    LD-Reg R1, R2   ; Load second argument
    ADDI R2, R2, 1
    
    CALL test_cond1
    LD-Reg R1, R3   ; Load second output
    ADDI R3, R3, 1
    CMP-SUB R0, R1
    JNZ error
    
    CMP-SUBI R2, outputs - args ; Check if all tests have been performed
    JNZ .loop
    
    LI R0, 1        ; Tests passed! Output a 1
    DEC-Reg R0
    HLT
    
    
; Called if a test fails
error:
    LI R0, 0xFF
    DEC-Reg R0
    HLT


test_cond0:     
    PUSH R2
    LI R2, 0x0F
        
    CMP-SUB R0, R1
    JNC (pc + 4)
    ANDI R2, R2, 0b11111110
    
    CMP-SUB R0, R1
    JC (pc + 4)
    ANDI R2, R2, 0b11111101
    
    CMP-SUB R0, R1
    JNZ (pc + 4)
    ANDI R2, R2, 0b11111011
    
    CMP-SUB R0, R1
    JZ (pc + 4)
    ANDI R2, R2, 0b11110111
    
    MOVE R0, R2
    POP R2
    RET

test_cond1:     
    PUSH R2
    LI R2, 0xFF
    
    CMP-SUB R0, R1
    JLE (pc + 4)
    ANDI R2, R2, 0b11111110
    
    CMP-SUB R0, R1
    JLT (pc + 4)
    ANDI R2, R2, 0b11111101
    
    CMP-SUB R0, R1
    JLEU (pc + 4)
    ANDI R2, R2, 0b11111011
    
    CMP-SUB R0, R1
    JSP (pc + 4)
    ANDI R2, R2, 0b11110111
    
    CMP-SUB R0, R1
    JP (pc + 4)
    ANDI R2, R2, 0b11101111
    
    CMP-SUB R0, R1
    JN (pc + 4)
    ANDI R2, R2, 0b11011111
    
    CMP-SUB R0, R1
    JNV (pc + 4)
    ANDI R2, R2, 0b10111111
    
    CMP-SUB R0, R1
    JV (pc + 4)
    ANDI R2, R2, 0b01111111
    
    MOVE R0, R2
    POP R2
    RET
    