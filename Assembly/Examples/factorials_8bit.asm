#include "CESCA.cpu"

; Computes factorials modulo 256. Tests the functionality of CALL and RET instructions.

NUM = 5     ; The number from which to compute the factorial. Result = (NUM)!

#bank "program"

    LI R0, NUM
    CALL factorial
    DEC-Reg R0      ; Output result
    HLT
    
    
; FACTORIAL SUBROUTINE      Pre: R0 = n     Post: R0 = n!
factorial: 
    PUSH R2         ; Store protected register
    MOVE R2, R0     ; Save number for later
    
    ADDI R0, R0, -1 ; Decrement number
    JP .recursion   ; if (n-1 >= 0) go to recursive call
    
    LI R0, 1    ; 0! = 1
    J .return

.recursion:
    CALL factorial   ; Compute (n - 1)! (leaves result in R0)
    
    MOVE R1, R2     ; Get n from protected register
    CALL multiply    ; n! = n * (n - 1)!
    
.return:
    POP R2          ; Restore protected register
    RET
    
    
; MULTIPLY SUBROUTINE       Pre: R0 = n, R1 = m     Post: R0 = n*m
multiply:
	PUSH R2         ; Store protected register
	MOVE R2, R0     ; Move n to R2
	LI R0, 0        ; R0 contains the result
	JZ .return		; if n == 0, return 0
	
.m_loop:
	CMP-ANDI R2, 0x01	; Test last bit
	JZ .endif
	ADD R0, R0, R1		; If last bit is 1, add to the result
	
.endif:
	SLL R1, R1		; Shift m to the left
	SRL R2, R2		; Shift n to the right
	JNZ .m_loop		; Keep looping until n == 0
	
.return:
	POP R2          ; Restore protected register
	RET
	