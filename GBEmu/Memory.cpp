#include "Memory.h"
#include "GB.h"
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
	if (MBCMode == 1 || MBCMode == 2 || MBCMode == 3) {
		if ((val & 0xF) == 0xA) ramEnabled = true;
		else ramEnabled = false;
	}
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
			currentROMBank = val ? val : 1;
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
		if (MBCMode == 3) {
			currentRAMBank = val & 0x3;
		}
	}
	if (address >= 0x6000 && address <= 0x7FFF) {
		if (MBCMode == 1 || MBCMode == 2) {
			if ((val & 0x1) == 1) ramBankingMode = true;
			else ramBankingMode = false;
		}
	}
}

Memory::Memory(GB *gb, const char* rom_fname, Controller &controller) : 
	gb(gb), 
	controller(controller) {
	m_MMU = new byte[0x10000]{};
	m_Cartridge = new byte[0x200000]{};
	m_CartridgeRAM = new byte[0x8000]{};
	m_VRAM = new byte[0x4000]{};
	m_PaletteData = new byte[0x80]{};
	
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
	// Pokemon TEST
	if (MBCVal == 0x13 || MBCVal == 0x10) {
		MBCMode = 3;
	}

	byte CGBEnable = m_Cartridge[0x143];
	if (CGBEnable == 0x80 || CGBEnable == 0xC0) {
		gb->SetCGBMode(true);
	}
}

Memory::~Memory() {
	delete[] m_MMU;
	delete[] m_Cartridge;
	delete[] m_CartridgeRAM;
	delete[] m_VRAM;
	delete[] m_PaletteData;
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
		return m_CartridgeRAM[address - 0xA000 + currentRAMBank * 0x2000];
	}
	if (address == 0xFF00) {
		return controller.GetInput();
	}
	if (address == 0xFF69 && gb->CGBModeEnabled()) {
		byte index = m_MMU[0xFF68] & 0x3F;
		return m_PaletteData[index];
	}
	if (address == 0xFF6B && gb->CGBModeEnabled()) {
		byte index = m_MMU[0xFF6A] & 0x3F;
		return m_PaletteData[index];
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
