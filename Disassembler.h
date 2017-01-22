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

  typedef std::string(Disassembler::*opCode)(const byte & op_code);
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

  std::string LD_r_n(const byte& op_code);
  std::string LD_r1_r2(const byte & op_code);
  std::string LD_r_HLm(const byte & op_code);
  std::string LD_HLm_r(const byte & op_code);
  std::string LD_HLm_n(const byte & op_code);
  std::string LD_A_BCm(const byte & op_code);
  std::string LD_A_DEm(const byte & op_code);
  std::string LD_A_nnm(const byte & op_code);
  std::string LD_BCm_A(const byte & op_code);
  std::string LD_DEm_A(const byte & op_code);
  std::string LD_nnm_A(const byte & op_code);
  std::string LD_A_Cm(const byte & op_code);
  std::string LD_Cm_A(const byte & op_code);
  std::string LDD_A_HLm(const byte & op_code);
  std::string LDD_HLm_A(const byte & op_code);
  std::string LDI_A_HLm(const byte & op_code);
  std::string LDI_HLm_A(const byte & op_code);
  std::string LDH_nm_A(const byte & op_code);
  std::string LDH_A_nm(const byte & op_code);
  std::string LD_BC_nn(const byte & op_code);
  std::string LD_DE_nn(const byte & op_code);
  std::string LD_HL_nn(const byte & op_code);
  std::string LD_SP_nn(const byte & op_code);
  std::string LD_SP_HL(const byte & op_code);
  std::string LDHL_SP_n(const byte & op_code);
  std::string LD_nnm_SP(const byte & op_code);
  std::string PUSH_AF(const byte & op_code);
  std::string PUSH_BC(const byte & op_code);
  std::string PUSH_DE(const byte & op_code);
  std::string PUSH_HL(const byte & op_code);
  std::string POP_AF(const byte & op_code);
  std::string POP_BC(const byte & op_code);
  std::string POP_DE(const byte & op_code);
  std::string POP_HL(const byte & op_code);
  std::string ADD_A_r(const byte & op_code);
  std::string ADD_A_HLm(const byte & op_code);
  std::string ADD_A_n(const byte & op_code);
  std::string ADC_A_r(const byte & op_code);
  std::string ADC_A_HLm(const byte & op_code);
  std::string ADC_A_n(const byte & op_code);
  std::string SUB_A_r(const byte & op_code);
  std::string SUB_A_HLm(const byte & op_code);
  std::string SUB_A_n(const byte & op_code);
  std::string SBC_A_r(const byte & op_code);
  std::string SBC_A_HLm(const byte & op_code);
  std::string SBC_A_n(const byte & op_code);
  std::string AND_A_r(const byte & op_code);
  std::string AND_A_HLm(const byte & op_code);
  std::string AND_A_n(const byte & op_code);
  std::string OR_A_r(const byte & op_code);
  std::string OR_A_HLm(const byte & op_code);
  std::string OR_A_n(const byte & op_code);
  std::string XOR_A_r(const byte & op_code);
  std::string XOR_A_HLm(const byte & op_code);
  std::string XOR_A_n(const byte & op_code);
  std::string CP_A_r(const byte & op_code);
  std::string CP_A_HLm(const byte & op_code);
  std::string CP_A_n(const byte & op_code);
  std::string INC_r(const byte & op_code);
  std::string INC_HLm(const byte & op_code);
  std::string DEC_r(const byte & op_code);
  std::string DEC_HLm(const byte & op_code);
  std::string ADD_HL_BC(const byte & op_code);
  std::string ADD_HL_DE(const byte & op_code);
  std::string ADD_HL_HL(const byte & op_code);
  std::string ADD_HL_SP(const byte & op_code);
  std::string ADD_SP_n(const byte & op_code);
  std::string INC_BC(const byte & op_code);
  std::string INC_DE(const byte & op_code);
  std::string INC_HL(const byte & op_code);
  std::string INC_SP(const byte & op_code);
  std::string DEC_BC(const byte & op_code);
  std::string DEC_DE(const byte & op_code);
  std::string DEC_HL(const byte & op_code);
  std::string DEC_SP(const byte & op_code);
  std::string SWAP_r(const byte & op_code);
  std::string SWAP_HLm(const byte & op_code);
  std::string DAA(const byte & op_code);
  std::string CPL(const byte & op_code);
  std::string CCF(const byte & op_code);
  std::string SCF(const byte & op_code);
  std::string NOP(const byte & op_code);
  std::string HALT(const byte & op_code);
  std::string STOP(const byte & op_code);
  std::string DI(const byte & op_code);
  std::string EI(const byte & op_code);
  std::string RLCA(const byte & op_code);
  std::string RLA(const byte & op_code);
  std::string RRCA(const byte & op_code);
  std::string RRA(const byte & op_code);
  std::string RLC_r(const byte & op_code);
  std::string RLC_HLm(const byte & op_code);
  std::string RL_r(const byte & op_code);
  std::string RL_HLm(const byte & op_code);
  std::string RRC_r(const byte & op_code);
  std::string RRC_HLm(const byte & op_code);
  std::string RR_r(const byte & op_code);
  std::string RR_HLm(const byte & op_code);
  std::string SLA_r(const byte & op_code);
  std::string SLA_HLm(const byte & op_code);
  std::string SRA_r(const byte & op_code);
  std::string SRA_HLm(const byte & op_code);
  std::string SRL_r(const byte & op_code);
  std::string SRL_HLm(const byte & op_code);
  std::string BIT_b_r(const byte & op_code);
  std::string BIT_b_HLm(const byte & op_code);
  std::string SET_b_r(const byte & op_code);
  std::string SET_b_HLm(const byte & op_code);
  std::string RES_b_r(const byte & op_code);
  std::string RES_b_HLm(const byte & op_code);
  std::string JP_nn(const byte & op_code);
  std::string JP_NZ_nn(const byte & op_code);
  std::string JP_Z_nn(const byte & op_code);
  std::string JP_NC_nn(const byte & op_code);
  std::string JP_C_nn(const byte & op_code);
  std::string JP_HL(const byte & op_code);
  std::string JR_n(const byte & op_code);
  std::string JR_NZ_n(const byte & op_code);
  std::string JR_Z_n(const byte & op_code);
  std::string JR_NC_n(const byte & op_code);
  std::string JR_C_n(const byte & op_code);
  std::string CALL_nn(const byte & op_code);
  std::string CALL_NZ_nn(const byte & op_code);
  std::string CALL_Z_nn(const byte & op_code);
  std::string CALL_NC_nn(const byte & op_code);
  std::string CALL_C_nn(const byte & op_code);
  std::string RST_n(const byte & op_code);
  std::string RET(const byte & op_code);
  std::string RET_NZ(const byte & op_code);
  std::string RET_Z(const byte & op_code);
  std::string RET_NC(const byte & op_code);
  std::string RET_C(const byte & op_code);
  std::string RETI(const byte & op_code);
};