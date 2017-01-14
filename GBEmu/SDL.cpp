#include "SDL.h"
#include "GB.h"
#include "Graphics.h"
#include "Controller.h"
#include "Memory.h"
#include <iostream>

SDL::SDL(GB *gb, Graphics & graphics, Controller & controller, Memory &MMU) :
  gb(gb),
  graphics(graphics),
  controller(controller),
  MMU(MMU) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("GBEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 3, 144 * 3, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
}

SDL::~SDL() {
}

void SDL::RenderScreen() {
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);
  byte* display = graphics.GetDisplayPixels();
  byte* pixels = new byte[160 * 144 * 4];

  int pitch = 0;
  SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch);

  memcpy(pixels, display, 160 * 144 * 4);

  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);

  SDL_RenderPresent(renderer);
}

bool SDL::HandleInput() {
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
  return controller.OnNewInput(newDpad, newButtons);
}

void SDL::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      SDL_DestroyTexture(texture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      MMU.SaveRAM();
    }
  }
}
