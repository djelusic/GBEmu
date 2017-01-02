#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Timers.h"
#include "Graphics.h"
#include "Controller.h"

#define CYCLES_PER_FRAME 69905
#define TIME_PER_FRAME 1.0 / 60.0

class GB {

private:

  int currentCycles;
  Controller controller;
  CPU Cpu;
  Memory MMU;
  Timers timers;
  Graphics graphics;

public:

  GB(const char* rom_fname);
  void Run();

private:

  void AdvanceFrame();

};