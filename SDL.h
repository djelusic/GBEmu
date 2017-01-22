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
  Graphics &graphics;
  Controller &controller;
  Memory &MMU;
  Debugger &debugger;

  bool quit;

public:
  SDL(GB *gb, Graphics &graphics, Controller &controller, Memory &MMU, Debugger &debugger);
  void RenderScreen();
  bool HandleInput();
  void HandleEvents();
  void UpdateFPS(int fps);
};