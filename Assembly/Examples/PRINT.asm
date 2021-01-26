#ifndef _PRINTLIB
#define _PRINTLIB
; Include library only once

; Make sure MATH is included
#include "MATH.asm"

PRINT:

.CENTINEL = 0xFF

; Prints an 8 bit signed integer
; Arguments: R0 = n
.int8:
    test R0
    jp .uint8 ; If n is positive, print as unsigned
    
    ; If number is strictly negative, output '-' and print absolute value
    mov LCD, "-"
    ; Negate number
    mov R1, 0
    sub R0, R1, R0
    ; Print unsigned integer and return
    jmp .uint8

; Prints an 8 bit unsigned integer
; Arguments: R0 = n
.uint8:
    push R2
    push R3
    mov R2, .CENTINEL
    push R2
    mov R2, 10

    jmp ..divide_loop.check ; Check conditions before doing first iteration
..divide_loop:
    call MATH.UDiv8
    push R3     ; Push R3 (remainder)

...check: ; If R0 < 10, exit loop and go to end
    cmp R2, R0          ; R2 still contains 10. If (10 <= lower bits), continue loop
    jleu ..divide_loop

; Loop ended: pop and print remainders (digits)
..print_loop:
    add R0, R0, "0" ; Convert to ASCII
    mov LCD, R0
    pop R0
    cmp R0, .CENTINEL
    jne ..print_loop
    
..end:
    pop R3
    pop R2
    ret



; Prints a 16 bit signed integer
; Arguments: R0-R1 = n (R0 contains lower bits)
.int16:
    test R1
    jp .uint16 ; If upper bits are positive, print unsigned integer
    
    ; If number is strictly negative, output '-' and print absolute value
    mov LCD, "-"
    ; Negate 16 bit number
    push R2
    mov R2, 0
    sub R0, R2, R0
    subb R1, R2, R1
    pop R2

    ; Print unsigned integer and return
    jmp .uint16

; TODO: Instead of making a loop of 16-bit divisions, do just 2 16-bit divisions (by 100)
; todo  n /= 100 -> get lower 2 digits (print separately)
; todo  n /= 100 -> get mid 2 digits (print separately)
; todo  now n <= 6, can be printed directly
; todo  problem: hard to delete leading zeros

; Prints a 16 bit unsigned integer
; Arguments: R0-R1 = n (R0 contains lower bits)
.uint16:
    push R2
    push R3
    mov R2, .CENTINEL
    push R2
    mov R2, 10

    jmp ..divide_loop.check ; Check conditions before doing first iteration
..divide_loop:
    call MATH.UDiv16
    push R3     ; Push R3 (remainder)

...check: ; If (R1,R0) < 10, exit loop and go to end
    test R1
    jnz ..divide_loop    ; Upper bits are not 0, continue loop
    cmp R2, R0          ; R2 still contains 10. If (10 <= lower bits), continue loop
    jleu ..divide_loop

; Loop ended: pop and print remainders (digits)
    jmp .uint8.print_loop ; Printing the characters and returning is identical in .uint8 and .uint16



.string:
    mov R1, [R0]
    test R1
    jz ..return
    mov LCD, R1
    inc R0
    jmp .string

..return:
    ret

; End library
#endif
