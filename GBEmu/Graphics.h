#pragma once
#include "Types.h"
#include <iomanip>
#include <SDL2/SDL.h>

#define CYCLES_PER_SCANLINE 456
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCROLL_Y 0xFF42
#define SCROLL_X 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define WINDOW_Y 0xFF4A
#define WINDOW_X 0xFF4B
#define BGPI 0xFF68
#define BGPD 0xFF69
#define OBPI 0xFF6A
#define OBPD 0xFF6B

class Memory;
class CPU;
class GB;

class Graphics {

private:

  int currentCycles;
  byte display[144][160][3];
  Memory &MMU;
  CPU &Cpu;
  GB *gb;

  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;

public:

  Graphics(GB *gb, Memory &MMU, CPU &Cpu);
  void Update(int cycles);
  void RenderScreen();
  void HandleSDLEvents();

private:

  void InitSDL();
  bool LCDEnabled();
  void SetMode(int mode);
  void DrawScanline();
  void DrawBackgroundTiles();
  void DrawWindowTiles();
  void DrawSprites();
  Color GetColor(int colorId, byte bgAttrs, bool isObj, byte objAttrs);
};