#include "CESCA.cpu"
#include "startup.asm"

; Program for checking if the keyboard / keyboard controller work.
; Reads inputs from the keyboard and displays them on the LCD screen.

SPECIAL_KEY = 0x80
RELEASED_KEY = 0x40
ENTER_KEY = 0x20

#bank program
    mov R0, 0
    
poll:
    mask IN, 0xFF   ; Infinite loop until a key is pressed
    jz poll
    
    mask IN, SPECIAL_KEY    ; Check if a special key has been pressed
    jnz .special
    
    mov R0, IN      ; Get the ASCII input
    mov LCD, R0     ; Display the character 
    mov IN, Ack     ; Acknowledge the input so the next one can be sent
    
    jmp poll        ; Return to polling
    
.special:
    mask IN, ENTER_KEY      ; Check if it's the Enter key
    jz ..backspace
    mask IN, RELEASED_KEY   ; Skip if the Enter key was released
    mov IN, Ack
    jnz poll
    hlt             ; If Enter was pressed, terminate program
    
..backspace:
    mov IN, Ack
    mov LCDcmd, Clr    ; If Backspace was pressed or released, clear the LCD
    jmp poll
    