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
    push R2         ; Store protected register
    mov R2, R0      ; Move n to R2
    mov R0, 0       ; R0 contains the result
    jz ..return     ; if n == 0, return 0
    
..m_loop:
    mask R2, 0x01   ; Test last bit
    jz ..endif
    add R0, R0, R1  ; If last bit is 1, add to the result
    
..endif:
    sll R1, R1      ; Shift m to the left
    srl R2, R2      ; Shift n to the right
    jnz ..m_loop    ; Keep looping until n == 0
    
..return:
    pop R2          ; Restore protected register
    ret




; 16 BIT MULTIPLY SUBROUTINE (computes n (8 bit) * m (16 bit))
; Arguments: R0 = n, R2-R3 = m (R2 contains lower bits)    Returns: R0-R1 = n*m (R0 contains lower bits)
; Logarithmic complexity. Absolute worst case: ~340 clock cycles

.UMult16:            ; The same algorithm works for signed and unsigned multiplication 
.Mult16:
    mov R1, 0       ; Set the result to 0
    push R1         ; Set the result to 0 (lower bits)
    
    test R0
    jz ..return     ; if B == 0, return 0
    
..m_loop:
    mask R0, 0x01   ; Test last bit
    jz ..endif
    swap R0         ; Load lower bits of result temporarily
    add R0, R0, R2
    addc R1, R1, R3 ; Add A to the result (16 bit add)
    swap R0         ; Restore B in R0
    
..endif:
    sll R2, R2          ; Shift lower A to the left
    sllc R3, R3         ; Shift upper A to the left with carry

    srl R0, R0          ; Shift B to the right
    jnz ..m_loop        ; Keep looping until B == 0
    
..return:
    pop R0      ; Load lower bits of result
    ret




; DIVIDE SUBROUTINE       Arguments: R0 = n, R2 = m     Returns: R0 = n/m, R3 = n%m
; Constant-ish complexity. Absolute worst case: ~350 clock cycles
; Adapted from James Sharman's video

.UDiv8:
    mov R3, 0      ; Initialize remainder
    
..noHeader:
    mov R1, 8
    push R1         ; Create counter in stack
    mov R1, 0      ; Initialize quotient

..d_loop:
    sll R0, R0      ; Shift top bit of n
    sllc R3, R3     ; Shift remainder and add shifted bit
    sll R1, R1      ; Quotient * 2
    
    cmp R3, R2
    jltu ..endif
    sub R3, R3, R2  ; Subtract denominator from remainder
    inc R1          ; Set bottom bit of quotient (we know it's zero)
    
..endif:
    swap R0
    dec R0          ; Decrement counter in stack
    swap R0
    jnz ..d_loop
    
    pop R0          ; Destroy counter in stack
    mov R0, R1
    ret




; 16 BIT DIVIDE SUBROUTINE
; Arguments: R0-R1 = n (R0 contains lower bits), R2 = m       Returns: R0-R1 = n/m (R0 contains lower bits), R3 = n%m
; Constant-ish complexity. Absolute worst case: ~750 clock cycles
; Adapted from James Sharman's video

.UDiv16:
    test R1
    jz ..div_by_8bit    ; If upper bits are 0, use 8 bit version
    
    push R0         ; Store lower numerator
    mov R0, R1      ; Use upper numerator as argument for 8 bit version
    call .UDiv8
    
    swap R0         ; Store high result and restore lower numerator at same time
    call .UDiv8.noHeader    ; Compute lower digits with remainder already set
    
    pop R1          ; Restore high result
    ret

..div_by_8bit:
    call .UDiv8
    mov R1, 0       ; Clear upper bits of result
    ret



; SIGNED DIVIDE SUBROUTINE       Arguments: R0 = n, R2 = m     Returns: R0 = n/m, R3 = n%m
; Performs an unsigned division and adjusts the sign of the result

.Div8:
    xor R3, R0, R2
    push R3         ; Store the XOR of the arguments for later
    
    ; Absolute value of R0
    test R0
    jp ..skip0
    not R0, R0      ; Negate R0
    add R0, R0, 1
..skip0:

    ; Absolute value of R2
    test R2
    jp ..skip2
    not R2, R2
    add R2, R2, 1
..skip2:
    
    call .UDiv8   ; Do unsigned division
    
    pop R2              ; Restore the XOR of the arguments
    mask R2, 0x80       ; Mask the sign bit
    jz ..return         ; If both arguments had the same sign (XOR = 0), return the positive result
    
    not R2, R2          ; Else invert the sign
    add R2, R2, 1
    
..return:
    ret

; End library
#endif
