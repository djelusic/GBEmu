#pragma once
#include <set>
#include "Types.h"

class CPU;
class Disassembler;

class Debugger {

private:

  CPU &cpu;
  Disassembler &disassembler;
  bool running;
  bool cont;
  std::set<word> breakpoints;

public:

  Debugger(CPU & cpu, Disassembler & disassembler);
  void ToggleActive();
  void Advance();

private:

  void PrintRegisterState();
  void PrintHelp();
};