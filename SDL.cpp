#include "SDL.h"
#include "GB.h"
#include "Graphics.h"
#include "Controller.h"
#include "Memory.h"
#include "Debugger.h"
#include <iostream>
#include <string>

SDL::SDL(GB *gb, Graphics & graphics, Controller & controller, Memory & MMU, Debugger & debugger) :
  gb(gb),
  graphics(graphics),
  controller(controller),
  MMU(MMU),
  debugger(debugger),
  quit(false) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("GBEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 3, 144 * 3, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
}

void SDL::RenderScreen() {
  if (quit) return;
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);
  byte* display = graphics.GetDisplayPixels();
  void *pixels;
  int pitch = 0;
  SDL_LockTexture(texture, nullptr, &pixels, &pitch);

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
  return controller.OnNewInput(newDpad, newButtons);
}

void SDL::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      quit = true;
      SDL_DestroyTexture(texture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      MMU.SaveRAM();
    }
    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
        gb->ToggleFrameLimit();
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_F1) {
        gb->SaveState();
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_F2) {
        gb->LoadState();
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_F9) {
        debugger.ToggleActive();
      }
    }
  }
}

void SDL::UpdateFPS(int fps) {
  SDL_SetWindowTitle(window, ("GBEmu " + std::to_string(fps)).c_str());
}
