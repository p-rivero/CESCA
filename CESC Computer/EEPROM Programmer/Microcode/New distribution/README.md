# New control EEPROM distribution

In order to greatly improve wiring and get rid of the inverters, the distribution of the signals inside the EEPROMs has been changed (both EEPROM inputs and outputs).

This folder contains the new code for generating the shuffled microcode. The meaning of each control signal hasn't changed, just it's physical encoding inside the EEPROM.

The code assumes that the new pin layout and connections for the EEPROM are the following:

<img src="https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/EEPROM%20Programmer/Microcode/New%20distribution/Pin%20connections.jpg?raw=true" alt="Connections" width="700" height="1286">
