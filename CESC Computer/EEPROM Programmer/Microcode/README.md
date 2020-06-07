The microcode of the computer is stored in 3 EEPROMs for a total of 24 control signals.

## EEPROM 0:
- **HLT:** Disables the clock output.
- **CLR:** Resets the microcode counter (starts next instruction). This allows for some instructions to take more clock cycles than others. 
- **OutEn:** If OutEn = 0, no main module is outputting tot the bus (for example: when the keyboard controller is publishing  its input to the bus, all outputs must be disabled to avoid bus contention).
- **OUT:** If OutEn = 1, OUT gets demultiplexed to create the output signal.
	- **OUT = 00:** ALU Out
	- **OUT = 01:** Memory (RAM) Out
	- **OUT = 10:** PC Out
	- **OUT = 11:** SP Out
- **ImmOp:** Forces the ALU to perform an immediate operation even if the Funct input doesn't match.
	- **ImmOp = 000:** Use Funct input
	- **ImmOp = 001:** Force ADDI
	- **ImmOp = 010:** Unused
	- **ImmOp = 011:** Unused
	- **ImmOp = 100:** Force MOVE (output contents of X reg.)
	- **ImmOp = 101:** Output contents of Y reg.
	- **ImmOp = 110:** Force SUBI
	- **ImmOp = 111:** Force ANDI
	
	The upper bit of ImmOp is also used in other signals that will never be active at the same time as ImmOp. When ImmOp_2 = 1, SP++ becomes SP-- and LdTmp will load the contents of the bus instead of a register.


## EEPROM 1:
- **MemBank:** Chooses which memory bank to access (complements the 8 bit address to achieve 1kB of memory):
	- **MemBank = 00:** Program memory low (where opcodes are fetched)
	- **MemBank = 01:** Program memory high (where arguments are fetched)
	- **MemBank = 10:** Data memory (where LD and ST operations are performed)
	- **MemBank = 11:** Stack (used by POP, PUSH, CALL...)
- **LdReg:** Writes the bus contents to Rd (Rd is connected directly from the IR, no control logic is required). 
- **LdTmp:** Loads a value into temporary register Y, and moves its old value to temporary register X at the same time (the value in X gets discarded). The value loaded into Y is:
	- **If ImmOp_2 = 0:** The last 2 bits on the bus indicate a register number. The value stored in that register is copied to Y.
	- **If ImmOp_2 = 1:** The value in the bus is copied to Y (used on ADDI, CMP-SUBI and CMP-ANDI).
- **LdFlg:** The flags register gets updated.
- **@In:** The MAR (Memory Address Register) loads the value in the bus.
- **MemIn:** Stores the value in the bus to the current address (combination of MAR and MemBank).
- **IrIn:** The IR (Instruction Register) loads the value in the bus.

## EEPROM 2:
- **PcIn:** A jump is performed: the PC (Program Counter) loads the value in the bus.
- **PC++:** Increments the PC. 
- **SP++:** Moves the SP (Stack Pointer) up and down:
	- **If ImmOp_2 = 0:** SP gets incremented (POP)
	- **If ImmOp_2 = 1:** SP gets decremented (PUSH)
- **DecIn:** Outputs the contents of the bus to the decimal display (Output Register loads the value in the bus).
- **LcdIn:** Sends data to the LCD module.
- **LcdDt:** Indicates to the LCD module what kind of data has been sent.
	- **If LcdDt = 0:** The data is a command.
	- **If LcdDt = 1:** The data is a character to be displayed.
- **Inp:** Get input (the keyboard controller outputs the current input to the bus).
- **Ack:** Sends the Acknowledge signal to the keyboard controller.


##

The code assumes that the finished pin layout and connections for the EEPROM are the following:
![Connections](https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/EEPROM%20Programmer/Microcode/Pin%20connections.jpg?raw=true)
