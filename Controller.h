#pragma once
#include "Types.h"
#include <SDL2/SDL.h>

class GB;
class Serializer;

class Controller {

private:

  byte dpad;
  byte buttons;
  bool dpadSelected;
  bool buttonsSelected;
  GB *gb;

public:

  void Reset();

  void Serialize(Serializer & s);
  void Deserialize(Serializer & s);

  Controller(GB *gb);
  bool OnNewInput(const byte & newDpad, const byte & newButtons);
  void SelectInput(const byte & val);
  byte GetInput();

};