#include "CESCA.cpu"
#include "startup.asm"

; FIBONACCI SEQUENCE
#bank program

    mov R0, 0
    mov R1, 1
    
fib:
    mov OUT, R1
    add R0, R0, R1
    jc .end
    mov OUT, R0
    add R1, R0, R1
    jnc fib

.end:
    hlt ; Stops at the last 8-bit number in the sequence (233)
    