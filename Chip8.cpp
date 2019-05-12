#include "pch.h"
#include "Chip8.h"


Chip8::Chip8()
{
}


Chip8::~Chip8()
{
}

void Chip8::init() {

	pc = 0x200; 
	opcode = 0; 
	I = 0; 
	sp = 0; 

	for (int i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[i]; 
	}	
}
void Chip8::emulateCycle() {
	// Fetch opcode

	memory[pc] = 0xA2; 
	memory[pc + 1] = 0xF0;

	opcode = memory[pc] << 8 | memory[pc + 1]; 


	// Decode opcode
	// Execute opcode

	
	switch (opcode && 0xF000) {

		// CLS, RET or SYS
	case 0x0000:
		break;

		// JP addr
	case 0x1000:
		pc = opcode && 0x0FFF; 
		break;
		// Call addr
	case 0x2000:
		sp++; 
		stack[sp] = pc; 
		pc = opcode && 0x0FFF; 
		break;
		// SE vx, byte
	case 0x3000:
		break;
		// SNE Vx, byte
	case 0x4000:
		break;
		// SE Vx, Vy
	case 0x5000:
		break;
	case 0x6000:
		break;
	case 0x7000:
		break;
	case 0x8000:

		break;
	case 0x9000:

		break;
		// Store NNN into index register I
	case 0xA000:
		I = opcode & 0x0FFF;
		pc += 2;
		break;

	case 0xB000:

		break;

	case 0XC000: 
		break; 
	case 0xD000: 
		break; 
	case 0xE000: 
		break;
	case 0xF000: 
		break; 
	default: 
		printf("Unknown opcode: 0x%X\n", opcode); 
	}
	//update timers

	if (delay_timer > 0)
		--delay_timer; 

	if (sound_timer > 0) {
	
		if(sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}
}
