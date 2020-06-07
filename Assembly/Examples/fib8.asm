#include "CESCA.cpu"

; FIBONACCI SEQUENCE
#bank "program"
    LI R0, 0
    LI R1, 1
    
fib:
    OUT-Reg R1
    ADD R0, R0, R1
    JC .end
    OUT-Reg R0
    ADD R1, R0, R1
    JNC fib

.end:
    HLT ; Stops at the last 8-bit number in the sequence (233)
    
