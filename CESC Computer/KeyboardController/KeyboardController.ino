#include "PS2Keyboard.h"

#define DATA_PIN 4
#define IRQ_PIN  3

// TODO: Define pins according to breadboard layout
#define BUS_0 -1
#define BUS_7 -1
#define CLK_INV -1
#define ACK_PIN -1
#define OUTPUT_ENABLE -1

PS2Keyboard keyboard;

// Character map for translating scancodes (from a spanish keyboard with non-standard layout)
// into ascii characters (all non-ascii characters have been removed)
const PROGMEM PS2Keymap_t PS2Keymap_CustomSpanish = {
  // without shift
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0 /*'º'*/, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
    0, 0, 'z', 's', 'a', 'w', '2', 0,
    0, 'c', 'x', 'd', 'e', '4', '3', 0,
    0, ' ', 'v', 'f', 't', 'r', '5', 0,
    0, 'n', 'b', 'h', 'g', 'y', '6', 0,
    0, 0, 'm', 'j', 'u', '7', '8', 0,
    0, ',', 'k', 'i', 'o', '0', '9', 0,
    0, '.', '-', 'l', 0 /*PS2_n_TILDE*/, 'p', '\'', 0,
    0, 0, 0/*'´'*/, 0, '`', 0/*'¡'*/, 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '+', 0, 0 /*PS2_c_CEDILLA*/, 0, 0,
    0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
  // with shift
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0 /*'ª'*/, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
    0, 0, 'Z', 'S', 'A', 'W', '"', 0,
    0, 'C', 'X', 'D', 'E', '$', 0 /*'·'*/, 0,
    0, ' ', 'V', 'F', 'T', 'R', '%', 0,
    0, 'N', 'B', 'H', 'G', 'Y', '&', 0,
    0, 0, 'M', 'J', 'U', '/', '(', 0,
    0, ';', 'K', 'I', 'O', '=', ')', 0,
    0, ':', '_', 'L', 0 /*PS2_N_TILDE*/, 'P', '?', 0,
    0, 0, 0 /*'¨'*/, 0, '^', 0 /*'¿'*/, 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '*', 0, 0 /*PS2_C_CEDILLA*/, 0, 0,
    0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
    1,
  // with altgr
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '\\', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 0, '|', 0,
    0, 0, 0, 0, 0, 0, '@', 0,
    0, 0, 0, 0, 0, '~', '#', 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0 /*'¬'*/, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, '{', 0, '[', 0, 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '}', 0, 0,
    0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 }
};



void setup() {
    keyboard.begin(DATA_PIN, IRQ_PIN, PS2Keymap_CustomSpanish);
}

// Convert an ascii keystroke to the CESCA keyboard format
byte cesca_translate(uint16_t character) {
    byte result;
    bool key_break = character & PS2_KEY_BREAK; // Detect if it's a break (stopped pressing the key)
    character &= 0xFF; // Keep just the ascii code
    
    // Check if character is a special key, and add BREAK field
    if (character == PS2_RIGHTARROW)
        result = 0b10000001 + 0b01000000*key_break;
    else if (character == PS2_DOWNARROW)
        result = 0b10000010 + 0b01000000*key_break;
    else if (character == PS2_UPARROW)
        result = 0b10000100 + 0b01000000*key_break;
    else if (character == PS2_LEFTARROW)
        result = 0b10001000 + 0b01000000*key_break;
    else if (character == PS2_DELETE or character == PS2_BACKSPACE)
        result = 0b10010000 + 0b01000000*key_break;
    else if (character == PS2_ENTER)
        result = 0b10100000 + 0b01000000*key_break;

    // Not a special key. If it's a 7 bit ascii code and not a break, output it without change
    else if (character < 128 and not key_break) result = character;
    else result = 0;
    
    return result;
}

byte currentChar = 0; // Fake "Input register"
bool outputting = false; // True if bus pins are outputting the contents of currentChar

// Send and hold a character to the bus
void busWrite(byte character) {
    for (int pin = BUS_0; pin <= BUS_7; pin++) { // For each bus pin:
        pinMode(pin, OUTPUT); // Set to output
        digitalWrite(pin, character & 1); // Write the least significant bit to the pin
        character = character >> 1; // Shift to the next bit
    }
    outputting = true;
}

// Stop outputting to the bus
void busHighZ() {
    for (int pin = BUS_0; pin <= BUS_7; pin++){ // For each bus pin:
        pinMode(pin, INPUT); // Set to high impedance
    }
    outputting = false;
}

void loop() {    
    // If current character is empty and not being cleared, check if a new character is available
    if (currentChar == 0 and not digitalRead(ACK_PIN) and keyboard.available()) {
        uint16_t r = keyboard.read();
        // Since PS2Keyboard library doesn't support sending commands to the keyboard, there is no way to disable Typematic Repeat
        // (keyboard keeps sending keystrokes for as long as the key is pressed down).
        // This behaviour is desirable in most cases and shouldn't cause problems, but it's something to be aware of.
        // This repeat could be filtered by waiting until a key break before accepting it again.

        // Set current character according to CESCA interface
        currentChar = cesca_translate(r);
    }

    // Only if OUTPUT_ENABLE is high, output current character to bus
    if (not outputting and digitalRead(OUTPUT_ENABLE))
        busWrite(currentChar);
    else if (outputting and not digitalRead(OUTPUT_ENABLE))
        busHighZ();

    // If ACK and CLK are high, clear current character
    if (currentChar != 0 and digitalRead(ACK_PIN) and not digitalRead(CLK_INV))
        currentChar = 0;
}
