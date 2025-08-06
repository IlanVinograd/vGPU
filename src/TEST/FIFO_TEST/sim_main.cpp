#include <verilated.h>
#include <VFIFO.h>
#include <memory>
#include <iostream>
#include <iomanip>

vluint64_t main_time = 0;
double sc_time_stamp() { return main_time; }

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    auto fifo = std::make_unique<VFIFO>();

    fifo->wclk = 0;
    fifo->rclk = 0;
    fifo->wrst_n = 0;
    fifo->rrst_n = 0;
    fifo->winc = 0;
    fifo->rinc = 0;
    fifo->wdata = 0;

    fifo->eval();
    main_time++;

    for (int i = 0; i < 5; ++i) {
        fifo->wclk = !fifo->wclk;
        fifo->rclk = !fifo->rclk;
        fifo->eval();
        main_time++;
    }
    fifo->wrst_n = 1;
    fifo->rrst_n = 1;

    const int wclk_period = 1;
    const int rclk_period = 4;
    int wclk_cnt = 0, rclk_cnt = 0;

    int data_to_write = 0;

    bool prev_rclk = fifo->rclk;
    bool prev_wclk = fifo->wclk;

    for (int cycle = 0; cycle < 1000000; ++cycle) {
        if (wclk_cnt++ >= wclk_period / 2) {
            fifo->wclk = !fifo->wclk;
            wclk_cnt = 0;
        }

        if (rclk_cnt++ >= rclk_period / 2) {
            fifo->rclk = !fifo->rclk;
            rclk_cnt = 0;
        }

        bool w_posedge = (!prev_wclk && fifo->wclk);
        if (!fifo->wfull) {
            fifo->winc = 1;
            fifo->wdata = data_to_write;
        } else {
            fifo->winc = 0;
        }

        if (w_posedge && fifo->winc) {
            std::cout << "[WRITE] " << std::setw(2) << data_to_write << std::endl;
            data_to_write++;
        }

        bool r_posedge = (!prev_rclk && fifo->rclk);
        fifo->rinc = !fifo->rempty;

        if (r_posedge && fifo->rinc) {
            std::cout << "        [READ]  " << std::setw(2) << (int)fifo->rdata << std::endl;
        }

        fifo->eval();
        main_time++;

        prev_rclk = fifo->rclk;
        prev_wclk = fifo->wclk;
    }

    fifo->final();
    return 0;
}