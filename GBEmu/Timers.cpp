#include "Timers.h"

Timers::Timers(CPU &Cpu, Memory &MMU) : Cpu(Cpu), MMU(MMU) {
  SetUpdateFrequency();
  currentCycles = 0;
  currentCyclesDR = 0;
}

bool Timers::TimerEnabled() {
  return !!(MMU.ReadByte(TMC) & 0x4);
}

void Timers::SetUpdateFrequency() {
  byte TMCFreq = MMU.ReadByte(TMC) & 0x3;
  switch (TMCFreq) {
    case 0: cyclesUntilUpdate = 1024; break;
    case 1: cyclesUntilUpdate = 16; break;
    case 2: cyclesUntilUpdate = 64; break;
    case 3: cyclesUntilUpdate = 256; break;
  }
}

void Timers::Update(int cycles) {
  UpdateTimer(cycles);
  UpdateDR(cycles);
}

void Timers::UpdateTimer(int cycles) {
  if (!TimerEnabled()) return;
  SetUpdateFrequency();
  currentCycles += cycles;
  if (currentCycles <= cyclesUntilUpdate) return;
  currentCycles = 0;
  byte timerValue = MMU.ReadByte(TIMA);
  if (MMU.ReadByte(TIMA) == 0xFF) {
    Cpu.RequestInterrupt(2);
    MMU.WriteByte(TIMA, MMU.ReadByte(TMA));
    return;
  }
  MMU.WriteByte(TIMA, MMU.ReadByte(TIMA) + 1);
}

void Timers::UpdateDR(int cycles) {
  currentCyclesDR += cycles;
  if (currentCyclesDR <= cyclesUntilUpdateDR) return;
  currentCyclesDR = 0;
  MMU.WriteByteDirect(DR, MMU.ReadByte(DR) + 1);
}
