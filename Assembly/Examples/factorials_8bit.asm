#include "CESCA.cpu"

; Computes factorials modulo 256. Tests the functionality of JAL and RET instructions.

NUM = 5		; The number from which to compute the factorial. Result = (NUM)!

#bank "program"

	LI R0, NUM
	JAL factorial
	DEC-Reg R0		; Output result
	HLT
	
	
; FACTORIAL SUBROUTINE		Pre: R0 = n		Post: R0 = n!
factorial: 
	PUSH R2 		; Store protected register
	MOVE R2, R0		; Save number for later
	
	ADDI R0, R0, -1	; Decrement number
	JP .recursion	; if (n-1 >= 0) go to recursive call
	
	LI R0, 1 	; 0! = 1
	J .return

.recursion:
	JAL factorial	; Compute (n - 1)! (leaves result in R0)
	
	MOVE R1, R2		; Get n from protected register
	JAL multiply 	; n! = n * (n - 1)!
	
.return:
	POP R2			; Restore protected register
	RET
	
	
; MULTIPLY SUBROUTINE		Pre: R0 = n, R1 = m		Post: R0 = n*m
multiply: 
	PUSH R2 		; Store protected register
	MOVE R2, R0 	; Move n to R2
	LI R0, 0		; R0 contains the sum
	JZ .return		; if n == 0, return 0
	
.m_loop:
	ADD R0, R0, R1	; sum += m
	ADDI R2, R2, -1	; Decrement n
	JNZ .m_loop
	
.return:
	POP R2			; Restore protected register
	RET
	