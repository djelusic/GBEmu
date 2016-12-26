#pragma once
#include "Types.h"
#include <iostream>

class Memory {

private:
	byte m_MMU[0x10000];
	byte m_Cartridge[0x200000];
	byte m_CartridgeRAM[0x8000];

	byte currentROMBank;
	byte currentRAMBank;

	bool ramEnabled;
	bool ramBankingMode;
	byte MBCMode;

	void LoadCartridge(const char* fname);
	void ToggleRAMEnabled(const word& address, const byte& val);
	void ChangeBanks(const word& address, const byte& val);

public:
	Memory(const char* rom_fname);
	
	byte ReadByte(const word& address);
	word ReadWord(const word& address);
	void WriteByte(const word& address, const byte& val);
	void WriteWord(const word& address, const word& val);
	void WriteByteDirect(const word& address, const byte& data);
};