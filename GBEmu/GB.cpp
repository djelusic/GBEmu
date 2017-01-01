#include "GB.h"
#include <ctime>
#include <chrono>
#include <thread>

GB::GB(const char* rom_fname) : 
  controller(),
  MMU(rom_fname, controller),
  Cpu(MMU, controller), 
  timers(Cpu, MMU),
  graphics(MMU, Cpu) {}

void GB::AdvanceFrame() {
  currentCycles = 0;
  while (currentCycles < CYCLES_PER_FRAME) {
    Cpu.HandleInterrupts();
    Cpu.HandleInput();
    int cycles = Cpu.Advance();
    timers.Update(cycles);
    graphics.Update(cycles);
    currentCycles += cycles;
  }
}

void GB::Run() {
  while (true) {
    graphics.HandleSDLEvents();
    clock_t begin = clock();
    AdvanceFrame();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    int remainingTime =  (int)((TIME_PER_FRAME - elapsedSecs) * 1000);
    // std::cout << elapsedSecs << std::endl;
    if (remainingTime < 0) remainingTime = 0;
    std::this_thread::sleep_for(
      std::chrono::milliseconds(remainingTime)
    );
  }
}