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
    .local_const = 2       ; Labels and constants that start with a dot (.) are local (only visible whithin 2 global labels)
    
    nop                    ; This is the first instruction that will be executed (after the startup code)
    mov R3, constant       ; Loads 1 into R3 (0x10 - (2 * 4 + 0x07) evaluates to 1). R3 is a protected register
    mov R2, .local_const   ; Loads 2 into R2. R2 is a protected register
    sub R1, R2, R3         ; R1 = R2 - R3. Note that the contents of R2 and R3 are unchanged
    mov OUT, R1            ; Outputs the contents of R1 to the decimal display
    jne .local_label       ; JNE is a macro that gets expanded to JNZ (jump if R2 - R3 != 0, therefore R2 != R3)
    j another_label
    
.local_label:
    mov R2, [num]          ; Loads 5 into R2 (protected register)
    mov R0, vector         ; Loads the address of vector[0]
    add R0, R0, 2
    mov R1, (R0)           ; Loads 1 into R1 (vector[2])
    call subroutine        ; Calls a subroutine. The arguments are in R0 and R1
    
..local_label2:     ; Symbols can have many layers. Labels with 2 dots are only visible whithin 2 labels with 1 dot.
    
    mov LCD, R0             ; The returned value of the subroutine is in R0. Outputs the result to the LCD display
    mov (R0), R2            ; Stores 5 into the address in R0. The value of R2 has been preserved by the subroutine
    hlt                     ; Program ends
    
.local_label3:
; From this point, ..local_label2 isn't available directly. It can still be accessed with "global_label.local_label.local_label2"
    jv global_label.local_label.local_label2    ; Example
    
another_label:
; From this point, .local_const and .local_label aren't available directly.
; They can still be accessed with "global_label.local_const" and "global_label.local_label"
    mov R0, R1              ; Copy contents of R1 to R0
    mov (result), R2        ; Store contents of R2 to the reserved space in data memory
    mov OUT, [vector+4]      ; Outputs -1 to the decimal display (vector[4])
    j global_label
    

subroutine:
    push R2                 ; Store to the stack the contents of the protected registers it needs to use
    
    mov R2, 0xAB
    
    ; ... [rest of the subroutine]
    
    call MATH.UMult16       ; Calls a subroutine from the MATH library.
    ; As long as the stack isn't full, there is no limit in the depth of subroutine calls.
    
    ; ... [rest of the subroutine]
    
    pop R2                  ; Restore protected registers
    ret                     ; Return to the point where the subroutine was called



; Libraries are included at the end of the file:

; Include MATH library
#include "MATH.asm"

