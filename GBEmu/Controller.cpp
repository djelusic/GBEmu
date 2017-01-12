#include "Controller.h"
#include "GB.h"

Controller::Controller(GB *gb) : gb(gb) {
  dpad = 0;
  buttons = 0;
  dpadSelected = false;
  buttonsSelected = false;
}

bool Controller::HandleInput() {
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  byte newDpad = 0;
  byte newButtons = 0;

  if (keys[SDL_SCANCODE_W]) {
    newDpad |= (1 << 2);
  }
  if (keys[SDL_SCANCODE_A]) {
    newDpad |= (1 << 1);
  }
  if (keys[SDL_SCANCODE_S]) {
    newDpad |= (1 << 3);
  }
  if (keys[SDL_SCANCODE_D]) {
    newDpad |= (1 << 0);
  }
  if (keys[SDL_SCANCODE_K]) {
    newButtons |= (1 << 3);
  }
  if (keys[SDL_SCANCODE_L]) {
    newButtons |= (1 << 2);
  }
  if (keys[SDL_SCANCODE_N]) {
    newButtons |= (1 << 1);
  }
  if (keys[SDL_SCANCODE_M]) {
    newButtons |= (1 << 0);
  }
  if (keys[SDL_SCANCODE_SPACE]) {
    gb->ToggleFrameLimit();
  }
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