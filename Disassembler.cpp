#include "Disassembler.h"
#include "Memory.h"
#include "GB.h"
#include "CPU.h"
#include <iomanip>
#include <sstream>

Disassembler::Disassembler(GB * gb, Memory & MMU, CPU &Cpu) : 
  gb(gb), 
  MMU(MMU),
  Cpu(Cpu) {
  for (int i = 0; i < 256; i++) {
    opCodeMap[i] = nullptr;
    opCodeMapCB[i] = nullptr;
  }

  //// 00
  //opCodeMap[0x00] = &Disassembler::NOP;
  //opCodeMap[0x01] = &Disassembler::LD_BC_nn;
  //opCodeMap[0x02] = &Disassembler::LD_BCm_A;
  //opCodeMap[0x03] = &Disassembler::INC_BC;
  //opCodeMap[0x04] = &Disassembler::INC_r;
  //opCodeMap[0x05] = &Disassembler::DEC_r;
  opCodeMap[0x06] = &Disassembler::LD_r_n;
  //opCodeMap[0x07] = &Disassembler::RLCA;
  //opCodeMap[0x08] = &Disassembler::LD_nnm_SP;
  //opCodeMap[0x09] = &Disassembler::ADD_HL_BC;
  //opCodeMap[0x0A] = &Disassembler::LD_A_BCm;
  //opCodeMap[0x0B] = &Disassembler::DEC_BC;
  //opCodeMap[0x0C] = &Disassembler::INC_r;
  //opCodeMap[0x0D] = &Disassembler::DEC_r;
  opCodeMap[0x0E] = &Disassembler::LD_r_n;
  //opCodeMap[0x0F] = &Disassembler::RRCA;

  //// 10
  //opCodeMap[0x10] = &Disassembler::STOP;
  //opCodeMap[0x11] = &Disassembler::LD_DE_nn;
  //opCodeMap[0x12] = &Disassembler::LD_DEm_A;
  //opCodeMap[0x13] = &Disassembler::INC_DE;
  //opCodeMap[0x14] = &Disassembler::INC_r;
  //opCodeMap[0x15] = &Disassembler::DEC_r;
  opCodeMap[0x16] = &Disassembler::LD_r_n;
  //opCodeMap[0x17] = &Disassembler::RLA;
  //opCodeMap[0x18] = &Disassembler::JR_n;
  //opCodeMap[0x19] = &Disassembler::ADD_HL_DE;
  //opCodeMap[0x1A] = &Disassembler::LD_A_DEm;
  //opCodeMap[0x1B] = &Disassembler::DEC_DE;
  //opCodeMap[0x1C] = &Disassembler::INC_r;
  //opCodeMap[0x1D] = &Disassembler::DEC_r;
  opCodeMap[0x1E] = &Disassembler::LD_r_n;
  //opCodeMap[0x1F] = &Disassembler::RRA;

  //// 20
  //opCodeMap[0x20] = &Disassembler::JR_NZ_n;
  //opCodeMap[0x21] = &Disassembler::LD_HL_nn;
  //opCodeMap[0x22] = &Disassembler::LDI_HLm_A;
  //opCodeMap[0x23] = &Disassembler::INC_HL;
  //opCodeMap[0x24] = &Disassembler::INC_r;
  //opCodeMap[0x25] = &Disassembler::DEC_r;
  opCodeMap[0x26] = &Disassembler::LD_r_n;
  //opCodeMap[0x27] = &Disassembler::DAA;
  //opCodeMap[0x28] = &Disassembler::JR_Z_n;
  //opCodeMap[0x29] = &Disassembler::ADD_HL_HL;
  //opCodeMap[0x2A] = &Disassembler::LDI_A_HLm;
  //opCodeMap[0x2B] = &Disassembler::DEC_HL;
  //opCodeMap[0x2C] = &Disassembler::INC_r;
  //opCodeMap[0x2D] = &Disassembler::DEC_r;
  opCodeMap[0x2E] = &Disassembler::LD_r_n;
  //opCodeMap[0x2F] = &Disassembler::CPL;

  //// 30
  //opCodeMap[0x30] = &Disassembler::JR_NC_n;
  //opCodeMap[0x31] = &Disassembler::LD_SP_nn;
  //opCodeMap[0x32] = &Disassembler::LDD_HLm_A;
  //opCodeMap[0x33] = &Disassembler::INC_SP;
  //opCodeMap[0x34] = &Disassembler::INC_HLm;
  //opCodeMap[0x35] = &Disassembler::DEC_HLm;
  //opCodeMap[0x36] = &Disassembler::LD_HLm_n;
  //opCodeMap[0x37] = &Disassembler::SCF;
  //opCodeMap[0x38] = &Disassembler::JR_C_n;
  //opCodeMap[0x39] = &Disassembler::ADD_HL_SP;
  //opCodeMap[0x3A] = &Disassembler::LDD_A_HLm;
  //opCodeMap[0x3B] = &Disassembler::DEC_SP;
  //opCodeMap[0x3C] = &Disassembler::INC_r;
  //opCodeMap[0x3D] = &Disassembler::DEC_r;
  opCodeMap[0x3E] = &Disassembler::LD_r_n;
  //opCodeMap[0x3F] = &Disassembler::CCF;

  // 40
  opCodeMap[0x40] = &Disassembler::LD_r1_r2;
  opCodeMap[0x41] = &Disassembler::LD_r1_r2;
  opCodeMap[0x42] = &Disassembler::LD_r1_r2;
  opCodeMap[0x43] = &Disassembler::LD_r1_r2;
  opCodeMap[0x44] = &Disassembler::LD_r1_r2;
  opCodeMap[0x45] = &Disassembler::LD_r1_r2;
  opCodeMap[0x46] = &Disassembler::LD_r_HLm;
  opCodeMap[0x47] = &Disassembler::LD_r1_r2;
  opCodeMap[0x48] = &Disassembler::LD_r1_r2;
  opCodeMap[0x49] = &Disassembler::LD_r1_r2;
  opCodeMap[0x4A] = &Disassembler::LD_r1_r2;
  opCodeMap[0x4B] = &Disassembler::LD_r1_r2;
  opCodeMap[0x4C] = &Disassembler::LD_r1_r2;
  opCodeMap[0x4D] = &Disassembler::LD_r1_r2;
  opCodeMap[0x4E] = &Disassembler::LD_r_HLm;
  opCodeMap[0x4F] = &Disassembler::LD_r1_r2;

  // 50
  opCodeMap[0x50] = &Disassembler::LD_r1_r2;
  opCodeMap[0x51] = &Disassembler::LD_r1_r2;
  opCodeMap[0x52] = &Disassembler::LD_r1_r2;
  opCodeMap[0x53] = &Disassembler::LD_r1_r2;
  opCodeMap[0x54] = &Disassembler::LD_r1_r2;
  opCodeMap[0x55] = &Disassembler::LD_r1_r2;
  opCodeMap[0x56] = &Disassembler::LD_r_HLm;
  opCodeMap[0x57] = &Disassembler::LD_r1_r2;
  opCodeMap[0x58] = &Disassembler::LD_r1_r2;
  opCodeMap[0x59] = &Disassembler::LD_r1_r2;
  opCodeMap[0x5A] = &Disassembler::LD_r1_r2;
  opCodeMap[0x5B] = &Disassembler::LD_r1_r2;
  opCodeMap[0x5C] = &Disassembler::LD_r1_r2;
  opCodeMap[0x5D] = &Disassembler::LD_r1_r2;
  opCodeMap[0x5E] = &Disassembler::LD_r_HLm;
  opCodeMap[0x5F] = &Disassembler::LD_r1_r2;

  // 60
  opCodeMap[0x60] = &Disassembler::LD_r1_r2;
  opCodeMap[0x61] = &Disassembler::LD_r1_r2;
  opCodeMap[0x62] = &Disassembler::LD_r1_r2;
  opCodeMap[0x63] = &Disassembler::LD_r1_r2;
  opCodeMap[0x64] = &Disassembler::LD_r1_r2;
  opCodeMap[0x65] = &Disassembler::LD_r1_r2;
  opCodeMap[0x66] = &Disassembler::LD_r_HLm;
  opCodeMap[0x67] = &Disassembler::LD_r1_r2;
  opCodeMap[0x68] = &Disassembler::LD_r1_r2;
  opCodeMap[0x69] = &Disassembler::LD_r1_r2;
  opCodeMap[0x6A] = &Disassembler::LD_r1_r2;
  opCodeMap[0x6B] = &Disassembler::LD_r1_r2;
  opCodeMap[0x6C] = &Disassembler::LD_r1_r2;
  opCodeMap[0x6D] = &Disassembler::LD_r1_r2;
  opCodeMap[0x6E] = &Disassembler::LD_r_HLm;
  opCodeMap[0x6F] = &Disassembler::LD_r1_r2;

  //// 70
  //opCodeMap[0x70] = &Disassembler::LD_HLm_r;
  //opCodeMap[0x71] = &Disassembler::LD_HLm_r;
  //opCodeMap[0x72] = &Disassembler::LD_HLm_r;
  //opCodeMap[0x73] = &Disassembler::LD_HLm_r;
  //opCodeMap[0x74] = &Disassembler::LD_HLm_r;
  //opCodeMap[0x75] = &Disassembler::LD_HLm_r;
  //opCodeMap[0x76] = &Disassembler::HALT;
  //opCodeMap[0x77] = &Disassembler::LD_HLm_r;
  opCodeMap[0x78] = &Disassembler::LD_r1_r2;
  opCodeMap[0x79] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7A] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7B] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7C] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7D] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7E] = &Disassembler::LD_r_HLm;
  opCodeMap[0x7F] = &Disassembler::LD_r1_r2;

  //// 80
  //opCodeMap[0x80] = &Disassembler::ADD_A_r;
  //opCodeMap[0x81] = &Disassembler::ADD_A_r;
  //opCodeMap[0x82] = &Disassembler::ADD_A_r;
  //opCodeMap[0x83] = &Disassembler::ADD_A_r;
  //opCodeMap[0x84] = &Disassembler::ADD_A_r;
  //opCodeMap[0x85] = &Disassembler::ADD_A_r;
  //opCodeMap[0x86] = &Disassembler::ADD_A_HLm;
  //opCodeMap[0x87] = &Disassembler::ADD_A_r;
  //opCodeMap[0x88] = &Disassembler::ADC_A_r;
  //opCodeMap[0x89] = &Disassembler::ADC_A_r;
  //opCodeMap[0x8A] = &Disassembler::ADC_A_r;
  //opCodeMap[0x8B] = &Disassembler::ADC_A_r;
  //opCodeMap[0x8C] = &Disassembler::ADC_A_r;
  //opCodeMap[0x8D] = &Disassembler::ADC_A_r;
  //opCodeMap[0x8E] = &Disassembler::ADC_A_HLm;
  //opCodeMap[0x8F] = &Disassembler::ADC_A_r;

  //// 90
  //opCodeMap[0x90] = &Disassembler::SUB_A_r;
  //opCodeMap[0x91] = &Disassembler::SUB_A_r;
  //opCodeMap[0x92] = &Disassembler::SUB_A_r;
  //opCodeMap[0x93] = &Disassembler::SUB_A_r;
  //opCodeMap[0x94] = &Disassembler::SUB_A_r;
  //opCodeMap[0x95] = &Disassembler::SUB_A_r;
  //opCodeMap[0x96] = &Disassembler::SUB_A_HLm;
  //opCodeMap[0x97] = &Disassembler::SUB_A_r;
  //opCodeMap[0x98] = &Disassembler::SBC_A_r;
  //opCodeMap[0x99] = &Disassembler::SBC_A_r;
  //opCodeMap[0x9A] = &Disassembler::SBC_A_r;
  //opCodeMap[0x9B] = &Disassembler::SBC_A_r;
  //opCodeMap[0x9C] = &Disassembler::SBC_A_r;
  //opCodeMap[0x9D] = &Disassembler::SBC_A_r;
  //opCodeMap[0x9E] = &Disassembler::SBC_A_HLm;
  //opCodeMap[0x9F] = &Disassembler::SBC_A_r;

  //// A0
  //opCodeMap[0xA0] = &Disassembler::AND_A_r;
  //opCodeMap[0xA1] = &Disassembler::AND_A_r;
  //opCodeMap[0xA2] = &Disassembler::AND_A_r;
  //opCodeMap[0xA3] = &Disassembler::AND_A_r;
  //opCodeMap[0xA4] = &Disassembler::AND_A_r;
  //opCodeMap[0xA5] = &Disassembler::AND_A_r;
  //opCodeMap[0xA6] = &Disassembler::AND_A_HLm;
  //opCodeMap[0xA7] = &Disassembler::AND_A_r;
  //opCodeMap[0xA8] = &Disassembler::XOR_A_r;
  //opCodeMap[0xA9] = &Disassembler::XOR_A_r;
  //opCodeMap[0xAA] = &Disassembler::XOR_A_r;
  //opCodeMap[0xAB] = &Disassembler::XOR_A_r;
  //opCodeMap[0xAC] = &Disassembler::XOR_A_r;
  //opCodeMap[0xAD] = &Disassembler::XOR_A_r;
  //opCodeMap[0xAE] = &Disassembler::XOR_A_HLm;
  //opCodeMap[0xAF] = &Disassembler::XOR_A_r;

  //// B0
  //opCodeMap[0xB0] = &Disassembler::OR_A_r;
  //opCodeMap[0xB1] = &Disassembler::OR_A_r;
  //opCodeMap[0xB2] = &Disassembler::OR_A_r;
  //opCodeMap[0xB3] = &Disassembler::OR_A_r;
  //opCodeMap[0xB4] = &Disassembler::OR_A_r;
  //opCodeMap[0xB5] = &Disassembler::OR_A_r;
  //opCodeMap[0xB6] = &Disassembler::OR_A_HLm;
  //opCodeMap[0xB7] = &Disassembler::OR_A_r;
  //opCodeMap[0xB8] = &Disassembler::CP_A_r;
  //opCodeMap[0xB9] = &Disassembler::CP_A_r;
  //opCodeMap[0xBA] = &Disassembler::CP_A_r;
  //opCodeMap[0xBB] = &Disassembler::CP_A_r;
  //opCodeMap[0xBC] = &Disassembler::CP_A_r;
  //opCodeMap[0xBD] = &Disassembler::CP_A_r;
  //opCodeMap[0xBE] = &Disassembler::CP_A_HLm;
  //opCodeMap[0xBF] = &Disassembler::CP_A_r;

  //// C0
  //opCodeMap[0xC0] = &Disassembler::RET_NZ;
  //opCodeMap[0xC1] = &Disassembler::POP_BC;
  //opCodeMap[0xC2] = &Disassembler::JP_NZ_nn;
  //opCodeMap[0xC3] = &Disassembler::JP_nn;
  //opCodeMap[0xC4] = &Disassembler::CALL_NZ_nn;
  //opCodeMap[0xC5] = &Disassembler::PUSH_BC;
  //opCodeMap[0xC6] = &Disassembler::ADD_A_n;
  //opCodeMap[0xC7] = &Disassembler::RST_n;
  //opCodeMap[0xC8] = &Disassembler::RET_Z;
  //opCodeMap[0xC9] = &Disassembler::RET;
  //opCodeMap[0xCA] = &Disassembler::JP_Z_nn;
  //opCodeMap[0xCB] = nullptr;
  //opCodeMap[0xCC] = &Disassembler::CALL_Z_nn;
  //opCodeMap[0xCD] = &Disassembler::CALL_nn;
  //opCodeMap[0xCE] = &Disassembler::ADC_A_n;
  //opCodeMap[0xCF] = &Disassembler::RST_n;

  //// D0
  //opCodeMap[0xD0] = &Disassembler::RET_NC;
  //opCodeMap[0xD1] = &Disassembler::POP_DE;
  //opCodeMap[0xD2] = &Disassembler::JP_NC_nn;
  //opCodeMap[0xD3] = nullptr;
  //opCodeMap[0xD4] = &Disassembler::CALL_NC_nn;
  //opCodeMap[0xD5] = &Disassembler::PUSH_DE;
  //opCodeMap[0xD6] = &Disassembler::SUB_A_n;
  //opCodeMap[0xD7] = &Disassembler::RST_n;
  //opCodeMap[0xD8] = &Disassembler::RET_C;
  //opCodeMap[0xD9] = &Disassembler::RETI;
  //opCodeMap[0xDA] = &Disassembler::JP_C_nn;
  //opCodeMap[0xDB] = nullptr;
  //opCodeMap[0xDC] = &Disassembler::CALL_C_nn;
  //opCodeMap[0xDD] = nullptr;
  //opCodeMap[0xDE] = &Disassembler::SBC_A_n;
  //opCodeMap[0xDF] = &Disassembler::RST_n;

  //// E0
  //opCodeMap[0xE0] = &Disassembler::LDH_nm_A;
  //opCodeMap[0xE1] = &Disassembler::POP_HL;
  //opCodeMap[0xE2] = &Disassembler::LD_Cm_A;
  //opCodeMap[0xE3] = nullptr;
  //opCodeMap[0xE4] = nullptr;
  //opCodeMap[0xE5] = &Disassembler::PUSH_HL;
  //opCodeMap[0xE6] = &Disassembler::AND_A_n;
  //opCodeMap[0xE7] = &Disassembler::RST_n;
  //opCodeMap[0xE8] = &Disassembler::ADD_SP_n;
  //opCodeMap[0xE9] = &Disassembler::JP_HL;
  //opCodeMap[0xEA] = &Disassembler::LD_nnm_A;
  //opCodeMap[0xEB] = nullptr;
  //opCodeMap[0xEC] = nullptr;
  //opCodeMap[0xED] = nullptr;
  //opCodeMap[0xEE] = &Disassembler::XOR_A_n;
  //opCodeMap[0xEF] = &Disassembler::RST_n;

  //// F0
  //opCodeMap[0xF0] = &Disassembler::LDH_A_nm;
  //opCodeMap[0xF1] = &Disassembler::POP_AF;
  //opCodeMap[0xF2] = &Disassembler::LD_A_Cm;
  //opCodeMap[0xF3] = &Disassembler::DI;
  //opCodeMap[0xF4] = nullptr;
  //opCodeMap[0xF5] = &Disassembler::PUSH_AF;
  //opCodeMap[0xF6] = &Disassembler::OR_A_n;
  //opCodeMap[0xF7] = &Disassembler::RST_n;
  //opCodeMap[0xF8] = &Disassembler::LDHL_SP_n;
  //opCodeMap[0xF9] = &Disassembler::LD_SP_HL;
  //opCodeMap[0xFA] = &Disassembler::LD_A_nnm;
  //opCodeMap[0xFB] = &Disassembler::EI;
  //opCodeMap[0xFC] = nullptr;
  //opCodeMap[0xFD] = nullptr;
  //opCodeMap[0xFE] = &Disassembler::CP_A_n;
  //opCodeMap[0xFF] = &Disassembler::RST_n;

  //// CB Prefix

  //// 00
  //opCodeMapCB[0x00] = &Disassembler::RLC_r;
  //opCodeMapCB[0x01] = &Disassembler::RLC_r;
  //opCodeMapCB[0x02] = &Disassembler::RLC_r;
  //opCodeMapCB[0x03] = &Disassembler::RLC_r;
  //opCodeMapCB[0x04] = &Disassembler::RLC_r;
  //opCodeMapCB[0x05] = &Disassembler::RLC_r;
  //opCodeMapCB[0x06] = &Disassembler::RLC_HLm;
  //opCodeMapCB[0x07] = &Disassembler::RLC_r;
  //opCodeMapCB[0x08] = &Disassembler::RRC_r;
  //opCodeMapCB[0x09] = &Disassembler::RRC_r;
  //opCodeMapCB[0x0A] = &Disassembler::RRC_r;
  //opCodeMapCB[0x0B] = &Disassembler::RRC_r;
  //opCodeMapCB[0x0C] = &Disassembler::RRC_r;
  //opCodeMapCB[0x0D] = &Disassembler::RRC_r;
  //opCodeMapCB[0x0E] = &Disassembler::RRC_HLm;
  //opCodeMapCB[0x0F] = &Disassembler::RRC_r;

  //// 10
  //opCodeMapCB[0x10] = &Disassembler::RL_r;
  //opCodeMapCB[0x11] = &Disassembler::RL_r;
  //opCodeMapCB[0x12] = &Disassembler::RL_r;
  //opCodeMapCB[0x13] = &Disassembler::RL_r;
  //opCodeMapCB[0x14] = &Disassembler::RL_r;
  //opCodeMapCB[0x15] = &Disassembler::RL_r;
  //opCodeMapCB[0x16] = &Disassembler::RL_HLm;
  //opCodeMapCB[0x17] = &Disassembler::RL_r;
  //opCodeMapCB[0x18] = &Disassembler::RR_r;
  //opCodeMapCB[0x19] = &Disassembler::RR_r;
  //opCodeMapCB[0x1A] = &Disassembler::RR_r;
  //opCodeMapCB[0x1B] = &Disassembler::RR_r;
  //opCodeMapCB[0x1C] = &Disassembler::RR_r;
  //opCodeMapCB[0x1D] = &Disassembler::RR_r;
  //opCodeMapCB[0x1E] = &Disassembler::RR_HLm;
  //opCodeMapCB[0x1F] = &Disassembler::RR_r;

  //// 20
  //opCodeMapCB[0x20] = &Disassembler::SLA_r;
  //opCodeMapCB[0x21] = &Disassembler::SLA_r;
  //opCodeMapCB[0x22] = &Disassembler::SLA_r;
  //opCodeMapCB[0x23] = &Disassembler::SLA_r;
  //opCodeMapCB[0x24] = &Disassembler::SLA_r;
  //opCodeMapCB[0x25] = &Disassembler::SLA_r;
  //opCodeMapCB[0x26] = &Disassembler::SLA_HLm;
  //opCodeMapCB[0x27] = &Disassembler::SLA_r;
  //opCodeMapCB[0x28] = &Disassembler::SRA_r;
  //opCodeMapCB[0x29] = &Disassembler::SRA_r;
  //opCodeMapCB[0x2A] = &Disassembler::SRA_r;
  //opCodeMapCB[0x2B] = &Disassembler::SRA_r;
  //opCodeMapCB[0x2C] = &Disassembler::SRA_r;
  //opCodeMapCB[0x2D] = &Disassembler::SRA_r;
  //opCodeMapCB[0x2E] = &Disassembler::SRA_HLm;
  //opCodeMapCB[0x2F] = &Disassembler::SRA_r;

  //// 30
  //opCodeMapCB[0x30] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x31] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x32] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x33] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x34] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x35] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x36] = &Disassembler::SWAP_HLm;
  //opCodeMapCB[0x37] = &Disassembler::SWAP_r;
  //opCodeMapCB[0x38] = &Disassembler::SRL_r;
  //opCodeMapCB[0x39] = &Disassembler::SRL_r;
  //opCodeMapCB[0x3A] = &Disassembler::SRL_r;
  //opCodeMapCB[0x3B] = &Disassembler::SRL_r;
  //opCodeMapCB[0x3C] = &Disassembler::SRL_r;
  //opCodeMapCB[0x3D] = &Disassembler::SRL_r;
  //opCodeMapCB[0x3E] = &Disassembler::SRL_HLm;
  //opCodeMapCB[0x3F] = &Disassembler::SRL_r;

  //// 40
  //opCodeMapCB[0x40] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x41] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x42] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x43] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x44] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x45] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x46] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x47] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x48] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x49] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x4A] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x4B] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x4C] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x4D] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x4E] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x4F] = &Disassembler::BIT_b_r;

  //// 50
  //opCodeMapCB[0x50] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x51] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x52] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x53] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x54] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x55] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x56] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x57] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x58] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x59] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x5A] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x5B] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x5C] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x5D] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x5E] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x5F] = &Disassembler::BIT_b_r;

  //// 60
  //opCodeMapCB[0x60] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x61] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x62] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x63] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x64] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x65] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x66] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x67] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x68] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x69] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x6A] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x6B] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x6C] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x6D] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x6E] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x6F] = &Disassembler::BIT_b_r;

  //// 70
  //opCodeMapCB[0x70] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x71] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x72] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x73] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x74] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x75] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x76] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x77] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x78] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x79] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x7A] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x7B] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x7C] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x7D] = &Disassembler::BIT_b_r;
  //opCodeMapCB[0x7E] = &Disassembler::BIT_b_HLm;
  //opCodeMapCB[0x7F] = &Disassembler::BIT_b_r;

  //// 80
  //opCodeMapCB[0x80] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x81] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x82] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x83] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x84] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x85] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x86] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0x87] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x88] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x89] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x8A] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x8B] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x8C] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x8D] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x8E] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0x8F] = &Disassembler::RES_b_r;

  //// 90
  //opCodeMapCB[0x90] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x91] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x92] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x93] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x94] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x95] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x96] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0x97] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x98] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x99] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x9A] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x9B] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x9C] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x9D] = &Disassembler::RES_b_r;
  //opCodeMapCB[0x9E] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0x9F] = &Disassembler::RES_b_r;

  //// A0
  //opCodeMapCB[0xA0] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA1] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA2] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA3] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA4] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA5] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA6] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0xA7] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA8] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xA9] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xAA] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xAB] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xAC] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xAD] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xAE] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0xAF] = &Disassembler::RES_b_r;

  //// B0
  //opCodeMapCB[0xB0] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB1] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB2] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB3] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB4] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB5] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB6] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0xB7] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB8] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xB9] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xBA] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xBB] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xBC] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xBD] = &Disassembler::RES_b_r;
  //opCodeMapCB[0xBE] = &Disassembler::RES_b_HLm;
  //opCodeMapCB[0xBF] = &Disassembler::RES_b_r;

  //// C0
  //opCodeMapCB[0xC0] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC1] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC2] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC3] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC4] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC5] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC6] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xC7] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC8] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xC9] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xCA] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xCB] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xCC] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xCD] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xCE] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xCF] = &Disassembler::SET_b_r;

  //// D0
  //opCodeMapCB[0xD0] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD1] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD2] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD3] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD4] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD5] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD6] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xD7] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD8] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xD9] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xDA] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xDB] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xDC] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xDD] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xDE] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xDF] = &Disassembler::SET_b_r;

  //// E0
  //opCodeMapCB[0xE0] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE1] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE2] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE3] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE4] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE5] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE6] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xE7] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE8] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xE9] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xEA] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xEB] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xEC] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xED] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xEE] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xEF] = &Disassembler::SET_b_r;

  //// F0
  //opCodeMapCB[0xF0] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF1] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF2] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF3] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF4] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF5] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF6] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xF7] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF8] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xF9] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xFA] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xFB] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xFC] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xFD] = &Disassembler::SET_b_r;
  //opCodeMapCB[0xFE] = &Disassembler::SET_b_HLm;
  //opCodeMapCB[0xFF] = &Disassembler::SET_b_r;
}

void Disassembler::Disassemble(word pc, int num_opcodes) {
  PC = pc;
  byte opCode = MMU.ReadByte(PC++);
  if (opCodeMap[opCode] != nullptr) {
    (this->*opCodeMap[opCode])(opCode);
  }
}

std::string Disassembler::ReadByte() {
  std::stringstream ret;
  ret << std::setfill('0') << std::setw(2) 
      << std::hex << (int)MMU.ReadByte(PC++);
  return ret.str();
}

std::string Disassembler::ReadWord() {
  std::stringstream ret;
  ret << std::setfill('0') << std::setw(4) 
      << std::hex << (int)MMU.ReadWord(PC);
  PC += 2;
  return ret.str();
}

std::string Disassembler::GetRegisterName(int r1_id, int r2_id) {
  std::string regMap = "BCDEHLFA";
  std::string name(1, regMap[r1_id]);
  if (r2_id >= 0) name += regMap[r2_id];
  return name;
}

std::string Disassembler::GetRegisterValue(int r1_id, int r2_id) {
  word val = Cpu.GetRegister(r1_id);
  if (r2_id >= 0) val = (val << 8) | Cpu.GetRegister(r2_id);
  std::stringstream ret;
  ret << std::setfill('0') << std::setw(4)
    << std::hex << (int)val;
  return ret.str();
}

void Disassembler::LD_r_n(const byte & op_code) {
  byte r_id = op_code >> 3;
  gb->DebugLog("LD   " + GetRegisterName(r_id) + "," + ReadByte());
}

void Disassembler::LD_r1_r2(const byte & op_code) {
  byte r1_id = (op_code & 0x38) >> 3;
  byte r2_id = op_code & 0x7;
  gb->DebugLog("LD   " + GetRegisterName(r1_id) + 
    "," + GetRegisterName(r2_id));
}

void Disassembler::LD_r_HLm(const byte & op_code) {
  byte r_id = (op_code >> 3) & 0x7;
  gb->DebugLog("LD   " + GetRegisterName(r_id) + 
    ",(" + GetRegisterValue(4, 5) + ")");
}
