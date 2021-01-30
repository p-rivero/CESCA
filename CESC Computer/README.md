# CESC Computer

This is the first iteration of the CESC computer. It's built on breadboards, and it's heavily inspired by Ben Eater's SAP-1 computer (the clock module and decimal decoder are identical).

The clock speed can be adjusted (with a potentiometer) up to **80 kHz**, but the circuit should be able to run at much higher speeds (however, that would require a crystal oscillator instead of the limited 555 circuit).

<img src="https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/Pictures/Computer.jpg?raw=true" alt="Computer" width="718" height="690">

<img src="https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/Pictures/ALU_Chips.jpg?raw=true" alt="Computer" width="720" height="485">


## Modules

Just like in Ben Eater's design, the computer is split in several modules that are connected to a main bus in the middle.

However, unlike the SAP-1, my computer does make use of microcontrollers for I/O. Some will say this is cheating, but I'm allowing it because the CPU itself is still made out of discrete logic chips (it works just fine without any microcontroller, it's just that the I/O is less convenient).

- Arduino Nano for programming the RAM (instead of having to use the DIP switches)
- Arduino Nano for interfacing with a PS/2 keyboard
- LCD module (like the one used in Ben Eater's 6502 series)

<img src="https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/Pictures/Computer_Modules.jpg?raw=true" alt="Computer modules" width="718" height="690">


## PS/2 Keyboard demo

<img src="https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/Pictures/keyboard.gif?raw=true" alt="Computer modules" width="600" height="860">
