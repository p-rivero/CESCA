This EEPROM is a translation layer between the Funct / control signals and the inputs of the 74hc181. It also controls the shift buffers and the Carry flag (this way the active low signal from the 74hc181 is inverted only when the operation is ADD, since the borrow from SUB doesn't need to be inverted).

The code assumes that the finished pin layout and connections for the EEPROM are the following:
![Connections](https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/EEPROM%20Programmer/ALU/Pin%20connections.jpg?raw=true)
