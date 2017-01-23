#include "GB.h"
#include "SDL.h"

int main(int argc, char ** argv) {
  //GB gb(argv[1]);
  GB gb("test_roms/zeldaoos.gbc");
  SDL sdl(&gb);
  sdl.Run();
  return 0;
}