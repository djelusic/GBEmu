#pragma once
#include "Types.h"
#include "Controller.h"
#include <iostream>

class Memory {

private:
	byte *m_MMU;
	byte *m_Cartridge;
	byte *m_CartridgeRAM;

	byte currentROMBank;
	byte currentRAMBank;

	bool ramEnabled;
	bool ramBankingMode;
	byte MBCMode;

	Controller &controller;

	void LoadCartridge(const char* fname);
	void ToggleRAMEnabled(const word& address, const byte& val);
	void ChangeBanks(const word& address, const byte& val);

public:
	Memory(const char* rom_fname, Controller &controller);
	~Memory();
	
	byte ReadByte(const word& address);
	word ReadWord(const word& address);
	void WriteByte(const word& address, const byte& val);
	void WriteWord(const word& address, const word& val);
	void WriteByteDirect(const word& address, const byte& data);
};