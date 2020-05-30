; ======================
;   SUBROUTINE LIBRARY
; ======================
; This file contains a collection of useful or interesting subroutines. The math library can be copied
; to a separate file to be included in other programs.

#include "CESCA.cpu"


#bank "data"
mult16_naive_A:     #res 2      ; First 16 bit operand for mult16_naive
mult16_naive_B:     #res 2      ; Second 16 bit operand for mult16_naive
    

#bank "program"


; ====================
;     MATH LIBRARY
; ====================

; MULTIPLY SUBROUTINE       Pre: R0 = n, R1 = m     Post: R0 = n*m
; Logarithmic complexity. Absolute worst case: ~200 clock cycles
mult8:
    PUSH R2         ; Store protected register
    MOVE R2, R0     ; Move n to R2
    LI R0, 0        ; R0 contains the result
    JZ .return      ; if n == 0, return 0
    
.m_loop:
    CMP-ANDI R2, 0x01   ; Test last bit
    JZ .endif
    ADD R0, R0, R1      ; If last bit is 1, add to the result
    
.endif:
    SLL R1, R1      ; Shift m to the left
    SRL R2, R2      ; Shift n to the right
    JNZ .m_loop     ; Keep looping until n == 0
    
.return:
    POP R2          ; Restore protected register
    RET


; ================================================================================

; 16 BIT MULTIPLY SUBROUTINE (computes n (8 bit) * m (16 bit))
; Pre: R0 = n, R2-R3 = m (R2 contains lower bits)     Post: R0-R1 = n*m (R0 contains lower bits)
; Logarithmic complexity. Absolute worst case: ~340 clock cycles

mult16:
    LI R1, 0        ; Set the result to 0
    PUSH R1         ; Set the result to 0 (lower bits)
    
    TEST R0
    JZ .return      ; if B == 0, return 0
    
.m_loop:
    CMP-ANDI R0, 0x01       ; Test last bit
    JZ .endif
    SWAP R0, R0             ; Load lower bits of result temporarily
    ADD R0, R0, R2
    ADDC R1, R1, R3         ; Add A to the result (16 bit add)
    SWAP R0, R0             ; Restore B in R0
    
.endif:
    SLL R2, R2          ; Shift lower A to the left
    SLLC R3, R3         ; Shift upper A to the left with carry

    SRL R0, R0          ; Shift B to the right
    JNZ .m_loop         ; Keep looping until B == 0
    
.return:
    POP R0      ; Load lower bits of result
    RET


; ================================================================================

; DIVIDE SUBROUTINE       Pre: R0 = n, R2 = m     Post: R0 = n/m, R3 = n%m
; Constant-ish complexity. Absolute worst case: ~350 clock cycles
; Adapted from James Sharman's video

div8:
    LI R3, 0        ; Initialize remainder
    
div8_noHeader:
    LI R1, 8
    PUSH R1         ; Create counter in stack
    LI R1, 0        ; Initialize quotient

.d_loop:
    SLL R0, R0      ; Shift top bit of n
    SLLC R3, R3     ; Shift remainder and add shifted bit
    SLL R1, R1      ; Quotient * 2
    
    CMP R3, R2
    JLTU .endif
    SUB R3, R3, R2  ; Subtract denominator from remainder
    INCR R1         ; Set bottom bit of quotient (we know it's zero)
    
.endif:
    SWAP R0, R0
    DECR R0         ; Decrement counter in stack
    SWAP R0, R0
    JNZ .d_loop
    
    POP R0          ; Destroy counter in stack
    MOVE R0, R1
    RET


; ================================================================================

; 16 BIT DIVIDE SUBROUTINE (computes n (8 bit) * m (16 bit))
; Pre: R0-R1 = n (R0 contains lower bits), R2 = m       Post: R0-R1 = n/m (R0 contains lower bits), R3 = n%m
; Constant-ish complexity. Absolute worst case: ~750 clock cycles
; Adapted from James Sharman's video

div16:
    TEST R1
    JZ div8         ; If upper bits are 0, use 8 bit version
    
    PUSH R0         ; Store lower numerator
    MOVE R0, R1     ; Use upper numerator as argument for 8 bit version
    CALL div8
    
    SWAP R0         ; Store high result and restore lower numerator at same time
    CALL div8_noHeader   ; Compute lower digits with remainder already set
    
    POP R1          ; Restore high result
    RET







; ===============================
;  OTHER INTERESTING SUBTOUTINES
; ===============================

; NAIVE MULTIPLY SUBROUTINE (SLOW!)       Pre: R0 = n, R1 = m     Post: R0 = n*m

mult8_naive: 
    PUSH R2         ; Store protected register
    MOVE R2, R0     ; Move n to R2
    LI R0, 0        ; R0 contains the result
    JZ .return      ; if n == 0, return 0
    
.m_loop:
    ADD R0, R0, R1  ; sum += m
    DECR R2         ; Decrement n
    JNZ .m_loop     ; Keep looping until n == 0
    
.return:
    POP R2          ; Restore protected register
    RET


; ================================================================================

; NAIVE 16 BIT MULTIPLY SUBROUTINE
; Pre: mult_A contains A, mult_B contains B       Post: R0/R1 = A*B
; All numbers are stored in little-endian format (R0 = lower bits and R1 = upper bits)

; VERY SLOW! Absolute worst case (if B is an 8 bit integer): ~12000 cycles (24 seconds at 500 Hz).
;     If B is a 16 bit integer, it could take more than 2.5 million cycles (83 minutes at 500 Hz).
;     Also there is no reason for B to also be 16 bytes, since the result wouldn't fit.
;     This subroutine is an example of how slow a linear algorithm is compared to a logarithmic one.

mult16_naive:
    PUSH R2
    PUSH R3
    LI R0, 0            ; R0/R1 contains the sum
    LI R1, 0
    LD-Addr R2, mult16_naive_B      ; Load B operand to R2/R3
    LD-Addr R3, mult16_naive_B + 1
    
    J .loop_cond
    
.loop:
    PUSH R2
    PUSH R3
    
    LD-Addr R2, mult16_naive_A  ; Load A operand to R2/R3 temporarily
    LD-Addr R3, mult16_naive_A + 1
    
    ADD R0, R0, R2      ; Add A to result
    ADDC R1, R1, R3
    
    POP R3
    POP R2
    
    DECR R2             ; The carry flag is inverted!!
    JC .loop_cond
    DECR R3             ; If borrow was caused, decrement upper bits
.loop_cond:
    CMP-OR R2, R3       ; Test if R2/R3 (B) is 0
    JNZ .loop           ; Repeat while B != 0

    POP R3
    POP R2
    RET
    

; ================================================================================

; FAST SRL (>= 5 positions) 
; When an SRL has to be performed on a register 5 or more times, this method is (very slightly) more efficient.
; Note that the number of shifted positions (p) is hardcoded.

    n = 8 - p
    ROL R0, R0 ; Repeat this line n times
    ; ...
    
    ANDI R0, R0, (2**n - 1) ; 2**n is the nth power of 2
    
; Example (as a subroutine)     Pre: R0 = N     Post: R0 = N >> 6
FastSRL_6:
    ROL R0, R0
    ROL R0, R0

    ANDI R0, R0, 3
    RET
    