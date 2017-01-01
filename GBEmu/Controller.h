#pragma once
#include "Types.h"

#if WINDOWS
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

class Controller {

private:

  byte dpad;
  byte buttons;
  bool dpadSelected;
  bool buttonsSelected;

public: 

  Controller();
  bool HandleInput();
  void SelectInput(const byte & val);
  byte GetInput();
  
};