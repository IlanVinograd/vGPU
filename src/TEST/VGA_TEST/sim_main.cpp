#include <SDL2/SDL.h>
#include <verilated.h>
#include <VRAM.h>
#include <memory>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
   
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    std::unique_ptr<VRAM> vram = std::make_unique<VRAM>();

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "ERROR: " << SDL_GetError(); 
    } else {
        SDL_Window* window = SDL_CreateWindow("VGA TEST",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        SDL_Delay(5000);
    }

    SDL_Quit();

    return 0;
}