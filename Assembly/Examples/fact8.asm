#include "CESCA.cpu"

; Computes factorials modulo 256. Tests the functionality of CALL and RET instructions.

NUM = 5     ; The number from which to compute the factorial. Result = (NUM)!

#bank "program"

    MOVI R0, NUM
    CALL factorial
    OUT-Reg R0      ; Output result
    HLT
    
    
; FACTORIAL SUBROUTINE      Arguments: R0 = n     Returns: R0 = n!
factorial: 
    PUSH R2         ; Store protected register
    MOV R2, R0      ; Save number for later
    
    DEC R0          ; Decrement n
    JP .recursion   ; if (n-1 >= 0) go to recursive call
    
    MOVI R0, 1      ; 0! is 1
    J .return

.recursion:
    CALL factorial  ; Compute (n - 1)! (leaves result in R0)
    
    MOV R1, R2      ; Get n from protected register
    CALL mult8      ; n! = n * (n - 1)!
    
.return:
    POP R2          ; Restore protected register
    RET
    
    
#include "MATH.asm"