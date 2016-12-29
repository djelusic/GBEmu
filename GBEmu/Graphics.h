#pragma once
#include "Memory.h"
#include "CPU.h"

#if WINDOWS
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

#define CYCLES_PER_SCANLINE 456
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCROLL_Y 0xFF42
#define SCROLL_X 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define COLOR_PALETTE 0xFF47
#define WINDOW_Y 0xFF4A
#define WINDOW_X 0xFF4B

class Graphics {

private:

  int currentCycles;
  byte display[144][160][3];
  Memory &MMU;
  CPU &Cpu;

  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  SDL_Event event;

public:

  Graphics(Memory &MMU, CPU &Cpu);
  void Update(int cycles);
  void RenderScreen();

private:

  void InitSDL();
  bool LCDEnabled();
  void SetMode(int mode);
  void HandleLCDStatus();
  void DrawScanline();
  void DrawTiles();
  void DrawSprites();
  int GetColor(int color_id);
};