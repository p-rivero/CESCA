# EEPROM programmer

Each folder contains C++ code for generating the contents of each EEPROM. The advantage of using C++ on a computer instead of using the Arduino itself is that the generator code doesn't need to be very efficient, and you only need 1 Arduino program that writes an arbitrary file into the EEPROM.

After the contents have been generated, `programmer.ino` can be loaded into an Arduino programmer like the one used by Ben Eater.

## Generating the contents
When `generate.cc` is compiled and executed, it generates one (or more) `output.hex` file and one (or more) `eeprom_contents.h` file.
- The `output.hex` file can be used in circuit simulator programs.
- The `eeprom_contents.h` file is the one you need for the Arduino programmer.

If you don't need to make changes to the code, the resulting `output.hex` and `eeprom_contents.h` files have already been generated in each folder, so you won't need to compile the program.

## Programming the EEPROM

**(`programmer.ino` isn't available yet)**

Simply move the corresponding `eeprom_contents.h` next to `programmer.ino` (or change the #include directive) and upload the program to your Arduino programmer.