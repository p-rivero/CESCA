# Assembling and sending programs

## Assembly examples

The `Examples` folder contains:

- Some assembly programs
- A math library
- The *cpudef* required for using customasm as the assembler

The `Useful extras` folder contains helper tools for writing assembly programs


## Sending programs

> **WARNING:** Before continuing, I highly recommend compiling `RAM_Programmer.exe` yourself (the source code is in `ram_programmer/ram_programmer.cc`). Otherwise, some systems might detect it as a virus.

In order to send programs to the Arduino RAM programmer, download *customasm*, an *Arduino IDE* and the *`ram_programmer`* folder.
Place `customasm.exe` and the `ram_programmer` folder next to `RAM_Programmer.exe` and run it from a console, providing the name of the assembly file as an argument.

Example (running from windows cmd)

```sh
...\Assembly> RAM_Programmer Examples\example.asm
```

The program will automatically launch Arduino IDE, from which you can upload the program to the Arduino RAM programmer.
