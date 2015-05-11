
#ifndef _opcodes_h_
   #define _opcodes_h_
   
// opcode information taken from:
// http://www.obelisk.demon.co.uk/6502/reference.html

// Cpu6502_opcodes
enum {
// Load/Store Operations

   // LDA - Load Accumulator
      LDA_Immediate_A9          = 0xA9,
      LDA_Zero_page_A5          = 0xA5,
      LDA_Zero_page_X_B5        = 0xB5,
      LDA_Absolute_AD           = 0xAD,
      LDA_Absolute_X_BD         = 0xBD,
      LDA_Absolute_Y_B9         = 0xB9,
      LDA_Indexed_Indirect_X_A1 = 0xA1,
      LDA_Indirect_Indexed_Y_B1 = 0xB1,
   
   // LDX - Load X Register
      LDX_Immediate_A2   = 0xA2,
      LDX_Zero_page_A6   = 0xA6,
      LDX_Zero_page_Y_B6 = 0xB6,
      LDX_Absolute_AE    = 0xAE,
      LDX_Absolute_Y_BE  = 0xBE,
      
   // LDY - Load Y Register
      LDY_Immediate_A0   = 0xA0,
      LDY_Zero_page_A4   = 0xA4,
      LDY_Zero_page_X_B4 = 0xB4,
      LDY_Absolute_AC    = 0xAC,
      LDY_Absolute_X_BC  = 0xBC,

   // STA - Store Accumulator
      STA_Zero_page_85   = 0x85,
      STA_Zero_page_X_95 = 0x95,
      STA_Absolute_8D    = 0x8D,
      STA_Absolute_X_9D  = 0x9D,
      STA_Absolute_Y_99  = 0x99,
      STA_Indexed_Indirect_X_81 = 0x81,
      STA_Indirect_Indexed_Y_91 = 0x91,

   // STX - Store X Register
      STX_Zero_page_86   = 0x86,
      STX_Zero_page_Y_96 = 0x96,
      STX_Absolute_8E    = 0x8E,
      
   // STY - Store Y Register
      STY_Zero_page_84   = 0x84,
      STY_Zero_page_X_94 = 0x94,
      STY_Absolute_8C    = 0x8C,

// Register Transfers

      TAX_AA = 0xAA, // TAX - Transfer Accumulator to X
      TAY_A8 = 0xA8, // TAY - Transfer Accumulator to Y
      TXA_8A = 0x8A, // TXA - Transfer X to Accumulator
      TYA_98 = 0x98, // TYA - Transfer Y to Accumulator

// Stack Operations

      TSX_BA = 0xBA, // TSX - Transfer Stack Pointer to X
      TXS_9A = 0x9A, // TXS - Transfer X to Stack Pointer
      PHA_48 = 0x48, // PHA - Push Accumulator
      PHP_08 = 0x08, // PHP - Push Processor Status
      PLA_68 = 0x68, // PLA - Pull Accumulator
      PLP_28 = 0x28, // PLP - Pull Processor Status
   
// Logical

   // AND - Logical AND
      AND_Immediate_29   = 0x29,
      AND_Zero_page_25   = 0x25,
      AND_Zero_page_X_35 = 0x35,
      AND_Absolute_2D    = 0x2D,
      AND_Absolute_X_3D  = 0x3D,
      AND_Absolute_Y_39  = 0x39,
      AND_Indexed_Indirect_X_21 = 0x21,
      AND_Indirect_Indexed_Y_31 = 0x31,
      
   // EOR - Exclusive OR
      EOR_Immediate_49   = 0x49,
      EOR_Zero_page_45   = 0x45,
      EOR_Zero_page_X_55 = 0x55,
      EOR_Absolute_4D    = 0x4D,
      EOR_Absolute_X_5D  = 0x5D,
      EOR_Absolute_Y_59  = 0x59,
      EOR_Indexed_Indirect_X_41 = 0x41,
      EOR_Indirect_Indexed_Y_51 = 0x51,
   
   // ORA - Logical Inclusive OR
      ORA_Immediate_09   = 0x09,
      ORA_Zero_page_05   = 0x05,
      ORA_Zero_page_X_15 = 0x15,
      ORA_Absolute_0D    = 0x0D,
      ORA_Absolute_X_1D  = 0x1D,
      ORA_Absolute_Y_19  = 0x19,
      ORA_Indexed_Indirect_X_01 = 0x01,
      ORA_Indirect_Indexed_Y_11 = 0x11,
      
   // BIT - Bit Test
      BIT_Zero_page_24 = 0x24,
      BIT_Absolute_2C  = 0x2C,

// Arithmetic

   // ADC - Add with Carry
      ADC_Immediate_69   = 0x69,
      ADC_Zero_page_65   = 0x65,
      ADC_Zero_page_X_75 = 0x75,
      ADC_Absolute_6D    = 0x6D,
      ADC_Absolute_X_7D  = 0x7D,
      ADC_Absolute_Y_79  = 0x79,
      ADC_Indexed_Indirect_X_61 = 0x61,
      ADC_Indirect_Indexed_Y_71 = 0x71,
      
   // SBC - Subtract with Carry
      SBC_Immediate_E9   = 0xE9,
      SBC_Zero_page_E5   = 0xE5,
      SBC_Zero_page_X_F5 = 0xF5,
      SBC_Absolute_ED    = 0xED,
      SBC_Absolute_X_FD  = 0xFD,
      SBC_Absolute_Y_F9  = 0xF9,
      SBC_Indexed_Indirect_X_E1 = 0xE1,
      SBC_Indirect_Indexed_Y_F1 = 0xF1,

   // CMP - Compare accumulator
      CMP_Immediate_C9   = 0xC9,
      CMP_Zero_page_C5   = 0xC5,
      CMP_Zero_page_X_D5 = 0xD5,
      CMP_Absolute_CD    = 0xCD,
      CMP_Absolute_X_DD  = 0xDD,
      CMP_Absolute_Y_D9  = 0xD9,
      CMP_Indexed_Indirect_X_C1 = 0xC1,
      CMP_Indirect_Indexed_Y_D1 = 0xD1,
      
   // CPX - Compare X register
      CPX_Immediate_E0 = 0xE0,
      CPX_Zero_page_E4 = 0xE4,
      CPX_Absolute_EC  = 0xEC,
   
   // CPY - Compare Y register
      CPY_Immediate_C0 = 0xC0,
      CPY_Zero_page_C4 = 0xC4,
      CPY_Absolute_CC  = 0xCC,

   // INC - Increment a memory location
      INC_Zero_page_E6   = 0xE6,
      INC_Zero_page_X_F6 = 0xF6,
      INC_Absolute_EE    = 0xEE,
      INC_Absolute_X_FE  = 0xFE,

      INX_E8 = 0xE8, // INX - Increment the X register
      INY_C8 = 0xC8, // INY - Increment the Y register
      
   // DEC - Decrement a memory location
      DEC_Zero_page_C6   = 0xC6,
      DEC_Zero_page_X_D6 = 0xD6,
      DEC_Absolute_CE    = 0xCE,
      DEC_Absolute_X_DE  = 0xDE,

      DEX_CA = 0xCA, // DEX - Decrement the X register
      DEY_88 = 0x88, // DEY - Decrement the Y register

// Shifts

   // ASL - Arithmetic Shift Left
      ASL_Accumulator_0A = 0x0A,
      ASL_Zero_page_06   = 0x06,
      ASL_Zero_page_X_16 = 0x16,
      ASL_Absolute_0E    = 0x0E,
      ASL_Absolute_X_1E  = 0x1E,

   // LSR - Logical Shift Right
      LSR_Accumulator_4A = 0x4A,
      LSR_Zero_page_46   = 0x46,
      LSR_Zero_page_X_56 = 0x56,
      LSR_Absolute_4E    = 0x4E,
      LSR_Absolute_X_5E  = 0x5E,
   
   // ROL - Rotate Left
      ROL_Accumulator_2A = 0x2A,
      ROL_Zero_page_26   = 0x26,
      ROL_Zero_page_X_36 = 0x36,
      ROL_Absolute_2E    = 0x2E,
      ROL_Absolute_X_3E  = 0x3E,

   // ROR - Rotate Right
      ROR_Accumulator_6A = 0x6A,
      ROR_Zero_page_66   = 0x66,
      ROR_Zero_page_X_76 = 0x76,
      ROR_Absolute_6E    = 0x6E,
      ROR_Absolute_X_7E  = 0x7E,
      
// Jumps & Calls

   // JMP - Jump
      JMP_Absolute_4C = 0x4C,
      JMP_Indirect_6C = 0x6C,
      
   // JSR - Jump to Subroutine
      JSR_20 = 0x20,
   
   // RTS - Return from Subroutine
      RTS_60 = 0x60,
      
// Branches
   
      BCC_Relative_90 = 0x90, // BCC - Branch if Carry Clear
      BCS_Relative_B0 = 0xB0, // BCS - Branch if Carry Set
      BEQ_Relative_F0 = 0xF0, // BEQ - Branch if Equal
      BMI_Relative_30 = 0x30, // BMI - Branch if Minus
      BNE_Relative_D0 = 0xD0, // BNE - Branch if Not Equal
      BPL_Relative_10 = 0x10, // BPL - Branch if Positive
      BVC_Relative_50 = 0x50, // BVC - Branch if Overflow Clear
      BVS_Relative_70 = 0x70, // BVS - Branch if Overflow Set

// Status Flag Changes

      CLC_18 = 0x18, // CLC - Clear Carry Flag
      CLD_D8 = 0xD8, // CLD - Clear Decimal Mode
      CLI_58 = 0x58, // CLI - Clear Interrupt Disable
      CLV_B8 = 0xB8, // CLV - Clear Overflow Flag
      SEC_38 = 0x38, // SEC - Set Carry Flag
      SED_F8 = 0xF8, // SED - Set Decimal Flag
      SEI_78 = 0x78, // SEI - Set Interrupt Disable
      
// System Functions

      BRK_00 = 0x00, // BRK - Force Interrupt
      NOP_EA = 0xEA, // NOP - No Operation
      RTI_40 = 0x40  // RTI - Return from Interrupt
};

#endif // #ifndef _opcodes_h_