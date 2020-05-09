#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

// Bit patterns for the digits 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
const vector<char> digits = { 0x6F, 0x0C, 0x5B, 0x5E, 0x3C, 0x76, 0x77, 0x4C, 0x7F, 0x7E, 0x7D, 0x37, 0x63, 0x1F, 0x73, 0x71 };
map<string, char> special_digits = {
	{ "-", 0x10 }, 
	{ "H", 0x3D },
	{ "S", 0x76 }
};

void generate(vector<char>& content) {

	// GENERATE ROM CONTENTS:
	
	// Unsigned mode
	cout << "Programming unsigned mode" << endl;
	for (int value = 0; value < 256; value++) {
		// 1s
		content[value] = digits[value % 10];
		
		// 10s
		content[value + 256] = digits[(value / 10) % 10];
		
		// 100s
		content[value + 512] = digits[(value / 100) % 10];
		
		// sign
		content[value + 768] = 0;
	}
	
	
	// Signed mode
	cout << "Programming signed mode" << endl;
	for (int value = 0; value < 128; value++) {
		// 1s
		content[value + 1024] = digits[abs(value) % 10];
		content[-value + 1024 + 256] = digits[abs(value) % 10];
		
		// 10s
		content[value + 1280] = digits[abs(value / 10) % 10];
		content[-value + 1280 + 256] = digits[abs(value / 10) % 10];
		
		// 100s
		content[value + 1536] = digits[abs(value / 100) % 10];
		content[-value + 1536 + 256] = digits[abs(value / 100) % 10];
		
		// sign
		content[value + 1792] = 0;
		content[value + 1792 + 128] = special_digits["-"];
	}
	// Program -128
	content[1024 + 128] = digits[8];
	content[1280 + 128] = digits[2];
	content[1536 + 128] = digits[1];
	
	
	// Hex mode
	cout << "Programming hex mode" << endl;
	for (int value = 0; value < 256; value++) {
		// 1s
		content[value + 2048] = digits[value % 16];
		
		// 10s
		content[value + 2304] = digits[(value / 16) % 16];
		
		// 100s
		content[value + 2560] = 0;
		
		// H for hex
		content[value + 2816]= special_digits["H"];
	}
	
	
	// Name of your build
	cout << "Programming CESC" << endl;
	for (int value = 0; value < 256; value++) {
		// 4th letter
		content[value + 3072] = digits[0xC];
		
		// 3rd letter
		content[value + 3328] = special_digits["S"];
		
		// 2nd letter
		content[value + 3584] = digits[0xE];
		
		// 1st letter
		content[value + 3840] = digits[0xC];
	}
}


int main() {
	int size = 4*1024;
	vector<char> content(size);
	
	generate(content);
	
	ofstream outputFile;
	outputFile.open ("output.hex");
	cout << "Writing HEX file" << endl;
	for (int i = 0; i < size/32; i++) {
		for (int j = 0; j < 32; j++)
			outputFile << setw(2) << setfill('0') << hex << int(content[32*i + j]) << ' ';
		outputFile << endl;
	}
	outputFile.close();

	outputFile.open ("eeprom_contents.h");
	cout << "Writing arduino format" << endl;
	outputFile << "const PROGMEM uint8_t EEPROM_CONTENTS[] = {";
	for (int i = 0; i < size/32; i++) {
		outputFile << endl << "\t";
		for (int j = 0; j < 32; j++) {
			outputFile << dec << int(content[32*i + j]);
			if (j < 31 or i < size/32 - 1) outputFile << ", ";
		}
	}
	outputFile << endl << "}" << endl;
	
	outputFile.close();
	cout << "Done." << endl;
}

