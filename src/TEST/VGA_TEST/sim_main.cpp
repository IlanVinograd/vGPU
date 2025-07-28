#include <SDL2/SDL.h>
#include <verilated.h>
#include <VRAM.h>
#include <memory>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// SDL глобальные указатели
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gTexture = nullptr;

// Инициализация SDL
bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    gWindow = SDL_CreateWindow("VGA Output",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!gWindow) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
    if (!gRenderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    gTexture = SDL_CreateTexture(gRenderer,
        SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!gTexture) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

// Очистка ресурсов
void close()
{
    SDL_DestroyTexture(gTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

// Главная функция
int main(int argc, char** argv) {
    std::cout << "[*] Starting VGA simulation...\n";

    Verilated::commandArgs(argc, argv);
    std::unique_ptr<VRAM> vram = std::make_unique<VRAM>();

    if (!init()) {
        std::cerr << "[!] SDL initialization failed.\n";
        return 1;
    }

    std::cout << "[*] Populating VRAM with pattern...\n";
    vram->clk_gpu = 0;
    vram->wr_en = 0;

    // Загрузка паттерна в VRAM
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            uint32_t addr = y * SCREEN_WIDTH + x;
            vram->gpu_addr = addr;
            vram->gpu_data_in = (x ^ y) & 0xFF;  // интересный паттерн
            vram->wr_en = 1;
            vram->clk_gpu = 1; vram->eval();
            vram->clk_gpu = 0; vram->eval();
        }
    }
    vram->wr_en = 0;

    std::cout << "[*] Reading back from VRAM to generate screen image...\n";

    // Чтение из VRAM и заполнение пикселей
    uint32_t* pixels = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            uint32_t addr = y * SCREEN_WIDTH + x;
            vram->vga_addr = addr;
            vram->clk_vga = 1; vram->eval();
            vram->clk_vga = 0; vram->eval();

            uint8_t p = vram->vga_pixel_out;
            pixels[y * SCREEN_WIDTH + x] = (p << 16) | (p << 8) | p; // grayscale
        }
    }

    SDL_UpdateTexture(gTexture, nullptr, pixels, SCREEN_WIDTH * sizeof(uint32_t));

    std::cout << "[*] Launching SDL window (press close to exit)...\n";

    // Event loop
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        SDL_RenderClear(gRenderer);
        SDL_RenderCopy(gRenderer, gTexture, nullptr, nullptr);
        SDL_RenderPresent(gRenderer);
        SDL_Delay(16); // ~60 FPS
    }

    delete[] pixels;
    close();
    std::cout << "[*] Simulation ended. Goodbye!\n";
    return 0;
}