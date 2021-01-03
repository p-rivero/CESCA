#ifndef _MATHLIB
#define _MATHLIB
; Include library only once

; ====================
;     MATH LIBRARY
; ====================
; This file contains a collection of useful math subroutines. Other files can include this library
; in order to call its functions, by simply adding the following line (at the end of the file!)
; #include "MATH.asm"
; However, the entire assembled library takes about 70 instructions so on large programs you will 
; need to copy/paste just the subroutines you are going to use in order to save space.


#bank program

MATH:

; MULTIPLY SUBROUTINE       Arguments: R0 = n, R1 = m     Returns: R0 = n*m
; Logarithmic complexity. Absolute worst case: ~200 clock cycles

.UMult:             ; The same algorithm works for signed and unsigned multiplication 
.Mult:
    PUSH R2         ; Store protected register
    MOV R2, R0      ; Move n to R2
    MOVI R0, 0      ; R0 contains the result
    JZ ..return      ; if n == 0, return 0
    
..m_loop:
    CMP-ANDI R2, 0x01   ; Test last bit
    JZ ..endif
    ADD R0, R0, R1      ; If last bit is 1, add to the result
    
..endif:
    SLL R1, R1      ; Shift m to the left
    SRL R2, R2      ; Shift n to the right
    JNZ ..m_loop     ; Keep looping until n == 0
    
..return:
    POP R2          ; Restore protected register
    RET




; 16 BIT MULTIPLY SUBROUTINE (computes n (8 bit) * m (16 bit))
; Arguments: R0 = n, R2-R3 = m (R2 contains lower bits)    Returns: R0-R1 = n*m (R0 contains lower bits)
; Logarithmic complexity. Absolute worst case: ~340 clock cycles

.UMult16:            ; The same algorithm works for signed and unsigned multiplication 
.Mult16:
    MOVI R1, 0      ; Set the result to 0
    PUSH R1         ; Set the result to 0 (lower bits)
    
    TEST R0
    JZ ..return      ; if B == 0, return 0
    
..m_loop:
    CMP-ANDI R0, 0x01       ; Test last bit
    JZ ..endif
    SWAP R0                 ; Load lower bits of result temporarily
    ADD R0, R0, R2
    ADDC R1, R1, R3         ; Add A to the result (16 bit add)
    SWAP R0                 ; Restore B in R0
    
..endif:
    SLL R2, R2          ; Shift lower A to the left
    SLLC R3, R3         ; Shift upper A to the left with carry

    SRL R0, R0          ; Shift B to the right
    JNZ ..m_loop         ; Keep looping until B == 0
    
..return:
    POP R0      ; Load lower bits of result
    RET




; DIVIDE SUBROUTINE       Arguments: R0 = n, R2 = m     Returns: R0 = n/m, R3 = n%m
; Constant-ish complexity. Absolute worst case: ~350 clock cycles
; Adapted from James Sharman's video

.UDiv8:
    MOVI R3, 0      ; Initialize remainder
    
..noHeader:
    MOVI R1, 8
    PUSH R1         ; Create counter in stack
    MOVI R1, 0      ; Initialize quotient

..d_loop:
    SLL R0, R0      ; Shift top bit of n
    SLLC R3, R3     ; Shift remainder and add shifted bit
    SLL R1, R1      ; Quotient * 2
    
    CMP R3, R2
    JLTU ..endif
    SUB R3, R3, R2  ; Subtract denominator from remainder
    INC R1          ; Set bottom bit of quotient (we know it's zero)
    
..endif:
    SWAP R0
    DEC R0          ; Decrement counter in stack
    SWAP R0
    JNZ ..d_loop
    
    POP R0          ; Destroy counter in stack
    MOV R0, R1
    RET




; 16 BIT DIVIDE SUBROUTINE (computes n (8 bit) * m (16 bit))
; Arguments: R0-R1 = n (R0 contains lower bits), R2 = m       Returns: R0-R1 = n/m (R0 contains lower bits), R3 = n%m
; Constant-ish complexity. Absolute worst case: ~750 clock cycles
; Adapted from James Sharman's video

.UDiv16:
    TEST R1
    JZ .UDiv8       ; If upper bits are 0, use 8 bit version
    
    PUSH R0         ; Store lower numerator
    MOV R0, R1      ; Use upper numerator as argument for 8 bit version
    CALL .UDiv8
    
    SWAP R0         ; Store high result and restore lower numerator at same time
    CALL .UDiv8.noHeader    ; Compute lower digits with remainder already set
    
    POP R1          ; Restore high result
    RET




; SIGNED DIVIDE SUBROUTINE       Arguments: R0 = n, R2 = m     Returns: R0 = n/m, R3 = n%m
; Performs an unsigned division and adjusts the sign of the result

.Div8:
    XOR R3, R0, R2
    PUSH R3         ; Store the XOR of the arguments for later
    
    ; Absolute value of R0
    TEST R0
    JP ..skip0
    NOT R0, R0
    ADDI R0, R0, 1
..skip0:

    ; Absolute value of R2
    TEST R2
    JP ..skip2
    NOT R2, R2
    ADDI R2, R2, 1
..skip2:
    
    CALL .UDiv8   ; Do unsigned division
    
    POP R2              ; Restore the XOR of the arguments
    CMP-ANDI R2, 0x80   ; Mask the sign bit
    JZ ..return         ; If both arguments had the same sign (XOR = 0), return the positive result
    
    NOT R2, R2          ; Else invert the sign
    ADDI R2, R2, 1
    
..return:
    RET

; End library
#endif
