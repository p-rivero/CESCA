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
    mov R2, args
    mov R3, outputs

.loop:
    mov R0, (R2)    ; Load first argument
    inc R2
    mov R1, (R2)    ; Load second argument
    dec R2
    
    call test_cond0
    mov R1, (R3)    ; Load first output
    inc R3
    cmp R0, R1
    jne error
    
    mov R0, (R2)    ; Load first argument
    inc R2
    mov R1, (R2)    ; Load second argument
    inc R2
    
    call test_cond1
    mov R1, (R3)    ; Load second output
    inc R3
    cmp R0, R1
    jne error
    
    cmp R2, outputs - args ; Check if all tests have been performed
    jne .loop
    
    mov R0, 1      ; Tests passed! Output a 1
    mov OUT, R0
    hlt
    
    
; Called if a test fails
error:
    mov R0, 0xFF
    mov OUT, R0
    hlt


test_cond0:     
    push R2
    mov R2, 0xF0
    
    cmp R0, R1
    jnc skip(1) ; skip 1 instruction (add)
    add R2, R2, 0b00000001
    
    cmp R0, R1
    jc skip(1)
    add R2, R2, 0b00000010
    
    cmp R0, R1
    jnz skip(1)
    add R2, R2, 0b00000100
    
    cmp R0, R1
    jz skip(1)
    add R2, R2, 0b00001000
    
    not R0, R2  ; Invert results
    pop R2
    ret

test_cond1:     
    push R2
    mov R2, 0x00
    
    cmp R0, R1
    jle skip(1)
    add R2, R2, 0b00000001
    
    cmp R0, R1
    jlt skip(1)
    add R2, R2, 0b00000010
    
    cmp R0, R1
    jleu skip(1)
    add R2, R2, 0b00000100
    
    cmp R0, R1
    jsp skip(1)
    add R2, R2, 0b00001000
    
    cmp R0, R1
    jp skip(1)
    add R2, R2, 0b00010000
    
    cmp R0, R1
    jn skip(1)
    add R2, R2, 0b00100000
    
    cmp R0, R1
    jnv skip(1)
    add R2, R2, 0b01000000
    
    cmp R0, R1
    jv skip(1)
    add R2, R2, 0b10000000
    
    not R0, R2  ; Invert results
    pop R2
    ret
    
