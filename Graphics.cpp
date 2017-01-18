#include "Graphics.h"
#include "Memory.h"
#include "CPU.h"
#include "GB.h"
#include "Serializer.h"

Graphics::Graphics(GB *gb, Memory &MMU, CPU &Cpu) :
  MMU(MMU),
  Cpu(Cpu),
  gb(gb),
  currentCycles(0) {
  display = new byte[144 * 160 * 4];
  for (int i = 0; i < 144 * 160 * 4; i++)
    display[i] = 0xFF;
}

Graphics::~Graphics() {
  delete[] display;
}

void Graphics::Update(int cycles) {
  byte LCDStat = MMU.ReadByte(STAT);
  byte currentMode = LCDStat & 0x3;
  byte currentScanline = MMU.ReadByte(LY);

  bool previousSTAT = STATInterrupt;
  STATInterrupt = false;

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
    MMU.HandleHBlank();
    if (LCDStat & 0x8) {
      STATInterrupt = true;
    }
  }
  if (currentMode == 0 && currentCycles >= 456) {
    currentCycles -= 456;
    DrawScanline();
    MMU.WriteByteDirect(LY, ++currentScanline);
    if (currentScanline == 144) {
      SetMode(1);
      Cpu.RequestInterrupt(0);
      if (LCDStat & 0x10) {
        STATInterrupt = true;
      }
    }
    else {
      SetMode(2);
      if (LCDStat & 0x20) {
        STATInterrupt = true;
      }
    }
  }
  if (currentMode == 1 && currentCycles >= 456) {
    currentCycles -= 456;
    MMU.WriteByteDirect(LY, ++currentScanline);
    if (currentScanline == 154) {
      if (vBlankCallback != nullptr) vBlankCallback();
      SetMode(2);
      currentScanline = 0;
      MMU.WriteByteDirect(LY, 0);
	    if (LCDStat & 0x20) {
        STATInterrupt = true;
	    }
    }
  }
  if (currentScanline == MMU.ReadByte(LYC)) {
    MMU.WriteByte(STAT, MMU.ReadByte(STAT) | 4);
    if (LCDStat & 0x40) {
      STATInterrupt = true;
    }
  }
  else MMU.WriteByte(STAT, MMU.ReadByte(STAT) & ~4);

  if (STATInterrupt && !previousSTAT) {
    Cpu.RequestInterrupt(1);
  }
}

void Graphics::SetMode(int mode) {
  MMU.WriteByte(STAT, (MMU.ReadByte(STAT) & 0xFC) | mode);
}

bool Graphics::LCDEnabled() {
  return !!(MMU.ReadByte(LCDC) & 0x80);
}

void Graphics::DrawScanline() {
  byte LCDControl = MMU.ReadByte(LCDC);
  bool backgroundEnabled = LCDControl & 1;
  bool windowEnabled = LCDControl & 0x20;
  bool spritesEnabled = LCDControl & 2;
  if (backgroundEnabled) DrawBackgroundTiles();
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
    byte tileId = MMU.ReadVRAM(tileIdAddr, 0);
    byte tileAttrs = gb->CGBModeEnabled() ? MMU.ReadVRAM(tileIdAddr, 1) : 0;
    int tileBankNumber = (tileAttrs & (1 << 3)) ? 1 : 0;

    word tileDataAddr = 0;
    if (tilesDataAddr == 0x8000) tileDataAddr = tilesDataAddr + tileId * 0x10;
    else tileDataAddr = tilesDataAddr + ((sbyte)tileId + 128) * 0x10;
    byte lineYOffset = (tileAttrs & (1 << 6)) ? (7 - (lineY % 8)) : (lineY % 8);

    byte lineData1 = MMU.ReadVRAM(tileDataAddr + lineYOffset * 2, tileBankNumber);
    byte lineData2 = MMU.ReadVRAM(tileDataAddr + lineYOffset * 2 + 1, tileBankNumber);
    int colorBit = (tileAttrs & (1 << 5)) ? (lineX % 8) : (7 - (lineX % 8));
    int colorData = 0;
    if (lineData1 & (1 << colorBit)) colorData |= 1;
    if (lineData2 & (1 << colorBit)) colorData |= 2;
    Color color = GetColor(colorData, tileAttrs, false, 0);
    if (y < 0 || y >= 144 || x < 0 || x >= 160) return;
    int idx = y * 160 * 4 + x * 4;
    display[idx + 0] = 0xFF;
    display[idx + 1] = color.R;
    display[idx + 2] = color.G;
    display[idx + 3] = color.B;
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
    byte tileId = MMU.ReadVRAM(tileIdAddr, 0);
    byte tileAttrs = gb->CGBModeEnabled() ? MMU.ReadVRAM(tileIdAddr, 1) : 0;
    int tileBankNumber = (tileAttrs & (1 << 3)) ? 1 : 0;

    word tileDataAddr = 0;
    if (tilesDataAddr == 0x8000) tileDataAddr = tilesDataAddr + tileId * 0x10;
    else tileDataAddr = tilesDataAddr + ((sbyte)tileId + 128) * 0x10;
    byte lineYOffset = (tileAttrs & (1 << 6)) ? (7 - (lineY % 8)) : (lineY % 8);

    byte lineData1 = MMU.ReadVRAM(tileDataAddr + lineYOffset * 2, tileBankNumber);
    byte lineData2 = MMU.ReadVRAM(tileDataAddr + lineYOffset * 2 + 1, tileBankNumber);
    int colorBit = (tileAttrs & (1 << 5)) ? (lineX % 8) : (7 - (lineX % 8));
    int colorData = 0;
    if (lineData1 & (1 << colorBit)) colorData |= 1;
    if (lineData2 & (1 << colorBit)) colorData |= 2;
    Color color = GetColor(colorData, tileAttrs, false, 0);
    if (y < 0 || y >= 144 || x < 0 || x >= 160) return;
    int idx = y * 160 * 4 + x * 4;
    display[idx + 0] = 0xFF;
    display[idx + 1] = color.R;
    display[idx + 2] = color.G;
    display[idx + 3] = color.B;
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
    if (attrs & (1 << 6)) line = spriteSize - 1 - line;
    if (spriteSize == 16) tileId &= 0xFE;
    int tileBankNumber = (attrs & (1 << 3)) ? 1 : 0;

    word tileDataAddr = 0x8000 + tileId * 16 + line * 2;
    byte tileData1 = MMU.ReadVRAM(tileDataAddr, tileBankNumber);
    byte tileData2 = MMU.ReadVRAM(tileDataAddr + 1, tileBankNumber);

    for (int xLine = 0; xLine < 8; xLine++) {
      int xFinal = x + xLine;
      if (xFinal >= 160 || xFinal < 0) continue;
      int colorBit = (attrs & (1 << 5)) ? xLine : (7 - xLine);
      int colorData = 0;
      if (tileData1 & (1 << colorBit)) colorData |= 1;
      if (tileData2 & (1 << colorBit)) colorData |= 2;
      if (colorData == 0) continue;
      Color color = GetColor(colorData, 0, true, attrs);

      // If LCDC bit 0 is set in CGB mode, sprites will always be displayed
      // on top of the background and window.
      // Note: if bit 0 is cleared, both window and background will not be rendered on a CGB.
      // This could lead to possible compatibility problems with monochrome GB games
      // if they choose to only display the window, so I chose not to emulate it here.
      byte LCDControl = MMU.ReadByte(LCDC);
      bool spriteMasterPriority = gb->CGBModeEnabled() && !(LCDControl & 1);
      if (!(attrs & (1 << 7)) || spriteMasterPriority) {
        int idx = currentScanline * 160 * 4 + xFinal * 4;
        display[idx + 0] = 0xFF;
        display[idx + 1] = color.R;
        display[idx + 2] = color.G;
        display[idx + 3] = color.B;
      }
    }
  }
}

Color Graphics::GetColor(int colorId, byte bgAttrs, bool isObj, byte objAttrs) {
  Color result;
  if (!gb->CGBModeEnabled()) {
    word paletteAddr = 0xFF47;
    if (isObj) paletteAddr = (objAttrs & (1 << 4)) ? 0xFF49 : 0xFF48;
    byte paletteData = MMU.ReadByte(paletteAddr);
    byte palette[4] = {
      static_cast<byte>(paletteData & 3),
      static_cast<byte>((paletteData >> 2) & 3),
      static_cast<byte>((paletteData >> 4) & 3),
      static_cast<byte>((paletteData >> 6) & 3)
    };
    switch (palette[colorId]) {
    case 0: result.R = 255; result.G = 255; result.B = 255; break;
    case 1: result.R = 200; result.G = 200; result.B = 200; break;
    case 2: result.R = 100; result.G = 100; result.B = 100; break;
    case 3: result.R = 0; result.G = 0; result.B = 0; break;
    }
  }
  else {
    int paletteNumber = isObj ? (objAttrs & 0x7) : (bgAttrs & 0x7);
    word colorAddr = paletteNumber * 8 + colorId * 2;
    if (isObj) colorAddr += 0x40;
    word colorData = MMU.GetPaletteData(colorAddr);
    // Convert from RGB555 to RGB888. Right shift the 5-bit value and OR it with its
    // most significant bits.
    result.B = ((colorData & 0x1F) << 3) | ((colorData & 0x1C) >> 2);
    result.G = (((colorData & 0x3E0) >> 5) << 3) | ((colorData & 0x380) >> 7);
    result.R = (((colorData & 0x7C00) >> 10) << 3) | ((colorData & 0x7000) >> 12);
  }
  return result;
}

byte* Graphics::GetDisplayPixels() {
  return display;
}

void Graphics::SetVblankCallback(std::function<void(void)> cb) {
  vBlankCallback = cb;
}

void Graphics::Serialize(Serializer & s) {
  s.Serialize<int>(currentCycles);
  s.Serialize<bool>(STATInterrupt);
}

void Graphics::Deserialize(Serializer & s) {
  currentCycles = s.Deserialize<int>();
  STATInterrupt = s.Deserialize<bool>();
}