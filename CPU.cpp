#include "CPU.h"
#include "Memory.h"
#include "Controller.h"
#include "GB.h"
#include "SDL.h"
#include "Serializer.h"

CPU::CPU(GB *gb, Memory &MMU, Controller &controller) : 
  gb(gb),
  MMU(MMU), 
  controller(controller) {
  registers[0] = 0x0; // B
  registers[1] = 0x13; // C
  registers[2] = 0x0; // D
  registers[3] = 0xD8; // E
  registers[4] = 0x1; // H
  registers[5] = 0x4D; // L
  registers[6] = 0xB0; // F
  registers[7] = 0x1; // A

  if (gb->CGBModeEnabled()) {
    registers[7] = 0x11;
  }

  SP = 0xFFFE;
  PC = 0x100;

  halted = false;
  stopped = false;
  interruptsEnabled = true;

  // 00
  opCodeMap[0x00] = &CPU::NOP;
  opCodeMap[0x01] = &CPU::LD_BC_nn;
  opCodeMap[0x02] = &CPU::LD_BCm_A;
  opCodeMap[0x03] = &CPU::INC_BC;
  opCodeMap[0x04] = &CPU::INC_r;
  opCodeMap[0x05] = &CPU::DEC_r;
  opCodeMap[0x06] = &CPU::LD_r_n;
  opCodeMap[0x07] = &CPU::RLCA;
  opCodeMap[0x08] = &CPU::LD_nnm_SP;
  opCodeMap[0x09] = &CPU::ADD_HL_BC;
  opCodeMap[0x0A] = &CPU::LD_A_BCm;
  opCodeMap[0x0B] = &CPU::DEC_BC;
  opCodeMap[0x0C] = &CPU::INC_r;
  opCodeMap[0x0D] = &CPU::DEC_r;
  opCodeMap[0x0E] = &CPU::LD_r_n;
  opCodeMap[0x0F] = &CPU::RRCA;

  // 10
  opCodeMap[0x10] = &CPU::STOP;
  opCodeMap[0x11] = &CPU::LD_DE_nn;
  opCodeMap[0x12] = &CPU::LD_DEm_A;
  opCodeMap[0x13] = &CPU::INC_DE;
  opCodeMap[0x14] = &CPU::INC_r;
  opCodeMap[0x15] = &CPU::DEC_r;
  opCodeMap[0x16] = &CPU::LD_r_n;
  opCodeMap[0x17] = &CPU::RLA;
  opCodeMap[0x18] = &CPU::JR_n;
  opCodeMap[0x19] = &CPU::ADD_HL_DE;
  opCodeMap[0x1A] = &CPU::LD_A_DEm;
  opCodeMap[0x1B] = &CPU::DEC_DE;
  opCodeMap[0x1C] = &CPU::INC_r;
  opCodeMap[0x1D] = &CPU::DEC_r;
  opCodeMap[0x1E] = &CPU::LD_r_n;
  opCodeMap[0x1F] = &CPU::RRA;

  // 20
  opCodeMap[0x20] = &CPU::JR_NZ_n;
  opCodeMap[0x21] = &CPU::LD_HL_nn;
  opCodeMap[0x22] = &CPU::LDI_HLm_A;
  opCodeMap[0x23] = &CPU::INC_HL;
  opCodeMap[0x24] = &CPU::INC_r;
  opCodeMap[0x25] = &CPU::DEC_r;
  opCodeMap[0x26] = &CPU::LD_r_n;
  opCodeMap[0x27] = &CPU::DAA;
  opCodeMap[0x28] = &CPU::JR_Z_n;
  opCodeMap[0x29] = &CPU::ADD_HL_HL;
  opCodeMap[0x2A] = &CPU::LDI_A_HLm;
  opCodeMap[0x2B] = &CPU::DEC_HL;
  opCodeMap[0x2C] = &CPU::INC_r;
  opCodeMap[0x2D] = &CPU::DEC_r;
  opCodeMap[0x2E] = &CPU::LD_r_n;
  opCodeMap[0x2F] = &CPU::CPL;

  // 30
  opCodeMap[0x30] = &CPU::JR_NC_n;
  opCodeMap[0x31] = &CPU::LD_SP_nn;
  opCodeMap[0x32] = &CPU::LDD_HLm_A;
  opCodeMap[0x33] = &CPU::INC_SP;
  opCodeMap[0x34] = &CPU::INC_HLm;
  opCodeMap[0x35] = &CPU::DEC_HLm;
  opCodeMap[0x36] = &CPU::LD_HLm_n;
  opCodeMap[0x37] = &CPU::SCF;
  opCodeMap[0x38] = &CPU::JR_C_n;
  opCodeMap[0x39] = &CPU::ADD_HL_SP;
  opCodeMap[0x3A] = &CPU::LDD_A_HLm;
  opCodeMap[0x3B] = &CPU::DEC_SP;
  opCodeMap[0x3C] = &CPU::INC_r;
  opCodeMap[0x3D] = &CPU::DEC_r;
  opCodeMap[0x3E] = &CPU::LD_r_n;
  opCodeMap[0x3F] = &CPU::CCF;

  // 40
  opCodeMap[0x40] = &CPU::LD_r1_r2;
  opCodeMap[0x41] = &CPU::LD_r1_r2;
  opCodeMap[0x42] = &CPU::LD_r1_r2;
  opCodeMap[0x43] = &CPU::LD_r1_r2;
  opCodeMap[0x44] = &CPU::LD_r1_r2;
  opCodeMap[0x45] = &CPU::LD_r1_r2;
  opCodeMap[0x46] = &CPU::LD_r_HLm;
  opCodeMap[0x47] = &CPU::LD_r1_r2;
  opCodeMap[0x48] = &CPU::LD_r1_r2;
  opCodeMap[0x49] = &CPU::LD_r1_r2;
  opCodeMap[0x4A] = &CPU::LD_r1_r2;
  opCodeMap[0x4B] = &CPU::LD_r1_r2;
  opCodeMap[0x4C] = &CPU::LD_r1_r2;
  opCodeMap[0x4D] = &CPU::LD_r1_r2;
  opCodeMap[0x4E] = &CPU::LD_r_HLm;
  opCodeMap[0x4F] = &CPU::LD_r1_r2;

  // 50
  opCodeMap[0x50] = &CPU::LD_r1_r2;
  opCodeMap[0x51] = &CPU::LD_r1_r2;
  opCodeMap[0x52] = &CPU::LD_r1_r2;
  opCodeMap[0x53] = &CPU::LD_r1_r2;
  opCodeMap[0x54] = &CPU::LD_r1_r2;
  opCodeMap[0x55] = &CPU::LD_r1_r2;
  opCodeMap[0x56] = &CPU::LD_r_HLm;
  opCodeMap[0x57] = &CPU::LD_r1_r2;
  opCodeMap[0x58] = &CPU::LD_r1_r2;
  opCodeMap[0x59] = &CPU::LD_r1_r2;
  opCodeMap[0x5A] = &CPU::LD_r1_r2;
  opCodeMap[0x5B] = &CPU::LD_r1_r2;
  opCodeMap[0x5C] = &CPU::LD_r1_r2;
  opCodeMap[0x5D] = &CPU::LD_r1_r2;
  opCodeMap[0x5E] = &CPU::LD_r_HLm;
  opCodeMap[0x5F] = &CPU::LD_r1_r2;

  // 60
  opCodeMap[0x60] = &CPU::LD_r1_r2;
  opCodeMap[0x61] = &CPU::LD_r1_r2;
  opCodeMap[0x62] = &CPU::LD_r1_r2;
  opCodeMap[0x63] = &CPU::LD_r1_r2;
  opCodeMap[0x64] = &CPU::LD_r1_r2;
  opCodeMap[0x65] = &CPU::LD_r1_r2;
  opCodeMap[0x66] = &CPU::LD_r_HLm;
  opCodeMap[0x67] = &CPU::LD_r1_r2;
  opCodeMap[0x68] = &CPU::LD_r1_r2;
  opCodeMap[0x69] = &CPU::LD_r1_r2;
  opCodeMap[0x6A] = &CPU::LD_r1_r2;
  opCodeMap[0x6B] = &CPU::LD_r1_r2;
  opCodeMap[0x6C] = &CPU::LD_r1_r2;
  opCodeMap[0x6D] = &CPU::LD_r1_r2;
  opCodeMap[0x6E] = &CPU::LD_r_HLm;
  opCodeMap[0x6F] = &CPU::LD_r1_r2;

  // 70
  opCodeMap[0x70] = &CPU::LD_HLm_r;
  opCodeMap[0x71] = &CPU::LD_HLm_r;
  opCodeMap[0x72] = &CPU::LD_HLm_r;
  opCodeMap[0x73] = &CPU::LD_HLm_r;
  opCodeMap[0x74] = &CPU::LD_HLm_r;
  opCodeMap[0x75] = &CPU::LD_HLm_r;
  opCodeMap[0x76] = &CPU::HALT;
  opCodeMap[0x77] = &CPU::LD_HLm_r;
  opCodeMap[0x78] = &CPU::LD_r1_r2;
  opCodeMap[0x79] = &CPU::LD_r1_r2;
  opCodeMap[0x7A] = &CPU::LD_r1_r2;
  opCodeMap[0x7B] = &CPU::LD_r1_r2;
  opCodeMap[0x7C] = &CPU::LD_r1_r2;
  opCodeMap[0x7D] = &CPU::LD_r1_r2;
  opCodeMap[0x7E] = &CPU::LD_r_HLm;
  opCodeMap[0x7F] = &CPU::LD_r1_r2;

  // 80
  opCodeMap[0x80] = &CPU::ADD_A_r;
  opCodeMap[0x81] = &CPU::ADD_A_r;
  opCodeMap[0x82] = &CPU::ADD_A_r;
  opCodeMap[0x83] = &CPU::ADD_A_r;
  opCodeMap[0x84] = &CPU::ADD_A_r;
  opCodeMap[0x85] = &CPU::ADD_A_r;
  opCodeMap[0x86] = &CPU::ADD_A_HLm;
  opCodeMap[0x87] = &CPU::ADD_A_r;
  opCodeMap[0x88] = &CPU::ADC_A_r;
  opCodeMap[0x89] = &CPU::ADC_A_r;
  opCodeMap[0x8A] = &CPU::ADC_A_r;
  opCodeMap[0x8B] = &CPU::ADC_A_r;
  opCodeMap[0x8C] = &CPU::ADC_A_r;
  opCodeMap[0x8D] = &CPU::ADC_A_r;
  opCodeMap[0x8E] = &CPU::ADC_A_HLm;
  opCodeMap[0x8F] = &CPU::ADC_A_r;

  // 90
  opCodeMap[0x90] = &CPU::SUB_A_r;
  opCodeMap[0x91] = &CPU::SUB_A_r;
  opCodeMap[0x92] = &CPU::SUB_A_r;
  opCodeMap[0x93] = &CPU::SUB_A_r;
  opCodeMap[0x94] = &CPU::SUB_A_r;
  opCodeMap[0x95] = &CPU::SUB_A_r;
  opCodeMap[0x96] = &CPU::SUB_A_HLm;
  opCodeMap[0x97] = &CPU::SUB_A_r;
  opCodeMap[0x98] = &CPU::SBC_A_r;
  opCodeMap[0x99] = &CPU::SBC_A_r;
  opCodeMap[0x9A] = &CPU::SBC_A_r;
  opCodeMap[0x9B] = &CPU::SBC_A_r;
  opCodeMap[0x9C] = &CPU::SBC_A_r;
  opCodeMap[0x9D] = &CPU::SBC_A_r;
  opCodeMap[0x9E] = &CPU::SBC_A_HLm;
  opCodeMap[0x9F] = &CPU::SBC_A_r;

  // A0
  opCodeMap[0xA0] = &CPU::AND_A_r;
  opCodeMap[0xA1] = &CPU::AND_A_r;
  opCodeMap[0xA2] = &CPU::AND_A_r;
  opCodeMap[0xA3] = &CPU::AND_A_r;
  opCodeMap[0xA4] = &CPU::AND_A_r;
  opCodeMap[0xA5] = &CPU::AND_A_r;
  opCodeMap[0xA6] = &CPU::AND_A_HLm;
  opCodeMap[0xA7] = &CPU::AND_A_r;
  opCodeMap[0xA8] = &CPU::XOR_A_r;
  opCodeMap[0xA9] = &CPU::XOR_A_r;
  opCodeMap[0xAA] = &CPU::XOR_A_r;
  opCodeMap[0xAB] = &CPU::XOR_A_r;
  opCodeMap[0xAC] = &CPU::XOR_A_r;
  opCodeMap[0xAD] = &CPU::XOR_A_r;
  opCodeMap[0xAE] = &CPU::XOR_A_HLm;
  opCodeMap[0xAF] = &CPU::XOR_A_r;

  // B0
  opCodeMap[0xB0] = &CPU::OR_A_r;
  opCodeMap[0xB1] = &CPU::OR_A_r;
  opCodeMap[0xB2] = &CPU::OR_A_r;
  opCodeMap[0xB3] = &CPU::OR_A_r;
  opCodeMap[0xB4] = &CPU::OR_A_r;
  opCodeMap[0xB5] = &CPU::OR_A_r;
  opCodeMap[0xB6] = &CPU::OR_A_HLm;
  opCodeMap[0xB7] = &CPU::OR_A_r;
  opCodeMap[0xB8] = &CPU::CP_A_r;
  opCodeMap[0xB9] = &CPU::CP_A_r;
  opCodeMap[0xBA] = &CPU::CP_A_r;
  opCodeMap[0xBB] = &CPU::CP_A_r;
  opCodeMap[0xBC] = &CPU::CP_A_r;
  opCodeMap[0xBD] = &CPU::CP_A_r;
  opCodeMap[0xBE] = &CPU::CP_A_HLm;
  opCodeMap[0xBF] = &CPU::CP_A_r;

  // C0
  opCodeMap[0xC0] = &CPU::RET_NZ;
  opCodeMap[0xC1] = &CPU::POP_BC;
  opCodeMap[0xC2] = &CPU::JP_NZ_nn;
  opCodeMap[0xC3] = &CPU::JP_nn;
  opCodeMap[0xC4] = &CPU::CALL_NZ_nn;
  opCodeMap[0xC5] = &CPU::PUSH_BC;
  opCodeMap[0xC6] = &CPU::ADD_A_n;
  opCodeMap[0xC7] = &CPU::RST_n;
  opCodeMap[0xC8] = &CPU::RET_Z;
  opCodeMap[0xC9] = &CPU::RET;
  opCodeMap[0xCA] = &CPU::JP_Z_nn;
  //opCodeMap[0xCB] MAPPED TO 0xCB MAP
  opCodeMap[0xCC] = &CPU::CALL_Z_nn;
  opCodeMap[0xCD] = &CPU::CALL_nn;
  opCodeMap[0xCE] = &CPU::ADC_A_n;
  opCodeMap[0xCF] = &CPU::RST_n;

  // D0
  opCodeMap[0xD0] = &CPU::RET_NC;
  opCodeMap[0xD1] = &CPU::POP_DE;
  opCodeMap[0xD2] = &CPU::JP_NC_nn;
  //opCodeMap[0xD3] UNUSED
  opCodeMap[0xD4] = &CPU::CALL_NC_nn;
  opCodeMap[0xD5] = &CPU::PUSH_DE;
  opCodeMap[0xD6] = &CPU::SUB_A_n;
  opCodeMap[0xD7] = &CPU::RST_n;
  opCodeMap[0xD8] = &CPU::RET_C;
  opCodeMap[0xD9] = &CPU::RETI;
  opCodeMap[0xDA] = &CPU::JP_C_nn;
  //opCodeMap[0xDB] UNUSED
  opCodeMap[0xDC] = &CPU::CALL_C_nn;
  //opCodeMap[0xDD] UNUSED
  opCodeMap[0xDE] = &CPU::SBC_A_n;
  opCodeMap[0xDF] = &CPU::RST_n;

  // E0
  opCodeMap[0xE0] = &CPU::LDH_nm_A;
  opCodeMap[0xE1] = &CPU::POP_HL;
  opCodeMap[0xE2] = &CPU::LD_Cm_A;
  //opCodeMap[0xE3] UNUSED
  //opCodeMap[0xE4] UNUSED
  opCodeMap[0xE5] = &CPU::PUSH_HL;
  opCodeMap[0xE6] = &CPU::AND_A_n;
  opCodeMap[0xE7] = &CPU::RST_n;
  opCodeMap[0xE8] = &CPU::ADD_SP_n;
  opCodeMap[0xE9] = &CPU::JP_HL;
  opCodeMap[0xEA] = &CPU::LD_nnm_A;
  //opCodeMap[0xEB] UNUSED
  //opCodeMap[0xEC] UNUSED
  //opCodeMap[0xED] UNUSED
  opCodeMap[0xEE] = &CPU::XOR_A_n;
  opCodeMap[0xEF] = &CPU::RST_n;

  // F0
  opCodeMap[0xF0] = &CPU::LDH_A_nm;
  opCodeMap[0xF1] = &CPU::POP_AF;
  opCodeMap[0xF2] = &CPU::LD_A_Cm;
  opCodeMap[0xF3] = &CPU::DI;
  //opCodeMap[0xF4] UNUSED
  opCodeMap[0xF5] = &CPU::PUSH_AF;
  opCodeMap[0xF6] = &CPU::OR_A_n;
  opCodeMap[0xF7] = &CPU::RST_n;
  opCodeMap[0xF8] = &CPU::LDHL_SP_n;
  opCodeMap[0xF9] = &CPU::LD_SP_HL;
  opCodeMap[0xFA] = &CPU::LD_A_nnm;
  opCodeMap[0xFB] = &CPU::EI;
  //opCodeMap[0xFC] UNUSED
  //opCodeMap[0xFD] UNUSED
  opCodeMap[0xFE] = &CPU::CP_A_n;
  opCodeMap[0xFF] = &CPU::RST_n;

  /*
  Z80 Command Set - CB
  */

  // 00
  opCodeMapCB[0x00] = &CPU::RLC_r;
  opCodeMapCB[0x01] = &CPU::RLC_r;
  opCodeMapCB[0x02] = &CPU::RLC_r;
  opCodeMapCB[0x03] = &CPU::RLC_r;
  opCodeMapCB[0x04] = &CPU::RLC_r;
  opCodeMapCB[0x05] = &CPU::RLC_r;
  opCodeMapCB[0x06] = &CPU::RLC_HLm;
  opCodeMapCB[0x07] = &CPU::RLC_r;
  opCodeMapCB[0x08] = &CPU::RRC_r;
  opCodeMapCB[0x09] = &CPU::RRC_r;
  opCodeMapCB[0x0A] = &CPU::RRC_r;
  opCodeMapCB[0x0B] = &CPU::RRC_r;
  opCodeMapCB[0x0C] = &CPU::RRC_r;
  opCodeMapCB[0x0D] = &CPU::RRC_r;
  opCodeMapCB[0x0E] = &CPU::RRC_HLm;
  opCodeMapCB[0x0F] = &CPU::RRC_r;

  // 10
  opCodeMapCB[0x10] = &CPU::RL_r;
  opCodeMapCB[0x11] = &CPU::RL_r;
  opCodeMapCB[0x12] = &CPU::RL_r;
  opCodeMapCB[0x13] = &CPU::RL_r;
  opCodeMapCB[0x14] = &CPU::RL_r;
  opCodeMapCB[0x15] = &CPU::RL_r;
  opCodeMapCB[0x16] = &CPU::RL_HLm;
  opCodeMapCB[0x17] = &CPU::RL_r;
  opCodeMapCB[0x18] = &CPU::RR_r;
  opCodeMapCB[0x19] = &CPU::RR_r;
  opCodeMapCB[0x1A] = &CPU::RR_r;
  opCodeMapCB[0x1B] = &CPU::RR_r;
  opCodeMapCB[0x1C] = &CPU::RR_r;
  opCodeMapCB[0x1D] = &CPU::RR_r;
  opCodeMapCB[0x1E] = &CPU::RR_HLm;
  opCodeMapCB[0x1F] = &CPU::RR_r;

  // 20
  opCodeMapCB[0x20] = &CPU::SLA_r;
  opCodeMapCB[0x21] = &CPU::SLA_r;
  opCodeMapCB[0x22] = &CPU::SLA_r;
  opCodeMapCB[0x23] = &CPU::SLA_r;
  opCodeMapCB[0x24] = &CPU::SLA_r;
  opCodeMapCB[0x25] = &CPU::SLA_r;
  opCodeMapCB[0x26] = &CPU::SLA_HLm;
  opCodeMapCB[0x27] = &CPU::SLA_r;
  opCodeMapCB[0x28] = &CPU::SRA_r;
  opCodeMapCB[0x29] = &CPU::SRA_r;
  opCodeMapCB[0x2A] = &CPU::SRA_r;
  opCodeMapCB[0x2B] = &CPU::SRA_r;
  opCodeMapCB[0x2C] = &CPU::SRA_r;
  opCodeMapCB[0x2D] = &CPU::SRA_r;
  opCodeMapCB[0x2E] = &CPU::SRA_HLm;
  opCodeMapCB[0x2F] = &CPU::SRA_r;

  // 30
  opCodeMapCB[0x30] = &CPU::SWAP_r;
  opCodeMapCB[0x31] = &CPU::SWAP_r;
  opCodeMapCB[0x32] = &CPU::SWAP_r;
  opCodeMapCB[0x33] = &CPU::SWAP_r;
  opCodeMapCB[0x34] = &CPU::SWAP_r;
  opCodeMapCB[0x35] = &CPU::SWAP_r;
  opCodeMapCB[0x36] = &CPU::SWAP_HLm;
  opCodeMapCB[0x37] = &CPU::SWAP_r;
  opCodeMapCB[0x38] = &CPU::SRL_r;
  opCodeMapCB[0x39] = &CPU::SRL_r;
  opCodeMapCB[0x3A] = &CPU::SRL_r;
  opCodeMapCB[0x3B] = &CPU::SRL_r;
  opCodeMapCB[0x3C] = &CPU::SRL_r;
  opCodeMapCB[0x3D] = &CPU::SRL_r;
  opCodeMapCB[0x3E] = &CPU::SRL_HLm;
  opCodeMapCB[0x3F] = &CPU::SRL_r;

  // 40
  opCodeMapCB[0x40] = &CPU::BIT_b_r;
  opCodeMapCB[0x41] = &CPU::BIT_b_r;
  opCodeMapCB[0x42] = &CPU::BIT_b_r;
  opCodeMapCB[0x43] = &CPU::BIT_b_r;
  opCodeMapCB[0x44] = &CPU::BIT_b_r;
  opCodeMapCB[0x45] = &CPU::BIT_b_r;
  opCodeMapCB[0x46] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x47] = &CPU::BIT_b_r;
  opCodeMapCB[0x48] = &CPU::BIT_b_r;
  opCodeMapCB[0x49] = &CPU::BIT_b_r;
  opCodeMapCB[0x4A] = &CPU::BIT_b_r;
  opCodeMapCB[0x4B] = &CPU::BIT_b_r;
  opCodeMapCB[0x4C] = &CPU::BIT_b_r;
  opCodeMapCB[0x4D] = &CPU::BIT_b_r;
  opCodeMapCB[0x4E] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x4F] = &CPU::BIT_b_r;

  // 50
  opCodeMapCB[0x50] = &CPU::BIT_b_r;
  opCodeMapCB[0x51] = &CPU::BIT_b_r;
  opCodeMapCB[0x52] = &CPU::BIT_b_r;
  opCodeMapCB[0x53] = &CPU::BIT_b_r;
  opCodeMapCB[0x54] = &CPU::BIT_b_r;
  opCodeMapCB[0x55] = &CPU::BIT_b_r;
  opCodeMapCB[0x56] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x57] = &CPU::BIT_b_r;
  opCodeMapCB[0x58] = &CPU::BIT_b_r;
  opCodeMapCB[0x59] = &CPU::BIT_b_r;
  opCodeMapCB[0x5A] = &CPU::BIT_b_r;
  opCodeMapCB[0x5B] = &CPU::BIT_b_r;
  opCodeMapCB[0x5C] = &CPU::BIT_b_r;
  opCodeMapCB[0x5D] = &CPU::BIT_b_r;
  opCodeMapCB[0x5E] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x5F] = &CPU::BIT_b_r;

  // 60
  opCodeMapCB[0x60] = &CPU::BIT_b_r;
  opCodeMapCB[0x61] = &CPU::BIT_b_r;
  opCodeMapCB[0x62] = &CPU::BIT_b_r;
  opCodeMapCB[0x63] = &CPU::BIT_b_r;
  opCodeMapCB[0x64] = &CPU::BIT_b_r;
  opCodeMapCB[0x65] = &CPU::BIT_b_r;
  opCodeMapCB[0x66] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x67] = &CPU::BIT_b_r;
  opCodeMapCB[0x68] = &CPU::BIT_b_r;
  opCodeMapCB[0x69] = &CPU::BIT_b_r;
  opCodeMapCB[0x6A] = &CPU::BIT_b_r;
  opCodeMapCB[0x6B] = &CPU::BIT_b_r;
  opCodeMapCB[0x6C] = &CPU::BIT_b_r;
  opCodeMapCB[0x6D] = &CPU::BIT_b_r;
  opCodeMapCB[0x6E] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x6F] = &CPU::BIT_b_r;

  // 70
  opCodeMapCB[0x70] = &CPU::BIT_b_r;
  opCodeMapCB[0x71] = &CPU::BIT_b_r;
  opCodeMapCB[0x72] = &CPU::BIT_b_r;
  opCodeMapCB[0x73] = &CPU::BIT_b_r;
  opCodeMapCB[0x74] = &CPU::BIT_b_r;
  opCodeMapCB[0x75] = &CPU::BIT_b_r;
  opCodeMapCB[0x76] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x77] = &CPU::BIT_b_r;
  opCodeMapCB[0x78] = &CPU::BIT_b_r;
  opCodeMapCB[0x79] = &CPU::BIT_b_r;
  opCodeMapCB[0x7A] = &CPU::BIT_b_r;
  opCodeMapCB[0x7B] = &CPU::BIT_b_r;
  opCodeMapCB[0x7C] = &CPU::BIT_b_r;
  opCodeMapCB[0x7D] = &CPU::BIT_b_r;
  opCodeMapCB[0x7E] = &CPU::BIT_b_HLm;
  opCodeMapCB[0x7F] = &CPU::BIT_b_r;

  // 80
  opCodeMapCB[0x80] = &CPU::RES_b_r;
  opCodeMapCB[0x81] = &CPU::RES_b_r;
  opCodeMapCB[0x82] = &CPU::RES_b_r;
  opCodeMapCB[0x83] = &CPU::RES_b_r;
  opCodeMapCB[0x84] = &CPU::RES_b_r;
  opCodeMapCB[0x85] = &CPU::RES_b_r;
  opCodeMapCB[0x86] = &CPU::RES_b_HLm;
  opCodeMapCB[0x87] = &CPU::RES_b_r;
  opCodeMapCB[0x88] = &CPU::RES_b_r;
  opCodeMapCB[0x89] = &CPU::RES_b_r;
  opCodeMapCB[0x8A] = &CPU::RES_b_r;
  opCodeMapCB[0x8B] = &CPU::RES_b_r;
  opCodeMapCB[0x8C] = &CPU::RES_b_r;
  opCodeMapCB[0x8D] = &CPU::RES_b_r;
  opCodeMapCB[0x8E] = &CPU::RES_b_HLm;
  opCodeMapCB[0x8F] = &CPU::RES_b_r;

  // 90
  opCodeMapCB[0x90] = &CPU::RES_b_r;
  opCodeMapCB[0x91] = &CPU::RES_b_r;
  opCodeMapCB[0x92] = &CPU::RES_b_r;
  opCodeMapCB[0x93] = &CPU::RES_b_r;
  opCodeMapCB[0x94] = &CPU::RES_b_r;
  opCodeMapCB[0x95] = &CPU::RES_b_r;
  opCodeMapCB[0x96] = &CPU::RES_b_HLm;
  opCodeMapCB[0x97] = &CPU::RES_b_r;
  opCodeMapCB[0x98] = &CPU::RES_b_r;
  opCodeMapCB[0x99] = &CPU::RES_b_r;
  opCodeMapCB[0x9A] = &CPU::RES_b_r;
  opCodeMapCB[0x9B] = &CPU::RES_b_r;
  opCodeMapCB[0x9C] = &CPU::RES_b_r;
  opCodeMapCB[0x9D] = &CPU::RES_b_r;
  opCodeMapCB[0x9E] = &CPU::RES_b_HLm;
  opCodeMapCB[0x9F] = &CPU::RES_b_r;

  // A0
  opCodeMapCB[0xA0] = &CPU::RES_b_r;
  opCodeMapCB[0xA1] = &CPU::RES_b_r;
  opCodeMapCB[0xA2] = &CPU::RES_b_r;
  opCodeMapCB[0xA3] = &CPU::RES_b_r;
  opCodeMapCB[0xA4] = &CPU::RES_b_r;
  opCodeMapCB[0xA5] = &CPU::RES_b_r;
  opCodeMapCB[0xA6] = &CPU::RES_b_HLm;
  opCodeMapCB[0xA7] = &CPU::RES_b_r;
  opCodeMapCB[0xA8] = &CPU::RES_b_r;
  opCodeMapCB[0xA9] = &CPU::RES_b_r;
  opCodeMapCB[0xAA] = &CPU::RES_b_r;
  opCodeMapCB[0xAB] = &CPU::RES_b_r;
  opCodeMapCB[0xAC] = &CPU::RES_b_r;
  opCodeMapCB[0xAD] = &CPU::RES_b_r;
  opCodeMapCB[0xAE] = &CPU::RES_b_HLm;
  opCodeMapCB[0xAF] = &CPU::RES_b_r;

  // B0
  opCodeMapCB[0xB0] = &CPU::RES_b_r;
  opCodeMapCB[0xB1] = &CPU::RES_b_r;
  opCodeMapCB[0xB2] = &CPU::RES_b_r;
  opCodeMapCB[0xB3] = &CPU::RES_b_r;
  opCodeMapCB[0xB4] = &CPU::RES_b_r;
  opCodeMapCB[0xB5] = &CPU::RES_b_r;
  opCodeMapCB[0xB6] = &CPU::RES_b_HLm;
  opCodeMapCB[0xB7] = &CPU::RES_b_r;
  opCodeMapCB[0xB8] = &CPU::RES_b_r;
  opCodeMapCB[0xB9] = &CPU::RES_b_r;
  opCodeMapCB[0xBA] = &CPU::RES_b_r;
  opCodeMapCB[0xBB] = &CPU::RES_b_r;
  opCodeMapCB[0xBC] = &CPU::RES_b_r;
  opCodeMapCB[0xBD] = &CPU::RES_b_r;
  opCodeMapCB[0xBE] = &CPU::RES_b_HLm;
  opCodeMapCB[0xBF] = &CPU::RES_b_r;

  // C0
  opCodeMapCB[0xC0] = &CPU::SET_b_r;
  opCodeMapCB[0xC1] = &CPU::SET_b_r;
  opCodeMapCB[0xC2] = &CPU::SET_b_r;
  opCodeMapCB[0xC3] = &CPU::SET_b_r;
  opCodeMapCB[0xC4] = &CPU::SET_b_r;
  opCodeMapCB[0xC5] = &CPU::SET_b_r;
  opCodeMapCB[0xC6] = &CPU::SET_b_HLm;
  opCodeMapCB[0xC7] = &CPU::SET_b_r;
  opCodeMapCB[0xC8] = &CPU::SET_b_r;
  opCodeMapCB[0xC9] = &CPU::SET_b_r;
  opCodeMapCB[0xCA] = &CPU::SET_b_r;
  opCodeMapCB[0xCB] = &CPU::SET_b_r;
  opCodeMapCB[0xCC] = &CPU::SET_b_r;
  opCodeMapCB[0xCD] = &CPU::SET_b_r;
  opCodeMapCB[0xCE] = &CPU::SET_b_HLm;
  opCodeMapCB[0xCF] = &CPU::SET_b_r;

  // D0
  opCodeMapCB[0xD0] = &CPU::SET_b_r;
  opCodeMapCB[0xD1] = &CPU::SET_b_r;
  opCodeMapCB[0xD2] = &CPU::SET_b_r;
  opCodeMapCB[0xD3] = &CPU::SET_b_r;
  opCodeMapCB[0xD4] = &CPU::SET_b_r;
  opCodeMapCB[0xD5] = &CPU::SET_b_r;
  opCodeMapCB[0xD6] = &CPU::SET_b_HLm;
  opCodeMapCB[0xD7] = &CPU::SET_b_r;
  opCodeMapCB[0xD8] = &CPU::SET_b_r;
  opCodeMapCB[0xD9] = &CPU::SET_b_r;
  opCodeMapCB[0xDA] = &CPU::SET_b_r;
  opCodeMapCB[0xDB] = &CPU::SET_b_r;
  opCodeMapCB[0xDC] = &CPU::SET_b_r;
  opCodeMapCB[0xDD] = &CPU::SET_b_r;
  opCodeMapCB[0xDE] = &CPU::SET_b_HLm;
  opCodeMapCB[0xDF] = &CPU::SET_b_r;

  // E0
  opCodeMapCB[0xE0] = &CPU::SET_b_r;
  opCodeMapCB[0xE1] = &CPU::SET_b_r;
  opCodeMapCB[0xE2] = &CPU::SET_b_r;
  opCodeMapCB[0xE3] = &CPU::SET_b_r;
  opCodeMapCB[0xE4] = &CPU::SET_b_r;
  opCodeMapCB[0xE5] = &CPU::SET_b_r;
  opCodeMapCB[0xE6] = &CPU::SET_b_HLm;
  opCodeMapCB[0xE7] = &CPU::SET_b_r;
  opCodeMapCB[0xE8] = &CPU::SET_b_r;
  opCodeMapCB[0xE9] = &CPU::SET_b_r;
  opCodeMapCB[0xEA] = &CPU::SET_b_r;
  opCodeMapCB[0xEB] = &CPU::SET_b_r;
  opCodeMapCB[0xEC] = &CPU::SET_b_r;
  opCodeMapCB[0xED] = &CPU::SET_b_r;
  opCodeMapCB[0xEE] = &CPU::SET_b_HLm;
  opCodeMapCB[0xEF] = &CPU::SET_b_r;

  // F0
  opCodeMapCB[0xF0] = &CPU::SET_b_r;
  opCodeMapCB[0xF1] = &CPU::SET_b_r;
  opCodeMapCB[0xF2] = &CPU::SET_b_r;
  opCodeMapCB[0xF3] = &CPU::SET_b_r;
  opCodeMapCB[0xF4] = &CPU::SET_b_r;
  opCodeMapCB[0xF5] = &CPU::SET_b_r;
  opCodeMapCB[0xF6] = &CPU::SET_b_HLm;
  opCodeMapCB[0xF7] = &CPU::SET_b_r;
  opCodeMapCB[0xF8] = &CPU::SET_b_r;
  opCodeMapCB[0xF9] = &CPU::SET_b_r;
  opCodeMapCB[0xFA] = &CPU::SET_b_r;
  opCodeMapCB[0xFB] = &CPU::SET_b_r;
  opCodeMapCB[0xFC] = &CPU::SET_b_r;
  opCodeMapCB[0xFD] = &CPU::SET_b_r;
  opCodeMapCB[0xFE] = &CPU::SET_b_HLm;
  opCodeMapCB[0xFF] = &CPU::SET_b_r;
}

int CPU::Advance() {
  int cycles = 0;
  if (!halted && !stopped) {
    byte opcode = ReadByte();
    if (opcode == 0xCB) {
      opcode = ReadByte();
      cycles += (this->*opCodeMapCB[opcode])(opcode);
    }
    else if (opCodeMap[opcode] != nullptr) {
      cycles += (this->*opCodeMap[opcode])(opcode);
    }
  }
  else cycles = 4;
  cycles += HandleInterrupts();
  return cycles;
}

void CPU::RequestInterrupt(int id) {
  MMU.WriteByte(IF, MMU.ReadByte(IF) | (1 << id));
}

int CPU::HandleInterrupts() {
  byte interrupt = MMU.ReadByte(IF) & MMU.ReadByte(IE) & 0x1F;
  if (interrupt) halted = false;
  if (!interruptsEnabled || !interrupt) return 0;
  int interruptCycles = 0;
  for (int i = 0; i < 5; i++) {
    if (interrupt & (1 << i)) {
      PerformInterrupt(i);
      interruptCycles += 20;
      break;
    }
  }
  return interruptCycles;
}

void CPU::SetInputCallback(std::function<bool(void)> cb) {
  inputCallback = cb;
}

void CPU::HandleInput() {
  if (inputCallback != nullptr && inputCallback()) {
    RequestInterrupt(4);
  }
}

void CPU::Serialize(Serializer & s) {
  s.SerializeArray<byte>(registers, 8);
  s.Serialize<word>(SP);
  s.Serialize<word>(PC);
  s.Serialize<bool>(halted);
  s.Serialize<bool>(stopped);
  s.Serialize<bool>(interruptsEnabled);
}

void CPU::Deserialize(Serializer & s) {
  s.DeserializeArray<byte>(registers, 8);
  SP = s.Deserialize<word>();
  PC = s.Deserialize<word>();
  halted = s.Deserialize<bool>();
  stopped = s.Deserialize<bool>();
  interruptsEnabled = s.Deserialize<bool>();
}

void CPU::PerformInterrupt(int id) {
  interruptsEnabled = false;
  MMU.WriteByte(IF, (MMU.ReadByte(IF) & ~(1 << id)));
  SP -= 2;
  MMU.WriteWord(SP, PC);
  switch (id) {
  case 0: PC = 0x40; break;
  case 1: PC = 0x48; break;
  case 2: PC = 0x50; break;
  case 3: PC = 0x58; break;
  case 4: PC = 0x60; break;
  }
}

word CPU::CombineRegisters(const byte & r1_id, const byte & r2_id) {
  return (registers[r1_id] << 8) | registers[r2_id];
}

void CPU::SplitIntoRegisters(const word & val, const byte & r1_id, const byte & r2_id) {
  registers[r1_id] = (val & 0xFF00) >> 8;
  registers[r2_id] = val & 0xFF;
}

byte CPU::ReadByte() {
  byte ret = MMU.ReadByte(PC);
  PC++;
  return ret;
}

word CPU::ReadWord() {
  word ret = MMU.ReadWord(PC);
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
  int result = b1 + b2;
  if ((byte)result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_SUBTRACT);
  if ((result & 0x100) == 0x100) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (((b1 & 0xF) + (b2 & 0xF) & 0x10) == 0x10) SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  return result;
}

void CPU::ADC(const byte & b) {
  int carry = GetFlag(FLAG_CARRY);
  byte result = registers[7] + b + carry;
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  if ((registers[7] & 0xF) + (b & 0xF) + carry > 0xF)
    SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  if ((int)registers[7] + (int)b + carry > 0xFF)
    SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  registers[7] = result;
}

word CPU::AddWords(const word & w1, const word & w2) {
  int result = w1 + w2;
  ResetFlag(FLAG_SUBTRACT);
  if ((result & 0x10000) == 0x10000) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if ((((w1 & 0xFFF) + (w2 & 0xFFF)) & 0x1000) == 0x1000) SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  return result;
}

byte CPU::SubtractBytes(const byte & b1, const byte & b2) {
  byte result = b1 - b2;
  if ((byte)result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  SetFlag(FLAG_SUBTRACT);
  if (b1 < b2) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (((b1 & 0xF) < (b2 & 0xF))) SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  return result;
}

void CPU::SBC(const byte & b) {
  int carry = GetFlag(FLAG_CARRY);
  byte result = registers[7] - b - carry;
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  if ((registers[7] & 0xF) < (b & 0xF) + carry)
    SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  if (registers[7] < b + carry)
    SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  SetFlag(FLAG_SUBTRACT);
  registers[7] = result;
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
  if ((result & 0xF) == 0) SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  return result;
}

byte CPU::DECByte(const byte & b) {
  byte result = b - 1;
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  SetFlag(FLAG_SUBTRACT);
  if ((result & 0xF) == 0xF) SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  return result;
}

byte CPU::RotateLeft(const byte & b, const bool & addCarry, const bool & isCB) {
  byte result = (b << 1);
  if (addCarry) result |= GetFlag(FLAG_CARRY);
  else result |= (b >> 7);
  if (b & (1 << 7)) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  // TODO: not sure what to do with the zero flag here, docs seem conflicted
  if (isCB && result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  return result;
}

byte CPU::RotateRight(const byte & b, const bool & addCarry, const bool & isCB) {
  byte result = (b >> 1);
  if (addCarry) result |= GetFlag(FLAG_CARRY) << 7;
  else result |= (b << 7);
  if (b & 1) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  // TODO: not sure what to do with the zero flag here, docs seem conflicted
  if (isCB && result == 0) SetFlag(FLAG_ZERO);
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
  byte r_id = (op_code >> 3) & 0x7;
  registers[r_id] = MMU.ReadByte(CombineRegisters(4, 5));
  return 8;
}

int CPU::LD_HLm_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  MMU.WriteByte(CombineRegisters(4, 5), registers[r_id]);
  return 8;
}

int CPU::LD_HLm_n(const byte & op_code) {
  MMU.WriteByte(CombineRegisters(4, 5), ReadByte());
  return 12;
}

int CPU::LD_A_BCm(const byte & op_code) {
  registers[7] = MMU.ReadByte(CombineRegisters(0, 1));
  return 8;
}

int CPU::LD_A_DEm(const byte & op_code) {
  registers[7] = MMU.ReadByte(CombineRegisters(2, 3));
  return 8;
}

int CPU::LD_A_nnm(const byte & op_code) {
  registers[7] = MMU.ReadByte(ReadWord());
  return 16;
}

int CPU::LD_BCm_A(const byte & op_code) {
  MMU.WriteByte(CombineRegisters(0, 1), registers[7]);
  return 8;
}

int CPU::LD_DEm_A(const byte & op_code) {
  MMU.WriteByte(CombineRegisters(2, 3), registers[7]);
  return 8;
}

int CPU::LD_nnm_A(const byte & op_code) {
  MMU.WriteByte(ReadWord(), registers[7]);
  return 16;
}

int CPU::LD_A_Cm(const byte & op_code) {
  registers[7] = MMU.ReadByte(0xFF00 + registers[1]);
  return 8;
}

int CPU::LD_Cm_A(const byte & op_code) {
  MMU.WriteByte(0xFF00 + registers[1], registers[7]);
  return 8;
}

int CPU::LDD_A_HLm(const byte & op_code) {
  word HL = CombineRegisters(4, 5);
  registers[7] = MMU.ReadByte(HL);
  SplitIntoRegisters(HL - 1, 4, 5);
  return 8;
}

int CPU::LDD_HLm_A(const byte & op_code) {
  word HL = CombineRegisters(4, 5);
  MMU.WriteByte(HL, registers[7]);
  SplitIntoRegisters(HL - 1, 4, 5);
  return 8;
}

int CPU::LDI_A_HLm(const byte & op_code) {
  word HL = CombineRegisters(4, 5);
  registers[7] = MMU.ReadByte(HL);
  SplitIntoRegisters(HL + 1, 4, 5);
  return 8;
}

int CPU::LDI_HLm_A(const byte & op_code) {
  word HL = CombineRegisters(4, 5);
  MMU.WriteByte(HL, registers[7]);
  SplitIntoRegisters(HL + 1, 4, 5);
  return 8;
}

int CPU::LDH_nm_A(const byte & op_code) {
  MMU.WriteByte(0xFF00 + ReadByte(), registers[7]);
  return 12;
}

int CPU::LDH_A_nm(const byte & op_code) {
  registers[7] = MMU.ReadByte(0xFF00 + ReadByte());
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
  int result = (int)SP + val;
  if (((SP & 0xFF) + (val & 0xFF) & 0x100) == 0x100) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (((SP & 0xF) + (val & 0xF) & 0x10) == 0x10) SetFlag(FLAG_HALF_CARRY);
  else ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_SUBTRACT);
  SplitIntoRegisters(result, 4, 5);
  return 12;
}

int CPU::LD_nnm_SP(const byte & op_code) {
  word addr = ReadWord();
  MMU.WriteWord(addr, SP);
  return 20;
}

int CPU::PUSH_AF(const byte & op_code) {
  MMU.WriteByte(--SP, registers[7]);
  MMU.WriteByte(--SP, registers[6]);
  return 16;
}

int CPU::PUSH_BC(const byte & op_code) {
  MMU.WriteByte(--SP, registers[0]);
  MMU.WriteByte(--SP, registers[1]);
  return 16;
}

int CPU::PUSH_DE(const byte & op_code) {
  MMU.WriteByte(--SP, registers[2]);
  MMU.WriteByte(--SP, registers[3]);
  return 16;
}

int CPU::PUSH_HL(const byte & op_code) {
  MMU.WriteByte(--SP, registers[4]);
  MMU.WriteByte(--SP, registers[5]);
  return 16;
}

int CPU::POP_AF(const byte & op_code) {
  registers[6] = MMU.ReadByte(SP++);
  registers[7] = MMU.ReadByte(SP++);
  // Set the lower 4 bits of F to 0, blargg's tests seem
  // to care about this.
  registers[6] &= 0xF0;
  return 12;
}

int CPU::POP_BC(const byte & op_code) {
  registers[1] = MMU.ReadByte(SP++);
  registers[0] = MMU.ReadByte(SP++);
  return 12;
}

int CPU::POP_DE(const byte & op_code) {
  registers[3] = MMU.ReadByte(SP++);
  registers[2] = MMU.ReadByte(SP++);
  return 12;
}

int CPU::POP_HL(const byte & op_code) {
  registers[5] = MMU.ReadByte(SP++);
  registers[4] = MMU.ReadByte(SP++);
  return 12;
}

int CPU::ADD_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[7] = AddBytes(registers[7], registers[r_id]);
  return 4;
}

int CPU::ADD_A_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  registers[7] = AddBytes(registers[7], val);
  return 8;
}

int CPU::ADD_A_n(const byte & op_code) {
  byte val = ReadByte();
  registers[7] = AddBytes(registers[7], val);
  return 8;
}

int CPU::ADC_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  ADC(registers[r_id]);
  return 4;
}

int CPU::ADC_A_HLm(const byte & op_code) {
  ADC(MMU.ReadByte(CombineRegisters(4, 5)));
  return 8;
}

int CPU::ADC_A_n(const byte & op_code) {
  ADC(ReadByte());
  return 8;
}

int CPU::SUB_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[7] = SubtractBytes(registers[7], registers[r_id]);
  return 4;
}

int CPU::SUB_A_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  registers[7] = SubtractBytes(registers[7], val);
  return 8;
}

int CPU::SUB_A_n(const byte & op_code) {
  byte val = ReadByte();
  registers[7] = SubtractBytes(registers[7], val);
  return 8;
}

int CPU::SBC_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  SBC(registers[r_id]);
  return 4;
}

int CPU::SBC_A_HLm(const byte & op_code) {
  SBC(MMU.ReadByte(CombineRegisters(4, 5)));
  return 8;
}

int CPU::SBC_A_n(const byte & op_code) {
  SBC(ReadByte());
  return 8;
}

int CPU::AND_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[7] = ANDBytes(registers[7], registers[r_id]);
  return 4;
}

int CPU::AND_A_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
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
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  registers[7] = ORBytes(registers[7], val);
  return 8;
}

int CPU::OR_A_n(const byte & op_code) {
  byte val = ReadByte();
  registers[7] = ORBytes(registers[7], val);
  return 8;
}

int CPU::XOR_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[7] = XORBytes(registers[7], registers[r_id]);
  return 4;
}

int CPU::XOR_A_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  registers[7] = XORBytes(registers[7], val);
  return 8;
}

int CPU::XOR_A_n(const byte & op_code) {
  byte val = ReadByte();
  registers[7] = XORBytes(registers[7], val);
  return 8;
}

int CPU::CP_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  CPBytes(registers[7], registers[r_id]);
  return 4;
}

int CPU::CP_A_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
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
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), INCByte(val));
  return 12;
}

int CPU::DEC_r(const byte & op_code) {
  byte r_id = (op_code >> 3) & 0x7;
  registers[r_id] = DECByte(registers[r_id]);
  return 4;
}

int CPU::DEC_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), DECByte(val));
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
  SP = result;
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
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  byte lower = val & 0xF;
  byte upper = val & 0xF0;
  MMU.WriteByte(CombineRegisters(4, 5), (lower << 4) | (upper >> 4));
  if (val == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_CARRY);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  return 16;
}

int CPU::DAA(const byte & op_code) {
  int A = registers[7];
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
  registers[7] = (byte)A;
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
  // TODO: Implement halt bug
  //if (!interruptsEnabled && (MMU.ReadByte(IE) & MMU.ReadByte(IF) & 0x1F)) {
  //  halted = false;
  //}
  return 4;
}

int CPU::STOP(const byte & op_code) {
  if (MMU.ReadByte(0xFF4D) & 1) {
    gb->SetDoubleSpeed(!(gb->IsDoubleSpeed()));
    MMU.WriteByte(0xFF4D, gb->IsDoubleSpeed() ? 0x80 : 0);
  }
  else {
    stopped = true;
  }
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
  registers[7] = RotateLeft(registers[7], false, false);
  return 4;
}

int CPU::RLA(const byte & op_code) {
  registers[7] = RotateLeft(registers[7], true, false);
  return 4;
}

int CPU::RRCA(const byte & op_code) {
  registers[7] = RotateRight(registers[7], false, false);
  return 4;
}

int CPU::RRA(const byte & op_code) {
  registers[7] = RotateRight(registers[7], true, false);
  return 4;
}

int CPU::RLC_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[r_id] = RotateLeft(registers[r_id], false, true);
  return 8;
}

int CPU::RLC_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), RotateLeft(val, false, true));
  return 16;
}

int CPU::RL_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[r_id] = RotateLeft(registers[r_id], true, true);
  return 8;
}

int CPU::RL_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), RotateLeft(val, true, true));
  return 16;
}

int CPU::RRC_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[r_id] = RotateRight(registers[r_id], false, true);
  return 8;
}

int CPU::RRC_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), RotateRight(val, false, true));
  return 16;
}
int CPU::RR_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  registers[r_id] = RotateRight(registers[r_id], true, true);
  return 8;
}

int CPU::RR_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), RotateRight(val, true, true));
  return 16;
}

int CPU::SLA_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  byte result = (registers[r_id] << 1);
  if (registers[r_id] & 0x80) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  registers[r_id] = result;
  return 8;
}

int CPU::SLA_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  byte result = (val << 1);
  if (val & 0x80) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  MMU.WriteByte(CombineRegisters(4, 5), result);
  return 16;
}

int CPU::SRA_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
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
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  byte result = (val >> 1) | (val & 0x80);
  if (val & 1) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  MMU.WriteByte(CombineRegisters(4, 5), result);
  return 16;
}

int CPU::SRL_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  byte result = (registers[r_id] >> 1) & 0x7F;
  if (registers[r_id] & 1) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  registers[r_id] = result;
  return 8;
}

int CPU::SRL_HLm(const byte & op_code) {
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  byte result = (val >> 1) & 0x7F;
  if (val & 1) SetFlag(FLAG_CARRY);
  else ResetFlag(FLAG_CARRY);
  if (result == 0) SetFlag(FLAG_ZERO);
  else ResetFlag(FLAG_ZERO);
  ResetFlag(FLAG_HALF_CARRY);
  ResetFlag(FLAG_SUBTRACT);
  MMU.WriteByte(CombineRegisters(4, 5), result);
  return 16;
}

int CPU::BIT_b_r(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  byte r_id = op_code & 0x7;
  if (registers[r_id] & (1 << b)) ResetFlag(FLAG_ZERO);
  else SetFlag(FLAG_ZERO);
  ResetFlag(FLAG_SUBTRACT);
  SetFlag(FLAG_HALF_CARRY);
  return 8;
}

int CPU::BIT_b_HLm(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  if (val & (1 << b)) ResetFlag(FLAG_ZERO);
  else SetFlag(FLAG_ZERO);
  ResetFlag(FLAG_SUBTRACT);
  SetFlag(FLAG_HALF_CARRY);
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
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), val | (1 << b));
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
  byte val = MMU.ReadByte(CombineRegisters(4, 5));
  MMU.WriteByte(CombineRegisters(4, 5), val & ~(1 << b));
  return 16;
}

int CPU::JP_nn(const byte & op_code) {
  word addr = ReadWord();
  PC = addr;
  return 12;
}

int CPU::JP_NZ_nn(const byte & op_code) {
  word addr = ReadWord();
  if (!GetFlag(FLAG_ZERO)) {
    PC = addr;
    return 16;
  }
  return 12;
}

int CPU::JP_Z_nn(const byte & op_code) {
  word addr = ReadWord();
  if (GetFlag(FLAG_ZERO)) {
    PC = addr;
    return 16;
  }
  return 12;
}

int CPU::JP_NC_nn(const byte & op_code) {
  word addr = ReadWord();
  if (!GetFlag(FLAG_CARRY)) {
    PC = addr;
    return 16;
  }
  return 12;
}

int CPU::JP_C_nn(const byte & op_code) {
  word addr = ReadWord();
  if (GetFlag(FLAG_CARRY)) {
    PC = addr;
    return 16;
  }
  return 12;
}

int CPU::JP_HL(const byte & op_code) {
  PC = CombineRegisters(4, 5);
  return 4;
}

int CPU::JR_n(const byte & op_code) {
  sbyte n = ReadByte();
  PC += n;
  return 8;
}

int CPU::JR_NZ_n(const byte & op_code) {
  sbyte n = ReadByte();
  if (!GetFlag(FLAG_ZERO)) {
    PC += n;
    return 12;
  }
  return 8;
}

int CPU::JR_Z_n(const byte & op_code) {
  sbyte n = ReadByte();
  if (GetFlag(FLAG_ZERO)) {
    PC += n;
    return 12;
  }
  return 8;
}

int CPU::JR_NC_n(const byte & op_code) {
  sbyte n = ReadByte();
  if (!GetFlag(FLAG_CARRY)) {
    PC += n;
    return 12;
  }
  return 8;
}

int CPU::JR_C_n(const byte & op_code) {
  sbyte n = ReadByte();
  if (GetFlag(FLAG_CARRY)) {
    PC += n;
    return 12;
  }
  return 8;
}

int CPU::CALL_nn(const byte & op_code) {
  SP -= 2;
  word newAddr = ReadWord();
  MMU.WriteWord(SP, PC);
  PC = newAddr;
  return 24;
}

int CPU::CALL_NZ_nn(const byte & op_code) {
  if (!GetFlag(FLAG_ZERO)) {
    CALL_nn(0);
    return 24;
  }
  PC += 2;
  return 12;
}

int CPU::CALL_Z_nn(const byte & op_code) {
  if (GetFlag(FLAG_ZERO)) {
    CALL_nn(0);
    return 24;
  }
  PC += 2;
  return 12;
}

int CPU::CALL_NC_nn(const byte & op_code) {
  if (!GetFlag(FLAG_CARRY)) {
    CALL_nn(0);
    return 24;
  }
  PC += 2;
  return 12;
}

int CPU::CALL_C_nn(const byte & op_code) {
  if (GetFlag(FLAG_CARRY)) {
    CALL_nn(0);
    return 24;
  }
  PC += 2;
  return 12;
}

int CPU::RST_n(const byte & op_code) {
  byte n = 8 * ((op_code >> 3) & 0x7);
  SP -= 2;
  MMU.WriteWord(SP, PC);
  PC = n;
  return 16;
}

int CPU::RET(const byte & op_code) {
  word addr = MMU.ReadWord(SP);
  SP += 2;
  PC = addr;
  return 8;
}

int CPU::RET_NZ(const byte & op_code) {
  if (!GetFlag(FLAG_ZERO)) {
    RET(0);
    return 20;
  }
  return 8;
}

int CPU::RET_Z(const byte & op_code) {
  if (GetFlag(FLAG_ZERO)) {
    RET(0);
    return 20;
  }
  return 8;
}

int CPU::RET_NC(const byte & op_code) {
  if (!GetFlag(FLAG_CARRY)) {
    RET(0);
    return 20;
  }
  return 8;
}

int CPU::RET_C(const byte & op_code) {
  if (GetFlag(FLAG_CARRY)) {
    RET(0);
    return 20;
  }
  return 8;
}

int CPU::RETI(const byte & op_code) {
  RET(0);
  EI(0);
  return 16;
}