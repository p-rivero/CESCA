#include "CESCA.cpu"
#include "startup.asm"

; Computes factorials modulo 256. Tests the functionality of CALL and RET instructions.

NUM = 5     ; The number from which to compute the factorial. Result = (NUM)!

#bank program

    mov R0, NUM
    call factorial
    mov OUT, R0      ; Output result
    hlt
    
    
; FACTORIAL SUBROUTINE      Arguments: R0 = n     Returns: R0 = n!
factorial:
    push R2         ; Store protected register
    mov R2, R0      ; Save number for later
    
    dec R0          ; Decrement n
    jp .recursion   ; if (n-1 >= 0) go to recursive call
    
    mov R0, 1      ; 0! is 1
    j .return

.recursion:
    call factorial  ; Compute (n - 1)! (leaves result in R0)
    
    mov R1, R2      ; Get n from protected register
    call MATH.UMult ; n! = n * (n-1)!
    
.return:
    pop R2          ; Restore protected register
    ret
    
    
#include "MATH.asm"