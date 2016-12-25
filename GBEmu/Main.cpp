#include "CPU.h"

int main(int argc, char ** argv) {
  CPU *cpu = new CPU(argv[1]);
  for (int i = 0; i < atoi(argv[2]); i++) {
    cpu->Advance();
  }
  return 0;
}