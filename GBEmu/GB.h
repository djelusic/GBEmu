#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Timers.h"
#include "Graphics.h"

class GB {

private:

  static const int CYCLES_PER_FRAME = 69905;
  static const double TIME_PER_FRAME = 1.0 / 60.0;
  int currentCycles;
  Timers timers;
  Memory MMU;
  CPU Cpu;
  Graphics graphics;

public:

  GB(const char* rom_fname);
  void Run();

private:

  void AdvanceFrame();

};