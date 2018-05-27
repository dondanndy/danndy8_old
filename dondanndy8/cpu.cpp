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

	//Once we have the opcode and the data associated whith it, let's execute it.
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

	case 0x8:
		op_8XYN(data);
		break;

	case 0x9:
		op_9XY0(data);
		break;

	case 0xA:
		op_ANNN(data);
		break;

	default:
		std::cout << "Opcode " << std::hex << opcode << " not implemented yet :(" << std::endl;
		//std::cout << "Data: " << std::hex << data << std::endl;
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
	unsigned short reg1 = data & 0xF00;
	unsigned short reg2 = data & 0xFF;

	if (V[reg1] == reg2) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void Cpu::op_4XNN(unsigned short data) {
	unsigned short reg1 = data & 0xF00 >> 4;
	unsigned short reg2 = data & 0xFF;


	if (V[reg1] != reg2) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void Cpu::op_5XY0(unsigned short data) {
	unsigned short reg1 = data & 0xF00 >> 4;
	unsigned short reg2 = data & 0xF0 >> 2;

	if (V[reg1] == V[reg2]) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void Cpu::op_6XNN(unsigned short data) {
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = data & 0xFF;

	V[reg1] = reg2;
		
	pc += 2;
}

void Cpu::op_7XNN(unsigned short data) {
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = data & 0xFF;

	V[reg1] += reg2;

	pc += 2;
}

void Cpu::op_8XYN(unsigned short data) {
	//First we have to get the lasts two bits:
	unsigned short selector = (data & 0xF);

	//And the data for choosing the registers
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = data & 0xF0 >> 4;

	switch (selector) {
	case 0x0: {
		V[reg1] = V[reg2];
		break;
	};
	case 0x1: {
		V[reg1] = V[reg1] | V[reg2];
		break;
	};
	case 0x2: {
		V[reg1] = V[reg1] & V[reg2];
		break;
	};
	case 0x3: {
		V[reg1] = V[reg1] ^ V[reg2];
		break;
	};
	case 0x4: {		
		V[reg1] += V[reg2];
		break;
	};
	case 0x5: {
		V[reg1] -= V[reg2];
		break;
	};
	case 0x6: {
		V[reg1] = V[reg2] >> 1;
		break;
	};
	case 0x7: {
		V[reg1] = V[reg2] - V[reg1];
		break;
	};
	case 0xE: {
		V[reg1] = V[reg2] << 1;
		break;
	};
		
	}

	pc += 2;
}

void Cpu::op_9XY0(unsigned short data) {
	unsigned char reg1 = data & 0xF00 >> 4;
	unsigned char reg2 = data & 0xF0 >> 2;

	if (V[reg1] != V[reg2]) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void Cpu::op_ANNN(unsigned short data) {
	I = data;

	pc += 2;
}

void Cpu::op_BNNN(unsigned short data) {
	pc = V[0] + data;
}

void Cpu::op_CXNN(unsigned short data) {
	unsigned char reg1 = data & 0xF00 >> 4;
	unsigned char reg2 = data & 0xFF;

	//Random number generator.
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);
	int rand = dist(rng);

	V[reg1] = rand& reg2;

	pc += 2;
}