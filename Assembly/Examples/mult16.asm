#include "CESCA.cpu"

; Multiplies 16 bit numbers and outputs the result to the LCD screen

#bank "data"
; All 16 bit numbers are stored in little-endian format
mult_A:     #res 2      ; First operand for mult16
mult_B:     #res 2      ; Second operand for mult16

#bank "program"
    
    num_A = 1000
    num_B = 30
    ; Expected result: 30000
    
    LI R0, num_A [7:0]      ; Store num_A in memory
    LI R1, num_A [15:8]
    ST-Addr R0, mult_A
    ST-Addr R1, mult_A + 1
    
    LI R0, num_B [7:0]      ; Store num_B in memory
    LI R1, num_B [15:8]
    ST-Addr R0, mult_B
    ST-Addr R1, mult_B + 1

    JAL mult16          ; Call multiplication subroutine
    
    LCD-Com 0x38
    LCD-Com 0x0E
    LCD-Com 0x06 ; LCD-Init
    JAL output16        ; Output result to LCD
    
    HLT
    
    
; 16 BIT MULTIPLY SUBROUTINE        Pre: mult_A contains A, mult_B contains B       Post: R0 = lower bits of A*B, R1 = upper bits of A*B
mult16:
    PUSH R2
    PUSH R3
    LI R0, 0            ; R0/R1 contains the sum
    LI R1, 0
    LD-Addr R2, mult_B      ; Load B operand to R2/R3
    LD-Addr R3, mult_B + 1
    
    J .loop_cond
    
.loop:
    PUSH R2
    PUSH R3
    
    LD-Addr R2, mult_A  ; Load A operand to R2/R3 temporarily
    LD-Addr R3, mult_A + 1
    
    ADD R0, R0, R2      ; Add A to result
    ADDC R1, R1, R3
    
    POP R3
    POP R2
    
    ADDI R2, R2, -1     ; Decrement B
    JC .loop_cond       ; Subtraction using ADDI: the carry flag is inverted
    ADDI R3, R3, -1
.loop_cond:
    CMP-OR R2, R3       ; Test if R2/R3 (B) is 0
    JNZ .loop           ; Repeat while B != 0

    POP R2
    POP R3
    RET
    
    
; LCD OUTPUT SUBROUTINE     Pre: R0 = lower bits of N, R1 = upper bits of N         Post: N has been sent to the LCD display
output16:
    ; To be finished
    RET
    