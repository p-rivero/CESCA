; Basic startup code
#bank program
    
    ; Reset vector (PC = 0x00): no need to take extra precautions
    MOVI R0, 0
    OUT-Reg R0  ; Clear 7 segment display
    LCD-Init    ; Initialize LCD display
    LCD-Clr     ; Clear LCD display
    
