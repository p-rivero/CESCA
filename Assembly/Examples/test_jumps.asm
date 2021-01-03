#include "CESCA.cpu"
#include "startup.asm"

; Program for checking if all the conditional jumps work. The decimal display outputs the current state:
; - Program running: 0
; - All tests passed: 1
; - A test failed: 0xFF (255)

; WARNING: THE J AND JNZ INTRUCTIONS NEED TO BE CHECKED MANUALLY, SINCE THIS TEST DEPENDS ON THEM

#bank data
; Arguments to be tested:
args:       #d8 0, 0,     25, 25,   -1, -1,   0, 100,   5, 127,   10, -10,  0, -128,  -1, -128,  -128, 0,  -128, 1,  2, 1,     -2, -1
; Expected outputs (inverted, since the method I had before used inverted results, and it's easier to just add a NOT instruction to the new method):
outputs:    #d8 9, 0x55,  9, 0x55,  9, 0x55,  6, 0x67,  6, 0x67,  6, 0x5C,  6, 0xA4,  5, 0x58,   5, 0x63,  5, 0x9B,  5, 0x58,  6, 0x67

#bank program
; Main program
    MOVI R2, args
    MOVI R3, outputs

.loop:
    LD-Reg R0, R2   ; Load first argument
    INC R2
    LD-Reg R1, R2   ; Load second argument
    DEC R2
    
    CALL test_cond0
    LD-Reg R1, R3   ; Load first output
    INC R3
    CMP R0, R1
    JNE error
    
    LD-Reg R0, R2   ; Load first argument
    INC R2
    LD-Reg R1, R2   ; Load second argument
    INC R2
    
    CALL test_cond1
    LD-Reg R1, R3   ; Load second output
    INC R3
    CMP R0, R1
    JNE error
    
    CMP R2, outputs - args ; Check if all tests have been performed
    JNE .loop
    
    MOVI R0, 1      ; Tests passed! Output a 1
    OUT-Reg R0
    HLT
    
    
; Called if a test fails
error:
    MOVI R0, 0xFF
    OUT-Reg R0
    HLT


test_cond0:     
    PUSH R2
    MOVI R2, 0xF0
    
    CMP-SUB R0, R1
    ; skip 1 instruction (ADDI)
    JNC skip(1)
    ADDI R2, R2, 0b00000001
    
    CMP-SUB R0, R1
    JC skip(1)
    ADDI R2, R2, 0b00000010
    
    CMP-SUB R0, R1
    JNZ skip(1)
    ADDI R2, R2, 0b00000100
    
    CMP-SUB R0, R1
    JZ skip(1)
    ADDI R2, R2, 0b00001000
    
    NOT R0, R2  ; Invert results
    POP R2
    RET

test_cond1:     
    PUSH R2
    MOVI R2, 0x00
    
    CMP-SUB R0, R1
    JLE skip(1)
    ADDI R2, R2, 0b00000001
    
    CMP-SUB R0, R1
    JLT skip(1)
    ADDI R2, R2, 0b00000010
    
    CMP-SUB R0, R1
    JLEU skip(1)
    ADDI R2, R2, 0b00000100
    
    CMP-SUB R0, R1
    JSP skip(1)
    ADDI R2, R2, 0b00001000
    
    CMP-SUB R0, R1
    JP skip(1)
    ADDI R2, R2, 0b00010000
    
    CMP-SUB R0, R1
    JN skip(1)
    ADDI R2, R2, 0b00100000
    
    CMP-SUB R0, R1
    JNV skip(1)
    ADDI R2, R2, 0b01000000
    
    CMP-SUB R0, R1
    JV skip(1)
    ADDI R2, R2, 0b10000000
    
    NOT R0, R2  ; Invert results
    POP R2
    RET
    