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

    for(int addr = 0; addr < SCREEN_WIDTH * SCREEN_HEIGHT; addr++) {
        int x = addr % SCREEN_WIDTH;
        int y = addr / SCREEN_WIDTH;

        float fx = (float)x / (SCREEN_WIDTH - 1);
        float fy = (float)y / (SCREEN_HEIGHT - 1);

        float red_weight   = (1.0f - fx) * fy;
        float green_weight = fx * fy;
        float blue_weight  = 1.0f - fy;

        uint8_t r = (uint8_t)(red_weight   * 7.0f) & 0x07;
        uint8_t g = (uint8_t)(green_weight * 7.0f) & 0x07;
        uint8_t b = (uint8_t)(blue_weight  * 3.0f) & 0x03;

        uint8_t color = (r << 5) | (g << 2) | b;

        vram->gpu_addr = addr;
        vram->gpu_data_in = color;
        vram->wr_en = 1;

        vram->clk_gpu = 1; vram->eval();
        vram->clk_gpu = 0; vram->eval();
    }
    vram->wr_en = 0;

    int8_t pixel[SCREEN_WIDTH*SCREEN_HEIGHT] = {};
    for(int addr = 0; addr < SCREEN_WIDTH * SCREEN_HEIGHT; addr++) {
        vram->vga_addr = addr;
        
        vram->clk_vga = 1; vram->eval();
        vram->clk_vga = 0; vram->eval();

        pixel[addr] = vram->vga_pixel_out;
    }

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