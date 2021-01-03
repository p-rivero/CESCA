#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define WRITE_DELAY 10  // Delay in milliseconds
#define PROGRESS_SZ 64 // Adjust width of the progress bar (how many bytes are written for each dot)

#include "eeprom_contents.h"
// eeprom_contents.h has an array named EEPROM_CONTENTS and an integer CONTENTS_SIZE.
// EEPROM_CONTENTS will be written to the EEPROM.


// Low level I/O functions (identical to Ben Eater's version):

// Set the address and the outputEnable signal
void setAddress(int address, bool outputEnable) {
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

    digitalWrite(SHIFT_LATCH, LOW);
    digitalWrite(SHIFT_LATCH, HIGH);
    digitalWrite(SHIFT_LATCH, LOW);
}

// Read a byte from an address
byte readEEPROM(int address) {
    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++)
        pinMode(pin, INPUT);
  
    setAddress(address, true);    // Set address and outputEnable

    byte data = 0;
    for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin--)
        data = (data << 1) + digitalRead(pin);
  
    return data;
}

// Write a byte to an address 
void writeEEPROM(int address, byte data) {
    setAddress(address, false);   // Set address and outputEnable
  
    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++) 
        pinMode(pin, OUTPUT);

    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++) {
        digitalWrite(pin, data & 1);
        data = data >> 1;
    }
  
    digitalWrite(WRITE_EN, LOW);
    digitalWrite(WRITE_EN, HIGH);
    delay(WRITE_DELAY);
}


// Utilities:

// Read a byte from EEPROM_CONTENTS, stored in PROGMEM
byte getContents(int index) {
    return pgm_read_byte_near(EEPROM_CONTENTS + index);
}

// Return a number in the format "DDD (0xHH)"
String decAndHex(int num) {
    return (String(num) + " (0x" + String(num, HEX) + ")");
}


// High level fuctions (slightly modified from Ben Eater's version):

// Program the EEPROM. The contents from eeprom_contents.h are stored in PROGMEM
void programEeprom() {
    Serial.println("\nProgramming EEPROM. Size: " + decAndHex(CONTENTS_SIZE));
    
    for (int i = 0; i < CONTENTS_SIZE/PROGRESS_SZ; i++) // Draw progress bar
        Serial.print(".");
    Serial.print("\n");
    
    for (int addr = 0; addr < CONTENTS_SIZE; addr++) {
        writeEEPROM(addr, getContents(addr));
    
        if (addr % PROGRESS_SZ == 0) 
            Serial.print(".");
    }
    Serial.println("\ndone!");
}

// Print the contents of the first addresses on the EEPROM to the serial monitor
void printContents(int sz) {
    Serial.println("\nDisplaying first " + String(sz) + " bytes:");
    
    for (int addr = 0; addr < sz; addr += 16) {
        byte data[16];
        for (int i = 0; i < 16; i++)
            data[i] = readEEPROM(addr + i);
        
        char buf[80];
        sprintf(buf, "0x%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
                addr, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
        
        Serial.println(buf);
    }
}

// Checks if all addresses contain their expected values
void runTest() {
    Serial.println("\nRunning test:");    
    for (int i = 0; i < CONTENTS_SIZE/PROGRESS_SZ; i++) // Draw progress bar
        Serial.print(".");
    Serial.print("\n");
    
    for (int addr = 0; addr < CONTENTS_SIZE; addr++) {
        if (readEEPROM(addr) != getContents(addr)) {
            Serial.print("\nERROR DETECTED AT ADDRESS " + decAndHex(addr) + ". Read data was ");
            Serial.println(decAndHex(readEEPROM(addr)) + " instead of " + decAndHex(getContents(addr)));
            return;
        }
    
        if (addr % PROGRESS_SZ == 0) 
            Serial.print(".");
    }
    Serial.println("\nNO ERRORS WERE DETECTED!");
}

void showHelp() {
    Serial.println("\nAvailable commands:");
    Serial.println("p       Program the EEPROM (recommended)");
    Serial.println("t       Test EEPROM to check if it matches the expected contents from eeprom_contents.h");
    Serial.println("d N     Display the first N addresses of the EEPROM. Ex: d 256");
    Serial.println("r A     Read the contents at address A (decimal!).   Ex: r 1024");
    Serial.println("w A D   Write data D to address A (both decimal!).   Ex: w 1024 0");
    Serial.println("h       Help: display the available commands again.");
    Serial.println("\nWARNING: all arguments must be entered in DECIMAL!");
}


void setup() {
    // Initialize pins:
    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(SHIFT_CLK, OUTPUT);
    pinMode(SHIFT_LATCH, OUTPUT);
    digitalWrite(WRITE_EN, HIGH);
    pinMode(WRITE_EN, OUTPUT);
    Serial.begin(57600);

    Serial.println("EEPROM PROGRAMMER\neeprom_contents.h loaded. Size: " + decAndHex(CONTENTS_SIZE));
    showHelp();
}

void loop() {
    // If an input is ready and no operations are being performed, read the input
    if (Serial.available() > 0) {
        byte inp = Serial.read();
        if (inp == 'p' or inp == 'P') {  // Program EEPROM
            programEeprom(); // Program the EEPROM
            runTest(); // Check if the written data is correct
            
        } else if (inp == 't' or inp == 'T') {  // Run test
            runTest();
            
        } else if (inp == 'd' or inp == 'D') {  // Display contents
            int num = Serial.parseInt();
            if (num < 0) Serial.println("\nError: the number of addresses to display must be positive!");
            else {
                int rounded = num - num%16;
                if (num%16 != 0) rounded += 16;
                printContents(rounded);
            }
            
        } else if (inp == 'r' or inp == 'R') {  // Read from address
            int addr = Serial.parseInt();
            if (addr < 0) Serial.println("\nError: the address must be positive!");
            else {
                byte r = readEEPROM(addr);
                Serial.println("\nAddress " + decAndHex(addr) + " contains: " + decAndHex(r));
            }
            
        } else if (inp == 'w' or inp == 'W') {  // Write to address
            int addr = Serial.parseInt();
            byte d = Serial.parseInt();
            if (addr < 0) Serial.println("\nError: the address must be positive!");
            else {
                writeEEPROM(addr, d);
                Serial.println("\nData " + decAndHex(d) + " has been written to address " + decAndHex(addr));
            }
            
        } else if (inp == 'h' or inp == 'H') {  // Show help message
            showHelp();
            
        } else if (inp != 0x0A and inp != 0x20) {   // Ignore newline and space characters
            Serial.println("\nInvalid command. Use \"h\" for help");
        }
    }
}
