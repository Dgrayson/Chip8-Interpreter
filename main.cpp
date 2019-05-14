// Chip8Emu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Chip8.h"
#include <iostream>
#include <SDL.h>
#include <chrono>
#include <thread>

#define fps 60

Chip8 chip8; 

int main(int argc, char **argv)
{
	chip8 = Chip8();

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = nullptr;

	window = SDL_CreateWindow("Chip-8 Emu", 
								SDL_WINDOWPOS_UNDEFINED, 
								SDL_WINDOWPOS_UNDEFINED, 
								1024, 512, 
								SDL_WINDOW_RESIZABLE);

	if (window == NULL) {
		std::cout << "Error Initializing Screen: " << SDL_GetError << std::endl; 
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, 1024, 512);

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	uint32_t pixels[2048];

	for(int i = 0; i < 2048; i++)
	{
		pixels[i] = 0; 
	}
	
	chip8.init(); 
	chip8.LoadGame(argv[1]); 
	

	Uint32 start_tick;
	SDL_Event event; 
	bool running = true;

	while(running){

		chip8.emulateCycle();

		start_tick = SDL_GetTicks();



		while(SDL_PollEvent(&event))
		{

			switch(event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:

				chip8.SetKeys(event.key.keysym.sym, 1); 

				break;
			case SDL_KEYUP:
				chip8.SetKeys(event.key.keysym.sym, 0);
				break;
			}
		}
		

		if (chip8.drawFlag)
		{
			chip8.drawFlag = false; 
			for(int  i = 0; i < 2048; i++) 
			{
				uint8_t pixel = chip8.gfx[i];


				pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
			}

			SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(Uint32));
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer); 
		}

		if((1000 / fps) > SDL_GetTicks() - start_tick)
		{
			SDL_Delay(1000 / fps - (SDL_GetTicks() - start_tick)); 
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer); 
	SDL_DestroyWindow(window);

	SDL_Quit(); 

	return 0; 
}

void drawGraphics() {

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
