#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Timers.h"
#include "Graphics.h"
#include "Controller.h"

#define CYCLES_PER_FRAME 70224
#define TIME_PER_FRAME 1.0 / 60.0

class GB {

private:

  int currentCycles;
  Controller controller;
  Memory MMU;
  CPU Cpu;
  Timers timers;
  Graphics graphics;
  bool CGBMode;
  bool framerateUnlocked;

public:

  GB(const char* rom_fname);
  void Run();
  void SetCGBMode(bool val);
  bool CGBModeEnabled();
  void ToggleFrameLimit();

private:

  void AdvanceFrame();

};