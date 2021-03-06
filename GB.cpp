#include "GB.h"
#include <ctime>
#include <functional>
#include <iostream>
#include <string>

GB::GB(const char* rom_fpath) :
  rom_fpath(rom_fpath),
  controller(this),
  MMU(this, controller),
  Cpu(this, MMU, controller),
  timers(Cpu, MMU),
  graphics(this, MMU, Cpu),
  serializer(0x20000),
  disassembler(this, MMU, Cpu),
  debugger(Cpu, disassembler),
  debugLogCallback(nullptr) {
  LoadROM(rom_fpath);
}

void GB::AdvanceFrame() {
  currentCycles = 0;
  while (currentCycles < CYCLES_PER_FRAME) {
    debugger.Advance();
    int cycles = Cpu.Advance();
    timers.Update(cycles);
    if (doubleSpeed) cycles /= 2;
    graphics.Update(cycles);
    currentCycles += cycles;
  }
  Cpu.HandleInput();
}

void GB::SetCGBMode(bool val) {
  CGBMode = val;
}

bool GB::CGBModeEnabled() {
  return CGBMode;
}

void GB::SetDoubleSpeed(bool val) {
  doubleSpeed = val;
}

bool GB::IsDoubleSpeed() {
  return doubleSpeed;
}

std::string GB::GetROMName() {
  return ROMName;
}

const char * GB::GetROMPath() {
  return rom_fpath;
}

void GB::Reset() {
  doubleSpeed = false;
  CGBMode = false;
  currentCycles = 0;
  controller.Reset();
  MMU.Reset();
  Cpu.Reset();
  timers.Reset();
  graphics.Reset();
}

void GB::LoadROM(const char * _rom_fpath) {
  if (_rom_fpath != nullptr) {
    rom_fpath = _rom_fpath;
    std::string path(rom_fpath);
    auto basename = path.substr(path.find_last_of("/\\") + 1);
    auto pos = basename.find_last_of(".");
    ROMName = basename.substr(0, pos);
    MMU.LoadROM(rom_fpath);
  }
  Reset();
}

void GB::Serialize(Serializer & s) {
  s.Serialize<int>(currentCycles);
  s.Serialize<bool>(CGBMode);
  s.Serialize<bool>(doubleSpeed);
}

void GB::Deserialize(Serializer & s) {
  currentCycles = s.Deserialize<int>();
  CGBMode = s.Deserialize<bool>();
  doubleSpeed = s.Deserialize<bool>();
}

byte * GB::SaveState(bool save_to_disk) {
  controller.Serialize(serializer);
  MMU.Serialize(serializer);
  Cpu.Serialize(serializer);
  timers.Serialize(serializer);
  graphics.Serialize(serializer);
  Serialize(serializer);
  if (save_to_disk) {
    serializer.SaveFile(("states/" + ROMName + ".state").c_str());
  }
  serializer.Reset();
  return serializer.GetData();
}

void GB::LoadState(const byte * data) {
  if (data != nullptr) {
    serializer.SetData(data);
  }
  else {
    serializer.LoadFile(("states/" + ROMName + ".state").c_str());
  }
  controller.Deserialize(serializer);
  MMU.Deserialize(serializer);
  Cpu.Deserialize(serializer);
  timers.Deserialize(serializer);
  graphics.Deserialize(serializer);
  Deserialize(serializer);
  serializer.Reset();
}

void GB::DebugLog(std::string s) {
  if (debugLogCallback != nullptr) {
    debugLogCallback(s);
  }
  else std::cout << s << std::endl;
}

Graphics * GB::GetGraphics() {
  return &graphics;
}

CPU * GB::GetCPU() {
  return &Cpu;
}

Memory * GB::GetMMU() {
  return &MMU;
}

Controller * GB::GetInput() {
  return &controller;
}

Debugger * GB::GetDebugger() {
  return &debugger;
}

Serializer * GB::GetSerializer() {
  return &serializer;
}
