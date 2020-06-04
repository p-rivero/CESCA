This EEPROM is an 8 bit decimal decoder. It's similar to the one from by Ben Eater's videos, with 2 small modifications:

## Output layout:
The order of the outputs (from most significant to less significant) has been changed to `X A F G B C D E` instead of `X A B C D E F G`. This makes the wiring easier.

`X` means disconnected (Ben connects it to the decimal point, but it's never used).

## New modes:
In addition to the original signed and unsigned decimal modes, 2 new display modes have been added:

- **Hex:** Displays `H xx`, where `xx` is the hexadecimal representation of the number.
- **Name of the build:** Displays `CESC`.

##

The code assumes that the finished pin layout and connections for the EEPROM are the following:
![Connections](https://github.com/p-rivero/CESCA/blob/master/CESC%20Computer/EEPROM%20Programmer/Decimal%20display/Pin%20connections.jpg?raw=true)
