
#include <assert.h>
#include <stdio.h>
#include "Cpu6502.h"

/* All instructions and addressing modes are static to this translation unit to keep
	them hidden from the public interface of Cpu6502, behaving as private methods.
	That's why their implementations are included here, so to make them all a single
	translation unit. */
#include "Addressing.c"
#include "Instructions.c"

// This addressing modes don't have a function, so this just updates the PC
#define _Implied()	cpu->pc += 1
#define _Immediate()	cpu->pc += 2
#define _ZeroPage()	cpu->pc += 2
#define _ZeroPageX()	cpu->pc += 2
#define _ZeroPageY()	cpu->pc += 2

#define ZeroPage() cpu->read_memory( cpu->sys, operand )
#define ZeroPageX() cpu->read_memory( cpu->sys, (byte)( operand + cpu->x ) )
#define ZeroPageY() cpu->read_memory( cpu->sys, (byte)( operand + cpu->y ) )
#define ZeroPageX_adr() (byte)( operand + cpu->x )
#define ZeroPageY_adr() (byte)( operand + cpu->y )
#define STr( address, register ) cpu->write_memory( cpu->sys, address, register );

void Cpu6502_CpuStep( Cpu6502 *cpu )
{
	#ifdef DEBUG
		int old_pc = cpu->pc; // make sure pc gets updated
	#endif
	
	cpu->cycles = 0;
	cpu->cycle_correction = 0;
	
	byte opcode = cpu->read_memory( cpu->sys, cpu->pc );

	// The 6502 reads the next byte in advance to gain time, this could have side effects, so it's not trivial
	byte operand = cpu->read_memory( cpu->sys, cpu->pc + 1 );

	switch( opcode )
	{
		case LDA_Immediate_A9: LDr( cpu, &cpu->a, operand ); _Immediate(); break;
		case LDX_Immediate_A2: LDr( cpu, &cpu->x, operand ); _Immediate(); break;
		case LDY_Immediate_A0: LDr( cpu, &cpu->y, operand ); _Immediate(); break;
		case LDA_Zero_page_A5: LDr( cpu, &cpu->a, ZeroPage() ); _ZeroPage(); break;
		case LDX_Zero_page_A6: LDr( cpu, &cpu->x, ZeroPage() ); _ZeroPage(); break;
		case LDY_Zero_page_A4: LDr( cpu, &cpu->y, ZeroPage() ); _ZeroPage(); break;
		case LDA_Zero_page_X_B5: LDr( cpu, &cpu->a, ZeroPageX() ); _ZeroPageX(); break;
		case LDY_Zero_page_X_B4: LDr( cpu, &cpu->y, ZeroPageX() ); _ZeroPageX(); break;
		case LDX_Zero_page_Y_B6: LDr( cpu, &cpu->x, ZeroPageY() ); _ZeroPageY(); break;
		case LDA_Absolute_AD: LDr( cpu, &cpu->a, Absolute( cpu, operand ) ); break;
		case LDX_Absolute_AE: LDr( cpu, &cpu->x, Absolute( cpu, operand ) ); break;
		case LDY_Absolute_AC: LDr( cpu, &cpu->y, Absolute( cpu, operand ) ); break;
		case LDA_Absolute_X_BD: LDr( cpu, &cpu->a, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case LDY_Absolute_X_BC: LDr( cpu, &cpu->y, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case LDA_Absolute_Y_B9: LDr( cpu, &cpu->a, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case LDX_Absolute_Y_BE: LDr( cpu, &cpu->x, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case LDA_Indexed_Indirect_X_A1: LDr( cpu, &cpu->a, Indexed_Indirect_X( cpu, operand ) ); break;
		case LDA_Indirect_Indexed_Y_B1: LDr( cpu, &cpu->a, Indirect_Indexed_Y( cpu, operand ) ); break;

		case STA_Zero_page_85: STr( operand, cpu->a ); _ZeroPage(); break;
		case STX_Zero_page_86: STr( operand, cpu->x ); _ZeroPage(); break;
		case STY_Zero_page_84: STr( operand, cpu->y ); _ZeroPage(); break;
		case STA_Zero_page_X_95: STr( ZeroPageX_adr(), cpu->a ); _ZeroPageX(); break;
		case STY_Zero_page_X_94: STr( ZeroPageX_adr(), cpu->y ); _ZeroPageX(); break;
		case STX_Zero_page_Y_96: STr( ZeroPageY_adr(), cpu->x ); _ZeroPageX(); break;
		case STA_Absolute_8D: STr( Absolute_adr( cpu, operand ), cpu->a ); break;
		case STX_Absolute_8E: STr( Absolute_adr( cpu, operand ), cpu->x ); break;
		case STY_Absolute_8C: STr( Absolute_adr( cpu, operand ), cpu->y ); break;
		case STA_Absolute_X_9D: STr( Absolute_Indexed_adr( cpu, operand, cpu->x ), cpu->a ); break;
		case STA_Absolute_Y_99: STr( Absolute_Indexed_adr( cpu, operand, cpu->y ), cpu->a ); break;
		case STA_Indexed_Indirect_X_81: STr( Indexed_Indirect_X_adr( cpu, operand ), cpu->a ); break;
		case STA_Indirect_Indexed_Y_91: STr( Indirect_Indexed_Y_adr( cpu, operand ), cpu->a ); break;
			
		case INX_E8: DeInXY( cpu, &cpu->x, +1 ); _Implied(); break;
		case DEX_CA: DeInXY( cpu, &cpu->x, -1 ); _Implied(); break;
		case INY_C8: DeInXY( cpu, &cpu->y, +1 ); _Implied(); break;
		case DEY_88: DeInXY( cpu, &cpu->y, -1 ); _Implied(); break;
		case INC_Zero_page_E6: IncDec( cpu, operand, +1 ); _ZeroPage(); break;
		case DEC_Zero_page_C6: IncDec( cpu, operand, -1 ); _ZeroPage(); break;
		case INC_Zero_page_X_F6: IncDec( cpu, ZeroPageX_adr(), +1 ); _ZeroPageX(); break;
		case DEC_Zero_page_X_D6: IncDec( cpu, ZeroPageX_adr(), -1 ); _ZeroPageX(); break;
		case INC_Absolute_EE: IncDec( cpu, Absolute_adr( cpu, operand ), +1 ); break;
		case DEC_Absolute_CE: IncDec( cpu, Absolute_adr( cpu, operand ), -1 ); break;

		case ADC_Immediate_69: ADC( cpu, operand ); _Immediate(); break;
		case SBC_Immediate_E9: SBC( cpu, operand ); _Immediate(); break;
		case ADC_Zero_page_65: ADC( cpu, ZeroPage() ); _ZeroPage(); break;
		case SBC_Zero_page_E5: SBC( cpu, ZeroPage() ); _ZeroPage(); break;
		case ADC_Zero_page_X_75: ADC( cpu, ZeroPageX() ); _ZeroPageX(); break;
		case SBC_Zero_page_X_F5: SBC( cpu, ZeroPageX() ); _ZeroPageX(); break;
		case ADC_Absolute_6D: ADC( cpu, Absolute( cpu, operand ) ); break;
		case SBC_Absolute_ED: SBC( cpu, Absolute( cpu, operand ) ); break;
		case ADC_Absolute_X_7D: ADC( cpu, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case SBC_Absolute_X_FD: SBC( cpu, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case ADC_Absolute_Y_79: ADC( cpu, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case SBC_Absolute_Y_F9: SBC( cpu, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case ADC_Indexed_Indirect_X_61: ADC( cpu, Indexed_Indirect_X( cpu, operand ) ); break;
		case SBC_Indexed_Indirect_X_E1: SBC( cpu, Indexed_Indirect_X( cpu, operand ) ); break;
		case ADC_Indirect_Indexed_Y_71: ADC( cpu, Indirect_Indexed_Y( cpu, operand ) ); break;
		case SBC_Indirect_Indexed_Y_F1: SBC( cpu, Indirect_Indexed_Y( cpu, operand ) ); break;
					
		case CMP_Immediate_C9: CPr( cpu, cpu->a, operand ); _Immediate(); break;
		case CPX_Immediate_E0: CPr( cpu, cpu->x, operand ); _Immediate(); break;
		case CPY_Immediate_C0: CPr( cpu, cpu->y, operand ); _Immediate(); break;
		case CMP_Zero_page_C5: CPr( cpu, cpu->a, ZeroPage() ); _ZeroPage(); break;
		case CPX_Zero_page_E4: CPr( cpu, cpu->x, ZeroPage() ); _ZeroPage(); break;
		case CPY_Zero_page_C4: CPr( cpu, cpu->y, ZeroPage() ); _ZeroPage(); break;
		case CMP_Zero_page_X_D5: CPr( cpu, cpu->a, ZeroPageX() ); _ZeroPageX(); break;
		case CMP_Absolute_CD: CPr( cpu, cpu->a, Absolute( cpu, operand ) ); break;
		case CPX_Absolute_EC: CPr( cpu, cpu->x, Absolute( cpu, operand ) ); break;
		case CPY_Absolute_CC: CPr( cpu, cpu->y, Absolute( cpu, operand ) ); break;
		case CMP_Absolute_X_DD: CPr( cpu, cpu->a, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case CMP_Absolute_Y_D9: CPr( cpu, cpu->a, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case CMP_Indexed_Indirect_X_C1: CPr( cpu, cpu->a, Indexed_Indirect_X( cpu, operand ) ); break;
			
		case ASL_Zero_page_06: ASL( cpu, operand ); _ZeroPage(); break;
		case LSR_Zero_page_46: LSR( cpu, operand ); _ZeroPage(); break;
		case ROL_Zero_page_26: ROL( cpu, operand ); _ZeroPage(); break;
		case ROR_Zero_page_66: ROR( cpu, operand ); _ZeroPage(); break;
		case ASL_Zero_page_X_16: ASL( cpu, ZeroPageX_adr() ); _ZeroPageX(); break;
		case LSR_Zero_page_X_56: LSR( cpu, ZeroPageX_adr() ); _ZeroPageX(); break;
		case ROL_Zero_page_X_36: ROL( cpu, ZeroPageX_adr() ); _ZeroPageX(); break;
		case ROR_Zero_page_X_76: ROR( cpu, ZeroPageX_adr() ); _ZeroPageX(); break;
		case ASL_Absolute_0E: ASL( cpu, Absolute_adr( cpu, operand ) ); break;
		case LSR_Absolute_4E: LSR( cpu, Absolute_adr( cpu, operand ) ); break;
		case ROL_Absolute_2E: ROL( cpu, Absolute_adr( cpu, operand ) ); break;
		case ROR_Absolute_6E: ROR( cpu, Absolute_adr( cpu, operand ) ); break;
		case ASL_Absolute_X_1E: ASL( cpu, Absolute_Indexed_adr( cpu, operand, cpu->x ) ); break;
		case LSR_Absolute_X_5E: LSR( cpu, Absolute_Indexed_adr( cpu, operand, cpu->x ) ); break;
		case ROL_Absolute_X_3E: ROL( cpu, Absolute_Indexed_adr( cpu, operand, cpu->x ) ); break;
		case ROR_Absolute_X_7E: ROR( cpu, Absolute_Indexed_adr( cpu, operand, cpu->x ) ); break;

		case TAX_AA: Trr( cpu, cpu->a, &cpu->x ); _Implied(); break;
		case TAY_A8: Trr( cpu, cpu->a, &cpu->y ); _Implied(); break;
		case TXA_8A: Trr( cpu, cpu->x, &cpu->a ); _Implied(); break;
		case TYA_98: Trr( cpu, cpu->y, &cpu->a ); _Implied(); break;
		case TSX_BA: Trr( cpu, cpu->sp, &cpu->x ); _Implied(); break;
		case TXS_9A: Trr( cpu, cpu->x, &cpu->sp ); _Implied(); break;

		case AND_Immediate_29: AND( cpu, operand ); _Immediate(); break;
		case EOR_Immediate_49: EOR( cpu, operand ); _Immediate(); break;
		case ORA_Immediate_09: ORA( cpu, operand ); _Immediate(); break;
		case AND_Zero_page_25: AND( cpu, ZeroPage() ); _ZeroPage(); break;
		case EOR_Zero_page_45: EOR( cpu, ZeroPage() ); _ZeroPage(); break;
		case ORA_Zero_page_05: ORA( cpu, ZeroPage() ); _ZeroPage(); break;
		case BIT_Zero_page_24: BIT( cpu, ZeroPage() ); _ZeroPage(); break;
		case AND_Zero_page_X_35: AND( cpu, ZeroPageX() ); _ZeroPageX(); break;
		case EOR_Zero_page_X_55: EOR( cpu, ZeroPageX() ); _ZeroPageX(); break;
		case ORA_Zero_page_X_15: ORA( cpu, ZeroPageX() ); _ZeroPageX(); break;
		case AND_Absolute_2D: AND( cpu, Absolute( cpu, operand ) ); break;
		case EOR_Absolute_4D: EOR( cpu, Absolute( cpu, operand ) ); break;
		case ORA_Absolute_0D: ORA( cpu, Absolute( cpu, operand ) ); break;
		case BIT_Absolute_2C: BIT( cpu, Absolute( cpu, operand ) ); break;
		case AND_Absolute_X_3D: AND( cpu, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case EOR_Absolute_X_5D: EOR( cpu, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case ORA_Absolute_X_1D: ORA( cpu, Absolute_Indexed( cpu, operand, cpu->x ) ); break;
		case AND_Absolute_Y_39: AND( cpu, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case EOR_Absolute_Y_59: EOR( cpu, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case ORA_Absolute_Y_19: ORA( cpu, Absolute_Indexed( cpu, operand, cpu->y ) ); break;
		case AND_Indexed_Indirect_X_21: AND( cpu, Indexed_Indirect_X( cpu, operand ) ); break;
		case EOR_Indexed_Indirect_X_41: EOR( cpu, Indexed_Indirect_X( cpu, operand ) ); break;
		case ORA_Indexed_Indirect_X_01: ORA( cpu, Indexed_Indirect_X( cpu, operand ) ); break;
		case AND_Indirect_Indexed_Y_31: AND( cpu, Indirect_Indexed_Y( cpu, operand ) ); break;
		case EOR_Indirect_Indexed_Y_51: EOR( cpu, Indirect_Indexed_Y( cpu, operand ) ); break;
		case ORA_Indirect_Indexed_Y_11: ORA( cpu, Indirect_Indexed_Y( cpu, operand ) ); break;
			
		case BEQ_Relative_F0: Branch( cpu, cpu->status.zero, 1, operand ); break;
		case BNE_Relative_D0: Branch( cpu, cpu->status.zero, 0, operand ); break;
		case BMI_Relative_30: Branch( cpu, cpu->status.negative, 1, operand ); break;
		case BPL_Relative_10: Branch( cpu, cpu->status.negative, 0, operand ); break;
		case BCS_Relative_B0: Branch( cpu, cpu->status.carry, 1, operand ); break;
		case BCC_Relative_90: Branch( cpu, cpu->status.carry, 0, operand ); break;
		case BVS_Relative_70: Branch( cpu, cpu->status.overflow, 1, operand ); break;
		case BVC_Relative_50: Branch( cpu, cpu->status.overflow, 0, operand ); break;

		case SEC_38: cpu->status.carry = 1; _Implied(); break;
		case CLC_18: cpu->status.carry = 0; _Implied(); break;
		case CLD_D8: cpu->status.decimal_mode = 0; _Implied(); break;
		case SED_F8: cpu->status.decimal_mode = 1; _Implied(); break;
		case SEI_78: cpu->status.interrupt_disable = 1; _Implied(); break;
		case CLI_58: cpu->status.interrupt_disable = 0; _Implied(); break;
		case CLV_B8: cpu->status.overflow = 0; _Implied(); break;

		case PHP_08: PHP( cpu ); _Implied(); break;
		case PHA_48: push( cpu, cpu->a ); _Implied(); break;
		case PLP_28: PLP( cpu ); _Implied(); break;
		case PLA_68: PLA( cpu ); _Implied(); break;

		case JMP_Absolute_4C: JMPabs( cpu, operand ); break;
		case JMP_Indirect_6C: JMPind( cpu, operand ); break;
		case JSR_20: JSR( cpu, operand ); break;
		case RTS_60: RTS( cpu ); break;
		case BRK_00: cpu->pc += 2; IRQ( cpu, 1 ); break; // The 6502 for some reason skips the byte following BRK
		case RTI_40: RTI( cpu ); break;
		case NOP_EA: _Implied(); break;

		default:
			printf( "Opcode $%02X not implemented\n", opcode );
			operand = operand; // dummy line for breakpoint
	}
	
	// Special cases need +1 or -1 correction if page crossing occurred
	cpu->cycles += cpu->cycle_correction;
	
	#ifdef DEBUG
		assert( old_pc != cpu->pc ); // make sure pc gets updated
		assert( cpu->cycles != 0 );
	#endif
}






