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
    push R2         ; Store protected register
    mov R2, R0      ; Move n to R2
    mov R0, 0       ; R0 contains the result
    jz .return      ; if n == 0, return 0
    
.m_loop:
    add R0, R0, R1  ; sum += m
    dec R2          ; Decrement n
    jnz .m_loop     ; Keep looping until n == 0
    
.return:
    pop R2          ; Restore protected register
    ret




; NAIVE 16 BIT MULTIPLY SUBROUTINE
; Arguments: mult_A contains A, mult_B contains B       Returns: R0/R1 = A*B
; All numbers are stored in little-endian format (R0 = lower bits and R1 = upper bits)

; VERY SLOW! Absolute worst case (if B is an 8 bit integer): ~12000 cycles (24 seconds at 500 Hz).
;     If B is a 16 bit integer, it could take more than 2.5 million cycles (83 minutes at 500 Hz).
;     Also there is no reason for B to also be 16 bytes, since the result wouldn't fit.
;     This subroutine is an example of how slow a linear algorithm is compared to a logarithmic one.

mult16_naive:
    push R2
    push R3
    mov R0, 0   ; R0/R1 contains the sum
    mov R1, 0
    mov R2, [mult16_naive_B]      ; Load B operand to R2/R3
    mov R3, [mult16_naive_B + 1]
    
    j .loop_cond
    
.loop:
    push R2
    push R3
    
    mov R2, [mult16_naive_A]    ; Load A operand to R2/R3 temporarily
    mov R3, [mult16_naive_A + 1]
    
    add R0, R0, R2      ; Add A to result
    addc R1, R1, R3
    
    pop R3
    pop R2
    
    dec R2              ; The carry flag is inverted!!
    jc .loop_cond
    dec R3              ; If borrow was caused, decrement upper bits
.loop_cond:
    or NONE, R2, R3     ; Test if R2/R3 (B) is 0
    jnz .loop           ; Repeat while B != 0

    pop R3
    pop R2
    ret
    



; FAST SRL (> 5 positions) 
; When an SRL has to be performed on a register more than 5 times, this method is (very slightly) more efficient.
; Note that the number of shifted positions (p) is hardcoded.

    n = 8 - p
    rol R0, R0 ; Repeat this line n times
    ; ...
    
    mov R1, 1<<n - 1 ; 1<<n is the nth power of 2
    and R0, R0, R1
    
; Example (as a subroutine)     Arguments: R0 = N     Returns: R0 = N >> 6
FastSRL_6:
    rol R0, R0
    rol R0, R0

    mov R1, 3
    and R0, R0, R1
    ret
    
