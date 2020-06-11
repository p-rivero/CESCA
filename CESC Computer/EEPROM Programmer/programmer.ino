#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define WRITE_DELAY 10  // Delay in milliseconds
#define PROGRESS_SZ 128 // Adjust width of the progress bar (how many bytes are written for each dot)
#define PRINT_SIZE 256  // Number of bytes to print (multiple of 16)

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


// High level fuctions (slightly modified):

// Read a byte from EEPROM_CONTENTS, stored in PROGMEM
byte getContents(int index) {
    return pgm_read_byte_near(EEPROM_CONTENTS + index);
}

// Program the EEPROM. The contents from eeprom_contents.h are stored in PROGMEM
void writeContents() {
    Serial.println("Programming EEPROM. Size: " + String(CONTENTS_SIZE) + " (0x" + String(CONTENTS_SIZE, HEX) + ")");
    
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
void printContents() {
    Serial.println("\nPrinting first " + String(PRINT_SIZE) + " bytes:");
    
    for (int addr = 0; addr < PRINT_SIZE; addr += 16) {
        byte data[16];
        for (int i = 0; i < 16; i++)
            data[i] = readEEPROM(addr + i);
        
        char buf[80];
        sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
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
            Serial.print("\nERROR DETECTED AT ADDRESS 0x" + String(addr, HEX) + ". Read data was 0x");
            Serial.println(String(readEEPROM(addr), HEX) + " instead of 0x" + String(getContents(addr), HEX));
            return;
        }
    
        if (addr % PROGRESS_SZ == 0) 
            Serial.print(".");
    }
    Serial.println("\nNO ERRORS WERE DETECTED!");
}



void setup() {
    // Initialize pins:
    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(SHIFT_CLK, OUTPUT);
    pinMode(SHIFT_LATCH, OUTPUT);
    digitalWrite(WRITE_EN, HIGH);
    pinMode(WRITE_EN, OUTPUT);
    Serial.begin(57600);
    
    // Program the EEPROM
    writeContents();
    
    // Print first bytes of the EEPROM
    printContents();
    
    // Check if the written data is correct
    runTest();
}

void loop() {
    // No code needed
}
