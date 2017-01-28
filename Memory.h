#pragma once
#include "Types.h"
#include "Controller.h"
#include <iostream>
#include <fstream>

class GB;
class Serializer;

class Memory {

private:
  byte *m_MMU;
  byte *m_Cartridge;
  byte *m_CartridgeRAM;
  byte *m_VRAM;
  byte *m_WRAM;
  byte *m_PaletteData;
  GB *gb;

  const char *rom_fpath;

  int currentROMBank;
  byte currentRAMBank;
  byte currentWRAMBank;

  bool ramEnabled;
  bool ramBankingMode;
  byte MBCMode;

  bool HDMAActive;
  word HDMASrc;
  word HDMADest;
  int HDMALength;

  Controller &controller;

  void ToggleRAMEnabled(const word& address, const byte& val);
  void ChangeBanks(const word& address, const byte& val);

public:
  Memory(GB *gb, Controller &controller);
  ~Memory();

  void Reset();

  void Serialize(Serializer & s);
  void Deserialize(Serializer & s);

  void LoadROM(const char* fname);

  byte ReadByte(const word& address);
  word ReadWord(const word& address);
  void WriteByte(const word& address, const byte& val);
  void WriteWord(const word& address, const word& val);
  void WriteByteDirect(const word& address, const byte& data);

  byte ReadVRAM(const word& address, const int& bankNumber);
  word GetPaletteData(const word& address);
  void InitiateDMATransfer(const byte& params);
  void HandleHBlank();

  void SaveRAM();
  void LoadRAM();
};