#include "CESCA.cpu"
#include "startup.asm"

; This program finds all primes that fit in 16 bits

#bank data
str_start: #d "Starting...\0"
str_found: #d "found: \0"
str_end: #d "Found all primes below 2^16\0"

temp_n: #res 2
temp_i: #res 1

#bank program

PRINT_PRIMES:
    mov R0, str_start
    call PRINT.string

    mov R2, 0 ; Counter of the current number we are testing
    mov R3, 0

.loop:
    mov R0, R2
    mov R1, R3
    call Is_Prime   ; If (R1,R0) is prime, returns a non-zero value
    test R0
    jz ..skip_print
    
    mov LCDCmd, Clr
    mov R0, str_found
    call PRINT.string
    mov R0, R2  ; Print number
    mov R1, R3
    call PRINT.uint16
..skip_print:

    inc R2
    jnc skip(2) ; If lower bits overflow, increment upper bits
    inc R3
    jc .end     ; If upper bits overflow, end execution

    jmp .loop   ; Continue looping

.end:
    ; Wait?
    mov LCDCmd, Clr
    mov R0, str_end
    call PRINT.string
    hlt



; bool Is_Prime(uint16_t n) {
;     if (n <= 1) return false;
;     if (n <= 3) return true;
;     if (n % 2 == 0 || n % 3 == 0) return false;
  
;     for (uint8_t i = 5; i * i <= n; i += 6)
;         if (n % i == 0 || n % (i + 2) == 0)
;             return false;
;     return true;
; }

; If (R1,R0) is prime, returns a non-zero value
Is_Prime:
    ; Store context
    push R2
    push R3
    ; Store n
    mov [temp_n], R0
    mov [temp_n+1], R1

.initial_tests:
    test R1
    jnz ..skip
    cmp R0, 1
    jleu .return.false  ; n <= 1
    cmp R0, 3
    jleu .return.true   ; n <= 3
..skip:
    mask R0, 0x01   ; n % 2
    jz .return.false

    mov R2, 3
    call MATH.UDiv16
    test R3         ; n % 3
    jz .return.false

    ; Begin for loop
    mov R0, 5
    mov [temp_i], R0    ; uint8_t i = 5
.loop:
    mov R2, R0  ; Copy i to R2
    mov R3, 0
    call MATH.UMult16   ; i * i

    mov R2, [temp_n+1]  ; Compare upper bits of (i*i) and n
    cmp R2, R1
    jltu .return.true   ; n < i*i: End for loop, return true
    jne ..skip
    mov R2, [temp_n]    ; Upper bits are equal, compare lower bits
    cmp R2, R0
    jltu .return.true   ; n < i*i: End for loop, return true
..skip:

    ; if (n % i == 0) return false;
    mov R0, [temp_n]    ; Load n
    mov R1, [temp_n+1]
    mov R2, [temp_i]    ; Load i
    call MATH.UDiv16
    test R3             ; n % i
    jz .return.false

    ; if (n % (i + 2) == 0) return false;
    mov R0, [temp_n]    ; Load n
    mov R1, [temp_n+1]
    add R2, R2, 2       ; R2 still contains i
    call MATH.UDiv16
    test R3             ; n % (i + 2)
    jz .return.false

    ; i += 6
    mov R0, [temp_i]
    add R0, R0, 6
    jc .return.true     ; i overflowed: End for loop, return true
    mov [temp_i], R0
    jmp .loop


; Restore context and return
.return:
..false:
    mov R0, 0
    jmp skip(1)
..true:
    mov R0, 1
    pop R3
    pop R2
    ret

#include "PRINT.asm"
