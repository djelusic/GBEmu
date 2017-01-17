#pragma once
#include "Types.h"
#include <functional>

#define CLOCK_SPEED 4194304

#define FLAG_ZERO 7
#define FLAG_SUBTRACT 6
#define FLAG_HALF_CARRY 5
#define FLAG_CARRY 4

#define IE 0xFFFF
#define IF 0xFF0F

class Memory;
class Controller;
class GB;
class SDL;

class CPU : Serializable {

private:

  byte registers[8];
  word SP;
  word PC;
  Memory &MMU;
  Controller &controller;
  GB *gb;

  bool halted;
  bool stopped;
  bool interruptsEnabled;

  typedef int(CPU::*opCode)(const byte & op_code);
  opCode opCodeMap[0xFF + 1];
  opCode opCodeMapCB[0xFF + 1];

  std::function<bool(void)> inputCallback;

public:

  CPU(GB *gb, Memory &MMU, Controller &controller);

  int Advance();
  void RequestInterrupt(int id);
  int HandleInterrupts();
  void SetInputCallback(std::function<bool(void)> cb);
  void HandleInput();

  void Serialize(const Serializer &s);
  void Deserialize(const Serializer &s);

private:

  void PerformInterrupt(int id);

  word CombineRegisters(const byte & r1_id, const byte & r2_id);
  void SplitIntoRegisters(const word & val, const byte & r1_id, const byte & r2_id);
  byte ReadByte();
  word ReadWord();
  byte GetFlag(const byte & flag);
  void SetFlag(const byte & flag);
  void ResetFlag(const byte & flag);

  byte AddBytes(const byte & b1, const byte & b2);
  void ADC(const byte & b);
  word AddWords(const word & w1, const word & w2);
  byte SubtractBytes(const byte & b1, const byte & b2);
  void SBC(const byte & b);
  word SubtractWords(const word & w1, const word & w2);
  byte ANDBytes(const byte & b1, const byte & b2);
  byte ORBytes(const byte & b1, const byte & b2);
  byte XORBytes(const byte & b1, const byte & b2);
  void CPBytes(const byte & b1, const byte & b2);
  byte INCByte(const byte & b);
  byte DECByte(const byte & b);
  byte RotateLeft(const byte & b, const bool & addCarry, const bool & isCB);
  byte RotateRight(const byte & b, const bool & addCarry, const bool & isCB);

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
  int SWAP_r(const byte & op_code);
  int SWAP_HLm(const byte & op_code);
  int DAA(const byte & op_code);
  int CPL(const byte & op_code);
  int CCF(const byte & op_code);
  int SCF(const byte & op_code);
  int NOP(const byte & op_code);
  int HALT(const byte & op_code);
  int STOP(const byte & op_code);
  int DI(const byte & op_code);
  int EI(const byte & op_code);
  int RLCA(const byte & op_code);
  int RLA(const byte & op_code);
  int RRCA(const byte & op_code);
  int RRA(const byte & op_code);
  int RLC_r(const byte & op_code);
  int RLC_HLm(const byte & op_code);
  int RL_r(const byte & op_code);
  int RL_HLm(const byte & op_code);
  int RRC_r(const byte & op_code);
  int RRC_HLm(const byte & op_code);
  int RR_r(const byte & op_code);
  int RR_HLm(const byte & op_code);
  int SLA_r(const byte & op_code);
  int SLA_HLm(const byte & op_code);
  int SRA_r(const byte & op_code);
  int SRA_HLm(const byte & op_code);
  int SRL_r(const byte & op_code);
  int SRL_HLm(const byte & op_code);
  int BIT_b_r(const byte & op_code);
  int BIT_b_HLm(const byte & op_code);
  int SET_b_r(const byte & op_code);
  int SET_b_HLm(const byte & op_code);
  int RES_b_r(const byte & op_code);
  int RES_b_HLm(const byte & op_code);
  int JP_nn(const byte & op_code);
  int JP_NZ_nn(const byte & op_code);
  int JP_Z_nn(const byte & op_code);
  int JP_NC_nn(const byte & op_code);
  int JP_C_nn(const byte & op_code);
  int JP_HL(const byte & op_code);
  int JR_n(const byte & op_code);
  int JR_NZ_n(const byte & op_code);
  int JR_Z_n(const byte & op_code);
  int JR_NC_n(const byte & op_code);
  int JR_C_n(const byte & op_code);
  int CALL_nn(const byte & op_code);
  int CALL_NZ_nn(const byte & op_code);
  int CALL_Z_nn(const byte & op_code);
  int CALL_NC_nn(const byte & op_code);
  int CALL_C_nn(const byte & op_code);
  int RST_n(const byte & op_code);
  int RET(const byte & op_code);
  int RET_NZ(const byte & op_code);
  int RET_Z(const byte & op_code);
  int RET_NC(const byte & op_code);
  int RET_C(const byte & op_code);
  int RETI(const byte & op_code);
};