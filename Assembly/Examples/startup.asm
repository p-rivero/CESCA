; Basic startup code

; Make sure ruledef has been included
#include "CESCA.cpu"

#bank program
    ; Reset vector (PC = 0x00): no need to take extra precautions
    mov R0, 0
    mov OUT, R0     ; Clear 7 segment display
    mov LCDcmd, Init   ; Initialize LCD display
    mov LCDcmd, Clr    ; Clear LCD display
    
