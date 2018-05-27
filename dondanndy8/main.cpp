#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include "cpu.h"

std::vector<char> Load_rom(std::string filename) {

	std::ifstream rom(filename, std::ios::binary);

	//Size of the file
	std::streampos size;
	rom.seekg(0, std::ios::end);
	size = rom.tellg();

	std::vector<char> rom_buf((int)size);

	rom.seekg(0, std::ios::beg);
	rom.read(&rom_buf[0], size);

	return rom_buf;
}

int main(int argc, char* argv[]) {
	std::cout << "Welcome to danndy8, a CHIP8 emulator" << std::endl;
	/*
	if (argc < 2) {
		std::cerr << "Please, specify a ROM to open." << std::endl;
	}
	else {
		std::vector<char> rom = Load_rom(argv[1]);
	}*/

	std::vector<char> rom = Load_rom("PONG");

	Cpu chip8(rom);
	//chip8.Init(rom);

	for (int i = 0; i < 40; i++) {
		chip8.Get_Opcode();
	}


	return 0;
}