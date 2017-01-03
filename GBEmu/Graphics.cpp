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
  byte LCDStat = MMU.ReadByte(STAT);
  byte currentMode = LCDStat & 0x3;
  byte currentScanline = MMU.ReadByte(LY);

   if (!LCDEnabled()) {
	SetMode(1);
	MMU.WriteByteDirect(LY, 0);
    return;
  }
  currentCycles += cycles;
  if (currentMode == 2 && currentCycles >= 80) {
    SetMode(3);
  }
  if (currentMode == 3 && currentCycles >= 252) {
    SetMode(0);
    DrawScanline();
    if (LCDStat & 0x8) Cpu.RequestInterrupt(1);
  }
  if (currentMode == 0 && currentCycles >= 456) {
    currentCycles = 0;
    MMU.WriteByteDirect(LY, currentScanline + 1);
    if (currentScanline == 143) {
      SetMode(1);
      Cpu.RequestInterrupt(0);
      if (LCDStat & 0x10) Cpu.RequestInterrupt(1);
      RenderScreen();
    }
    else {
      SetMode(2);
      if (LCDStat & 0x20) Cpu.RequestInterrupt(1);
    }
  }
  if (currentMode == 1 && currentCycles >= 456) {
    currentCycles = 0;
    MMU.WriteByteDirect(LY, currentScanline + 1);
    if (currentScanline == 153) {
      SetMode(2);
      MMU.WriteByteDirect(LY, 0);
      if (LCDStat & 0x20) Cpu.RequestInterrupt(1);
    }
  }
  if (currentScanline == MMU.ReadByte(LYC)) {
    MMU.WriteByte(STAT, MMU.ReadByte(STAT) | 4);
    if (LCDStat & 0x40) Cpu.RequestInterrupt(1);
  }
  else MMU.WriteByte(STAT, MMU.ReadByte(STAT) & ~4);
}

void Graphics::SetMode(int mode) {
  MMU.WriteByte(STAT, (MMU.ReadByte(STAT) & 0xFC) | mode);
}

void Graphics::RenderScreen() {
  // SDL_PumpEvents();
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  byte* pixels = new byte[160 * 144 * 4];

  int pitch = 0;
  SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch);

  for (int i = 0; i < 144; i++) {
    for (int j = 0; j < 160; j++) {
        int idx = i * 160 * 4 + j * 4;
        pixels[idx] = 0xFF;
        pixels[idx + 1] = display[i][j][0];
        pixels[idx + 2] = display[i][j][1];
        pixels[idx + 3] = display[i][j][2];
        // if (display[i][j][0] == 255 || display[i][j][0] == 200 || display[i][j][0] == 100) std::cout << '-';
        // else std::cout << 0;
    }
    // std::cout << std::endl;
  }
  // std::cout << std::endl;

  for (int i = 0x9A60; i < 0x9A6A; i++) {
    std::cout << std::hex << (int)MMU.ReadByte(i);
  }
  std::cout << std::endl << std::endl;

  //for (int i = 0x8F10; i < 0x8F20; i++) {
  //  std::cout << std::bitset<8>(MMU.ReadByte(i));
  //  if (i % 2 == 1) std::cout << std::endl;
  //}
  std::cout << std::endl << std::endl;

  std::cout << "Sprite palette 1: " << std::hex << (int)MMU.ReadByte(0xFF48) << std::endl;
  std::cout << "Sprite palette 2: " << std::hex << (int)MMU.ReadByte(0xFF49) << std::endl;

  for (int i = 0xFE00; i < 0xFEA0; i++) {
    if (i % 4 == 0) std::cout << " ";
    if (i % 32 == 0) std::cout << std::endl;
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)(MMU.ReadByte(i));
  }
  std::cout << std::endl << std::endl;

  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);

  SDL_RenderPresent(renderer);
}

bool Graphics::LCDEnabled() {
  return !!(MMU.ReadByte(LCDC) & 0x80);
}

void Graphics::DrawScanline() {
  byte LCDControl = MMU.ReadByte(LCDC);
  bool backgroundEnabled = LCDControl & 1;
  bool windowEnabled = LCDControl & 0x20;
  bool spritesEnabled = LCDControl & 2;
  DrawBackgroundTiles();
  if (windowEnabled) DrawWindowTiles();
  if (spritesEnabled) DrawSprites();
}

void Graphics::DrawBackgroundTiles() {
  byte LCDControl = MMU.ReadByte(LCDC);
  word backgroundTilesAddr = (LCDControl & (1 << 3)) ? 0x9C00 : 0x9800;
  word tilesDataAddr = (LCDControl & (1 << 4)) ? 0x8000 : 0x8800;

  byte y = MMU.ReadByte(LY);
  byte lineY = y + MMU.ReadByte(SCROLL_Y);
  byte tileY = (lineY / 8);

  for (int x = 0; x < 160; x++) {
    byte lineX = x + MMU.ReadByte(SCROLL_X);
    byte tileX = (lineX / 8);

    word tileIdAddr = backgroundTilesAddr + tileY * 32 + tileX;
    byte tileId = MMU.ReadByte(tileIdAddr);

    word tileDataAddr = 0;
    if (tilesDataAddr == 0x8000) tileDataAddr = tilesDataAddr + tileId * 0x10;
    else tileDataAddr = tilesDataAddr + ((sbyte)tileId + 128) * 0x10;
    
    byte lineData1 = MMU.ReadByte(tileDataAddr + (lineY % 8) * 2);
    byte lineData2 = MMU.ReadByte(tileDataAddr + (lineY % 8) * 2 + 1);
    int colorBit = 7 - (lineX % 8);
    int colorData = 0;
    if (lineData1 & (1 << colorBit)) colorData |= 1;
    if (lineData2 & (1 << colorBit)) colorData |= 2;
    byte r, g, b;
    switch(GetColor(colorData, 0xFF47)) {
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

void Graphics::DrawWindowTiles() {
  byte LCDControl = MMU.ReadByte(LCDC);
  word windowTilesAddr = (LCDControl & (1 << 6)) ? 0x9C00 : 0x9800;
  word tilesDataAddr = (LCDControl & (1 << 4)) ? 0x8000 : 0x8800;

  byte y = MMU.ReadByte(LY);
  int lineY = y - MMU.ReadByte(WINDOW_Y);
  if (lineY < 0) return;
  byte tileY = lineY / 8;

  for (int x = 0; x < 160; x++) {
    int lineX = x - MMU.ReadByte(WINDOW_X) + 7;
    if (lineX < 0) continue;
    byte tileX = lineX / 8;

    word tileIdAddr = windowTilesAddr + tileY * 32 + tileX;
    byte tileId = MMU.ReadByte(tileIdAddr);

    word tileDataAddr = 0;
    if (tilesDataAddr == 0x8000) tileDataAddr = tilesDataAddr + tileId * 0x10;
    else tileDataAddr = tilesDataAddr + ((sbyte)tileId + 128) * 0x10;
    
    byte lineData1 = MMU.ReadByte(tileDataAddr + (lineY % 8) * 2);
    byte lineData2 = MMU.ReadByte(tileDataAddr + (lineY % 8) * 2 + 1);
    int colorBit = 7 - (lineX % 8);
    int colorData = 0;
    if (lineData1 & (1 << colorBit)) colorData |= 1;
    if (lineData2 & (1 << colorBit)) colorData |= 2;
    byte r, g, b;
    switch(GetColor(colorData, 0xFF47)) {
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

void Graphics::DrawSprites() {
  for (int sprite = 39; sprite >= 0; sprite--) {
    int idx = sprite * 4;
    byte y = MMU.ReadByte(0xFE00 + idx) - 16;
    byte x = MMU.ReadByte(0xFE00 + idx + 1) - 8;
    byte tileId = MMU.ReadByte(0xFE00 + idx + 2);
    byte attrs = MMU.ReadByte(0xFE00 + idx + 3);

    int currentScanline = MMU.ReadByte(LY);
    int spriteSize = (MMU.ReadByte(LCDC) & (1 << 2)) ? 16 : 8;
    if (currentScanline < y || currentScanline >= y + spriteSize) continue;
    int line = currentScanline - y;
    if (attrs & (1 << 6)) line = spriteSize - line;
    if (spriteSize == 16) tileId &= 0xFE;

    word tileDataAddr = 0x8000 + tileId * 16 + line * 2;
    byte tileData1 = MMU.ReadByte(tileDataAddr);
    byte tileData2 = MMU.ReadByte(tileDataAddr + 1);

    for (int xLine = 0; xLine < 8; xLine++) {
      int xFinal = x + xLine;
      if (xFinal >= 160 || xFinal < 0) continue;
      int colorBit = (attrs & (1 << 5)) ? xLine : (7 - xLine);
      int colorData = 0;
      if (tileData1 & (1 << colorBit)) colorData |= 1;
      if (tileData2 & (1 << colorBit)) colorData |= 2;
      if (colorData == 0) continue;
      byte r, g, b;
      word paletteAddr = (attrs & (1 << 4)) ? 0xFF49 : 0xFF48;
      switch(GetColor(colorData, paletteAddr)) {
        case 0: r = 255; g = 255; b = 255; break;
        case 1: r = 200; g = 200; b = 200; break;
        case 2: r = 100; g = 100; b = 100; break;
        case 3: r = 0; g = 0; b = 0; break;
      }
      //if (!(attrs & (1 << 7))) {
        display[currentScanline][xFinal][0] = r;
        display[currentScanline][xFinal][1] = g;
        display[currentScanline][xFinal][2] = b;
      //}
    }
  }
}

int Graphics::GetColor(int colorId, word paletteAddr) {
  byte palette = MMU.ReadByte(paletteAddr);
  switch(colorId) {
    case 0: return palette & 3;
    case 1: return (palette >> 2) & 3;
    case 2: return (palette >> 4) & 3;
    case 3: return (palette >> 6) & 3; 
  }
  return 0;
}

void Graphics::HandleSDLEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      SDL_DestroyTexture(texture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }
}