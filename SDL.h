#pragma once
#include "Types.h"
#include <SDL2/SDL.h>

class GB;
class Graphics;
class Controller;
class Memory;
class Debugger;

class SDL {

private:
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  GB *gb;

  bool quit;
  bool framerateUnlocked;
  int fps;

public:
  SDL(GB *gb);
  void RenderScreen();
  bool HandleInput();
  void HandleEvents();
  void ToggleFrameLimit();
  void UpdateFPS(int fps);
  void Run();
};