#pragma once
#include "Types.h"
#include <SDL2/SDL.h>

class GB;
class Graphics;
class Controller;
class Memory;

class SDL {

private:
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  GB *gb;
  Graphics &graphics;
  Controller &controller;
  Memory &MMU;

  byte *pixels;

public:
  SDL(GB *gb, Graphics &graphics, Controller &controller, Memory &MMU);
  ~SDL();
  void RenderScreen();
  bool HandleInput();
  void HandleEvents();
};