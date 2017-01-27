#include "Controller.h"
#include "GB.h"
#include "Serializer.h"

Controller::Controller(GB *gb) : gb(gb) {
  Reset();
}

void Controller::Reset() {
  dpad = 0;
  buttons = 0;
  dpadSelected = false;
  buttonsSelected = false;
}

void Controller::Serialize(Serializer & s) {
  s.Serialize<byte>(dpad);
  s.Serialize<byte>(buttons);
  s.Serialize<bool>(dpadSelected);
  s.Serialize<bool>(buttonsSelected);
}

void Controller::Deserialize(Serializer & s) {
  dpad = s.Deserialize<byte>();
  buttons = s.Deserialize<byte>();
  dpadSelected = s.Deserialize<bool>();
  buttonsSelected = s.Deserialize<bool>();
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