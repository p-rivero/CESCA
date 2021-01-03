# Old assembly syntax

The mnemonics used by the CESCA assembly language have been modified to look like those of the more complete CESC16 language. Note that **only the mnemonics have been changed:** the instructions themselves (their binary encoding and the operations they perform) remain completely unchanged.


This folder contains the same programs as Assembly/Examples, but using the legacy language (their assembled encoding is identical).

> **Warning:** This folder will not be updated. If new assembly programs are created or improved, they will be written directly using the new mnemonics, so there may be some discrepancies between the programs in this folder and the new programs (their assembled encoding will no longer be the same).


## Important aspects of the new menmonics:

**Registers have been renamed:**
```asm
R0 => t0
R1 => t1
R2 => s0
R3 => s1
```

**All instructions and macros are written in lowercase:**
```asm
ADD R0, R0, R0  => add t0, t0, t0
CMP R0, R3      => cmp t0, s1
```

**Some instructions have been unified into the same mnemonic.**

Instead of using different mnemonics, they use different addressing modes and fake registers (`NONE`, `IN`, `Ack`, `OUT`, `LCD`, `LCDcmd`...):
```asm
ADD R0, R0, R1  => add t0, t0, t1       ; Add t0 and t1, store result in t0
ADDI R0, R0, 3  => add t0, t0, 3        ; Add 3 to t0, store result in t0
CMP-ADD R0, R1  => add NONE, t0, t1     ; Add t0 and t1, set flags without storing result

MOV R0, R1      => mov t0, t1           ; Move contents of t1 into t0
MOVI R0, 0x30   => mov t0, 0x30         ; Move immediate value 0x30 into t0
ST-Addr R0, 10  => mov (10), t0         ; Store contents of t0 into memory address 10
ST-Reg R0, R1   => mov (t1), t0         ; Store contents of t0 into memory address (stored in t1)
LD-Addr R0, 10  => mov t0, (10)         ; Load contents from memory address 10 into t0
LD-Reg R0, R1   => mov t0, (t1)         ; Load contents from memory address (at t1) into t0
OUT-Reg R0      => mov OUT, t0          ; Output the contents of t0
LCD-Imm "A"     => mov LCD, "A"         ; Display the character "A" on the LCD
LCD-Addr 0x20   => mov LCD, (0x20)      ; Display the character stored in address 0x20 on the LCD
LCD-Com 0x12    => mov LCDcmd, 0x12     ; Send command 0x12 to the LCD
LCD-Clr         => mov LCDcmd, Clr      ; Clear the LCD
IN R0           => mov t0, IN           ; Get input into t0
IN-Ack          => mov IN, Ack          ; Send Acknowledge signal to input device
```

**Some instructions have been renamed:**
```asm
CMP-SUBI R2, 0xF3 => cmp s0, 0xF3
CMP-ANDI R2, 0x80 => mask s0, 0x80
```
