#pragma once
#include "Types.h"
#include <string>

class GB;
class Memory;
class CPU;

class Disassembler {

private:

  word PC;
  GB *gb;
  Memory &MMU;
  CPU &Cpu;

  typedef void(Disassembler::*opCode)(const byte & op_code);
  opCode opCodeMap[0xFF + 1];
  opCode opCodeMapCB[0xFF + 1];

public:

  Disassembler(GB *gb, Memory &MMU, CPU &Cpu);
  void Disassemble(word PC, int num_opcodes);

private:

  std::string ReadByte();
  std::string ReadWord();
  
  std::string GetRegisterName(int r1_id, int r2_id = -1);
  std::string GetRegisterValue(int r1_id, int r2_id = -1);

  void LD_r_n(const byte& op_code);
  void LD_r1_r2(const byte & op_code);
  void LD_r_HLm(const byte & op_code);
  void LD_HLm_r(const byte & op_code);
  void LD_HLm_n(const byte & op_code);
  void LD_A_BCm(const byte & op_code);
  void LD_A_DEm(const byte & op_code);
  void LD_A_nnm(const byte & op_code);
  void LD_BCm_A(const byte & op_code);
  void LD_DEm_A(const byte & op_code);
  void LD_nnm_A(const byte & op_code);
  void LD_A_Cm(const byte & op_code);
  void LD_Cm_A(const byte & op_code);
  void LDD_A_HLm(const byte & op_code);
  void LDD_HLm_A(const byte & op_code);
  void LDI_A_HLm(const byte & op_code);
  void LDI_HLm_A(const byte & op_code);
  void LDH_nm_A(const byte & op_code);
  void LDH_A_nm(const byte & op_code);
  void LD_BC_nn(const byte & op_code);
  void LD_DE_nn(const byte & op_code);
  void LD_HL_nn(const byte & op_code);
  void LD_SP_nn(const byte & op_code);
  void LD_SP_HL(const byte & op_code);
  void LDHL_SP_n(const byte & op_code);
  void LD_nnm_SP(const byte & op_code);
  void PUSH_AF(const byte & op_code);
  void PUSH_BC(const byte & op_code);
  void PUSH_DE(const byte & op_code);
  void PUSH_HL(const byte & op_code);
  void POP_AF(const byte & op_code);
  void POP_BC(const byte & op_code);
  void POP_DE(const byte & op_code);
  void POP_HL(const byte & op_code);
  void ADD_A_r(const byte & op_code);
  void ADD_A_HLm(const byte & op_code);
  void ADD_A_n(const byte & op_code);
  void ADC_A_r(const byte & op_code);
  void ADC_A_HLm(const byte & op_code);
  void ADC_A_n(const byte & op_code);
  void SUB_A_r(const byte & op_code);
  void SUB_A_HLm(const byte & op_code);
  void SUB_A_n(const byte & op_code);
  void SBC_A_r(const byte & op_code);
  void SBC_A_HLm(const byte & op_code);
  void SBC_A_n(const byte & op_code);
  void AND_A_r(const byte & op_code);
  void AND_A_HLm(const byte & op_code);
  void AND_A_n(const byte & op_code);
  void OR_A_r(const byte & op_code);
  void OR_A_HLm(const byte & op_code);
  void OR_A_n(const byte & op_code);
  void XOR_A_r(const byte & op_code);
  void XOR_A_HLm(const byte & op_code);
  void XOR_A_n(const byte & op_code);
  void CP_A_r(const byte & op_code);
  void CP_A_HLm(const byte & op_code);
  void CP_A_n(const byte & op_code);
  void INC_r(const byte & op_code);
  void INC_HLm(const byte & op_code);
  void DEC_r(const byte & op_code);
  void DEC_HLm(const byte & op_code);
  void ADD_HL_BC(const byte & op_code);
  void ADD_HL_DE(const byte & op_code);
  void ADD_HL_HL(const byte & op_code);
  void ADD_HL_SP(const byte & op_code);
  void ADD_SP_n(const byte & op_code);
  void INC_BC(const byte & op_code);
  void INC_DE(const byte & op_code);
  void INC_HL(const byte & op_code);
  void INC_SP(const byte & op_code);
  void DEC_BC(const byte & op_code);
  void DEC_DE(const byte & op_code);
  void DEC_HL(const byte & op_code);
  void DEC_SP(const byte & op_code);
  void SWAP_r(const byte & op_code);
  void SWAP_HLm(const byte & op_code);
  void DAA(const byte & op_code);
  void CPL(const byte & op_code);
  void CCF(const byte & op_code);
  void SCF(const byte & op_code);
  void NOP(const byte & op_code);
  void HALT(const byte & op_code);
  void STOP(const byte & op_code);
  void DI(const byte & op_code);
  void EI(const byte & op_code);
  void RLCA(const byte & op_code);
  void RLA(const byte & op_code);
  void RRCA(const byte & op_code);
  void RRA(const byte & op_code);
  void RLC_r(const byte & op_code);
  void RLC_HLm(const byte & op_code);
  void RL_r(const byte & op_code);
  void RL_HLm(const byte & op_code);
  void RRC_r(const byte & op_code);
  void RRC_HLm(const byte & op_code);
  void RR_r(const byte & op_code);
  void RR_HLm(const byte & op_code);
  void SLA_r(const byte & op_code);
  void SLA_HLm(const byte & op_code);
  void SRA_r(const byte & op_code);
  void SRA_HLm(const byte & op_code);
  void SRL_r(const byte & op_code);
  void SRL_HLm(const byte & op_code);
  void BIT_b_r(const byte & op_code);
  void BIT_b_HLm(const byte & op_code);
  void SET_b_r(const byte & op_code);
  void SET_b_HLm(const byte & op_code);
  void RES_b_r(const byte & op_code);
  void RES_b_HLm(const byte & op_code);
  void JP_nn(const byte & op_code);
  void JP_NZ_nn(const byte & op_code);
  void JP_Z_nn(const byte & op_code);
  void JP_NC_nn(const byte & op_code);
  void JP_C_nn(const byte & op_code);
  void JP_HL(const byte & op_code);
  void JR_n(const byte & op_code);
  void JR_NZ_n(const byte & op_code);
  void JR_Z_n(const byte & op_code);
  void JR_NC_n(const byte & op_code);
  void JR_C_n(const byte & op_code);
  void CALL_nn(const byte & op_code);
  void CALL_NZ_nn(const byte & op_code);
  void CALL_Z_nn(const byte & op_code);
  void CALL_NC_nn(const byte & op_code);
  void CALL_C_nn(const byte & op_code);
  void RST_n(const byte & op_code);
  void RET(const byte & op_code);
  void RET_NZ(const byte & op_code);
  void RET_Z(const byte & op_code);
  void RET_NC(const byte & op_code);
  void RET_C(const byte & op_code);
  void RETI(const byte & op_code);
};