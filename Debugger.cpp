#include "Debugger.h"
#include "CPU.h"
#include "Disassembler.h"
#include "Format.h"
#include <iostream>
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
  else if (cont) {
    cont = false;
    disassembler.Disassemble(cpu.GetPC(), 5);
  }
  bool loop = true;
  while (loop) {
    std::string command;
    std::cout << "dbg> ";
    std::cin >> command;
    if (command == "s" || command == "step") {
      loop = false;
      disassembler.Disassemble(cpu.GetPC(), 5);
    }
    else if (command == "bp" || command == "breakpoint") {
      word arg;
      std::cin >> std::hex >> arg;
      breakpoints.insert(arg);
      for (word addr : breakpoints) {
        std::cout << Format::Word(addr) << std::endl;
      }
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
        std::cout << Format::Word(addr);
      }
    }
    else if (command == "r" || command == "registers") {
      PrintRegisterState();
    }
    else if (command == "c" || command == "continue") {
      cont = true;
      loop = false;
      if (breakpoints.empty()) {
        cont = false;
        running = false;
      }
    }
    else if (command == "e" || command == "exit") {
      cont = false;
      loop = false;
      running = false;
    }
    else if (command == "disasm") {
      int start, cnt;
      std::cin >> std::hex >> start;
      std::cin >> std::dec >> cnt;
      disassembler.Disassemble(start, cnt);
    }
    else if (command == "h" || command == "help") {
      PrintHelp();
    }
    else {
      std::cout << "Unknown command, use 'h' or 'help' to show all commands." << std::endl;
    }
  }
}

void Debugger::PrintRegisterState() {
  std::cout
    << "AF: " << cpu.GetRegisterValue(7, 6) << std::endl
    << "BC: " << cpu.GetRegisterValue(0, 1) << std::endl
    << "DE: " << cpu.GetRegisterValue(2, 3) << std::endl
    << "HL: " << cpu.GetRegisterValue(4, 5) << std::endl
    << "PC: " << Format::Word(cpu.GetPC()) << std::endl
    << "SP: " << Format::Word(cpu.GetSP()) << std::endl;
}

void Debugger::PrintHelp() {
  std::cout
    << "Commands:" << std::endl
    << "s (step):               step into next instruction" << std::endl
    << "bp (breakpoint):        <addr>: set breakpoint at <addr>" << std::endl
    << "db (delete_breakpoint): <addr>: delete breakpoint at <addr>" << std::endl
    << "l (list):               list all breakpoints" << std::endl
    << "r (registers):          show CPU register values" << std::endl
    << "c (continue):           continue execution until a breakpoint is hit" << std::endl
    << "e (exit):               exit debugger" << std::endl
    << "disasm <start> <count>: disassemble <count> instructions in current program starting at <start>" << std::endl
    << "h (help):               show help" << std::endl;
}
