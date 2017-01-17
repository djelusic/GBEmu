#include "GB.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <functional>

GB::GB(const char* rom_fname) :
  controller(this),
  MMU(this, rom_fname, controller),
  Cpu(this, MMU, controller),
  timers(Cpu, MMU),
  graphics(this, MMU, Cpu),
  sdl(this, graphics, controller, MMU),
  framerateUnlocked(false),
  doubleSpeed(false) {
  Cpu.SetInputCallback([this](void) -> bool {return sdl.HandleInput();});
  graphics.SetVblankCallback([this](void) -> void {sdl.RenderScreen();});
}

void GB::AdvanceFrame() {
  currentCycles = 0;
  while (currentCycles < CYCLES_PER_FRAME) {
    int cycles = Cpu.Advance();
    timers.Update(cycles);
    if (doubleSpeed) cycles /= 2;
    graphics.Update(cycles);
    currentCycles += cycles;
  }
  Cpu.HandleInput();
}

void GB::Run() {
  while (true) {
    sdl.HandleEvents();
    clock_t begin = clock();
    AdvanceFrame();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    int remainingTime = (int)((TIME_PER_FRAME - elapsedSecs) * 1000);
    // std::cout << "Remaining time: " << remainingTime << std::endl;
    if (remainingTime < 0) remainingTime = 0;
    if (!framerateUnlocked) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(remainingTime)
      );
    }
  }
}

void GB::SetCGBMode(bool val) {
  CGBMode = val;
}

bool GB::CGBModeEnabled() {
  return CGBMode;
}

void GB::ToggleFrameLimit() {
  framerateUnlocked = !framerateUnlocked;
}

void GB::SetDoubleSpeed(bool val) {
  doubleSpeed = val;
}

bool GB::IsDoubleSpeed() {
  return doubleSpeed;
}
