#pragma once
#include "Types.h"
#include "Memory.h"

#define FLAG_ZERO 7
#define FLAG_SUBTRACT 6
#define FLAG_HALF_CARRY 5
#define FLAG_CARRY 4

class CPU {

private:

	byte registers[8];
	word SP;
	word PC;
	Memory* MMU;

public:
	
	CPU();
	~CPU();

private:

	word CombineRegisters(const byte & r1_id, const byte & r2_id);
	void SplitIntoRegisters(const word & val, const byte & r1_id, const byte & r2_id);
	byte ReadByte();
	word ReadWord();
	byte GetFlag(const byte & flag);
	void SetFlag(const byte & flag);
	void ResetFlag(const byte & flag);
	
	byte AddBytes(const byte & b1, const byte & b2);
	word AddWords(const word & w1, const word & w2);
	byte SubtractBytes(const byte & b1, const byte & b2);
	word SubtractWords(const word & w1, const word & w2);
	byte ANDBytes(const byte & b1, const byte & b2);
	byte ORBytes(const byte & b1, const byte & b2);
	byte XORBytes(const byte & b1, const byte & b2);
	void CPBytes(const byte & b1, const byte & b2);
	byte INCByte(const byte & b);
	byte DECByte(const byte & b);

	int LD_r_n(const byte& op_code);
	int LD_r1_r2(const byte & op_code);
	int LD_r_HLm(const byte & op_code);
	int LD_HLm_r(const byte & op_code);
	int LD_HLm_n(const byte & op_code);
	int LD_A_BCm(const byte & op_code);
	int LD_A_DEm(const byte & op_code);
	int LD_A_nnm(const byte & op_code);
	int LD_BCm_A(const byte & op_code);
	int LD_DEm_A(const byte & op_code);
	int LD_nnm_A(const byte & op_code);
	int LD_A_Cm(const byte & op_code);
	int LD_Cm_A(const byte & op_code);
	int LDD_A_HLm(const byte & op_code);
	int LDD_HLm_A(const byte & op_code);
	int LDI_A_HLm(const byte & op_code);
	int LDI_HLm_A(const byte & op_code);
	int LDH_nm_A(const byte & op_code);
	int LDH_A_nm(const byte & op_code);
	int LD_BC_nn(const byte & op_code);
	int LD_DE_nn(const byte & op_code);
	int LD_HL_nn(const byte & op_code);
	int LD_SP_nn(const byte & op_code);
	int LD_SP_HL(const byte & op_code);
	int LDHL_SP_n(const byte & op_code);
	int LD_nnm_SP(const byte & op_code);
	int PUSH_AF(const byte & op_code);
	int PUSH_BC(const byte & op_code);
	int PUSH_DE(const byte & op_code);
	int PUSH_HL(const byte & op_code);
	int POP_AF(const byte & op_code);
	int POP_BC(const byte & op_code);
	int POP_DE(const byte & op_code);
	int POP_HL(const byte & op_code);
	int ADD_A_r(const byte & op_code);
	int ADD_A_HLm(const byte & op_code);
	int ADD_A_n(const byte & op_code);
	int ADC_A_r(const byte & op_code);
	int ADC_A_HLm(const byte & op_code);
	int ADC_A_n(const byte & op_code);
	int SUB_A_r(const byte & op_code);
	int SUB_A_HLm(const byte & op_code);
	int SUB_A_n(const byte & op_code);
	int SBC_A_r(const byte & op_code);
	int SBC_A_HLm(const byte & op_code);
	int SBC_A_n(const byte & op_code);
	int AND_A_r(const byte & op_code);
	int AND_A_HLm(const byte & op_code);
	int AND_A_n(const byte & op_code);
	int OR_A_r(const byte & op_code);
	int OR_A_HLm(const byte & op_code);
	int OR_A_n(const byte & op_code);
	int XOR_A_r(const byte & op_code);
	int XOR_A_HLm(const byte & op_code);
	int XOR_A_n(const byte & op_code);
	int CP_A_r(const byte & op_code);
	int CP_A_HLm(const byte & op_code);
	int CP_A_n(const byte & op_code);
	int INC_r(const byte & op_code);
	int INC_HLm(const byte & op_code);
	int DEC_r(const byte & op_code);
	int DEC_HLm(const byte & op_code);
	int ADD_HL_BC(const byte & op_code);
	int ADD_HL_DE(const byte & op_code);
	int ADD_HL_HL(const byte & op_code);
	int ADD_HL_SP(const byte & op_code);
	int ADD_SP_n(const byte & op_code);
	int INC_BC(const byte & op_code);
	int INC_DE(const byte & op_code);
	int INC_HL(const byte & op_code);
	int INC_SP(const byte & op_code);
	int DEC_BC(const byte & op_code);
	int DEC_DE(const byte & op_code);
	int DEC_HL(const byte & op_code);
	int DEC_SP(const byte & op_code);
};