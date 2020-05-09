; ======================
;   SUBROUTINE LIBRARY
; ======================
; This file contains a collection of useful or interesting subroutines.

#include "CESCA.cpu"


#bank "data"
mult_A:		#res 2		; First operand for mult16
mult_B:		#res 2		; Second operand for mult16
	

#bank "program"

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
	


; 16 BIT MULTIPLY SUBROUTINE		Pre: mult_A contains A, mult_B contains B		Post: R0/R1 = A*B
; All numbers are stored in little-endian format (R0,R2 = lower bits and R1,R3 = upper bits)
mult16:
	PUSH R2
	PUSH R3
	LI R0, 0			; R0/R1 contains the sum
	LI R1, 0
	LD-Addr R2, mult_B		; Load B operand to R2/R3
	LD-Addr R3, mult_B + 1
	
	J .loop_cond
	
.loop:
	PUSH R2
	PUSH R3
	
	LD-Addr R2, mult_A	; Load A operand to R2/R3 temporarily
	LD-Addr R3, mult_A + 1
	
	ADD R0, R0, R2		; Add A to result
	ADDC R1, R1, R3
	
	POP R3
	POP R2
	
	ADDI R2, R2, -1		; Decrement B
	JC .loop_cond		; Subtraction using ADDI: the carry flag is inverted!!
	ADDI R3, R3, -1
.loop_cond:
	CMP-OR R2, R3		; Test if R2/R3 (B) is 0
	JNZ .loop			; Repeat while B != 0

	POP R2
	POP R3
	RET
	


; FAST SRL (>= 5 positions) 
; When an SRL has to be performed on a register 5 or more times, this method is (very slightly) more efficient.
; Note that the number of shifted positions (p) is hardcoded.

	n = 8 - p
	ROL R0, R0 ; Repeat this line n times
	; ...
	
	ANDI R0, R0, (2**n - 1)	; 2**n is the nth power of 2
	
; Example (as a subroutine)		Pre: R0 = N		Post: R0 = N >> 6
FastSRL_6:
	ROL R0, R0
	ROL R0, R0

	ANDI R0, R0, 3
	RET
	