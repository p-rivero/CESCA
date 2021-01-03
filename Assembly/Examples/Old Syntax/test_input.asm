#include "CESCA.cpu"
#include "startup.asm"

; Program for checking if the keyboard / keyboard controller work.
; Reads inputs from the keyboard and displays them on the LCD screen.

SPECIAL_KEY = 0x80
RELEASED_KEY = 0x40
ENTER_KEY = 0x20

#bank program
    MOVI R0, 0
    LCD-Init
    
poll:
    JNIN poll       ; Infinite loop until a key is pressed
    
    CMP-IN SPECIAL_KEY     ; Check if a special key has been pressed
    JNZ .special
    
    IN R0           ; Get the ASCII input
    LCD-Reg R0      ; Display the character 
    IN-Ack          ; Acknowledge the input so the next one can be sent
    
    J poll          ; Return to polling
    
.special:
    CMP-IN ENTER_KEY    ; Check if it's the Enter key
    JZ ..backspace
    CMP-IN RELEASED_KEY ; Skip if the Enter key was released
    IN-Ack
    JNZ poll
    HLT             ; If Enter was pressed, terminate program
    
..backspace:
    IN-Ack
    LCD-Clr         ; If Backspace was pressed or released, clear the LCD
    J poll
    