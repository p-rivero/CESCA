# EEPROM programmer

Each folder contains C++ code for generating the contents of each EEPROM. The advantage of using C++ on a computer instead of using the Arduino itself is that the generator code doesn't need to be very efficient, and you only need 1 Arduino program that writes an arbitrary file into the EEPROM, instead of several different Arduino files.

After the contents have been generated, `programmer.ino` can be loaded into an Arduino programmer like the one used by Ben Eater.

## Generating the contents
When `generate.cc` is compiled and executed, it generates one (or more) `output.hex` file and one (or more) `eeprom_contents.h` file.

- The `output.hex` file can be used in circuit simulator programs.
- The `eeprom_contents.h` file is the one you need for the Arduino programmer.

If you don't need to make changes to the code, the resulting `output.hex` and `eeprom_contents.h` files have already been generated in each folder, so you won't need to compile the program.

## Programming the EEPROM

The pin connections on my Arduino programmer are the same as Ben's. The only difference is the extra address outputs from the shift registers to the AT28C64B I used. Simply move the corresponding `eeprom_contents.h` next to `programmer.ino` (or change the #include directive) and upload the program to your Arduino programmer. Open the serial monitor to see the progress.

**Improvements over Ben's original programmer:**

- Added progress bars that indicate the state of the current operation.
- Added a content check once the programming has finished: reads all the addresses and compares its content with the value it was supposed to have. If it detects a conflict, an error message is printed to the serial monitor and the check is aborted.
- Added a serial interface that allows the user to select which operation to perform from a command line menu.

**Demo of the serial interface:**

![Demo](https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/EEPROM%20Programmer/demo.gif?raw=true)
