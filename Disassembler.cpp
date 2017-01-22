#include "Disassembler.h"
#include "Memory.h"
#include "GB.h"
#include "CPU.h"
#include "Format.h"

Disassembler::Disassembler(GB * gb, Memory & MMU, CPU &Cpu) : 
  gb(gb), 
  MMU(MMU),
  Cpu(Cpu) {
  for (int i = 0; i < 256; i++) {
    opCodeMap[i] = nullptr;
    opCodeMapCB[i] = nullptr;
  }

  // 00
  opCodeMap[0x00] = &Disassembler::NOP;
  opCodeMap[0x01] = &Disassembler::LD_BC_nn;
  opCodeMap[0x02] = &Disassembler::LD_BCm_A;
  opCodeMap[0x03] = &Disassembler::INC_BC;
  opCodeMap[0x04] = &Disassembler::INC_r;
  opCodeMap[0x05] = &Disassembler::DEC_r;
  opCodeMap[0x06] = &Disassembler::LD_r_n;
  opCodeMap[0x07] = &Disassembler::RLCA;
  opCodeMap[0x08] = &Disassembler::LD_nnm_SP;
  opCodeMap[0x09] = &Disassembler::ADD_HL_BC;
  opCodeMap[0x0A] = &Disassembler::LD_A_BCm;
  opCodeMap[0x0B] = &Disassembler::DEC_BC;
  opCodeMap[0x0C] = &Disassembler::INC_r;
  opCodeMap[0x0D] = &Disassembler::DEC_r;
  opCodeMap[0x0E] = &Disassembler::LD_r_n;
  opCodeMap[0x0F] = &Disassembler::RRCA;

  // 10
  opCodeMap[0x10] = &Disassembler::STOP;
  opCodeMap[0x11] = &Disassembler::LD_DE_nn;
  opCodeMap[0x12] = &Disassembler::LD_DEm_A;
  opCodeMap[0x13] = &Disassembler::INC_DE;
  opCodeMap[0x14] = &Disassembler::INC_r;
  opCodeMap[0x15] = &Disassembler::DEC_r;
  opCodeMap[0x16] = &Disassembler::LD_r_n;
  opCodeMap[0x17] = &Disassembler::RLA;
  opCodeMap[0x18] = &Disassembler::JR_n;
  opCodeMap[0x19] = &Disassembler::ADD_HL_DE;
  opCodeMap[0x1A] = &Disassembler::LD_A_DEm;
  opCodeMap[0x1B] = &Disassembler::DEC_DE;
  opCodeMap[0x1C] = &Disassembler::INC_r;
  opCodeMap[0x1D] = &Disassembler::DEC_r;
  opCodeMap[0x1E] = &Disassembler::LD_r_n;
  opCodeMap[0x1F] = &Disassembler::RRA;

  // 20
  opCodeMap[0x20] = &Disassembler::JR_NZ_n;
  opCodeMap[0x21] = &Disassembler::LD_HL_nn;
  opCodeMap[0x22] = &Disassembler::LDI_HLm_A;
  opCodeMap[0x23] = &Disassembler::INC_HL;
  opCodeMap[0x24] = &Disassembler::INC_r;
  opCodeMap[0x25] = &Disassembler::DEC_r;
  opCodeMap[0x26] = &Disassembler::LD_r_n;
  opCodeMap[0x27] = &Disassembler::DAA;
  opCodeMap[0x28] = &Disassembler::JR_Z_n;
  opCodeMap[0x29] = &Disassembler::ADD_HL_HL;
  opCodeMap[0x2A] = &Disassembler::LDI_A_HLm;
  opCodeMap[0x2B] = &Disassembler::DEC_HL;
  opCodeMap[0x2C] = &Disassembler::INC_r;
  opCodeMap[0x2D] = &Disassembler::DEC_r;
  opCodeMap[0x2E] = &Disassembler::LD_r_n;
  opCodeMap[0x2F] = &Disassembler::CPL;

  // 30
  opCodeMap[0x30] = &Disassembler::JR_NC_n;
  opCodeMap[0x31] = &Disassembler::LD_SP_nn;
  opCodeMap[0x32] = &Disassembler::LDD_HLm_A;
  opCodeMap[0x33] = &Disassembler::INC_SP;
  opCodeMap[0x34] = &Disassembler::INC_HLm;
  opCodeMap[0x35] = &Disassembler::DEC_HLm;
  opCodeMap[0x36] = &Disassembler::LD_HLm_n;
  opCodeMap[0x37] = &Disassembler::SCF;
  opCodeMap[0x38] = &Disassembler::JR_C_n;
  opCodeMap[0x39] = &Disassembler::ADD_HL_SP;
  opCodeMap[0x3A] = &Disassembler::LDD_A_HLm;
  opCodeMap[0x3B] = &Disassembler::DEC_SP;
  opCodeMap[0x3C] = &Disassembler::INC_r;
  opCodeMap[0x3D] = &Disassembler::DEC_r;
  opCodeMap[0x3E] = &Disassembler::LD_r_n;
  opCodeMap[0x3F] = &Disassembler::CCF;

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

  // 70
  opCodeMap[0x70] = &Disassembler::LD_HLm_r;
  opCodeMap[0x71] = &Disassembler::LD_HLm_r;
  opCodeMap[0x72] = &Disassembler::LD_HLm_r;
  opCodeMap[0x73] = &Disassembler::LD_HLm_r;
  opCodeMap[0x74] = &Disassembler::LD_HLm_r;
  opCodeMap[0x75] = &Disassembler::LD_HLm_r;
  opCodeMap[0x76] = &Disassembler::HALT;
  opCodeMap[0x77] = &Disassembler::LD_HLm_r;
  opCodeMap[0x78] = &Disassembler::LD_r1_r2;
  opCodeMap[0x79] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7A] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7B] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7C] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7D] = &Disassembler::LD_r1_r2;
  opCodeMap[0x7E] = &Disassembler::LD_r_HLm;
  opCodeMap[0x7F] = &Disassembler::LD_r1_r2;

  // 80
  opCodeMap[0x80] = &Disassembler::ADD_A_r;
  opCodeMap[0x81] = &Disassembler::ADD_A_r;
  opCodeMap[0x82] = &Disassembler::ADD_A_r;
  opCodeMap[0x83] = &Disassembler::ADD_A_r;
  opCodeMap[0x84] = &Disassembler::ADD_A_r;
  opCodeMap[0x85] = &Disassembler::ADD_A_r;
  opCodeMap[0x86] = &Disassembler::ADD_A_HLm;
  opCodeMap[0x87] = &Disassembler::ADD_A_r;
  opCodeMap[0x88] = &Disassembler::ADC_A_r;
  opCodeMap[0x89] = &Disassembler::ADC_A_r;
  opCodeMap[0x8A] = &Disassembler::ADC_A_r;
  opCodeMap[0x8B] = &Disassembler::ADC_A_r;
  opCodeMap[0x8C] = &Disassembler::ADC_A_r;
  opCodeMap[0x8D] = &Disassembler::ADC_A_r;
  opCodeMap[0x8E] = &Disassembler::ADC_A_HLm;
  opCodeMap[0x8F] = &Disassembler::ADC_A_r;

  // 90
  opCodeMap[0x90] = &Disassembler::SUB_A_r;
  opCodeMap[0x91] = &Disassembler::SUB_A_r;
  opCodeMap[0x92] = &Disassembler::SUB_A_r;
  opCodeMap[0x93] = &Disassembler::SUB_A_r;
  opCodeMap[0x94] = &Disassembler::SUB_A_r;
  opCodeMap[0x95] = &Disassembler::SUB_A_r;
  opCodeMap[0x96] = &Disassembler::SUB_A_HLm;
  opCodeMap[0x97] = &Disassembler::SUB_A_r;
  opCodeMap[0x98] = &Disassembler::SBC_A_r;
  opCodeMap[0x99] = &Disassembler::SBC_A_r;
  opCodeMap[0x9A] = &Disassembler::SBC_A_r;
  opCodeMap[0x9B] = &Disassembler::SBC_A_r;
  opCodeMap[0x9C] = &Disassembler::SBC_A_r;
  opCodeMap[0x9D] = &Disassembler::SBC_A_r;
  opCodeMap[0x9E] = &Disassembler::SBC_A_HLm;
  opCodeMap[0x9F] = &Disassembler::SBC_A_r;

  // A0
  opCodeMap[0xA0] = &Disassembler::AND_A_r;
  opCodeMap[0xA1] = &Disassembler::AND_A_r;
  opCodeMap[0xA2] = &Disassembler::AND_A_r;
  opCodeMap[0xA3] = &Disassembler::AND_A_r;
  opCodeMap[0xA4] = &Disassembler::AND_A_r;
  opCodeMap[0xA5] = &Disassembler::AND_A_r;
  opCodeMap[0xA6] = &Disassembler::AND_A_HLm;
  opCodeMap[0xA7] = &Disassembler::AND_A_r;
  opCodeMap[0xA8] = &Disassembler::XOR_A_r;
  opCodeMap[0xA9] = &Disassembler::XOR_A_r;
  opCodeMap[0xAA] = &Disassembler::XOR_A_r;
  opCodeMap[0xAB] = &Disassembler::XOR_A_r;
  opCodeMap[0xAC] = &Disassembler::XOR_A_r;
  opCodeMap[0xAD] = &Disassembler::XOR_A_r;
  opCodeMap[0xAE] = &Disassembler::XOR_A_HLm;
  opCodeMap[0xAF] = &Disassembler::XOR_A_r;

  // B0
  opCodeMap[0xB0] = &Disassembler::OR_A_r;
  opCodeMap[0xB1] = &Disassembler::OR_A_r;
  opCodeMap[0xB2] = &Disassembler::OR_A_r;
  opCodeMap[0xB3] = &Disassembler::OR_A_r;
  opCodeMap[0xB4] = &Disassembler::OR_A_r;
  opCodeMap[0xB5] = &Disassembler::OR_A_r;
  opCodeMap[0xB6] = &Disassembler::OR_A_HLm;
  opCodeMap[0xB7] = &Disassembler::OR_A_r;
  opCodeMap[0xB8] = &Disassembler::CP_A_r;
  opCodeMap[0xB9] = &Disassembler::CP_A_r;
  opCodeMap[0xBA] = &Disassembler::CP_A_r;
  opCodeMap[0xBB] = &Disassembler::CP_A_r;
  opCodeMap[0xBC] = &Disassembler::CP_A_r;
  opCodeMap[0xBD] = &Disassembler::CP_A_r;
  opCodeMap[0xBE] = &Disassembler::CP_A_HLm;
  opCodeMap[0xBF] = &Disassembler::CP_A_r;

  // C0
  opCodeMap[0xC0] = &Disassembler::RET_NZ;
  opCodeMap[0xC1] = &Disassembler::POP_BC;
  opCodeMap[0xC2] = &Disassembler::JP_NZ_nn;
  opCodeMap[0xC3] = &Disassembler::JP_nn;
  opCodeMap[0xC4] = &Disassembler::CALL_NZ_nn;
  opCodeMap[0xC5] = &Disassembler::PUSH_BC;
  opCodeMap[0xC6] = &Disassembler::ADD_A_n;
  opCodeMap[0xC7] = &Disassembler::RST_n;
  opCodeMap[0xC8] = &Disassembler::RET_Z;
  opCodeMap[0xC9] = &Disassembler::RET;
  opCodeMap[0xCA] = &Disassembler::JP_Z_nn;
  opCodeMap[0xCB] = nullptr;
  opCodeMap[0xCC] = &Disassembler::CALL_Z_nn;
  opCodeMap[0xCD] = &Disassembler::CALL_nn;
  opCodeMap[0xCE] = &Disassembler::ADC_A_n;
  opCodeMap[0xCF] = &Disassembler::RST_n;

  // D0
  opCodeMap[0xD0] = &Disassembler::RET_NC;
  opCodeMap[0xD1] = &Disassembler::POP_DE;
  opCodeMap[0xD2] = &Disassembler::JP_NC_nn;
  opCodeMap[0xD3] = nullptr;
  opCodeMap[0xD4] = &Disassembler::CALL_NC_nn;
  opCodeMap[0xD5] = &Disassembler::PUSH_DE;
  opCodeMap[0xD6] = &Disassembler::SUB_A_n;
  opCodeMap[0xD7] = &Disassembler::RST_n;
  opCodeMap[0xD8] = &Disassembler::RET_C;
  opCodeMap[0xD9] = &Disassembler::RETI;
  opCodeMap[0xDA] = &Disassembler::JP_C_nn;
  opCodeMap[0xDB] = nullptr;
  opCodeMap[0xDC] = &Disassembler::CALL_C_nn;
  opCodeMap[0xDD] = nullptr;
  opCodeMap[0xDE] = &Disassembler::SBC_A_n;
  opCodeMap[0xDF] = &Disassembler::RST_n;

  // E0
  opCodeMap[0xE0] = &Disassembler::LDH_nm_A;
  opCodeMap[0xE1] = &Disassembler::POP_HL;
  opCodeMap[0xE2] = &Disassembler::LD_Cm_A;
  opCodeMap[0xE3] = nullptr;
  opCodeMap[0xE4] = nullptr;
  opCodeMap[0xE5] = &Disassembler::PUSH_HL;
  opCodeMap[0xE6] = &Disassembler::AND_A_n;
  opCodeMap[0xE7] = &Disassembler::RST_n;
  opCodeMap[0xE8] = &Disassembler::ADD_SP_n;
  opCodeMap[0xE9] = &Disassembler::JP_HL;
  opCodeMap[0xEA] = &Disassembler::LD_nnm_A;
  opCodeMap[0xEB] = nullptr;
  opCodeMap[0xEC] = nullptr;
  opCodeMap[0xED] = nullptr;
  opCodeMap[0xEE] = &Disassembler::XOR_A_n;
  opCodeMap[0xEF] = &Disassembler::RST_n;

  // F0
  opCodeMap[0xF0] = &Disassembler::LDH_A_nm;
  opCodeMap[0xF1] = &Disassembler::POP_AF;
  opCodeMap[0xF2] = &Disassembler::LD_A_Cm;
  opCodeMap[0xF3] = &Disassembler::DI;
  opCodeMap[0xF4] = nullptr;
  opCodeMap[0xF5] = &Disassembler::PUSH_AF;
  opCodeMap[0xF6] = &Disassembler::OR_A_n;
  opCodeMap[0xF7] = &Disassembler::RST_n;
  opCodeMap[0xF8] = &Disassembler::LDHL_SP_n;
  opCodeMap[0xF9] = &Disassembler::LD_SP_HL;
  opCodeMap[0xFA] = &Disassembler::LD_A_nnm;
  opCodeMap[0xFB] = &Disassembler::EI;
  opCodeMap[0xFC] = nullptr;
  opCodeMap[0xFD] = nullptr;
  opCodeMap[0xFE] = &Disassembler::CP_A_n;
  opCodeMap[0xFF] = &Disassembler::RST_n;

  // CB Prefix

  // 00
  opCodeMapCB[0x00] = &Disassembler::RLC_r;
  opCodeMapCB[0x01] = &Disassembler::RLC_r;
  opCodeMapCB[0x02] = &Disassembler::RLC_r;
  opCodeMapCB[0x03] = &Disassembler::RLC_r;
  opCodeMapCB[0x04] = &Disassembler::RLC_r;
  opCodeMapCB[0x05] = &Disassembler::RLC_r;
  opCodeMapCB[0x06] = &Disassembler::RLC_HLm;
  opCodeMapCB[0x07] = &Disassembler::RLC_r;
  opCodeMapCB[0x08] = &Disassembler::RRC_r;
  opCodeMapCB[0x09] = &Disassembler::RRC_r;
  opCodeMapCB[0x0A] = &Disassembler::RRC_r;
  opCodeMapCB[0x0B] = &Disassembler::RRC_r;
  opCodeMapCB[0x0C] = &Disassembler::RRC_r;
  opCodeMapCB[0x0D] = &Disassembler::RRC_r;
  opCodeMapCB[0x0E] = &Disassembler::RRC_HLm;
  opCodeMapCB[0x0F] = &Disassembler::RRC_r;

  // 10
  opCodeMapCB[0x10] = &Disassembler::RL_r;
  opCodeMapCB[0x11] = &Disassembler::RL_r;
  opCodeMapCB[0x12] = &Disassembler::RL_r;
  opCodeMapCB[0x13] = &Disassembler::RL_r;
  opCodeMapCB[0x14] = &Disassembler::RL_r;
  opCodeMapCB[0x15] = &Disassembler::RL_r;
  opCodeMapCB[0x16] = &Disassembler::RL_HLm;
  opCodeMapCB[0x17] = &Disassembler::RL_r;
  opCodeMapCB[0x18] = &Disassembler::RR_r;
  opCodeMapCB[0x19] = &Disassembler::RR_r;
  opCodeMapCB[0x1A] = &Disassembler::RR_r;
  opCodeMapCB[0x1B] = &Disassembler::RR_r;
  opCodeMapCB[0x1C] = &Disassembler::RR_r;
  opCodeMapCB[0x1D] = &Disassembler::RR_r;
  opCodeMapCB[0x1E] = &Disassembler::RR_HLm;
  opCodeMapCB[0x1F] = &Disassembler::RR_r;

  // 20
  opCodeMapCB[0x20] = &Disassembler::SLA_r;
  opCodeMapCB[0x21] = &Disassembler::SLA_r;
  opCodeMapCB[0x22] = &Disassembler::SLA_r;
  opCodeMapCB[0x23] = &Disassembler::SLA_r;
  opCodeMapCB[0x24] = &Disassembler::SLA_r;
  opCodeMapCB[0x25] = &Disassembler::SLA_r;
  opCodeMapCB[0x26] = &Disassembler::SLA_HLm;
  opCodeMapCB[0x27] = &Disassembler::SLA_r;
  opCodeMapCB[0x28] = &Disassembler::SRA_r;
  opCodeMapCB[0x29] = &Disassembler::SRA_r;
  opCodeMapCB[0x2A] = &Disassembler::SRA_r;
  opCodeMapCB[0x2B] = &Disassembler::SRA_r;
  opCodeMapCB[0x2C] = &Disassembler::SRA_r;
  opCodeMapCB[0x2D] = &Disassembler::SRA_r;
  opCodeMapCB[0x2E] = &Disassembler::SRA_HLm;
  opCodeMapCB[0x2F] = &Disassembler::SRA_r;

  // 30
  opCodeMapCB[0x30] = &Disassembler::SWAP_r;
  opCodeMapCB[0x31] = &Disassembler::SWAP_r;
  opCodeMapCB[0x32] = &Disassembler::SWAP_r;
  opCodeMapCB[0x33] = &Disassembler::SWAP_r;
  opCodeMapCB[0x34] = &Disassembler::SWAP_r;
  opCodeMapCB[0x35] = &Disassembler::SWAP_r;
  opCodeMapCB[0x36] = &Disassembler::SWAP_HLm;
  opCodeMapCB[0x37] = &Disassembler::SWAP_r;
  opCodeMapCB[0x38] = &Disassembler::SRL_r;
  opCodeMapCB[0x39] = &Disassembler::SRL_r;
  opCodeMapCB[0x3A] = &Disassembler::SRL_r;
  opCodeMapCB[0x3B] = &Disassembler::SRL_r;
  opCodeMapCB[0x3C] = &Disassembler::SRL_r;
  opCodeMapCB[0x3D] = &Disassembler::SRL_r;
  opCodeMapCB[0x3E] = &Disassembler::SRL_HLm;
  opCodeMapCB[0x3F] = &Disassembler::SRL_r;

  // 40
  opCodeMapCB[0x40] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x41] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x42] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x43] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x44] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x45] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x46] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x47] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x48] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x49] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x4A] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x4B] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x4C] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x4D] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x4E] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x4F] = &Disassembler::BIT_b_r;

  // 50
  opCodeMapCB[0x50] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x51] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x52] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x53] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x54] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x55] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x56] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x57] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x58] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x59] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x5A] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x5B] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x5C] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x5D] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x5E] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x5F] = &Disassembler::BIT_b_r;

  // 60
  opCodeMapCB[0x60] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x61] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x62] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x63] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x64] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x65] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x66] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x67] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x68] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x69] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x6A] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x6B] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x6C] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x6D] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x6E] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x6F] = &Disassembler::BIT_b_r;

  // 70
  opCodeMapCB[0x70] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x71] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x72] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x73] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x74] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x75] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x76] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x77] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x78] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x79] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x7A] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x7B] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x7C] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x7D] = &Disassembler::BIT_b_r;
  opCodeMapCB[0x7E] = &Disassembler::BIT_b_HLm;
  opCodeMapCB[0x7F] = &Disassembler::BIT_b_r;

  // 80
  opCodeMapCB[0x80] = &Disassembler::RES_b_r;
  opCodeMapCB[0x81] = &Disassembler::RES_b_r;
  opCodeMapCB[0x82] = &Disassembler::RES_b_r;
  opCodeMapCB[0x83] = &Disassembler::RES_b_r;
  opCodeMapCB[0x84] = &Disassembler::RES_b_r;
  opCodeMapCB[0x85] = &Disassembler::RES_b_r;
  opCodeMapCB[0x86] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0x87] = &Disassembler::RES_b_r;
  opCodeMapCB[0x88] = &Disassembler::RES_b_r;
  opCodeMapCB[0x89] = &Disassembler::RES_b_r;
  opCodeMapCB[0x8A] = &Disassembler::RES_b_r;
  opCodeMapCB[0x8B] = &Disassembler::RES_b_r;
  opCodeMapCB[0x8C] = &Disassembler::RES_b_r;
  opCodeMapCB[0x8D] = &Disassembler::RES_b_r;
  opCodeMapCB[0x8E] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0x8F] = &Disassembler::RES_b_r;

  // 90
  opCodeMapCB[0x90] = &Disassembler::RES_b_r;
  opCodeMapCB[0x91] = &Disassembler::RES_b_r;
  opCodeMapCB[0x92] = &Disassembler::RES_b_r;
  opCodeMapCB[0x93] = &Disassembler::RES_b_r;
  opCodeMapCB[0x94] = &Disassembler::RES_b_r;
  opCodeMapCB[0x95] = &Disassembler::RES_b_r;
  opCodeMapCB[0x96] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0x97] = &Disassembler::RES_b_r;
  opCodeMapCB[0x98] = &Disassembler::RES_b_r;
  opCodeMapCB[0x99] = &Disassembler::RES_b_r;
  opCodeMapCB[0x9A] = &Disassembler::RES_b_r;
  opCodeMapCB[0x9B] = &Disassembler::RES_b_r;
  opCodeMapCB[0x9C] = &Disassembler::RES_b_r;
  opCodeMapCB[0x9D] = &Disassembler::RES_b_r;
  opCodeMapCB[0x9E] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0x9F] = &Disassembler::RES_b_r;

  // A0
  opCodeMapCB[0xA0] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA1] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA2] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA3] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA4] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA5] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA6] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0xA7] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA8] = &Disassembler::RES_b_r;
  opCodeMapCB[0xA9] = &Disassembler::RES_b_r;
  opCodeMapCB[0xAA] = &Disassembler::RES_b_r;
  opCodeMapCB[0xAB] = &Disassembler::RES_b_r;
  opCodeMapCB[0xAC] = &Disassembler::RES_b_r;
  opCodeMapCB[0xAD] = &Disassembler::RES_b_r;
  opCodeMapCB[0xAE] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0xAF] = &Disassembler::RES_b_r;

  // B0
  opCodeMapCB[0xB0] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB1] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB2] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB3] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB4] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB5] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB6] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0xB7] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB8] = &Disassembler::RES_b_r;
  opCodeMapCB[0xB9] = &Disassembler::RES_b_r;
  opCodeMapCB[0xBA] = &Disassembler::RES_b_r;
  opCodeMapCB[0xBB] = &Disassembler::RES_b_r;
  opCodeMapCB[0xBC] = &Disassembler::RES_b_r;
  opCodeMapCB[0xBD] = &Disassembler::RES_b_r;
  opCodeMapCB[0xBE] = &Disassembler::RES_b_HLm;
  opCodeMapCB[0xBF] = &Disassembler::RES_b_r;

  // C0
  opCodeMapCB[0xC0] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC1] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC2] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC3] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC4] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC5] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC6] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xC7] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC8] = &Disassembler::SET_b_r;
  opCodeMapCB[0xC9] = &Disassembler::SET_b_r;
  opCodeMapCB[0xCA] = &Disassembler::SET_b_r;
  opCodeMapCB[0xCB] = &Disassembler::SET_b_r;
  opCodeMapCB[0xCC] = &Disassembler::SET_b_r;
  opCodeMapCB[0xCD] = &Disassembler::SET_b_r;
  opCodeMapCB[0xCE] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xCF] = &Disassembler::SET_b_r;

  // D0
  opCodeMapCB[0xD0] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD1] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD2] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD3] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD4] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD5] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD6] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xD7] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD8] = &Disassembler::SET_b_r;
  opCodeMapCB[0xD9] = &Disassembler::SET_b_r;
  opCodeMapCB[0xDA] = &Disassembler::SET_b_r;
  opCodeMapCB[0xDB] = &Disassembler::SET_b_r;
  opCodeMapCB[0xDC] = &Disassembler::SET_b_r;
  opCodeMapCB[0xDD] = &Disassembler::SET_b_r;
  opCodeMapCB[0xDE] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xDF] = &Disassembler::SET_b_r;

  // E0
  opCodeMapCB[0xE0] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE1] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE2] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE3] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE4] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE5] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE6] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xE7] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE8] = &Disassembler::SET_b_r;
  opCodeMapCB[0xE9] = &Disassembler::SET_b_r;
  opCodeMapCB[0xEA] = &Disassembler::SET_b_r;
  opCodeMapCB[0xEB] = &Disassembler::SET_b_r;
  opCodeMapCB[0xEC] = &Disassembler::SET_b_r;
  opCodeMapCB[0xED] = &Disassembler::SET_b_r;
  opCodeMapCB[0xEE] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xEF] = &Disassembler::SET_b_r;

  // F0
  opCodeMapCB[0xF0] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF1] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF2] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF3] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF4] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF5] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF6] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xF7] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF8] = &Disassembler::SET_b_r;
  opCodeMapCB[0xF9] = &Disassembler::SET_b_r;
  opCodeMapCB[0xFA] = &Disassembler::SET_b_r;
  opCodeMapCB[0xFB] = &Disassembler::SET_b_r;
  opCodeMapCB[0xFC] = &Disassembler::SET_b_r;
  opCodeMapCB[0xFD] = &Disassembler::SET_b_r;
  opCodeMapCB[0xFE] = &Disassembler::SET_b_HLm;
  opCodeMapCB[0xFF] = &Disassembler::SET_b_r;
}

void Disassembler::Disassemble(word pc, int num_opcodes) {
  PC = pc;
  for (int i = 0; i < num_opcodes; i++) {
    std::string pcStr = Format::Word(PC);
    byte opCode = MMU.ReadByte(PC++);
    if (opCode == 0xCB) {
      opCode = MMU.ReadByte(PC++);
      if (opCodeMapCB[opCode] != nullptr) {
        gb->DebugLog(pcStr + " CB " + (this->*opCodeMapCB[opCode])(opCode));
      }
    }
    else if (opCodeMap[opCode] != nullptr) {
      gb->DebugLog(pcStr + "    " + (this->*opCodeMap[opCode])(opCode));
    }
  }
}

std::string Disassembler::ReadByte(bool is_signed) {
  int val = is_signed ? static_cast<sbyte>(MMU.ReadByte(PC)) : MMU.ReadByte(PC);
  PC++;
  return Format::Byte(val);
}

std::string Disassembler::ReadWord(bool is_signed) {
  int val = is_signed ? static_cast<sword>(MMU.ReadWord(PC)) : MMU.ReadWord(PC);
  PC += 2;
  return Format::Word(val);
}

std::string Disassembler::LD_r_n(const byte & op_code) {
  byte r_id = op_code >> 3;
  return "LD   " + Cpu.GetRegisterName(r_id) + "," + ReadByte();
}

std::string Disassembler::LD_r1_r2(const byte & op_code) {
  byte r1_id = (op_code & 0x38) >> 3;
  byte r2_id = op_code & 0x7;
  return "LD   " + Cpu.GetRegisterName(r1_id) + 
    "," + Cpu.GetRegisterName(r2_id);
}

std::string Disassembler::LD_r_HLm(const byte & op_code) {
  byte r_id = (op_code >> 3) & 0x7;
  return "LD   " + Cpu.GetRegisterName(r_id) + 
    ",(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::LD_HLm_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "LD   (" + Cpu.GetRegisterValue(4, 5) + ")" + 
    Cpu.GetRegisterName(r_id);
}

std::string Disassembler::LD_HLm_n(const byte & op_code) {
  return "LD   (" + Cpu.GetRegisterValue(4, 5) + ")" +
    ReadByte();
}

std::string Disassembler::LD_A_BCm(const byte & op_code) {
  return "LD   A,(" + Cpu.GetRegisterValue(0, 1) + ")";
}

std::string Disassembler::LD_A_DEm(const byte & op_code) {
  return "LD   A,(" + Cpu.GetRegisterValue(2, 3) + ")";
}

std::string Disassembler::LD_A_nnm(const byte & op_code) {
  return "LD   A,(" + ReadWord() + ")";
}

std::string Disassembler::LD_BCm_A(const byte & op_code) {
  return "LD   (" + Cpu.GetRegisterValue(0, 1) + "),A";
}

std::string Disassembler::LD_DEm_A(const byte & op_code) {
  return "LD   (" + Cpu.GetRegisterValue(2, 3) + "),A";
}

std::string Disassembler::LD_nnm_A(const byte & op_code) {
  return "LD   (" + ReadWord() + "),A";
}

std::string Disassembler::LD_A_Cm(const byte & op_code) {
  return "LD   A,(FF00+" + Cpu.GetRegisterValue(1) + ")";
}

std::string Disassembler::LD_Cm_A(const byte & op_code) {
  return "LD   (FF00+" + Cpu.GetRegisterValue(1) +"),A";
}

std::string Disassembler::LDD_A_HLm(const byte & op_code) {
  return "LDD  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::LDD_HLm_A(const byte & op_code) {
  return "LDD  (" + Cpu.GetRegisterValue(4, 5) + "),A";
}

std::string Disassembler::LDI_A_HLm(const byte & op_code) {
  return "LDI  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::LDI_HLm_A(const byte & op_code) {
  return "LDI  (" + Cpu.GetRegisterValue(4, 5) + "),A";
}

std::string Disassembler::LDH_nm_A(const byte & op_code) {
  return "LDH  (" + ReadByte() + "),A";
}

std::string Disassembler::LDH_A_nm(const byte & op_code) {
  return "LDH  A,(" + ReadByte() + ")";
}

std::string Disassembler::LD_BC_nn(const byte & op_code) {
  return "LD   BC," + ReadWord();
}

std::string Disassembler::LD_DE_nn(const byte & op_code) {
  return "LD   DE," + ReadWord();
}

std::string Disassembler::LD_HL_nn(const byte & op_code) {
  return "LD   HL,"+ ReadWord();
}

std::string Disassembler::LD_SP_nn(const byte & op_code) {
  return "LD   SP," + ReadWord();
}

std::string Disassembler::LD_SP_HL(const byte & op_code) {
  return "LD   SP,HL";
}

std::string Disassembler::LDHL_SP_n(const byte & op_code) {
  return "LDHL SP," + ReadByte(true);
}

std::string Disassembler::LD_nnm_SP(const byte & op_code) {
  return "LD   (" + ReadWord() + "),SP";
}

std::string Disassembler::PUSH_AF(const byte & op_code) {
  return "PUSH AF";
}

std::string Disassembler::PUSH_BC(const byte & op_code) {
  return "PUSH BC";
}

std::string Disassembler::PUSH_DE(const byte & op_code) {
  return "PUSH DE";
}

std::string Disassembler::PUSH_HL(const byte & op_code) {
  return "PUSH HL";
}

std::string Disassembler::POP_AF(const byte & op_code) {
  return "POP  AF";
}

std::string Disassembler::POP_BC(const byte & op_code) {
  return "POP  BC";
}

std::string Disassembler::POP_DE(const byte & op_code) {
  return "POP  DE";
}

std::string Disassembler::POP_HL(const byte & op_code) {
  return "POP  HL";
}

std::string Disassembler::ADD_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "ADD  A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::ADD_A_HLm(const byte & op_code) {
  return "ADD  A,(" + Cpu.GetRegisterValue(4,5) + ")";
}

std::string Disassembler::ADD_A_n(const byte & op_code) {
  return "ADD  A," + ReadByte();
}

std::string Disassembler::ADC_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "ADC  A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::ADC_A_HLm(const byte & op_code) {
  return "ADC  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::ADC_A_n(const byte & op_code) {
  return "ADC  A," + ReadByte();
}

std::string Disassembler::SUB_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "SUB  A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SUB_A_HLm(const byte & op_code) {
  return "SUB  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::SUB_A_n(const byte & op_code) {
  return "SUB  A," + ReadByte();
}

std::string Disassembler::SBC_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "SBC  A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SBC_A_HLm(const byte & op_code) {
  return "SBC  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::SBC_A_n(const byte & op_code) {
  return "SBC  A," + ReadByte();
}

std::string Disassembler::AND_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "AND  A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::AND_A_HLm(const byte & op_code) {
  return "AND  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::AND_A_n(const byte & op_code) {
  return "AND  A," + ReadByte();
}

std::string Disassembler::OR_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "OR   A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::OR_A_HLm(const byte & op_code) {
  return "OR   A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::OR_A_n(const byte & op_code) {
  return "OR   A," + ReadByte();
}

std::string Disassembler::XOR_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "XOR  A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::XOR_A_HLm(const byte & op_code) {
  return "XOR  A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::XOR_A_n(const byte & op_code) {
  return "XOR  A," + ReadByte();
}

std::string Disassembler::CP_A_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "CP   A," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::CP_A_HLm(const byte & op_code) {
  return "CP   A,(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::CP_A_n(const byte & op_code) {
  return "CP   A," + ReadByte();
}

std::string Disassembler::INC_r(const byte & op_code) {
  byte r_id = (op_code >> 3) & 0x7;
  return "INC  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::INC_HLm(const byte & op_code) {
  return "INC  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::DEC_r(const byte & op_code) {
  byte r_id = (op_code >> 3) & 0x7;
  return "DEC  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::DEC_HLm(const byte & op_code) {
  return "DEC  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::ADD_HL_BC(const byte & op_code) {
  return "ADD  HL,BC";
}

std::string Disassembler::ADD_HL_DE(const byte & op_code) {
  return "ADD  HL,DE";
}

std::string Disassembler::ADD_HL_HL(const byte & op_code) {
  return "ADD  HL,HL";
}

std::string Disassembler::ADD_HL_SP(const byte & op_code) {
  return "ADD  HL,SP";
}

std::string Disassembler::ADD_SP_n(const byte & op_code) {
  return "ADD  SP," + ReadByte(true);
}

std::string Disassembler::INC_BC(const byte & op_code) {
  return "INC  BC";
}

std::string Disassembler::INC_DE(const byte & op_code) {
  return "INC  DE";
}

std::string Disassembler::INC_HL(const byte & op_code) {
  return "INC  HL";
}

std::string Disassembler::INC_SP(const byte & op_code) {
  return "INC  SP";
}

std::string Disassembler::DEC_BC(const byte & op_code) {
  return "DEC  BC";
}

std::string Disassembler::DEC_DE(const byte & op_code) {
  return "DEC  DE";
}

std::string Disassembler::DEC_HL(const byte & op_code) {
  return "DEC  HL";
}

std::string Disassembler::DEC_SP(const byte & op_code) {
  return "DEC  SP";
}

std::string Disassembler::SWAP_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "SWAP " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SWAP_HLm(const byte & op_code) {
  return "SWAP (" + Cpu.GetRegisterValue(4,5) + ")";
}

std::string Disassembler::DAA(const byte & op_code) {
  return "DAA";
}

std::string Disassembler::CPL(const byte & op_code) {
  return "CPL  A";
}

std::string Disassembler::CCF(const byte & op_code) {
  return "CCF";
}

std::string Disassembler::SCF(const byte & op_code) {
  return "SCF";
}

std::string Disassembler::NOP(const byte & op_code) {
  return "NOP";
}

std::string Disassembler::HALT(const byte & op_code) {
  return "HALT";
}

std::string Disassembler::STOP(const byte & op_code) {
  return "DAA";
}

std::string Disassembler::DI(const byte & op_code) {
  return "DI";
}

std::string Disassembler::EI(const byte & op_code) {
  return "EI";
}

std::string Disassembler::RLCA(const byte & op_code) {
  return "RLCA";
}

std::string Disassembler::RLA(const byte & op_code) {
  return "RLA";
}

std::string Disassembler::RRCA(const byte & op_code) {
  return "RRCA";
}

std::string Disassembler::RRA(const byte & op_code) {
  return "RRA";
}

std::string Disassembler::RLC_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "RLC  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::RLC_HLm(const byte & op_code) {
  return "RLC  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::RL_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "RL   " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::RL_HLm(const byte & op_code) {
  return "RL   (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::RRC_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "RRC  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::RRC_HLm(const byte & op_code) {
  return "RRC  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::RR_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "RR   " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::RR_HLm(const byte & op_code) {
  return "RR   (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::SLA_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "SLA  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SLA_HLm(const byte & op_code) {
  return "SLA  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::SRA_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "SRA  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SRA_HLm(const byte & op_code) {
  return "SRA  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::SRL_r(const byte & op_code) {
  byte r_id = op_code & 0x7;
  return "SRL  " + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SRL_HLm(const byte & op_code) {
  return "SRL  (" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::BIT_b_r(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  byte r_id = op_code & 0x7;
  return "BIT  " + std::to_string((int)b) + "," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::BIT_b_HLm(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  return "BIT  " + std::to_string((int)b) + ",(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::SET_b_r(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  byte r_id = op_code & 0x7;
  return "SET  " + std::to_string((int)b) + "," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::SET_b_HLm(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  return "SET  " + std::to_string((int)b) + ",(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::RES_b_r(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  byte r_id = op_code & 0x7;
  return "RES  " + std::to_string((int)b) + "," + Cpu.GetRegisterName(r_id);
}

std::string Disassembler::RES_b_HLm(const byte & op_code) {
  byte b = (op_code >> 3) & 0x7;
  return "RES  " + std::to_string((int)b) + ",(" + Cpu.GetRegisterValue(4, 5) + ")";
}

std::string Disassembler::JP_nn(const byte & op_code) {
  return "JP   " + ReadWord();
}

std::string Disassembler::JP_NZ_nn(const byte & op_code) {
  return "JP   NZ," + ReadWord();
}

std::string Disassembler::JP_Z_nn(const byte & op_code) {
  return "JP   Z," + ReadWord();
}

std::string Disassembler::JP_NC_nn(const byte & op_code) {
  return "JP   NC," + ReadWord();
}

std::string Disassembler::JP_C_nn(const byte & op_code) {
  return "JP   C," + ReadWord();
}

std::string Disassembler::JP_HL(const byte & op_code) {
  return "JP   HL";
}

std::string Disassembler::JR_n(const byte & op_code) {
  return "JR   " + ReadByte(true);
}

std::string Disassembler::JR_NZ_n(const byte & op_code) {
  return "JR   NZ," + ReadByte(true);
}

std::string Disassembler::JR_Z_n(const byte & op_code) {
  return "JR   Z," + ReadByte(true);
}

std::string Disassembler::JR_NC_n(const byte & op_code) {
  return "JR   NC," + ReadByte(true);
}

std::string Disassembler::JR_C_n(const byte & op_code) {
  return "JR   C," + ReadByte(true);
}

std::string Disassembler::CALL_nn(const byte & op_code) {
  return "CALL " + ReadWord();
}

std::string Disassembler::CALL_NZ_nn(const byte & op_code) {
  return "CALL NZ," + ReadWord();
}

std::string Disassembler::CALL_Z_nn(const byte & op_code) {
  return "CALL Z," + ReadWord();
}

std::string Disassembler::CALL_NC_nn(const byte & op_code) {
  return "CALL NC," + ReadWord();
}

std::string Disassembler::CALL_C_nn(const byte & op_code) {
  return "CALL C," + ReadWord();
}

std::string Disassembler::RST_n(const byte & op_code) {
  return "RST  " + ReadByte();
}

std::string Disassembler::RET(const byte & op_code) {
  return "RET";
}

std::string Disassembler::RET_NZ(const byte & op_code) {
  return "RET  NZ";
}

std::string Disassembler::RET_Z(const byte & op_code) {
  return "RET  Z";
}

std::string Disassembler::RET_NC(const byte & op_code) {
  return "RET  NC";
}

std::string Disassembler::RET_C(const byte & op_code) {
  return "RET  C";
}

std::string Disassembler::RETI(const byte & op_code) {
  return "RETI";
}

