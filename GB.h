#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Timers.h"
#include "Graphics.h"
#include "Controller.h"
#include "SDL.h"
#include "Serializer.h"
#include "Disassembler.h"
#include "Debugger.h"

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
  Serializer serializer;
  Disassembler disassembler;
  Debugger debugger;
  SDL sdl;
  bool CGBMode;
  bool framerateUnlocked;
  bool doubleSpeed;
  int fps;
  std::string ROMName;
  std::function<void(std::string)> debugLogCallback;

public:

  GB(const char* rom_fpath);
  void Run();
  void SetCGBMode(bool val);
  bool CGBModeEnabled();
  void ToggleFrameLimit();
  void SetDoubleSpeed(bool val);
  bool IsDoubleSpeed();

  std::string GetROMName();

  void Serialize(Serializer & s);
  void Deserialize(Serializer & s);

  void SaveState();
  void LoadState();

  void DebugLog(std::string s);

private:

  void AdvanceFrame();

};