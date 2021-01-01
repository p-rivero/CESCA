#include "CESCA.cpu"
#include "startup.asm"

; Program for checking if the keyboard / keyboard controller work.
; Reads inputs from the keyboard and displays them on the LCD screen.

#bank program
    MOVI R0, 0
    LCD-Init
    
poll:
    JNIN poll       ; Infinite loop until a key is pressed
    
    CMP-IN 0x80     ; Check if a special key has been pressed
    JNZ .special
    
    IN R0           ; Get the ASCII input
    LCD-Reg R0      ; Display the character 
    IN-Ack          ; Acknowledge the input so the next one can be sent
    
    J poll          ; Return to polling
    
.special:
    CMP-IN 0x20     ; Check if it's the Enter key
    IN-Ack
    JZ ..backspace
    HLT             ; If it's Enter, terminate program
    
..backspace:
    LCD-Clr         ; If it's Backspace, clear the LCD
    J poll
    