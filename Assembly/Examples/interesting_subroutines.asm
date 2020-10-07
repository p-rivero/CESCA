; ===========================
;   INTERESTING SUBROUTINES
; ===========================
; This file contains a collection of subroutines that are not really useful by themselves, but
; that might result interesting, amusing or thought-provoking.

#include "CESCA.cpu"


#bank data
mult16_naive_A:     #res 2      ; First 16 bit operand for mult16_naive
mult16_naive_B:     #res 2      ; Second 16 bit operand for mult16_naive
    
#bank program

; NAIVE MULTIPLY SUBROUTINE (SLOW!)       Arguments: R0 = n, R1 = m     Returns: R0 = n*m

mult8_naive: 
    PUSH R2         ; Store protected register
    MOV R2, R0      ; Move n to R2
    MOVI R0, 0      ; R0 contains the result
    JZ .return      ; if n == 0, return 0
    
.m_loop:
    ADD R0, R0, R1  ; sum += m
    DEC R2          ; Decrement n
    JNZ .m_loop     ; Keep looping until n == 0
    
.return:
    POP R2          ; Restore protected register
    RET




; NAIVE 16 BIT MULTIPLY SUBROUTINE
; Arguments: mult_A contains A, mult_B contains B       Returns: R0/R1 = A*B
; All numbers are stored in little-endian format (R0 = lower bits and R1 = upper bits)

; VERY SLOW! Absolute worst case (if B is an 8 bit integer): ~12000 cycles (24 seconds at 500 Hz).
;     If B is a 16 bit integer, it could take more than 2.5 million cycles (83 minutes at 500 Hz).
;     Also there is no reason for B to also be 16 bytes, since the result wouldn't fit.
;     This subroutine is an example of how slow a linear algorithm is compared to a logarithmic one.

mult16_naive:
    PUSH R2
    PUSH R3
    MOVI R0, 0          ; R0/R1 contains the sum
    MOVI R1, 0
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
    
    DEC R2              ; The carry flag is inverted!!
    JC .loop_cond
    DEC R3              ; If borrow was caused, decrement upper bits
.loop_cond:
    CMP-OR R2, R3       ; Test if R2/R3 (B) is 0
    JNZ .loop           ; Repeat while B != 0

    POP R3
    POP R2
    RET
    



; FAST SRL (> 5 positions) 
; When an SRL has to be performed on a register more than 5 times, this method is (very slightly) more efficient.
; Note that the number of shifted positions (p) is hardcoded.

    n = 8 - p
    ROL R0, R0 ; Repeat this line n times
    ; ...
    
    MOVI R1, (2**n - 1) ; 2**n is the nth power of 2
    AND R0, R0, R1
    
; Example (as a subroutine)     Arguments: R0 = N     Returns: R0 = N >> 6
FastSRL_6:
    ROL R0, R0
    ROL R0, R0

    MOVI R1, 3
    AND R0, R0, R1
    RET
    