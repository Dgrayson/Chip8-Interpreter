#include "pch.h"
#include "Chip8.h"
#include <random>
#include <SDL.h>
#include <iostream>
#include <ctime>


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

	for(int i = 0; i <= 2048; i++)
	{
		gfx[i] = 0;
	}

	for(int i = 0; i < 16; i++)
	{
		stack[i] = 0;
		key[i] = 0;
		V[i] = 0; 
	}

	for(int i = 0; i < 4096; i++)
	{
		memory[i] = 0; 
	}

	for (int i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[i]; 
	}

	srand(time(NULL)); 
}
void Chip8::emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1]; 

	//std::cout << opcode << std::endl; 

	// Decode opcode
	// Execute opcode


	switch (opcode & 0xF000) {

	case 0x0000:
	{
		{
			switch (opcode & 0x000F) {
				/*Clear the display.*/
			case 0x0000: // 0x000E
				for (int i = 0; i < 2048; i++) {
					gfx[i] = 0;
				}
				drawFlag = true;
				pc += 2;
				break;
				/*Return from a subroutine.

				The interpreter sets the program counter to the address at the top of the stack,
				then subtracts 1 from the stack pointer.*/
			case 0x000E:
				// Potential issue
				sp--; 
				pc = stack[sp];
				pc += 2; 
				break;
			default:
				std::cout << "Uknown opcode" << opcode;
				exit(3); 
				
			}
		}
	}
	break;

	/*Jump to location nnn.

	The interpreter sets the program counter to nnn.*/
	case 0x1000:
		pc = opcode & 0x0FFF;
		break;
	/*Call subroutine at nnn.

	The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.*/
	case 0x2000:
		// Potenial issue
		
		stack[sp] = pc;
		sp++; 
		pc = opcode & 0x0FFF;
		break;
	/*Skip next instruction if Vx = kk.

	The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.*/
	case 0x3000:

		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
			pc += 4;
		}
		else
			pc += 2; 
		break;
	/*Skip next instruction if Vx != kk.

	The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.*/
	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
			pc += 4;
		}
		else
			pc += 2; 
		break;
	/*Skip next instruction if Vx = Vy.

	The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.*/
	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[opcode & 0x00F0] >> 4) {
			pc += 4;
		}
		else
			pc += 2; 
		break;
	/*Set Vx = kk.

	The interpreter puts the value kk into register Vx.*/
	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2;
		break;
	/*Set Vx = Vx + kk.

	Adds the value kk to the value of register Vx, then stores the result in Vx. */
	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;
		break;
	case 0x8000:
		switch (opcode & 0x000F) {
		/*Set Vx = Vy.

		Stores the value of register Vy in register Vx.*/
		case 0x0000: //8xy0
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		/*Set Vx = Vx OR Vy.

		Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
		A bitwise OR compares the corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. 
		Otherwise, it is 0. */
		case 0x0001:

			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break; 

		/*Set Vx = Vx AND Vy.

		Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. 
		A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. 
		Otherwise, it is 0. */
		case 0x0002: 
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break; 

		/*Set Vx = Vx XOR Vy.

		Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. 
		An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. 
		Otherwise, it is 0. */
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		/*Set Vx = Vx + Vy, set VF = carry.

		The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
		Only the lowest 8 bits of the result are kept, and stored in Vx.*/
		case 0x0004:
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; 
			else
				V[0xF] = 0; 

			
			pc += 2; 
			break;
		
		/*Set Vx = Vx - Vy, set VF = NOT borrow.

		If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.*/
		case 0x0005:
			if (V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 0;
			else
				V[0xF] = 1;

			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] - V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		/*Set Vx = Vx SHR 1.

		If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.*/
		case 0x0006: 

			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;

			V[(opcode & 0x0F00) >> 8] >>= 1; 

			pc += 2;
			break; 

		/*Set Vx = Vy - Vx, set VF = NOT borrow.

		If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.*/
		case 0x0007:

			if (V[(opcode & 0x0F00) >> 8] >> V[(opcode & 0x00F0) >> 4])
				V[0XF] = 0;
			else
				V[0XF] = 1;

			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]; 
			pc += 2;
			break;
		case 0x000E:
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7; 
			V[(opcode & 0x0F00) >> 8] <<= 1; 
			pc += 2; 
			break;
		}

		break;
	/*Skip next instruction if Vx != Vy.

	The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.*/
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2; 

		break;
	/*Set I = nnn.

	The value of register I is set to nnn.*/
	case 0xA000:
		I = opcode & 0x0FFF;
		pc += 2;
		break;
	/*Jump to location nnn + V0.

	The program counter is set to nnn plus the value of V0.*/
	case 0xB000:
		pc = (opcode & 0x0FFF) + V[0]; 
		break;

	/*Set Vx = random byte AND kk.

	The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. 
	The results are stored in Vx. See instruction 8xy2 for more information on AND.*/
	case 0XC000: {
		// Random number from 0 to 255
		int num = rand() % 255;

		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & num;
		pc += 2;
	}
		break; 
	/*Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.*/
	case 0xD000: 
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;

		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						V[0xF] = 1;
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
	}
	break; 
	case 0xE000: 

		switch (opcode & 0x00FF) {
		/*Skip next instruction if key with the value of Vx is pressed.

		Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.*/
		case 0x009E: 
			if (key[V[(opcode & 0x0F00) >> 8]] != 0)
				pc += 4;
			else
				pc += 2; 
		/*Skip next instruction if key with the value of Vx is not pressed.

		Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.*/
		case 0x00A1: 
			if (key[V[(opcode & 0x0F00) >> 8]] == 0)
				pc += 4;
			else
				pc += 2;
			break; 
		}
		
		break;
	case 0xF000: 

		switch (opcode & 0x00FF) {
		/*Set Vx = delay timer value.

		The value of DT is placed into Vx.*/
		case 0x0007: 
			V[(opcode & 0x0F00) >> 8] = delay_timer; 
			pc += 2; 
			break; 
		case 0x000A: {
			/*Wait for a key press, store the value of the key in Vx.

			All execution stops until a key is pressed, then the value of that key is stored in Vx.*/
			bool key_pressed = false;

			for (int i = 0; i <= 15; i++) {
				if (key[i] != 0)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					key_pressed = true;
				}
			}

			if (!key_pressed)
				return;

			pc += 2;
		}
		break; 
		case 0x0015: 
			/*Set delay timer = Vx.

			DT is set equal to the value of Vx.*/
			delay_timer = V[(opcode & 0x0F00) >> 8]; 
			pc += 2; 
			break;
		case 0x0018:
			/*Set sound timer = Vx.

			ST is set equal to the value of Vx.*/
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2; 
			break; 
		case 0x001E: 
			/*Set I = I + Vx.

			The values of I and Vx are added, and the results are stored in I.*/

			if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
				V[0xF] = 1;
			else
				V[0xF] = 0;

			I += V[(opcode & 0x0F00) >> 8];
			pc += 2; 
			break; 
		case 0x0029: 
			
			I = V[(opcode & 0x0F00) >> 8] * 0x5; 
			pc += 2; 
			break;
		case 0x0033: 
			// Store BCD representation of Vx in memory locations I, I+1, and I+2.
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100; 
			memory[I] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I] = V[(opcode & 0x0F00) >> 8] % 10;
			pc += 2; 
			break; 
		case 0x0055: {
			/*Store registers V0 thasdfzxcvxrough Vx in memory starting at location I.

			The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I*/
			for (int x = 0; x <= ((opcode & 0x0F00) >> 8); x++) {
				memory[I + x] = V[x];
			}
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
		}
		break; 

		case 0x0065: {
			/*Read registers V0 through Vx from memory starting at location I.

			The interpreter reads values from memory starting at location I into registers V0 through Vx.*/
			for (int x = 0; x <= ((opcode & 0x0F00) >> 8); x++) {
				V[x] = memory[I + x];
			}

			I += ((opcode & 0x0F00) >> 8) + 1; 
			pc += 2;
		}
			break;

		default:
			std::cout << "Unknown opcode" << std::endl; 
			break; 
		}
		break; 
	default:
		std::cout << "Unknown opcode" << std::endl; 
		exit(3); 
	}
	//update timers

	if (delay_timer > 0)
		--delay_timer; 

	if (sound_timer > 0) {
		--sound_timer;
	}
}

void Chip8::LoadGame(char* fileName)
{
	std::cout << "Loading ROM..." << fileName << std::endl;
	FILE* rom;
	fopen_s(&rom, fileName, "rb");

	if (rom == nullptr)
		std::cout << "Failed to open rom" << std::endl; 

	fseek(rom, 0, SEEK_END);

	long size = ftell(rom);

	rewind(rom); 

	char* buffer = (char*)malloc(sizeof(char) * size);

	fread(buffer, sizeof(char), size, rom);

	for (int i = 0; i < size; i++)
	{
		memory[i + 512] = buffer[i];
	}

	fclose(rom);
	free(buffer);

	std::cout << "File Loaded" << std::endl;
	
}

void Chip8::SetKeys(SDL_Keycode sym, int eventType)
{

	std::cout << "keyPressed" << sym << std::endl; 
	switch(sym)
	{
	case SDLK_1:
		key[0] = eventType; 
		break;
	case SDLK_2:
		key[1] = eventType;
		break;
	case SDLK_3:
		key[2] = eventType;
		break;
	case SDLK_4:
		key[3] = eventType;
		break;
	case SDLK_q: 
		key[4] = eventType;
		break;
	case SDLK_w:
		key[5] = eventType;
		break;
	case SDLK_e: 
		key[6] = eventType;
		break;
	case SDLK_r:
		key[7] = eventType;
		break;
	case SDLK_a:
		key[8] = eventType;
		break;
	case SDLK_s:
		key[9] = eventType;
		break;
	case SDLK_d:
		key[10] = eventType;
		break;
	case SDLK_f:
		key[11] = eventType;
		break;
	case SDLK_z:
		key[12] = eventType;
		break;
	case SDLK_x:
		key[13] = eventType;
		break;
	case SDLK_c:
		key[14] = eventType;
		break;
	case SDLK_v:
		key[14] = eventType;
		break;
	default:
		// do nothing
		break;
	}

}

