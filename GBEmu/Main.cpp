#include "GB.h"

int main(int argc, char ** argv) {
  //GB gb(argv[1]);
  GB gb("test_roms/mariotennis.gbc");
  gb.Run();
  return 0;
}