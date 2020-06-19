; =====================
;   ASSEMBLER EXAMPLE
; =====================
; This example shows some important aspects of the CESCA assembler, as well as customasm in general. The program itself doesn't do
; anything useful and some parts are even unreachable. It's meant to give an intuition of how would a real program look like.

#include "CESCA.cpu" ; Include cpudef

; constant that can be used as a number
constant = 0x10 - (2 * 4 + 0x07)


; global variables (you can use #d8, #res and #str):
#bank "data"
num:        #d8 5
result:     #res 1 ; reserves 1 byte
vector:     #d8 3, 2, 1, 0, -1
    
    
; program memory
#bank "program"

global_label:
    .local_const = 2        ; labels and constants that start with a dot (.) are local (only visible whithin 2 global labels)
    
    NOP                     ; This is the first instruction that will be executed (PC=0x00). It's a NOP so we're off to a bad start :(
    MOVI R3, constant       ; Loads 1 into R3 (0x10 - (2 * 4 + 0x07) evaluates to 1). R3 is a protected register
    MOVI R2, .local_const   ; Loads 2 into R2. R2 is a protected register
    SUB R1, R2, R3
    OUT-Reg R1              ; Outputs the result of the subtraction to the decimal display
    JNZ .local_label
    J another_label
    
.local_label:
    LD-Addr R2, num         ; Loads 5 into R2 (protected register)
    MOVI R0, vector         ; Loads the address of vector[0]
    ADDI R0, R0, 2
    LD-Reg R1, R0           ; Loads 1 into R1 (vector[2])
    CALL subroutine         ; Calls a subroutine. The arguments are in R0 and R1
    
    LCD-Reg R0              ; The returned value of the subroutine is in R0. Outputs the result to the LCD display
    ST-Reg R2, R0           ; Stores 5 into the address in R0. The value of R2 has been preserved by the subroutine
    HLT                     ; Program ends
    
another_label:              ; From this point, .local_const and .local_label aren't available anymore
    MOV R0, R1              ; Copies the contents of R1 to R0
    ST-Addr R2, result      ; Stores the contents of R2 to the reserved space in data memory
    OUT-Addr vector + 4     ; Outputs -1 to the decimal display (vector[4])
    J global_label
    

subroutine:
    PUSH R2                 ; Stores to the stack the contents of the protected registers it needs to use
    
    MOVI R2, 0xAB
    
    ; ... rest of the subroutine
    
    CALL mult16             ; Calls a subroutine from the MATH library.
    ; As long as the stack isn't full, there is no limit in the depth of subroutine calls.
    
    ; ... rest of the subroutine
    
    POP R2                  ; Restores protected registers
    RET                     ; Returns to the point where the subroutine was called



; Libraries are included at the end of the file:

#include "MATH.asm"  ; Include MATH library