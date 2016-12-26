#include "GB.h"

int main(int argc, char ** argv) {
  GB gb(argv[1]);
  gb.Run();
  return 0;
}