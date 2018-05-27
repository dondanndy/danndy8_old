#include "cpu.h"
#include <iostream>

Cpu::Cpu(std::vector<char>& rom) {
		//Let's set the registers.
		pc = 0x200;
		V = { 0 };
		I = 0;
		memory = { 0 };

		//We load the ROM.
		for (int i = 0; i < (int)rom.size(); i++) {
			memory[i + 512] = rom[i];
		};

		//We load the fontset.
		for (int i = 0; i < 80; ++i) {
			memory[i] = fontset[i];
		};
}

unsigned short Cpu::Get_Opcode() {
	unsigned short opcode;
	opcode = memory[pc] << 8 | memory[pc + 1];

	Decode_Opcode(opcode);

	return opcode;
}

void Cpu::Decode_Opcode(unsigned short addr) {
	unsigned short opcode;
	unsigned short data;


	opcode = (addr & 0xF000) >> 12;
	data = (addr & 0x0FFF);
	
	//std::cout << std::hex << opcode << std::endl;
	//std::cout << std::hex << data << std::endl;

	//Once we have the opcode and the data associated whith it, lets execute it.
	switch (opcode) {
	case 0x1:
		op_1NNN(data);
		break;

	case 0x3:
		op_3XNN(data);
		break;

	case 0x4:
		op_4XNN(data);
		break;

	case 0x5:
		op_5XY0(data);
		break;

	case 0x6:
		op_6XNN(data);
		break;

	case 0x7:
		op_7XNN(data);
		break;

	case 0x9:
		op_9XY0(data);
		break;

	case 0xA:
		op_ANNN(data);
		break;

	default:
		std::cout << "Opcode " << std::hex << opcode << " not implemented yet :(" << std::endl;
		pc += 2;
	};
}

void Cpu::op_1NNN(unsigned short data) {
	pc = data;
}

void Cpu::op_2NNN(unsigned short data) {
	//TODO
}

void Cpu::op_3XNN(unsigned short data) {
	unsigned char num1 = data & 0xF00 >> 4;
	unsigned char num2 = data & 0xFF;

	if (V[num1] == num2) {
		pc += 4;
	}

}

void Cpu::op_4XNN(unsigned short data) {
	unsigned char num1 = data & 0xF00 >> 4;
	unsigned char num2 = data & 0xFF;

	if (V[num1] != num2) {
		pc += 4;
	}

}

void Cpu::op_5XY0(unsigned short data) {
	unsigned char num1 = data & 0xF00 >> 4;
	unsigned char num2 = data & 0xF0 >> 2;

	if (V[num1] == V[num2]) {
		pc += 4;
	}

}

void Cpu::op_6XNN(unsigned short data) {
	unsigned short num1 = (data & 0xF00) >> 8;
	unsigned short num2 = data & 0xFF;

	V[num1] = num2;
		
	pc += 2;
}

void Cpu::op_7XNN(unsigned short data) {
	unsigned short num1 = (data & 0xF00) >> 8;
	unsigned short num2 = data & 0xFF;

	V[num1] += num2;

	pc += 2;
}

void Cpu::op_ANNN(unsigned short data) {
	I = data;

	pc += 2;
}

void Cpu::op_9XY0(unsigned short data) {
	unsigned char num1 = data & 0xF00 >> 4;
	unsigned char num2 = data & 0xF0 >> 2;

	if (V[num1] != V[num2]) {
		pc += 4;
	}

}
