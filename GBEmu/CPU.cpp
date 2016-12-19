#include "CPU.h"

CPU::CPU() {
	registers[0] = 0x0; // B
	registers[1] = 0x13; // C
	registers[2] = 0x0; // D
	registers[3] = 0xD8; // E
	registers[4] = 0x1; // H
	registers[5] = 0x4D; // L
	registers[6] = 0xB0; // F
	registers[7] = 0x1; // A

	SP = 0xFFFE;
	PC = 0x0;

	halted = false;
	interruptsEnabled = true;
	
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
	return (registers[6] & (1 << flag)) ? 1 : 0;
}

void CPU::SetFlag(const byte & flag) {
	registers[6] |= 1 << flag;
}

void CPU::ResetFlag(const byte & flag) {
	registers[6] &= ~(1 << flag);
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

byte CPU::RotateLeft(const byte & b, const byte & addCarry) {
	byte result = (b << 1);
	if (addCarry) result |= GetFlag(FLAG_CARRY);
	else result |= (b >> 7);
	if (b & (1 << 7)) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	// TODO: not sure what to do with the zero flag here, docs seem conflicted
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	return result;
}

byte CPU::RotateRight(const byte & b, const bool & addCarry) {
	byte result = (b >> 1);
	if (addCarry) result |= GetFlag(FLAG_CARRY) << 7;
	else result |= (b << 7);
	if (b & 1) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	// TODO: not sure what to do with the zero flag here, docs seem conflicted
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
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
	registers[7] = MMU->ReadByte(CombineRegisters(0, 1));
	return 8;
}

int CPU::LD_A_DEm(const byte & op_code) {
	registers[7] = MMU->ReadByte(CombineRegisters(2, 3));
	return 8;
}

int CPU::LD_A_nnm(const byte & op_code) {
	registers[7] = MMU->ReadByte(ReadWord());
	return 16;
}

int CPU::LD_BCm_A(const byte & op_code) {
	MMU->WriteByte(CombineRegisters(0, 1), registers[7]);
	return 8;
}

int CPU::LD_DEm_A(const byte & op_code) {
	MMU->WriteByte(CombineRegisters(2, 3), registers[7]);
	return 8;
}

int CPU::LD_nnm_A(const byte & op_code) {
	MMU->WriteByte(ReadWord(), registers[7]);
	return 16;
}

int CPU::LD_A_Cm(const byte & op_code) {
	registers[7] = MMU->ReadByte(0xFF00 + registers[1]);
	return 8;
}

int CPU::LD_Cm_A(const byte & op_code) {
	MMU->WriteByte(0xFF00 + registers[1], registers[7]);
	return 8;
}

int CPU::LDD_A_HLm(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	registers[7] = MMU->ReadByte(HL);
	SplitIntoRegisters(HL - 1, 4, 5);
	return 8;
}

int CPU::LDD_HLm_A(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	MMU->WriteByte(HL, registers[7]);
	SplitIntoRegisters(HL - 1, 4, 5);
	return 8;
}

int CPU::LDI_A_HLm(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	registers[7] = MMU->ReadByte(HL);
	SplitIntoRegisters(HL + 1, 4, 5);
	return 8;
}

int CPU::LDI_HLm_A(const byte & op_code) {
	word HL = CombineRegisters(4, 5);
	MMU->WriteByte(HL, registers[7]);
	SplitIntoRegisters(HL + 1, 4, 5);
	return 8;
}

int CPU::LDH_nm_A(const byte & op_code) {
	MMU->WriteByte(0xFF00 + ReadByte(), registers[7]);
	return 12;
}

int CPU::LDH_A_nm(const byte & op_code) {
	registers[7] = MMU->ReadByte(0xFF00 + ReadByte());
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
	MMU->WriteByte(SP--, registers[6]);
	MMU->WriteByte(SP--, registers[7]);
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
	registers[7] = MMU->ReadByte(SP++);
	registers[6] = MMU->ReadByte(SP++);
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
	registers[7] = AddBytes(registers[7], registers[r_id]);
	return 4;
}

int CPU::ADD_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[7] = AddBytes(registers[7], val);
	return 8;
}

int CPU::ADD_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[7] = AddBytes(registers[7], val);
	return 8;
}

int CPU::ADC_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	registers[7] = AddBytes(registers[7], registers[r_id] + GetFlag(FLAG_CARRY));
	return 4;
}

int CPU::ADC_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5)) + GetFlag(FLAG_CARRY);
	registers[7] = AddBytes(registers[7], val);
	return 8;
}

int CPU::ADC_A_n(const byte & op_code) {
	byte val = ReadByte() + GetFlag(FLAG_CARRY);
	registers[7] = AddBytes(registers[7], val);
	return 8;
}

int CPU::SUB_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[7] = SubtractBytes(registers[7], registers[r_id]);
	return 4;
}

int CPU::SUB_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[7] = SubtractBytes(registers[7], val);
	return 8;
}

int CPU::SUB_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[7] = SubtractBytes(registers[7], val);
	return 8;
}

int CPU::SBC_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	registers[7] = SubtractBytes(registers[7], registers[r_id] + GetFlag(FLAG_CARRY));
	return 4;
}

int CPU::SBC_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5)) + GetFlag(FLAG_CARRY);
	registers[7] = SubtractBytes(registers[7], val);
	return 8;
}

int CPU::SBC_A_n(const byte & op_code) {
	byte val = ReadByte() + GetFlag(FLAG_CARRY);
	registers[7] = SubtractBytes(registers[7], val);
	return 8;
}

int CPU::AND_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[7] = ANDBytes(registers[7], registers[r_id]);
	return 4;
}

int CPU::AND_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[7] = ANDBytes(registers[7], val);
	return 8;
}

int CPU::AND_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[7] = ANDBytes(registers[7], val);
	return 8;
}

int CPU::OR_A_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[7] = ORBytes(registers[7], registers[r_id]);
	return 4;
}

int CPU::OR_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[7] = ORBytes(registers[7], val);
	return 8;
}

int CPU::OR_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[7] = ORBytes(registers[7], val);
	return 8;
}

int CPU::XOR_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	registers[7] = XORBytes(registers[7], registers[r_id]);
	return 4;
}

int CPU::XOR_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	registers[7] = XORBytes(registers[7], val);
	return 8;
}

int CPU::XOR_A_n(const byte & op_code) {
	byte val = ReadByte();
	registers[7] = XORBytes(registers[7], val);
	return 8;
}

int CPU::CP_A_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	CPBytes(registers[7], registers[r_id]);
	return 4;
}

int CPU::CP_A_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	CPBytes(registers[7], val);
	return 8;
}

int CPU::CP_A_n(const byte & op_code) {
	byte val = ReadByte();
	CPBytes(registers[7], val);
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

int CPU::SWAP_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	byte lower = registers[r_id] & 0xF;
	byte upper = registers[r_id] & 0xF0;
	registers[r_id] = (lower << 4) | (upper >> 4);
	if (registers[r_id] == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_CARRY);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	return 8;
}

int CPU::SWAP_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	byte lower = val & 0xF;
	byte upper = val & 0xF0;
	MMU->WriteByte(CombineRegisters(4, 5), (lower << 4) | (upper >> 4));
	if (val == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_CARRY);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	return 16;
}

int CPU::DAA(const byte & op_code) {
	byte A = registers[7];
	if (!GetFlag(FLAG_SUBTRACT)) {
		if (GetFlag(FLAG_HALF_CARRY) || (A & 0xF) > 9) A += 0x6;
		if (GetFlag(FLAG_CARRY) || (A > 0x9F)) A += 0x60;
	}
	else {
		if (GetFlag(FLAG_HALF_CARRY)) A = (A - 0x6) & 0xFF;
		if (GetFlag(FLAG_CARRY)) A = A - 0x60;
	}
	ResetFlag(FLAG_HALF_CARRY);
	if ((A & 0x100) == 0x100) SetFlag(FLAG_CARRY);
	A &= 0xFF;
	if (A == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	registers[7] = A;
	return 4;
}

int CPU::CPL(const byte & op_code) {
	registers[7] = ~registers[7];
	SetFlag(FLAG_SUBTRACT);
	SetFlag(FLAG_HALF_CARRY);
	return 4;
}

int CPU::CCF(const byte & op_code) {
	if (GetFlag(FLAG_CARRY)) ResetFlag(FLAG_CARRY);
	else SetFlag(FLAG_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	ResetFlag(FLAG_HALF_CARRY);
	return 4;
}

int CPU::SCF(const byte & op_code) {
	SetFlag(FLAG_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	ResetFlag(FLAG_HALF_CARRY);
	return 4;
}

int CPU::NOP(const byte & op_code) {
	return 4;
}

int CPU::HALT(const byte & op_code) {
	halted = true;
	return 4;
}

int CPU::DI(const byte & op_code) {
	interruptsEnabled = false;
	return 4;
}

int CPU::EI(const byte & op_code) {
	interruptsEnabled = true;
	return 4;
}

int CPU::RLCA(const byte & op_code) {
	registers[7] = RotateLeft(registers[7], false);
	return 4;
}

int CPU::RLA(const byte & op_code) {
	registers[7] = RotateLeft(registers[7], true);
	return 4;
}

int CPU::RRCA(const byte & op_code) {
	registers[7] = RotateRight(registers[7], false);
	return 4;
}

int CPU::RRA(const byte & op_code) {
	registers[7] = RotateRight(registers[7], true);
	return 4;
}

int CPU::RLC_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[r_id] = RotateLeft(registers[r_id], false);
	return 8;
}

int CPU::RLC_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(CombineRegisters(4, 5), RotateLeft(val, false));
	return 16;
}

int CPU::RL_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[r_id] = RotateLeft(registers[r_id], true);
	return 8;
}

int CPU::RL_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(CombineRegisters(4, 5), RotateLeft(val, true));
	return 16;
}

int CPU::RRC_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[r_id] = RotateRight(registers[r_id], false);
	return 8;
}

int CPU::RRC_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(CombineRegisters(4, 5), RotateRight(val, false));
	return 16;
}
int CPU::RR_r(const byte & op_code) {
	byte r_id = op_code & 0x7;
	registers[r_id] = RotateRight(registers[r_id], true);
	return 8;
}

int CPU::RR_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(CombineRegisters(4, 5), RotateRight(val, true));
	return 16;
}

int CPU::SRA_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	byte result = (registers[r_id] >> 1) | (registers[r_id] & 0x80);
	if (registers[r_id] & 1) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	registers[r_id] = result;
	return 8;
}

int CPU::SRA_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	byte result = (val >> 1) | (val & 0x80);
	if (val & 1) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	MMU->WriteByte(CombineRegisters(4, 5), val);
	return 16;
}

int CPU::SRL_r(const byte & op_code) {
	byte r_id = (op_code >> 1) & 0x7;
	byte result = (registers[r_id] << 1) & 0x7F;
	if (registers[r_id] & 0x80) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	registers[r_id] = result;
	return 8;
}

int CPU::SRL_HLm(const byte & op_code) {
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	byte result = (val << 1) & 0x7F;
	if (val & 0x80) SetFlag(FLAG_CARRY);
	else ResetFlag(FLAG_CARRY);
	if (result == 0) SetFlag(FLAG_ZERO);
	else ResetFlag(FLAG_ZERO);
	ResetFlag(FLAG_HALF_CARRY);
	ResetFlag(FLAG_SUBTRACT);
	MMU->WriteByte(CombineRegisters(4, 5), val);
	return 16;
}

int CPU::BIT_b_r(const byte & op_code) {
	byte b = (op_code >> 3) & 0x7;
	byte r_id = op_code & 0x7;
	if (registers[r_id] & (1 << b)) ResetFlag(FLAG_ZERO);
	else SetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	ResetFlag(FLAG_HALF_CARRY);
	return 8;
}

int CPU::BIT_b_HLm(const byte & op_code) {
	byte b = (op_code >> 3) & 0x7;
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	if (val & (1 << b)) ResetFlag(FLAG_ZERO);
	else SetFlag(FLAG_ZERO);
	ResetFlag(FLAG_SUBTRACT);
	ResetFlag(FLAG_HALF_CARRY);
	return 16;
}

int CPU::SET_b_r(const byte & op_code) {
	byte b = (op_code >> 3) & 0x7;
	byte r_id = op_code & 0x7;
	registers[r_id] |= 1 << b;
	return 8;
}

int CPU::SET_b_HLm(const byte & op_code) {
	byte b = (op_code >> 3) & 0x7;
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(CombineRegisters(4, 5), val | (1 << b));
	return 16;
}

int CPU::RES_b_r(const byte & op_code) {
	byte b = (op_code >> 3) & 0x7;
	byte r_id = op_code & 0x7;
	registers[r_id] &= ~(1 << b);
	return 8;
}

int CPU::RES_b_HLm(const byte & op_code) {
	byte b = (op_code >> 3) & 0x7;
	byte val = MMU->ReadByte(CombineRegisters(4, 5));
	MMU->WriteByte(CombineRegisters(4, 5), val & ~(1 << b));
	return 16;
}
