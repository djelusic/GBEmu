#include "Graphics.h"

Graphics::Graphics(Memory &MMU, CPU &Cpu) : 
  MMU(MMU),
  Cpu(Cpu),
  currentCycles(0) {
    InitSDL();
  }

void Graphics::InitSDL() {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("GBEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
}

void Graphics::Update(int cycles) {
  HandleLCDStatus();
  if (!LCDEnabled()) {
    MMU.WriteByteDirect(LY, 0);
    return;
  }
  currentCycles += cycles; 

  if (currentCycles >= CYCLES_PER_SCANLINE) {
    currentCycles = 0;

    byte newLineCount = MMU.ReadByte(LY) + 1;
    MMU.WriteByteDirect(LY, newLineCount <= 153 ? newLineCount : 0);

    if (newLineCount == 144) Cpu.RequestInterrupt(0);
    if (newLineCount < 144) DrawScanline();
  }
}

void Graphics::RenderScreen() {
  SDL_PumpEvents();
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  byte* pixels = new byte[160 * 144 * 4];

  int pitch = 0;
  SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch);

  for (int i = 0; i < 144; i++) {
    for (int j = 0; j < 160; j++) {
        int idx = i * 160 * 4 + j * 4;
        pixels[idx] = 0xFF;
        pixels[idx + 1] = display[i][j][2];
        pixels[idx + 2] = display[i][j][1];
        pixels[idx + 3] = display[i][j][0];
    }
  }

  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);

  SDL_RenderPresent(renderer);
  
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      SDL_DestroyTexture(texture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }
}

bool Graphics::LCDEnabled() {
  return !!(MMU.ReadByte(LCDC) & 0x80);
}

void Graphics::SetMode(int mode) {
  MMU.WriteByte(STAT, (MMU.ReadByte(STAT) & 0xFC) | 1);
}

void Graphics::HandleLCDStatus() {
  byte LCDStat = MMU.ReadByte(STAT);
  byte currentMode = LCDStat & 0x3;
  byte currentScanline = MMU.ReadByte(LY);

  if (!LCDEnabled()) {
    SetMode(1);
    MMU.WriteByteDirect(LY, 0);
    return;
  }
  if (currentScanline >= 144) {
    SetMode(1);
    if (currentMode != 1 && (LCDStat & 0x10)) Cpu.RequestInterrupt(1);
  }
  else if (currentCycles <= 80 && currentMode != 2) {
    SetMode(2);
    if (LCDStat & 0x20) Cpu.RequestInterrupt(1);
  }
  else if (currentCycles > 80 && currentCycles <= 252 && currentMode != 3) {
    SetMode(3);
  }
  else if (currentCycles > 252 && currentMode != 0) {
    SetMode(0);
    if (LCDStat & 0x8) Cpu.RequestInterrupt(1);
  }
  if (currentScanline == MMU.ReadByte(LYC)) {
    MMU.WriteByte(STAT, MMU.ReadByte(STAT) | 4);
    if (LCDStat & 0x40) Cpu.RequestInterrupt(1);
  }
  else MMU.WriteByte(STAT, MMU.ReadByte(STAT) & ~4);
}

void Graphics::DrawScanline() {
  byte LCDControl = MMU.ReadByte(LCDC);
  bool backgroundEnabled = LCDControl & 1;
  bool spritesEnabled = LCDControl & 2;
  if (backgroundEnabled) DrawTiles();
  // if (spritesEnabled) DrawSprites();
}

void Graphics::DrawTiles() {
  byte LCDControl = MMU.ReadByte(LCDC);
  word backgroundTilesAddr = (LCDControl & (1 << 3)) ? 0x9C00 : 0x9800;
  word windowTilesAddr = (LCDControl & (1 << 6)) ? 0x9C00 : 0x9800;
  word tilesDataAddr = (LCDControl & (1 << 4)) ? 0x8000 : 0x8800;

  byte y = MMU.ReadByte(LY);
  byte lineY = y + MMU.ReadByte(SCROLL_Y);
  byte tileY = (byte)(lineY / 8) % 32;

  for (int x = 0; x < 160; x++) {
    byte lineX = x + MMU.ReadByte(SCROLL_X);
    byte tileX = (byte)(lineX / 8) % 32;

    word tileIdAddr = backgroundTilesAddr + tileY * 32 + tileX;
    byte tileId = MMU.ReadByte(tileIdAddr);

    word tileDataAddr = 0;
    if (tilesDataAddr == 0x8000) tileDataAddr = tilesDataAddr + tileId * 0x10;
    else tileDataAddr = tilesDataAddr + ((sbyte)tileId + 128) * 0x10;
    
    byte lineData1 = MMU.ReadByte(tileDataAddr + (lineY % 8) * 2);
    byte lineData2 = MMU.ReadByte(tileDataAddr + (lineY % 8) * 2 + 1);
    int colorBit = 7 - (lineX % 8);
    int colorData = ((lineData1 & (1 << colorBit)) >> colorBit) |
                    ((lineData2 & (1 << colorBit)) >> (colorBit - 1));
    byte r, g, b;
    switch(GetColor(colorData)) {
      case 0: r = 255; g = 255; b = 255; break;
      case 1: r = 200; g = 200; b = 200; break;
      case 2: r = 100; g = 100; b = 100; break;
      case 3: r = 0; g = 0; b = 0; break;
    }
    display[y][x][0] = r;
    display[y][x][1] = g;
    display[y][x][2] = b;
  }
}

int Graphics::GetColor(int colorId) {
  byte palette = MMU.ReadByte(COLOR_PALETTE);
  switch(colorId) {
    case 0: return (palette & 1) | (palette & 2);
    case 1: return ((palette & 0x4) >> 2) | ((palette & 0x8) >> 2);
    case 2: return ((palette & 0x10) >> 4) | ((palette & 0x20) >> 4);
    case 3: return ((palette & 0x40) >> 6) | ((palette & 0x80) >> 6); 
  }
  return 0;
}