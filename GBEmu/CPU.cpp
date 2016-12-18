#include "CPU.h"

CPU::CPU() {
	registers[0] = 0x0; // B
	registers[1] = 0x13; // C
	registers[2] = 0x0; // D
	registers[3] = 0xD8; // E
	registers[4] = 0x1; // H
	registers[5] = 0x4D; // L
	registers[6] = 0x1; // A
	registers[7] = 0xB0; // F

	SP = 0xFFFE;
	PC = 0x0;
	
	MMU = new Memory();
}

word CPU::CombineRegisters(const byte & r1_id, const byte & r2_id) {
	return (registers[r1_id] << 8) | registers[r2_id];
}

void CPU::SplitIntoRegisters(const word & val, const byte & r1_id, const byte & r2_id) {
	registers[r1_id] = (val & 0xFF00) >> 8;
	registers[r2_id] = val & 0xFF;
}

byte CPU::ReadByte() {
	byte ret = MMU->ReadByte(PC);
	PC++;
	return ret;
}

word CPU::ReadWord() {
	word ret = MMU->ReadWord(PC);
	PC += 2;
	return ret;
}

byte CPU::GetFlag(const byte & flag) {
	return (registers[7] & (1 << flag)) ? 1 : 0;
}

void CPU::SetFlag(const byte & flag) {
	registers[7] |= 1 << flag;
}

void CPU::ResetFlag(const byte & flag) {
	registers[7] &= ~(1 << flag);
}

byte CPU::AddBytes(const byte & b1, const byte & b2) {
	auto result = b1 + b2;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	if ((result & 0x100) == 0x100) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if ((((b1 & 0xF) + (b2 & 0xF)) & 0x10) == 0x10) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	return result;
}

word CPU::AddWords(const word & w1, const word & w2) {
	auto result = w1 + w2;
	ResetFlag(FLAG_SUBTRACT);
	if ((result & 0x10000) == 0x10000) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if ((((w1 & 0xFFF) + (w2 & 0xFFF)) & 0x1000) == 0x1000) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	return result;
}

byte CPU::SubtractBytes(const byte & b1, const byte & b2) {
	byte result = b1 - b2;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	SetFlag(FLAG_SUBTRACT);
	if (b1 < b2) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (((b1 & 0xF) < (b2 & 0xF))) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	return result;
}

word CPU::SubtractWords(const word & w1, const word & w2) {
	word result = w1 - w2;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	SetFlag(FLAG_SUBTRACT);
	if (w1 < w2) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (((w1 & 0xFFF) < (w2 & 0xFFF))) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	return result;
}

byte CPU::ANDBytes(const byte & b1, const byte & b2) {
	byte result = b1 & b2;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	SetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_CARRY);
	return result;
}

byte CPU::ORBytes(const byte & b1, const byte & b2) {
	byte result = b1 | b2;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_CARRY);
	return result;
}

byte CPU::XORBytes(const byte & b1, const byte & b2) {
	byte result = b1 ^ b2;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_CARRY);
	return result;
}

void CPU::CPBytes(const byte & b1, const byte & b2) {
	SubtractBytes(b1, b2);
	// TODO: not sure about this one, docs say to set H and C flags if no borrow
}

byte CPU::INCByte(const byte & b) {
	byte result = b + 1;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	if (b == 0xF) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	return result;
}

byte CPU::DECByte(const byte & b) {
	byte result = b - 1;
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	SetFlag(FLAG_SUBTRACT);
	if ((b == 0xF) == 0) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	return result;
}

int CPU::LD_r_n(const byte & op_code) {
	byte r_id = op_code >> 3;
	registers[r_id] = ReadByte();
	return 8;
}

int CPU::LD_r1_r2(const byte & op_code) {
	byte r1_id = (op_code & 0x38) >> 3;
	byte r2_id = op_code & 0x7;
	registers[r1_id] = registers[r2_id];
	return 4;
}

int CPU::LD_r_HLm(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[r_id] = MMU->ReadByte(CombineRegisters(4, 5));
	return 8;
}

int CPU::LD_HLm_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	MMU->WriteByte(CombineRegisters(4, 5), registers[r_id]);
	return 8;
}

int CPU::LD_HLm_n(const byte & op_code) {
	MMU->WriteByte(CombineRegisters(4, 5), ReadByte());
	return 12;
}

int CPU::LD_A_BCm(const byte & op_code) {
	registers[6] = MMU->ReadByte(CombineRegisters(0, 1));
	return 8;
}

int CPU::LD_A_DEm(const byte & op_code) {
	registers[6] = MMU->ReadByte(CombineRegisters(2, 3));
	return 8;
}

int CPU::LD_A_nnm(const byte & op_code) {
	registers[6] = MMU->ReadByte(ReadWord());
	return 16;
}

int CPU::LD_BCm_A(const byte & op_code) {
	MMU->WriteByte(CombineRegisters(0, 1), registers[6]);
	return 8;
}

int CPU::LD_DEm_A(const byte & op_code) {
	MMU->WriteByte(CombineRegisters(2, 3), registers[6]);
	return 8;
}

int CPU::LD_nnm_A(const byte & op_code) {
	MMU->WriteByte(ReadWord(), registers[6]);
	return 16;
}

int CPU::LD_A_Cm(const byte & op_code) {
	registers[6] = MMU->ReadByte(0xFF00 + registers[1]);
	return 8;
}

int CPU::LD_Cm_A(const byte & op_code) {
	MMU->WriteByte(0xFF00 + registers[1], registers[6]);
	return 8;
}

int CPU::LDD_A_HLm(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	registers[6] = MMU->ReadByte(HL);
	SplitIntoRegisters(HL - 1, 4, 5);
	return 8;
}

int CPU::LDD_HLm_A(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	MMU->WriteByte(HL, registers[6]);
	SplitIntoRegisters(HL - 1, 4, 5);
	return 8;
}

int CPU::LDI_A_HLm(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	registers[6] = MMU->ReadByte(HL);
	SplitIntoRegisters(HL + 1, 4, 5);
	return 8;
}

int CPU::LDI_HLm_A(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	MMU->WriteByte(HL, registers[6]);
	SplitIntoRegisters(HL + 1, 4, 5);
	return 8;
}

int CPU::LDH_nm_A(const byte & op_code) {
	MMU->WriteByte(0xFF00 + ReadByte(), registers[6]);
	return 12;
}

int CPU::LDH_A_nm(const byte & op_code) {
	registers[6] = MMU->ReadByte(0xFF00 + ReadByte());
	return 12;
}

int CPU::LD_BC_nn(const byte & op_code) {
	word value = ReadWord();
	SplitIntoRegisters(value, 0, 1);
	return 12;
}

int CPU::LD_DE_nn(const byte & op_code) {
	word value = ReadWord();
	SplitIntoRegisters(value, 2, 3);
	return 12;
}

int CPU::LD_HL_nn(const byte & op_code) {
	word value = ReadWord();
	SplitIntoRegisters(value, 4, 5);
	return 12;
}

int CPU::LD_SP_nn(const byte & op_code) {
	SP = ReadWord();
	return 12;
}

int CPU::LD_SP_HL(const byte & op_code) {
	SP = CombineRegisters(4, 5);
	return 8;
}

int CPU::LDHL_SP_n(const byte & op_code) {
	sbyte val = ReadByte();
	auto result = SP + val;
	if ((result & 0x10000) == 0x10000) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (((SP & 0xFF + val) & 0x100) == 0x100) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	SplitIntoRegisters(result, 4, 5);
	return 12;
}

int CPU::LD_nnm_SP(const byte & op_code) {
	word addr = ReadWord();
	MMU->WriteWord(addr, SP);
	return 20;
}

int CPU::PUSH_AF(const byte & op_code) {
	MMU->WriteByte(SP--, registers[7]);
	MMU->WriteByte(SP--, registers[6]);
	return 16;
}

int CPU::PUSH_BC(const byte & op_code) {
	MMU->WriteByte(SP--, registers[1]);
	MMU->WriteByte(SP--, registers[0]);
	return 16;
}

int CPU::PUSH_DE(const byte & op_code) {
	MMU->WriteByte(SP--, registers[3]);
	MMU->WriteByte(SP--, registers[2]);
	return 16;
}

int CPU::PUSH_HL(const byte & op_code) {
	MMU->WriteByte(SP--, registers[5]);
	MMU->WriteByte(SP--, registers[4]);
	return 16;
}

int CPU::POP_AF(const byte & op_code) {
	registers[6] = MMU->ReadByte(SP++);
	registers[7] = MMU->ReadByte(SP++);
	return 12;
}

int CPU::POP_BC(const byte & op_code) {
	registers[0] = MMU->ReadByte(SP++);
	registers[1] = MMU->ReadByte(SP++);
	return 12;
}

int CPU::POP_DE(const byte & op_code) {
	registers[2] = MMU->ReadByte(SP++);
	registers[3] = MMU->ReadByte(SP++);
	return 12;
}

int CPU::POP_HL(const byte & op_code) {
	registers[4] = MMU->ReadByte(SP++);
	registers[5] = MMU->ReadByte(SP++);
	return 12;
}

int CPU::ADD_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[6] = AddBytes(registers[6], registers[r_id]);
	return 4;
}

int CPU::ADD_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[6] = AddBytes(registers[6], val);
	return 8;
}

int CPU::ADD_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[6] = AddBytes(registers[6], val);
	return 8;
}

int CPU::ADC_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	registers[6] = AddBytes(registers[6], registers[r_id] + GetFlag(FLAG_CARRY));
	return 4;
}

int CPU::ADC_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5)) + GetFlag(FLAG_CARRY);
	registers[6] = AddBytes(registers[6], val);
	return 8;
}

int CPU::ADC_A_n(const byte & op_code) {
	byte val = ReadByte() + GetFlag(FLAG_CARRY);
	registers[6] = AddBytes(registers[6], val);
	return 8;
}

int CPU::SUB_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[6] = SubtractBytes(registers[6], registers[r_id]);
	return 4;
}

int CPU::SUB_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[6] = SubtractBytes(registers[6], val);
	return 8;
}

int CPU::SUB_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[6] = SubtractBytes(registers[6], val);
	return 8;
}

int CPU::SBC_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	registers[6] = SubtractBytes(registers[6], registers[r_id] + GetFlag(FLAG_CARRY));
	return 4;
}

int CPU::SBC_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5)) + GetFlag(FLAG_CARRY);
	registers[6] = SubtractBytes(registers[6], val);
	return 8;
}

int CPU::SBC_A_n(const byte & op_code) {
	byte val = ReadByte() + GetFlag(FLAG_CARRY);
	registers[6] = SubtractBytes(registers[6], val);
	return 8;
}

int CPU::AND_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[6] = ANDBytes(registers[6], registers[r_id]);
	return 4;
}

int CPU::AND_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[6] = ANDBytes(registers[6], val);
	return 8;
}

int CPU::AND_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[6] = ANDBytes(registers[6], val);
	return 8;
}

int CPU::OR_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[6] = ORBytes(registers[6], registers[r_id]);
	return 4;
}

int CPU::OR_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[6] = ORBytes(registers[6], val);
	return 8;
}

int CPU::OR_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[6] = ORBytes(registers[6], val);
	return 8;
}

int CPU::XOR_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	registers[6] = XORBytes(registers[6], registers[r_id]);
	return 4;
}

int CPU::XOR_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[6] = XORBytes(registers[6], val);
	return 8;
}

int CPU::XOR_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[6] = XORBytes(registers[6], val);
	return 8;
}

int CPU::CP_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	CPBytes(registers[6], registers[r_id]);
	return 4;
}

int CPU::CP_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	CPBytes(registers[6], val);
	return 8;
}

int CPU::CP_A_n(const byte & op_code) {
	byte val = ReadByte();
	CPBytes(registers[6], val);
	return 8;
}

int CPU::INC_r(const byte & op_code) {
	byte r_id = (op_code >> 3) & 0x7;
	registers[r_id] = INCByte(registers[r_id]);
	return 4;
}

int CPU::INC_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(INCByte(val), CombineRegisters(4, 5));
	return 12;
}

int CPU::DEC_r(const byte & op_code) {
	byte r_id = (op_code >> 3) & 0x7;
	registers[r_id] = DECByte(registers[r_id]);
	return 4;
}

int CPU::DEC_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(DECByte(val), CombineRegisters(4, 5));
	return 12;
}

int CPU::ADD_HL_BC(const byte & op_code) {
	word result = AddWords(CombineRegisters(4, 5), CombineRegisters(0, 1));
	SplitIntoRegisters(result, 4, 5);
	return 8;
}

int CPU::ADD_HL_DE(const byte & op_code) {
	word result = AddWords(CombineRegisters(4, 5), CombineRegisters(2, 3));
	SplitIntoRegisters(result, 4, 5);
	return 8;
}

int CPU::ADD_HL_HL(const byte & op_code) {
	word result = AddWords(CombineRegisters(4, 5), CombineRegisters(4, 5));
	SplitIntoRegisters(result, 4, 5);
	return 8;
}

int CPU::ADD_HL_SP(const byte & op_code) {
	word result = AddWords(CombineRegisters(4, 5), SP);
	SplitIntoRegisters(result, 4, 5);
	return 8;
}

int CPU::ADD_SP_n(const byte & op_code) {
	sbyte val = ReadByte();
	word result = (int)SP + val;
	ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	if ((result & 0xF) < (SP & 0xF)) SetFlag(FLAG_HALF_CARRY);
	else ResetFlag(FLAG_HALF_CARRY);
	if ((result & 0xFF) < (SP & 0xFF)) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	return 16;
}

int CPU::INC_BC(const byte & op_code) {
	SplitIntoRegisters(CombineRegisters(0, 1) + 1, 0, 1);
	return 8;
}

int CPU::INC_DE(const byte & op_code) {
	SplitIntoRegisters(CombineRegisters(2, 3) + 1, 2, 3);
	return 8;
}

int CPU::INC_HL(const byte & op_code) {
	SplitIntoRegisters(CombineRegisters(4, 5) + 1, 4, 5);
	return 8;
}

int CPU::INC_SP(const byte & op_code) {
	SP++;
	return 8;
}

int CPU::DEC_BC(const byte & op_code) {
	SplitIntoRegisters(CombineRegisters(0, 1) - 1, 0, 1);
	return 8;
}

int CPU::DEC_DE(const byte & op_code) {
	SplitIntoRegisters(CombineRegisters(2, 3) - 1, 2, 3);
	return 8;
}

int CPU::DEC_HL(const byte & op_code) {
	SplitIntoRegisters(CombineRegisters(4, 5) - 1, 4, 5);
	return 8;
}

int CPU::DEC_SP(const byte & op_code) {
	SP--;
	return 8;
}