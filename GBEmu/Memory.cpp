#include "Memory.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>

void Memory::LoadCartridge(const char* fname) {
	std::ifstream input(fname, std::ios::binary);
	std::vector<byte> buffer((
		std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
	int j = 0;
	for (auto i = buffer.begin(); i != buffer.end(); i++, j++)
		m_Cartridge[j] = (byte)*i;
}

void Memory::ToggleRAMEnabled(const word& address, const byte & val) {
	if (MBCMode == 2 && (address & 0x100) != 0) return;
	if (MBCMode == 1 || MBCMode == 2) {
		if ((val & 0xF) == 0xA) ramEnabled = true;
		else ramEnabled = false;
	}
}

void Memory::ChangeBanks(const word & address, const byte & val) {
	if (address >= 0x0 && address <= 0x1FFF) {
		ToggleRAMEnabled(address, val);
	}
	if (address >= 0x2000 && address <= 0x3FFF) {
		currentROMBank = (currentROMBank & 0xE0) | (val & 0x1F);
		if ((currentROMBank & 0x1) == 0) currentROMBank ++;
		// TODO: what if MBC is 0??
	}
	if (address >= 0x4000 && address <= 0x5FFF) {
		if (MBCMode == 1) {
			if (ramBankingMode) currentRAMBank = val & 0x3;
			else {
				currentROMBank = (currentROMBank & 0x1F) | (val & 0xE0);
				if ((currentROMBank & 0x1) == 0) currentROMBank++;
			}
		}
	}
	if (address >= 0x6000 && address <= 0x7FFF) {
		if ((val & 0x1) == 1) ramBankingMode = true;
		else ramBankingMode = false;
	}
}

Memory::Memory(const char* rom_fname) {
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
	m_MMU[0xFFFF] = 0x00;

	currentROMBank = 1;
	currentRAMBank = 0;

	ramEnabled = false;
	ramBankingMode = false;
	MBCMode = 0;

	LoadCartridge(rom_fname);

	byte MBCVal = m_Cartridge[0x147];
	if (MBCVal > 0 && MBCVal <= 3) {
		MBCMode = 1;
	}
	if (MBCVal > 3 && MBCVal <= 6) {
		MBCMode = 2;
	}
}

byte Memory::ReadByte(const word& address) {
	if (address <= 0x3FFF) {
		return m_Cartridge[address];
	}
	if (address >= 0x4000 && address <= 0x7FFF) {
		return m_Cartridge[address + (currentROMBank - 1) * 0x4000];
	}
	if (address >= 0xA000 && address <= 0x9FFF) {
		return m_CartridgeRAM[address + currentRAMBank * 0x2000];
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
	if (address >= 0xA000 && address <= 0xBFFF && ramEnabled) {
		m_CartridgeRAM[address - 0xA000 + currentRAMBank * 0x2000] = val;
		return;
	}
	if (address >= 0xE000 && address <= 0xFDFF) {
		m_MMU[address] = val;
		m_MMU[address - 0x2000] = val;
		return;
	}
	if (address >= 0xFEA0 && address <= 0xFEFF) {
		// Not usable
		return;
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
	m_MMU[address] = val;
}

void Memory::WriteWord(const word & address, const word & val) {
	WriteByte(address, val & 0xFF);
	WriteByte(address + 1, val >> 8);
}

void Memory::WriteByteDirect(const word & address, const byte & data) {
	m_MMU[address] = data;
}