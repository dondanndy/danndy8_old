#include "cpu.h"

Cpu::Cpu(std::vector<char>& rom) {
		//Let's set the registers.
		pc = 0x200;
		V = { 0 };
		I = 0;
		memory = { 0 };

		gfx = { 0 };
		draw_screen = false;

		stack = { 0 };
		stack_pointer = 0;

		delay_timer = 0;
		sound_timer = 0;

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
	
	//std::cout << "opcode: " << std::hex << opcode << std::endl;
	//std::cout << "pc: " << std::hex << pc << std::endl;

	//Once we have the opcode and the data associated whith it, let's execute it.
	switch (opcode) {
	case 0x0:
		switch (data)	{
		case 0xE0:        
			op_00E0(data);
			break;

		case 0xEE:     
			op_00EE(data);
			break;
		}
		break;

	case 0x1:
		op_1NNN(data);
		break;

	case 0x2:
		op_2NNN(data);
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

	case 0xB:
		op_BNNN(data);
		break;

	case 0xC:
		op_CXNN(data);
		break;

	case 0xD:
		op_DXYN(data);
		break;

	case 0xE:
		op_EXNN(data);
		break;

	case 0xF:
		op_FXNN(data);
		break;

	default:
		std::cout << "Opcode " << std::hex << opcode << " not implemented yet :(" << std::endl;
		//std::cout << "Data: " << std::hex << data << std::endl;
		pc += 2;
	};
}

void Cpu::op_00E0(unsigned short data) {
	gfx = { 0 };

	pc += 2;
}

void Cpu::op_00EE(unsigned short data) {
	stack_pointer--;

	pc = stack[stack_pointer];

	pc += 2;
}

void Cpu::op_1NNN(unsigned short data) {
	pc = data;
}

void Cpu::op_2NNN(unsigned short data) {
	stack[stack_pointer] = pc;

	stack_pointer++;

	pc = data;
}

void Cpu::op_3XNN(unsigned short data) {
	unsigned short reg1 = data & 0xF00 >> 8;
	unsigned short reg2 = data & 0xFF;

	if (V[reg1] == reg2) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void Cpu::op_4XNN(unsigned short data) {
	unsigned short reg1 = data & 0xF00 >> 8;
	unsigned short reg2 = data & 0xFF;


	if (V[reg1] != reg2) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void Cpu::op_5XY0(unsigned short data) {
	unsigned short reg1 = data & 0xF00 >> 8;
	unsigned short reg2 = data & 0xF0 >> 4;

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
	case 0x0:
		V[reg1] = V[reg2];
		break;

	case 0x1:
		V[reg1] = V[reg1] | V[reg2];
		break;

	case 0x2:
		V[reg1] = V[reg1] & V[reg2];
		break;

	case 0x3:
		V[reg1] = V[reg1] ^ V[reg2];
		break;

	case 0x4:
		if (V[reg2] > (0xFF - V[reg1])) {
			V[0xF] = 1; //carry
		} else{
			V[0xF] = 0;
		}

		V[reg1] += V[reg2];
		break;

	case 0x5:
		if (V[reg2] < V[reg1]) {
			V[0xF] = 1;
		}
		else {
				V[0xF] = 0;
		};

		V[reg1] -= V[reg2];
		break;

	case 0x6:
		V[reg1] = V[reg2] >> 1;
		break;

	case 0x7:
		if (V[reg2] < V[reg1]) {
			V[0xF] = 1;
		}
		else {
			V[0xF] = 0;
		};

		V[reg1] = V[reg2] - V[reg1];
		break;

	case 0xE:
		V[reg1] = V[reg2] << 1;
		break;
	}

	pc += 2;
}

void Cpu::op_9XY0(unsigned short data) {
	unsigned char reg1 = data & 0xF00 >> 8;
	unsigned char reg2 = data & 0xF0 >> 4;

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
	unsigned char reg1 = data & 0xF00 >> 8;
	unsigned char reg2 = data & 0xFF;

	//Random number generator.
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);
	int rand = dist(rng);

	V[reg1] = rand & reg2;

	pc += 2;
}

void Cpu::op_DXYN(unsigned short data) {
	unsigned short x = V[(data & 0xF00) >> 8];
	unsigned short y = V[(data & 0xF0) >> 4];
	unsigned short height = data & 0xF;
	unsigned short pixel;

	V[0xF] = 0;
	for (int yline = 0; yline < height; yline++) {
		pixel = memory[I + yline];
		for (int xline = 0; xline < 8; xline++) {
			if (pixel &(0x80 >> xline) != 0) {
				if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
					V[0xF] = 1;
				}
				gfx[(x + xline + ((y + yline) * 64))] ^= 1;
			}
		}
	}

	drawFlag = true;
	
	pc += 2;
}

void Cpu::op_EXNN(unsigned short data) {
	unsigned char selector = data & 0xFF;
	unsigned char reg = (data & 0xF00) >> 8;

	switch (selector) {
	case 0x9E: {
		if (keys[V[reg]]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	case 0xA1: {
		if (!keys[V[reg]]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	}
}

void Cpu::op_FXNN(unsigned short data) {
	unsigned char selector = data & 0xFF;
	unsigned char reg = (data & 0xF00) >> 8;

	switch (selector)
	{
	case 0x07:
		V[reg] = delay_timer;
		break;

	/*case 0x0A:
		V[reg] = delay_timer;
		break;*/

	case 0x15:
		delay_timer = V[reg];
		break;

	case 0x18:
		sound_timer = V[reg];
		break;

	case 0x1E:
		I += V[reg];
		break;

	case 0x33:
		reg = (data & 0xF00) >> 8;

		memory[I] = V[reg] / 100;
		memory[I + 1] = (V[reg] / 10) % 10;
		memory[I + 2] = (V[reg] % 100) % 10;

		break;

	default:
		std::cout << "Opcode: F" << data << " not implemented :(" << std::endl;
		break;
	}

	pc += 2;
}