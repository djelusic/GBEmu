#pragma once
#include "Types.h"
#include <SDL2/SDL.h>

class GB;

class Controller {

private:

  byte dpad;
  byte buttons;
  bool dpadSelected;
  bool buttonsSelected;
  GB *gb;

public: 

  Controller(GB *gb);
  bool HandleInput();
  void SelectInput(const byte & val);
  byte GetInput();
  
};