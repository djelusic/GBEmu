#include "Controller.h"
#include "GB.h"

Controller::Controller(GB *gb) : gb(gb) {
  dpad = 0;
  buttons = 0;
  dpadSelected = false;
  buttonsSelected = false;
}

bool Controller::OnNewInput(const byte & newDpad, const byte & newButtons) {
  // Check if input has changed
  bool inputChanged = (dpadSelected && (dpad ^ newDpad)) || (buttonsSelected && (buttons ^ newButtons));
  dpad = newDpad;
  buttons = newButtons;

  return inputChanged;
}

void Controller::SelectInput(const byte & val) {
  dpadSelected = !!((~val) & 0x10);
  buttonsSelected = !!((~val) & 0x20);
}

byte Controller::GetInput() {
  byte input = 0;
  if (dpadSelected) {
    input |= 1 << 4;
    input |= dpad;
  }
  if (buttonsSelected) {
    input |= 1 << 5;
    input |= buttons;
  }
  return ~input;
}