#pragma once
#include "Types.h"
#include <iomanip>
#include <functional>

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
  byte* display;
  Memory &MMU;
  CPU &Cpu;
  GB *gb;

  bool STATInterrupt = false;

  std::function<void(void)> vBlankCallback;

public:

  Graphics(GB *gb, Memory &MMU, CPU &Cpu);
  ~Graphics();
  void Update(int cycles);
  byte* GetDisplayPixels();
  void SetVblankCallback(std::function<void(void)> cb);

private:

  bool LCDEnabled();
  void SetMode(int mode);
  void DrawScanline();
  void DrawBackgroundTiles();
  void DrawWindowTiles();
  void DrawSprites();
  Color GetColor(int colorId, byte bgAttrs, bool isObj, byte objAttrs);
};