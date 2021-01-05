# Old assembly syntax

The mnemonics used by the CESCA assembly language have been modified to look like those of the more complete CESC16 language. Note that **only the mnemonics have been changed:** the instructions themselves (their binary encoding and the operations they perform) remain completely unchanged.


This folder contains the same programs as Assembly/Examples, but using the legacy language (their assembled encoding is identical).

> **Warning:** This folder will not be updated. If new assembly programs are created or improved, they will be written directly using the new mnemonics, so there may be some discrepancies between the programs in this folder and the new programs (their assembled encoding will no longer be the same).


## Important aspects of the new menmonics:

**All instructions and macros are written in lowercase:**
```asm
ADD R0, R0, R0  => add R0, R0, R0
CMP R0, R3      => cmp R0, R3
```

**Some instructions have been unified into the same mnemonic.**

Instead of using different mnemonics, they use different addressing modes and fake registers (`NONE`, `IN`, `Ack`, `OUT`, `LCD`, `LCDcmd`...):
```asm
ADD R0, R0, R1  => add R0, R0, R1       ; Add R0 and R1, store result in R0
ADDI R0, R0, 3  => add R0, R0, 3        ; Add 3 to R0, store result in R0
CMP-ADD R0, R1  => add NONE, R0, R1     ; Add R0 and R1, set flags without storing result

MOV R0, R1      => mov R0, R1           ; Move contents of R1 into R0
MOVI R0, 0x30   => mov R0, 0x30         ; Move immediate value 0x30 into R0
ST-Addr R0, 10  => mov (10), R0         ; Store contents of R0 into memory address 10
ST-Reg R0, R1   => mov [R1], R0         ; Store contents of R0 into memory address (stored in R1)
LD-Addr R0, 10  => mov R0, (10)         ; Load contents from memory address 10 into R0
LD-Reg R0, R1   => mov R0, [R1]         ; Load contents from memory address (at R1) into R0
OUT-Reg R0      => mov OUT, R0          ; Output the contents of R0
LCD-Imm "A"     => mov LCD, "A"         ; Display the character "A" on the LCD
LCD-Addr 0x20   => mov LCD, (0x20)      ; Display the character stored in address 0x20 on the LCD
LCD-Com 0x12    => mov LCDcmd, 0x12     ; Send command 0x12 to the LCD
LCD-Clr         => mov LCDcmd, Clr      ; Clear the LCD
IN R0           => mov R0, IN           ; Get input into R0
IN-Ack          => mov IN, Ack          ; Send Acknowledge signal to input device
```

**Some instructions have been renamed:**
```asm
CMP-SUBI R2, 0xF3 => cmp R2, 0xF3
CMP-ANDI R2, 0x80 => mask R2, 0x80
```
