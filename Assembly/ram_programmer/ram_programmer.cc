#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Name used for temporary file
#define TEMP_NAME "temp.assembled"
#define ARDUINO_FILE "ram_programmer\\ram_programmer.ino"
#define ARDUINO_HEADER "ram_programmer\\program.h"

// Check if a file is located at the same folder
bool file_exists(const char* filename) {
    FILE* check;
    
    bool exists = not fopen_s(&check, filename, "r");
    if (exists) fclose(check);
    
    return exists;
}

// Read the assembled hexstring and convert it to a header file for Arduino
void process_hexstr() {
    // Read hexstring
    ifstream ifs(TEMP_NAME);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    ifs.close();

    // Output to new file
    ofstream outputFile;
    outputFile.open(ARDUINO_HEADER);
    outputFile << "const PROGMEM char PROGRAM_CONTENTS[] = \"" << content << "\";" << endl;
    outputFile.close();

    // Try to delete temporary file. If error code isn't 0, print error
    if (remove(TEMP_NAME)) cout << "Error deleting temporary files!" << endl;
}


int main(int argc, char* argv[]) {

    if (argc == 1) { // No arguments have been provided
        cout << "CESC PROGRAM SENDER" << endl;
        cout << "How to use: Specify (as an argument) the name of the file you want to assemble." << endl << endl;
        cout << "Arduino IDE will be launched in order to send the file. Make sure you have it installed on your computer." << endl;
        cout << "Once it launches, press UPLOAD (arrow at the top left) and open the SERIAL MONITOR (magnifing glass at the top right)." << endl;
        cout << "Then, follow the instructions on the serial monitor (or just type \"p\" and press Enter)." << endl;
    }
    else if (not file_exists("customasm.exe")) // One or more arguments have been provided, but customasm.exe can't be found
        cout << "ERROR: customasm.exe can't be found! Download it from github.com/hlorenzi/customasm/releases and place it next to this file" << endl;

    else if (not file_exists(ARDUINO_FILE)) // ram_programmer.ino can't be found
        cout << "ERROR: ram_programmer.ino can't be found! Download it from github.com/p-rivero/CESCA/tree/master/Assembly and place it on a folder called \"ram_programmer\" next to this file" << endl;

    else { // No errors detected
        // Call customasm for assembling the file
        cout << "Assembling file: " << argv[1] << endl << endl;
        char buff[512];
        snprintf(buff, 512, "\"customasm -f hexstr -o %s %s \"", TEMP_NAME, argv[1]);
        system(buff);
        
        if (not file_exists(TEMP_NAME)) {
            cout << "Assembled file doesn't exist! Please read assembler error above." << endl;
            return 0;
        }
        
        // Generate header file
        process_hexstr();

        // Open Arduino IDE
        system(ARDUINO_FILE);
    }
}