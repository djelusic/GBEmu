#include "GB.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <functional>
#include <iostream>
#include <string>

GB::GB(const char* rom_fpath) :
  controller(this),
  MMU(this, rom_fpath, controller),
  Cpu(this, MMU, controller),
  timers(Cpu, MMU),
  graphics(this, MMU, Cpu),
  serializer(0x20000),
  disassembler(this, MMU, Cpu),
  debugger(Cpu, disassembler),
  sdl(this, graphics, controller, MMU, debugger),
  framerateUnlocked(false),
  doubleSpeed(false),
  debugLogCallback(nullptr) {
  Cpu.SetInputCallback([this](void) -> bool {return sdl.HandleInput(); });
  graphics.SetVblankCallback([this](void) -> void {sdl.RenderScreen(); });
  std::string path(rom_fpath);
  auto basename = path.substr(path.find_last_of("/\\") + 1);
  auto pos = basename.find_last_of(".");
  ROMName = basename.substr(0, pos);
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

void GB::Run() {
  while (true) {
    sdl.HandleEvents();
    clock_t begin = clock();
    AdvanceFrame();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    double frameTime = elapsedSecs;
    if (!framerateUnlocked && elapsedSecs < TIME_PER_FRAME) {
      frameTime = TIME_PER_FRAME;
    }
    fps = static_cast<int>((fps * 0.9) + ((1.0 / frameTime) * 0.1));
    sdl.UpdateFPS(fps);
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

std::string GB::GetROMName() {
  return ROMName;
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

void GB::SaveState() {
  controller.Serialize(serializer);
  MMU.Serialize(serializer);
  Cpu.Serialize(serializer);
  timers.Serialize(serializer);
  graphics.Serialize(serializer);
  Serialize(serializer);
  serializer.SaveFile(("states/" + ROMName + ".state").c_str());
  serializer.Reset();
}

void GB::LoadState() {
  serializer.LoadFile(("states/" + ROMName + ".state").c_str());
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
