// Pins:
#include "program.h"

#define BUS_0 3     // First bus pin
#define BUS_7 10    // Last bus pin
#define RAM_WR 13   // RAM write enable
#define RAM_OUT 16  // A2: RAM output enable (active low!!)
#define ADDR_IN 17  // A3: MAR load enable
#define BANK_0 18   // A4: Override memory bank (least significant)
#define BANK_1 19   // A5: Override memory bank (most significant)
#define HOLD_HIGH 14    // Hold A0 pin high
#define HOLD_LOW 15     // Hold A1 pin low
#define CLK_IN 2    // Clock input in order to make sure it's turned off

#define NUM_LINES 8     // Amount of addresses to print for each line


// LOW LEVEL I/O FUNCTIONS:

// Outputs and holds a value to the bus (all bus pins MUST be in output mode)
void writeBUS(byte val) {
    for (int pin = BUS_0; pin <= BUS_7; pin++) { // For each bus pin:
        digitalWrite(pin, val & 1); // Write the least significant bit to the pin
        val = val >> 1; // Shift to the next bit
    }
}

// Reads the contents of the bus (all bus pins MUST be in input mode)
byte readBUS() {
    byte data = 0;
    for (int pin = BUS_7; pin >= BUS_0; pin--) // For each bus pin, starting with the most significant bit
        data = (data << 1) + digitalRead(pin); // Shift the value and add the bit at the end
    return data;
}

// Set the address held by the MAR (all bus pins MUST be in output mode)
void setAddress(byte address) {
    // Write address to the bus
    writeBUS(address);

    digitalWrite(ADDR_IN, LOW); // Address loads on falling edge
    digitalWrite(ADDR_IN, HIGH);
}

// Write instructions and data to an address in RAM (all bus pins MUST be in output mode)
// contents[0] = opcode, contents[1] = argument, contents[2] = data
void writeRAM(byte address, byte contents[], int sz) {
    // Write the address to the MAR
    setAddress(address);

    // For each element in contents[], set the bank address pins and write to RAM
    for (int bank = 0; bank < sz; bank++){
        digitalWrite(BANK_0, bank & 1); // Write the least significant bit of the bank
        digitalWrite(BANK_1, bank & 2); // Write the most significant bit of the bank
        
        writeBUS(contents[bank]);
        digitalWrite(RAM_WR, HIGH);  // RAM loads on rising edge
        digitalWrite(RAM_WR, LOW);
    }
}

// Reads the contents of the 4 banks with a given address in RAM and stores them in contents[]
void readRAM(byte address, byte contents[4]) {
    // Write the address to the MAR
    setAddress(address);

    // Set bus pins to input
    for (int pin = BUS_0; pin <= BUS_7; pin++)
        pinMode(pin, INPUT);

    // Enable RAM output (RAM_OUT is active low)
    digitalWrite(RAM_OUT, LOW);
    
    for (int bank = 0; bank < 4; bank++) {
        digitalWrite(BANK_0, bank & 1); // Write the least significant bit of the bank
        digitalWrite(BANK_1, bank & 2); // Write the most significant bit of the bank
        
        contents[bank] = readBUS();
    }

    // Disable RAM output (RAM_OUT is active low)
    digitalWrite(RAM_OUT, HIGH); 
    
    // Return bus pins to output
    for (int pin = BUS_0; pin <= BUS_7; pin++)
        pinMode(pin, OUTPUT);
}


// UTILITIES:

// Return a number in the format "DDD (0xHH)"
String decAndHex(int num) {
    return (String(num) + " (0x" + String(num, HEX) + ")");
}

// Read a byte from PROGRAM_CONTENTS, stored in PROGMEM
byte getContents(int index) {
    return pgm_read_byte_near(PROGRAM_CONTENTS + index);
}

// Show help dialog
void showHelp() {
    Serial.println("\nAvailable commands:");
    Serial.println("p          Program the hardcoded hexstring (#include directive)");
    Serial.println("d N        Display the first N addresses on the RAM. Ex: d 256");
    Serial.println("r A        Read the contents at address A.   Ex: r 1024");
    Serial.println("w A n ...  Write n bytes to address A. Followed by n values.   Ex: w 1024 3  0 6 8");
    Serial.println("q          Quit: return the Arduino to a high-Z state.");
    Serial.println("h          Help: display the available commands again.");
    Serial.println("\nWARNING: all arguments must be entered in DECIMAL!");
}

// Initialize the pins that need to be set to output
void initPins(){
    digitalWrite(HOLD_HIGH, HIGH);
    pinMode(HOLD_HIGH, OUTPUT);
    digitalWrite(HOLD_LOW, LOW);
    pinMode(HOLD_LOW, OUTPUT);


    digitalWrite(ADDR_IN, HIGH); // Address loads on falling edge
    pinMode(ADDR_IN, OUTPUT);
    digitalWrite(RAM_WR, LOW);
    pinMode(RAM_WR, OUTPUT);
    digitalWrite(RAM_OUT, HIGH); // RAM_OUT is active low
    pinMode(RAM_OUT, OUTPUT);
    digitalWrite(BANK_0, LOW);
    pinMode(BANK_0, OUTPUT);
    digitalWrite(BANK_1, LOW);
    pinMode(BANK_1, OUTPUT);

    // Set bus pins to output
    for (int pin = BUS_0; pin <= BUS_7; pin++)
        pinMode(pin, OUTPUT);
}

// Reset all pins to highZ state, so the computer can start running again
void resetPins(){
    pinMode(ADDR_IN, INPUT);
    pinMode(RAM_WR, INPUT);
    pinMode(RAM_OUT, INPUT);
    pinMode(BANK_0, INPUT);
    pinMode(BANK_1, INPUT);
    
    for (int pin = BUS_0; pin <= BUS_7; pin++)
        pinMode(pin, INPUT);

    pinMode(HOLD_HIGH, INPUT);
    pinMode(HOLD_LOW, INPUT);
}


// HIGH LEVEL FUNCTIONS:

// Print the contents of N addresses (starting from a base address "addr") of RAM to the serial monitor
void printAddr(byte addr, const int N) {
    // Print base address
    char buf[16];
    sprintf(buf, "0x%02x:", addr);
    Serial.print(buf);

    byte data[4];
    // Read N addresses
    for (int i = 0; i < N; i++) {
        readRAM(addr + i, data);
        sprintf(buf, "    %02x%02x %02x %02x", data[0], data[1], data[2], data[3]);
        Serial.print(buf);
    }
    Serial.print("\n");
}

// Print the contents of the first addresses of RAM to the serial monitor
void printContents(int num) {
    Serial.println("\nDisplaying first " + String(num) + " addresses:");
    
    for (int addr = 0; addr < num; addr += NUM_LINES)
        printAddr(byte(addr), NUM_LINES);
}


// ENTERING A PROGRAM

int char2int(char input) {
    if (input >= '0' and input <= '9') return input - '0';
    return input - 'a' + 10;
}

void programHexstring() {
    byte data[3];
    for (int i = 0; i < 1024; i += 4) {
        // Read instructions
        data[0] = char2int(getContents(i)) * 16 + char2int(getContents(i + 1));
        data[1] = char2int(getContents(i + 2)) * 16 + char2int(getContents(i + 3));

        // Read data
        int j = i/2 + 1024;
        data[2] = char2int(getContents(j)) * 16 + char2int(getContents(j + 1));

        writeRAM(i/4, data, 3);
    }
    Serial.println("Done!");
}


// INITIALIZATION OF THE ARDUINO

void setup() {
    // Set pins to highZ, just to be safe
    resetPins();
    // If there was a serial connection, end it
    Serial.end();
    
    // Initialize serial connection:
    Serial.begin(57600);

    Serial.println("Send \"u\" to start programmer UI or \"p\" to just send the program");
    
    while (not Serial.available())
        delay(10);
    
    char r = Serial.read();
    if (r == 'u' or r == 'U') {
        initPins();
        showHelp();
        
    } else if (r == 'p') {
        initPins();
        programHexstring();
        printContents(256);
        setup();
        
    } else setup();
}

void loop() {
    // If an input is ready and no operations are being performed, read the input
    if (Serial.available() > 0) {
        byte inp = Serial.read();
        if (inp == 'p' or inp == 'P') {
            programHexstring();
        } else if (inp == 'd' or inp == 'D') {  // Display contents
            int num = Serial.parseInt();
            if (num < 0) Serial.println("\nError: the number must be positive!");
            else {
                int rounded = num - num%NUM_LINES;
                if (num%NUM_LINES != 0) rounded += NUM_LINES;
                printContents(rounded);
            }
            
        } else if (inp == 'r' or inp == 'R') {  // Read from address
            int addr = Serial.parseInt();
            if (addr < 0) Serial.println("\nError: the address must be positive!");
            else {
                Serial.println("\nDisplaying contents at address " + decAndHex(addr) + ":");
                printAddr(addr, 1);
            }
            
        } else if (inp == 'w' or inp == 'W') {  // Write to address
            int addr = Serial.parseInt();
            int sz = Serial.parseInt();
            byte d[sz];
            for (int i = 0; i < sz; i++)
                d[i] = Serial.parseInt();
                
            if (addr < 0) Serial.println("\nError: the address must be positive!");
            else if (sz < 1 or sz > 3) Serial.println("\nError: the amount of banks must be between 1 and 3!");
            else {
                writeRAM(addr, d, sz);
                Serial.println("\nThe data has been written to address " + decAndHex(addr));
            }
            
        } else if (inp == 'h' or inp == 'H') {  // Show help message
            showHelp();
            
        } else if (inp == 'q' or inp == 'Q') {  // Quit
            setup();
            
        } else if (inp != 0x0A and inp != 0x20) {   // Ignore newline and space characters
            Serial.print("\nInvalid command:\"");
            Serial.print(char(inp));
            Serial.println("\". Use \"h\" for help");
        }
    }
}
