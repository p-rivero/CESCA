; =====================
;   ASSEMBLER EXAMPLE
; =====================
; This example shows some important aspects of the CESCA assembler, as well as customasm in general. The program itself doesn't do
; anything useful and some parts are even unreachable. It's meant to give an intuition of how would a real program look like.

; Include ruledef
#include "CESCA.cpu"
; Include some startup code for initializing the 7-segment display and the LCD
#include "startup.asm"

; constant that can be used as a number
constant = 0x10 - (2 * 4 + 0x07)


; global variables (you can use #d8, #res and #str):
#bank data
num:        #d8 5, 0
result:     #res 1 ; reserves 1 byte
vector:     #d8 3, 2, 1, 0, -1
    
    
; program memory
#bank program

global_label:
    .local_const = 2        ; Labels and constants that start with a dot (.) are local (only visible whithin 2 global labels)
    
    NOP                     ; This is the first instruction that will be executed (after the startup code)
    MOVI R3, constant       ; Loads 1 into R3 (0x10 - (2 * 4 + 0x07) evaluates to 1). R3 is a protected register
    MOVI R2, .local_const   ; Loads 2 into R2. R2 is a protected register
    SUB R1, R2, R3          ; R1 = R2 - R3. Note that the contents of R2 and R3 are unchanged
    OUT-Reg R1              ; Outputs the contents of R1 to the decimal display
    JNE .local_label       ; JNE is a macro that gets expanded to JNZ (jump if R2 - R3 != 0, therefore R2 != R3)
    J another_label
    
.local_label:
    LD-Addr R2, num         ; Loads 5 into R2 (protected register)
    MOVI R0, vector         ; Loads the address of vector[0]
    ADDI R0, R0, 2
    LD-Reg R1, R0           ; Loads 1 into R1 (vector[2])
    CALL subroutine         ; Calls a subroutine. The arguments are in R0 and R1
    
..local_label2:     ; Symbols can have many layers. Labels with 2 dots are only visible whithin 2 labels with 1 dot.
    
    LCD-Reg R0              ; The returned value of the subroutine is in R0. Outputs the result to the LCD display
    ST-Reg R2, R0           ; Stores 5 into the address in R0. The value of R2 has been preserved by the subroutine
    HLT                     ; Program ends
    
.local_label3:
; From this point, ..local_label2 isn't available directly. It can still be accessed with "global_label.local_label.local_label2"
    JV global_label.local_label.local_label2    ; Example
    
another_label:
; From this point, .local_const and .local_label aren't available directly.
; They can still be accessed with "global_label.local_const" and "global_label.local_label"
    MOV R0, R1              ; Copy contents of R1 to R0
    ST-Addr R2, result      ; Store contents of R2 to the reserved space in data memory
    OUT-Addr vector + 4     ; Outputs -1 to the decimal display (vector[4])
    J global_label
    

subroutine:
    PUSH R2                 ; Store to the stack the contents of the protected registers it needs to use
    
    MOVI R2, 0xAB
    
    ; ... [rest of the subroutine]
    
    CALL MATH.UMult16       ; Calls a subroutine from the MATH library.
    ; As long as the stack isn't full, there is no limit in the depth of subroutine calls.
    
    ; ... [rest of the subroutine]
    
    POP R2                  ; Restore protected registers
    RET                     ; Return to the point where the subroutine was called



; Libraries are included at the end of the file:

; Include MATH library
#include "MATH.asm"

