#include "Debugger.h"
#include "CPU.h"
#include "Disassembler.h"
#include <iostream>
#include <iomanip>
#include <string>

Debugger::Debugger(CPU & cpu, Disassembler & disassembler) :
  cpu(cpu),
  disassembler(disassembler),
  running(false) {}

void Debugger::ToggleActive() {
  running = !running;
}

void Debugger::Advance() {
  if (!running) return;
  if (cont && breakpoints.find(cpu.GetPC()) == breakpoints.end()) {
    return;
  }
  else {
    cont = false;
    disassembler.Disassemble(cpu.GetPC(), 5);
  }
  std::string command;
  std::cout << "dbg> ";
  std::cin >> command;
  if (command == "s" || command == "step") {}
  else if (command == "bp" || command == "breakpoint") {
    word arg;
    std::cin >> std::hex >> arg;
    breakpoints.insert(arg);
  }
  else if (command == "db" || command == "delete_breakpoint") {
    word arg;
    std::cin >> std::hex >> arg;
    if (arg) {
      breakpoints.erase(arg);
    }
    else {
      breakpoints.clear();
    }
  }
  else if (command == "l" || command == "list") {
    for (word addr : breakpoints) {
      std::cout << std::setfill('0') << std::setw(4)
        << std::hex << (int)addr << std::endl;
    }
    std::cout << std::endl;
  }
  else if (command == "c" || command == "continue") {
    cont = true;
  }
  else if (command == "e" || command == "exit") {
    cont = false;
    ToggleActive();
  }
}
