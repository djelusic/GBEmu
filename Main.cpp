#include "GB.h"

int main(int argc, char ** argv) {
  //GB gb(argv[1]);
  GB gb("test_roms/pokemon_crystal.gbc");
  gb.Run();
  return 0;
}