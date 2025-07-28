#include <iostream>
#include <verilated.h>
#include <VRAM.h>
#include <memory>
#include <vector>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    std::unique_ptr<VRAM> vram = std::make_unique<VRAM>();

    const int TEST_SIZE = 100000;
    std::vector<uint8_t> test_data(TEST_SIZE);

    for (int i = 0; i < TEST_SIZE; i++) {
        test_data[i] = (i * 17) & 0xFF;
    }

    std::cout << "=== TEST 1: GPU Write ===\n";
    for (int addr = 0; addr < TEST_SIZE; addr++) {
        vram->gpu_addr = addr;
        vram->gpu_data_in = test_data[addr];
        vram->wr_en = 1;

        vram->clk_gpu = 1; vram->eval();
        vram->clk_gpu = 0; vram->eval();
    }
    vram->wr_en = 0;

    std::cout << "=== TEST 2: VGA Read ===\n";
    bool all_passed = true;
    for (int addr = 0; addr < TEST_SIZE; addr++) {
        vram->vga_addr = addr;

        vram->clk_vga = 1; vram->eval();
        vram->clk_vga = 0; vram->eval();

        uint8_t read_val = vram->vga_pixel_out;
        std::cout << "Addr " << addr
                  << " | Expected: " << (int)test_data[addr]
                  << " | Got: " << (int)read_val
                  << (read_val == test_data[addr] ? " [PASSED]\n" : " [FAILED]\n");

        if (read_val != test_data[addr]) {
            all_passed = false;
        }
    }

    std::cout << "\n=== RESULT: " << (all_passed ? "ALL TESTS PASSED" : "SOME TESTS FAILED") << "\n";

    return all_passed ? 0 : 1;
}