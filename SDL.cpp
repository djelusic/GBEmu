#include "SDL.h"
#include "GB.h"
#include "Graphics.h"
#include "Controller.h"
#include "Memory.h"
#include "Debugger.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

SDL::SDL(GB *gb) :
  gb(gb) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("GBEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 3, 144 * 3, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
  gb->GetCPU()->SetInputCallback([this](void) -> bool {return HandleInput(); });
  gb->GetGraphics()->SetVblankCallback([this](void) -> void { RenderScreen(); });
}

void SDL::RenderScreen() {
  if (quit) return;
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);
  byte* display = gb->GetGraphics()->GetDisplayPixels();
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
  return gb->GetInput()->OnNewInput(newDpad, newButtons);
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
      gb->GetMMU()->SaveRAM();
    }
    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
        ToggleFrameLimit();
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_F1) {
        gb->SaveState();
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_F2) {
        gb->LoadState();
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_F9) {
        gb->GetDebugger()->ToggleActive();
      }
    }
  }
}

void SDL::ToggleFrameLimit() {
  framerateUnlocked = !framerateUnlocked;
}

void SDL::UpdateFPS(int fps) {
  SDL_SetWindowTitle(window, ("GBEmu " + std::to_string(fps)).c_str());
}

void SDL::Run() {
  while (true) {
    HandleEvents();
    clock_t begin = clock();
    gb->AdvanceFrame();
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    double frameTime = elapsedSecs;
    if (!framerateUnlocked && elapsedSecs < TIME_PER_FRAME) {
      frameTime = TIME_PER_FRAME;
    }
    fps = static_cast<int>((fps * 0.9) + ((1.0 / frameTime) * 0.1));
    UpdateFPS(fps);
    int remainingTime = (int)((TIME_PER_FRAME - elapsedSecs) * 1000);
    // std::cout << "Remaining time: " << remainingTime << std::endl;
    if (remainingTime < 0) remainingTime = 0;
    if (!framerateUnlocked) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(remainingTime)
      );
    }
  }
}
