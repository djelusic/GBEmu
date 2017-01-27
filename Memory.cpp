#include "Memory.h"
#include "GB.h"
#include "Serializer.h"
#include <fstream>
#include <iostream>

void Memory::LoadCartridge(const char* fpath) {
  std::ifstream fin(fpath, std::ios::in | std::ios::binary);
  fin.seekg(0, std::ios::end);
  int length = fin.tellg();
  fin.seekg(0, std::ios::beg);
  fin.read((char*)m_Cartridge, length);
  fin.close();
}

void Memory::ToggleRAMEnabled(const word& address, const byte & val) {
  if (MBCMode == 2 && (address & 0x100) != 0) return;
  if ((val & 0xA) == 0xA) ramEnabled = true;
  else ramEnabled = false;
}

void Memory::ChangeBanks(const word & address, const byte & val) {
  if (address >= 0x0 && address <= 0x1FFF) {
    ToggleRAMEnabled(address, val);
  }
  if (address >= 0x2000 && address <= 0x3FFF) {
    if (MBCMode == 1 || MBCMode == 2) {
      currentROMBank = (currentROMBank & 0xE0) | (val & 0x1F);
      if (currentROMBank % 0x20 == 0) currentROMBank++;
    }
    if (MBCMode == 3) {
      currentROMBank = (val & 0x7F);
      if (currentROMBank == 0) currentROMBank = 1;
    }
    if (MBCMode == 5) {
      if (address <= 0x2FFF) currentROMBank = (currentROMBank & 0xFF00) | val;
      else currentROMBank = (currentROMBank & 0xFF) | ((val & 1) << 8);
    }
  }
  if (address >= 0x4000 && address <= 0x5FFF) {
    if (MBCMode == 1) {
      if (ramBankingMode) currentRAMBank = val & 0x3;
      else {
        currentROMBank = (currentROMBank & 0x1F) | (val & 0xE0);
        if (currentROMBank % 0x20 == 0) currentROMBank++;
      }
    }
    if (MBCMode == 3 || MBCMode == 5) {
      currentRAMBank = val;
    }
  }
  if (address >= 0x6000 && address <= 0x7FFF) {
    if (MBCMode == 1 || MBCMode == 2) {
      if ((val & 0x1) == 1) ramBankingMode = true;
      else ramBankingMode = false;
    }
  }
}

void Memory::SaveRAM() {
  std::string ROMName(gb->GetROMName());
  std::ofstream fout("battery/" + ROMName + ".sav", std::ios::out | std::ios::binary);
  fout.write((char*)m_CartridgeRAM, 0x20000);
  fout.close();
}

void Memory::LoadRAM() {
  std::string ROMName(gb->GetROMName());
  std::ifstream fin("battery/" + ROMName + ".sav", std::ios::in | std::ios::binary);
  fin.seekg(0, std::ios::end);
  int length = fin.tellg();
  fin.seekg(0, std::ios::beg);
  fin.read((char*)m_CartridgeRAM, length);
  fin.close();
}

Memory::Memory(GB *gb, Controller &controller) :
  gb(gb),
  controller(controller) {
  m_MMU = new byte[0x10000]();
  m_Cartridge = new byte[0x800000]();
  m_CartridgeRAM = new byte[0x20000]();
  m_VRAM = new byte[0x4000]();
  m_WRAM = new byte[0x8000]();
  m_PaletteData = new byte[0x80]();
  Reset();
}

Memory::~Memory() {
  SaveRAM();
  delete[] m_MMU;
  delete[] m_Cartridge;
  delete[] m_CartridgeRAM;
  delete[] m_VRAM;
  delete[] m_WRAM;
  delete[] m_PaletteData;
}

void Memory::Reset() {
  memset(m_MMU, 0, 0x10000);
  memset(m_Cartridge, 0, 0x800000);
  memset(m_CartridgeRAM, 0, 0x20000);
  memset(m_VRAM, 0, 0x4000);
  memset(m_WRAM, 0, 0x8000);
  memset(m_PaletteData, 0, 0x80);

  LoadRAM();

  for (int i = 0; i < 0x40; i++) {
    m_PaletteData[i] = 0xFF;
  }

  m_MMU[0xFF05] = 0x00;
  m_MMU[0xFF06] = 0x00;
  m_MMU[0xFF07] = 0x00;
  m_MMU[0xFF10] = 0x80;
  m_MMU[0xFF11] = 0xBF;
  m_MMU[0xFF12] = 0xF3;
  m_MMU[0xFF14] = 0xBF;
  m_MMU[0xFF16] = 0x3F;
  m_MMU[0xFF17] = 0x00;
  m_MMU[0xFF19] = 0xBF;
  m_MMU[0xFF1A] = 0x7F;
  m_MMU[0xFF1B] = 0xFF;
  m_MMU[0xFF1C] = 0x9F;
  m_MMU[0xFF1E] = 0xBF;
  m_MMU[0xFF20] = 0xFF;
  m_MMU[0xFF21] = 0x00;
  m_MMU[0xFF22] = 0x00;
  m_MMU[0xFF23] = 0xBF;
  m_MMU[0xFF24] = 0x77;
  m_MMU[0xFF25] = 0xF3;
  m_MMU[0xFF26] = 0xF1;
  m_MMU[0xFF40] = 0x91;
  m_MMU[0xFF42] = 0x00;
  m_MMU[0xFF43] = 0x00;
  m_MMU[0xFF45] = 0x00;
  m_MMU[0xFF47] = 0xFC;
  m_MMU[0xFF48] = 0xFF;
  m_MMU[0xFF49] = 0xFF;
  m_MMU[0xFF4A] = 0x00;
  m_MMU[0xFF4B] = 0x00;
  m_MMU[0xFF70] = 0x01;
  m_MMU[0xFFFF] = 0x00;

  currentROMBank = 1;
  currentRAMBank = 0;
  currentWRAMBank = 1;

  ramEnabled = false;
  ramBankingMode = false;
  MBCMode = 0;

  LoadCartridge(gb->GetROMPath());

  byte MBCVal = m_Cartridge[0x147];
  if (MBCVal > 0 && MBCVal <= 3) {
    MBCMode = 1;
  }
  if (MBCVal > 3 && MBCVal <= 6) {
    MBCMode = 2;
  }
  if (MBCVal >= 0xF && MBCVal <= 0x13) {
    MBCMode = 3;
  }
  if (MBCVal >= 0x19 && MBCVal <= 0x1E) {
    MBCMode = 5;
  }

  byte CGBEnable = m_Cartridge[0x143];
  if (CGBEnable == 0x80 || CGBEnable == 0xC0) {
    gb->SetCGBMode(true);
  }
}

void Memory::Serialize(Serializer &s) {
  s.SerializeArray<byte>(m_MMU, 0x10000);
  s.SerializeArray<byte>(m_VRAM, 0x4000);
  s.SerializeArray<byte>(m_WRAM, 0x8000);
  s.SerializeArray<byte>(m_PaletteData, 0x80);
  s.Serialize<int>(currentROMBank);
  s.Serialize<byte>(currentRAMBank);
  s.Serialize<byte>(currentWRAMBank);
  s.Serialize<bool>(ramEnabled);
  s.Serialize<bool>(ramBankingMode);
  s.Serialize<byte>(MBCMode);
  s.Serialize<bool>(HDMAActive);
  s.Serialize<word>(HDMASrc);
  s.Serialize<word>(HDMADest);
  s.Serialize<int>(HDMALength);
}

void Memory::Deserialize(Serializer &s) {
  s.DeserializeArray<byte>(m_MMU, 0x10000);
  s.DeserializeArray<byte>(m_VRAM, 0x4000);
  s.DeserializeArray<byte>(m_WRAM, 0x8000);
  s.DeserializeArray<byte>(m_PaletteData, 0x80);
  currentROMBank = s.Deserialize<int>();
  currentRAMBank = s.Deserialize<byte>();
  currentWRAMBank = s.Deserialize<byte>();
  ramEnabled = s.Deserialize<bool>();
  ramBankingMode = s.Deserialize<bool>();
  MBCMode = s.Deserialize<byte>();
  HDMAActive = s.Deserialize<bool>();
  HDMASrc = s.Deserialize<word>();
  HDMADest = s.Deserialize<word>();
  HDMALength = s.Deserialize<int>();
}

byte Memory::ReadByte(const word& address) {
  if (address <= 0x3FFF) {
    return m_Cartridge[address];
  }
  if (address >= 0x4000 && address <= 0x7FFF) {
    return m_Cartridge[address + (currentROMBank - 1) * 0x4000];
  }
  if (address >= 0x8000 && address <= 0x9FFF) {
    byte vramBank = m_MMU[0xFF4F] & 1;
    if (!gb->CGBModeEnabled()) vramBank = 0;
    return m_VRAM[address - 0x8000 + vramBank * 0x2000];
  }
  if (address >= 0xA000 && address <= 0xBFFF && ramEnabled) {
    if (MBCMode == 3 && currentRAMBank > 3) return 0;
    return m_CartridgeRAM[address - 0xA000 + currentRAMBank * 0x2000];
  }
  if (address >= 0xC000 && address <= 0xCFFF) {
    return m_WRAM[address - 0xC000];
  }
  if (address >= 0xD000 && address <= 0xDFFF) {
    return m_WRAM[address - 0xD000 + currentWRAMBank * 0x1000];
  }
  if (address >= 0xE000 && address <= 0xFDFF) {
    return ReadByte(address - 0x2000);
  }
  if (address == 0xFF00) {
    return controller.GetInput();
  }
  if (address == 0xFF26) {
    // Temporary workaround for some games that don't work without sound
    // TODO: implement proper sound support
    return 0;
  }
  if (address == 0xFF69 && gb->CGBModeEnabled()) {
    byte index = m_MMU[0xFF68] & 0x3F;
    return m_PaletteData[index];
  }
  if (address == 0xFF6B && gb->CGBModeEnabled()) {
    byte index = m_MMU[0xFF6A] & 0x3F;
    return m_PaletteData[index];
  }
  if (address == 0xFF70 && gb->CGBModeEnabled()) {
    return currentWRAMBank | ~0x7;
  }
  return address <= 0xFFFF ? m_MMU[address] : 0;
}

word Memory::ReadWord(const word & address) {
  word ret = ReadByte(address);
  ret |= (ReadByte(address + 1) << 8);
  return ret;
}

void Memory::WriteByte(const word & address, const byte & val) {
  if (address <= 0x7FFF) {
    ChangeBanks(address, val);
    return;
  }
  if (address >= 0x8000 && address <= 0x9FFF) {
    byte vramBank = m_MMU[0xFF4F] & 1;
    if (!gb->CGBModeEnabled()) vramBank = 0;
    m_VRAM[address - 0x8000 + vramBank * 0x2000] = val;
    return;
  }
  if (address >= 0xA000 && address <= 0xBFFF) {
    if (!ramEnabled) return;
    if (MBCMode == 3 && currentRAMBank > 3) return;
    m_CartridgeRAM[address - 0xA000 + currentRAMBank * 0x2000] = val;
    return;
  }
  if (address >= 0xC000 && address <= 0xCFFF) {
    m_WRAM[address - 0xC000] = val;
    return;
  }
  if (address >= 0xD000 && address <= 0xDFFF) {
    m_WRAM[address - 0xD000 + currentWRAMBank * 0x1000] = val;
    return;
  }
  if (address >= 0xE000 && address <= 0xFDFF) {
    WriteByte(address - 0x2000, val);
    return;
  }
  if (address >= 0xFEA0 && address <= 0xFEFF) {
    // Not usable
    return;
  }
  if (address == 0xFF00) {
    controller.SelectInput(val);
  }
  // redirect serial port to standard output
  if (address == 0xFF02 && val == 0x81) {
    std::cout << (char)(m_MMU[0xFF01]);
  }
  if (address == 0xFF04 || address == 0xFF44) {
    m_MMU[address] = 0;
    return;
  }
  if (address == 0xFF46) {
    word DMAAddr = val << 8;
    for (int i = 0; i < 0xA0; i++)
      WriteByte(0xFE00 + i, ReadByte(DMAAddr + i));
  }
  if (address == 0xFF55 && gb->CGBModeEnabled()) {
    // TODO: CGB DMA timings
    InitiateDMATransfer(val);
    return;
  }
  if (address == 0xFF69 && gb->CGBModeEnabled()) {
    byte currIndex = m_MMU[0xFF68] & 0x3F;
    m_PaletteData[currIndex] = val;
    if (m_MMU[0xFF68] & (1 << 7)) {
      // Auto increment index
      if (++currIndex == 0x40) currIndex = 0;
      m_MMU[0xFF68] = (1 << 7) | currIndex;
    }
    return;
  }
  if (address == 0xFF6B && gb->CGBModeEnabled()) {
    byte currIndex = m_MMU[0xFF6A] & 0x3F;
    m_PaletteData[0x40 + currIndex] = val;
    if (m_MMU[0xFF6A] & (1 << 7)) {
      // Auto increment index
      if (++currIndex == 0x40) currIndex = 0;
      m_MMU[0xFF6A] = (1 << 7) | currIndex;
    }
    return;
  }
  if (address == 0xFF70 && gb->CGBModeEnabled()) {
    currentWRAMBank = val & 0x7;
    if (currentWRAMBank == 0) currentWRAMBank = 1;
  }
  m_MMU[address] = val;
}

void Memory::WriteWord(const word & address, const word & val) {
  WriteByte(address, val & 0xFF);
  WriteByte(address + 1, val >> 8);
}

void Memory::WriteByteDirect(const word & address, const byte & data) {
  m_MMU[address] = data;
}

byte Memory::ReadVRAM(const word & address, const int & bankNumber) {
  return m_VRAM[address - 0x8000 + bankNumber * 0x2000];
}

word Memory::GetPaletteData(const word & address) {
  return m_PaletteData[address] | (m_PaletteData[address + 1] << 8);
}

void Memory::InitiateDMATransfer(const byte & params) {
  word srcAddr = (m_MMU[0xFF52] & 0xF0) | (m_MMU[0xFF51] << 8);
  word destAddr = (m_MMU[0xFF54] & 0xF0) | (((m_MMU[0xFF53] & 0x1F) | (1 << 7)) << 8);
  int transferLength = ((params & 0x7F) + 1) * 0x10;
  if (HDMAActive && !(params & (1 << 7))) {
    // Abort HDMA transfer
    HDMAActive = false;
    m_MMU[0xFF55] = (1 << 7);
  }
  if (params & (1 << 7)) {
    HDMAActive = true;
    HDMASrc = srcAddr;
    HDMADest = destAddr;
    HDMALength = transferLength;
    m_MMU[0xFF55] = params & 0x7F;
  }
  else {
    for (int i = 0; i < transferLength; i++) {
      WriteByte(destAddr + i, ReadByte(srcAddr + i));
    }
    m_MMU[0xFF55] = 0xFF;
  }
}

void Memory::HandleHBlank() {
  if (!HDMAActive) return;
  for (int i = 0; i < 0x10; i++) {
    WriteByte(HDMADest++, ReadByte(HDMASrc++));
  }
  HDMALength -= 0x10;
  m_MMU[0xFF55] = (HDMALength / 0x10 - 1);
  if (!HDMALength) {
    HDMAActive = false;
    m_MMU[0xFF55] = 0xFF;
  }
}
