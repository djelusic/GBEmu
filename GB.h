#pragma once
#include "CPU.h"
#include "Memory.h"
#include "Timers.h"
#include "Graphics.h"
#include "Controller.h"
#include "Serializer.h"
#include "Disassembler.h"
#include "Debugger.h"

#define CYCLES_PER_FRAME 70224
#define TIME_PER_FRAME 1.0 / 60.0

class GB {

private:

  const char * rom_fpath;
  std::string ROMName;

  std::function<void(std::string)> debugLogCallback;

  bool CGBMode;
  bool doubleSpeed;
  int currentCycles;

  Controller controller;
  Memory MMU;
  CPU Cpu;
  Timers timers;
  Graphics graphics;
  Serializer serializer;
  Disassembler disassembler;
  Debugger debugger;

public:

  GB(const char* rom_fpath);
  void AdvanceFrame();
  void SetCGBMode(bool val);
  bool CGBModeEnabled();
  void SetDoubleSpeed(bool val);
  bool IsDoubleSpeed();

  std::string GetROMName();
  const char * GetROMPath();

  void Reset();
  void LoadROM(const char * rom_fpath);

  void Serialize(Serializer & s);
  void Deserialize(Serializer & s);

  void SaveState();
  void LoadState();

  void DebugLog(std::string s);

  Graphics* GetGraphics();
  CPU* GetCPU();
  Memory* GetMMU();
  Controller* GetInput();
  Debugger* GetDebugger();

};