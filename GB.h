#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Timers.h"
#include "Graphics.h"
#include "Controller.h"
#include "SDL.h"
#include "Serializer.h"

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
  SDL sdl;
  Serializer serializer;
  bool CGBMode;
  bool framerateUnlocked;
  bool doubleSpeed;
  int fps;

public:

  GB(const char* rom_fname);
  void Run();
  void SetCGBMode(bool val);
  bool CGBModeEnabled();
  void ToggleFrameLimit();
  void SetDoubleSpeed(bool val);
  bool IsDoubleSpeed();

  void Serialize(Serializer & s);
  void Deserialize(Serializer & s);

  void SaveState(const char * fpath);
  void LoadState(const char * fpath);

private:

  void AdvanceFrame();

};