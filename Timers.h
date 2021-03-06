#pragma once

#define DR 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TMC 0xFF07

class CPU;
class Memory;
class Serializer;

class Timers {

private:

  int cyclesUntilUpdate;
  int currentCycles;
  int currentCyclesDR;
  static const int cyclesUntilUpdateDR = 256;

  CPU &Cpu;
  Memory &MMU;

public:

  Timers(CPU &Cpu, Memory &MMU);

  void Update(int cycles);
  bool TimerEnabled();
  void SetUpdateFrequency();

  void Reset();

  void Serialize(Serializer & s);
  void Deserialize(Serializer & s);

private:

  void UpdateTimer(int cycles);
  void UpdateDR(int cycles);

};