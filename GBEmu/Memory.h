#pragma once
#include "Types.h"
#include "Controller.h"
#include <iostream>
#include <fstream>

class GB;

class Memory {

private:
	byte *m_MMU;
	byte *m_Cartridge;
	byte *m_CartridgeRAM;
	byte *m_VRAM;
	byte *m_WRAM;
	byte *m_PaletteData;
	GB *gb;

	const char *rom_fname;

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

	void LoadCartridge(const char* fname);
	void ToggleRAMEnabled(const word& address, const byte& val);
	void ChangeBanks(const word& address, const byte& val);

public:
	Memory(GB *gb, const char* rom_fname, Controller &controller);
	~Memory();
	
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