#include <SDL2/SDL.h>
#include <verilated.h>
#include <VRAM.h>
#include <memory>
#include <iostream>
#include <cmath>
#include <algorithm>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    std::unique_ptr<VRAM> vram = std::make_unique<VRAM>();

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("VGA TEST",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    Uint8* pixels;
    int pitch;

    int8_t pixel[SCREEN_WIDTH * SCREEN_HEIGHT] = {};
    float t = 500.0f;
    bool use_back_buffer = true;

    bool running = true;
    while (running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        uint32_t gpu_base = use_back_buffer ? 307200 : 0;
        for(int addr = 0; addr < SCREEN_WIDTH * SCREEN_HEIGHT; addr++) {
            int x = addr % SCREEN_WIDTH;
            int y = addr / SCREEN_WIDTH;

            float fx = static_cast<float>(x) / (SCREEN_WIDTH - 1);
            float fy = static_cast<float>(y) / (SCREEN_HEIGHT - 1);

            float red_weight = (1.0f - fx) * (fy + 0.5f * cosf(t + fx * 3.0f));
            float green_weight = fx * (fy + 0.5f * cosf(t + fy * 3.0f));
            float blue_weight = (1.0f - fy) * (fx + 0.5f * cosf(t + fy * 3.0f));


            uint8_t r = static_cast<uint8_t>(std::clamp(red_weight, 0.0f, 1.0f) * 7.0f) & 0x07;
            uint8_t g = static_cast<uint8_t>(std::clamp(green_weight, 0.0f, 1.0f) * 7.0f) & 0x07;
            uint8_t b = static_cast<uint8_t>(std::clamp(blue_weight, 0.0f, 1.0f) * 3.0f) & 0x03;

            uint8_t color = (r << 5) | (g << 2) | b;

            vram->gpu_addr = gpu_base + addr;
            vram->gpu_data_in = color;
            vram->wr_en = 1;

            vram->clk_gpu = 1; vram->eval();
            vram->clk_gpu = 0; vram->eval();
        }
        vram->wr_en = 0;

        uint32_t vga_base = use_back_buffer ? 0 : 307200;
        for (int addr = 0; addr < SCREEN_WIDTH * SCREEN_HEIGHT; addr++) {
            vram->vga_addr = vga_base + addr;

            vram->clk_vga = 1; vram->eval();
            vram->clk_vga = 0; vram->eval();

            pixel[addr] = vram->vga_pixel_out;
        }
        
        use_back_buffer = !use_back_buffer;

        SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                int addr = y * SCREEN_WIDTH + x;
                pixels[y * pitch + x] = pixel[addr];
            }
        }
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
        t += 0.05f;
    }

    SDL_Quit();

    return 0;
}