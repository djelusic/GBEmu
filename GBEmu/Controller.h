#pragma once
#include "Types.h"
#include <SDL2/SDL.h>

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