
#include <assert.h>
#include <stdio.h>
#include "Cpu6502.h"

/* All instructions and addressing modes are static to this translation unit to keep
	them hidden from the public interface of Cpu6502, behaving as private methods.
	That's why their implementations are included here, so to make them all a single
	translation unit. */
#include "Addressing.c"
#include "Instructions.c"

void Cpu6502_CpuStep( Cpu6502 *cpu )
{
	#ifdef DEBUG
		int old_pc = cpu->pc; // make sure pc gets updated
	#endif
	
	byte opcode = cpu->read_memory( cpu->sys, cpu->pc );

	// The 6502 reads the next byte in advance to gain time, this could have side effects, so it's not trivial
	byte operand = cpu->read_memory( cpu->sys, cpu->pc + 1 );

	// This addressing modes don't have a function, so this just updates the PC
	#define Implied()		cpu->pc += 1
	#define Immediate()	cpu->pc += 2
	#define ZeroPage()	cpu->pc += 2
	#define ZeroPageX()	cpu->pc += 2
	#define ZeroPageY()	cpu->pc += 2

	switch( opcode )
	{
		case LDA_Immediate_A9: LDr( cpu, &cpu->a, operand ); Immediate(); break;
		case LDX_Immediate_A2: LDr( cpu, &cpu->x, operand ); Immediate(); break;
		case LDY_Immediate_A0: LDr( cpu, &cpu->y, operand ); Immediate(); break;
		case LDA_Zero_page_A5: LDr( cpu, &cpu->a, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case LDX_Zero_page_A6: LDr( cpu, &cpu->x, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case LDY_Zero_page_A4: LDr( cpu, &cpu->y, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case LDA_Zero_page_X_B5: LDr( cpu, &cpu->a, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;
		case LDY_Zero_page_X_B4: LDr( cpu, &cpu->y, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;

		case STA_Zero_page_85: cpu->write_memory( cpu->sys, operand, cpu->a ); ZeroPage(); break;
		case STX_Zero_page_86: cpu->write_memory( cpu->sys, operand, cpu->x ); ZeroPage(); break;
		case STY_Zero_page_84: cpu->write_memory( cpu->sys, operand, cpu->y ); ZeroPage(); break;
		case STA_Absolute_8D: cpu->write_memory( cpu->sys, Absolute( cpu, operand ), cpu->a ); break;
		case STX_Absolute_8E: cpu->write_memory( cpu->sys, Absolute( cpu, operand ), cpu->x ); break;
		case STY_Absolute_8C: cpu->write_memory( cpu->sys, Absolute( cpu, operand ), cpu->y ); break;
		case STA_Zero_page_X_95: cpu->write_memory( cpu->sys, (byte)( operand + cpu->x ), cpu->a ); ZeroPageX(); break;
		case STY_Zero_page_X_94: cpu->write_memory( cpu->sys, (byte)( operand + cpu->x ), cpu->y ); ZeroPageX(); break;
			
		case INX_E8: DeInXY( cpu, &cpu->x, +1 ); Implied(); break;
		case DEX_CA: DeInXY( cpu, &cpu->x, -1 ); Implied(); break;
		case INY_C8: DeInXY( cpu, &cpu->y, +1 ); Implied(); break;
		case DEY_88: DeInXY( cpu, &cpu->y, -1 ); Implied(); break;
		case INC_Zero_page_E6: IncDec( cpu, operand, +1 ); ZeroPage(); break;
		case DEC_Zero_page_C6: IncDec( cpu, operand, -1 ); ZeroPage(); break;
		case INC_Zero_page_X_F6: IncDec( cpu, (byte)( operand + cpu->x ), +1 ); ZeroPageX(); break;
		case DEC_Zero_page_X_D6: IncDec( cpu, (byte)( operand + cpu->x ), -1 ); ZeroPageX(); break;

		case ADC_Immediate_69: ADC( cpu, operand ); Immediate(); break;
		case SBC_Immediate_E9: SBC( cpu, operand ); Immediate(); break;
		case ADC_Zero_page_X_75: ADC( cpu, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;
		case SBC_Zero_page_X_F5: SBC( cpu, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;

		case ASL_Zero_page_06: ASL( cpu, operand ); ZeroPage(); break;
		case LSR_Zero_page_46: LSR( cpu, operand ); ZeroPage(); break;
		case ROL_Zero_page_26: ROL( cpu, operand ); ZeroPage(); break;
		case ROR_Zero_page_66: ROR( cpu, operand ); ZeroPage(); break;
		case ASL_Zero_page_X_16: ASL( cpu, (byte)( operand + cpu->x ) ); ZeroPageX(); break;
		case LSR_Zero_page_X_56: LSR( cpu, (byte)( operand + cpu->x ) ); ZeroPageX(); break;
		case ROL_Zero_page_X_36: ROL( cpu, (byte)( operand + cpu->x ) ); ZeroPageX(); break;
		case ROR_Zero_page_X_76: ROR( cpu, (byte)( operand + cpu->x ) ); ZeroPageX(); break;

		case CMP_Zero_page_C5: CPr( cpu, cpu->a, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case CPX_Zero_page_E4: CPr( cpu, cpu->x, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case CPY_Zero_page_C4: CPr( cpu, cpu->y, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case CMP_Immediate_C9: CPr( cpu, cpu->a, operand ); Immediate(); break;
		case CPX_Immediate_E0: CPr( cpu, cpu->x, operand ); Immediate(); break;
		case CPY_Immediate_C0: CPr( cpu, cpu->y, operand ); Immediate(); break;
		case CMP_Absolute_CD: CPr( cpu, cpu->a, cpu->read_memory( cpu->sys, Absolute( cpu, operand ) ) ); break;
		case CPX_Absolute_EC: CPr( cpu, cpu->x, cpu->read_memory( cpu->sys, Absolute( cpu, operand ) ) ); break;
		case CPY_Absolute_CC: CPr( cpu, cpu->y, cpu->read_memory( cpu->sys, Absolute( cpu, operand ) ) ); break;
		case CMP_Zero_page_X_D5: CPr( cpu, cpu->a, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;

		case TAX_AA: Trr( cpu, cpu->a, &cpu->x ); Implied(); break;
		case TAY_A8: Trr( cpu, cpu->a, &cpu->y ); Implied(); break;
		case TXA_8A: Trr( cpu, cpu->x, &cpu->a ); Implied(); break;
		case TYA_98: Trr( cpu, cpu->y, &cpu->a ); Implied(); break;
		case TSX_BA: Trr( cpu, cpu->sp, &cpu->x ); Implied(); break;
		case TXS_9A: Trr( cpu, cpu->x, &cpu->sp ); Implied(); break;

		case AND_Immediate_29: AND( cpu, operand ); Immediate(); break;
		case EOR_Immediate_49: EOR( cpu, operand ); Immediate(); break;
		case ORA_Immediate_09: ORA( cpu, operand ); Immediate(); break;
		case BIT_Zero_page_24: BIT( cpu, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case BIT_Absolute_2C: BIT( cpu, cpu->read_memory( cpu->sys, Absolute( cpu, operand ) ) ); break;
		case AND_Zero_page_X_35: AND( cpu, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;
		case EOR_Zero_page_X_55: EOR( cpu, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;
		case ORA_Zero_page_X_15: ORA( cpu, cpu->read_memory( cpu->sys, operand + cpu->x ) ); ZeroPageX(); break;
			
		case BEQ_Relative_F0: Branch( cpu, cpu->status.zero, 1, operand ); break;
		case BNE_Relative_D0: Branch( cpu, cpu->status.zero, 0, operand ); break;
		case BMI_Relative_30: Branch( cpu, cpu->status.negative, 1, operand ); break;
		case BPL_Relative_10: Branch( cpu, cpu->status.negative, 0, operand ); break;
		case BCS_Relative_B0: Branch( cpu, cpu->status.carry, 1, operand ); break;
		case BCC_Relative_90: Branch( cpu, cpu->status.carry, 0, operand ); break;
		case BVS_Relative_70: Branch( cpu, cpu->status.overflow, 1, operand ); break;
		case BVC_Relative_50: Branch( cpu, cpu->status.overflow, 0, operand ); break;

		case SEC_38: cpu->status.carry = 1; Implied(); break;
		case CLC_18: cpu->status.carry = 0; Implied(); break;
		case CLD_D8: cpu->status.decimal_mode = 0; Implied(); break;
		case SED_F8: cpu->status.decimal_mode = 1; Implied(); break;
		case SEI_78: cpu->status.interrupt_disable = 1; Implied(); break;
		case CLI_58: cpu->status.interrupt_disable = 0; Implied(); break;
		case CLV_B8: cpu->status.overflow = 0; Implied(); break;

		case PHP_08: PHP( cpu ); Implied(); break;
		case PHA_48: push( cpu, cpu->a ); Implied(); break;
		case PLP_28: PLP( cpu ); Implied(); break;
		case PLA_68: PLA( cpu ); Implied(); break;

		case JMP_Absolute_4C: JMPabs( cpu, operand ); break;
		//case JMP_Indirect_6C:
		case JSR_20: JSR( cpu, operand ); break;
		case RTS_60: RTS( cpu ); break;
		case BRK_00: cpu->pc += 2; IRQ( cpu, 1 ); break; // The 6502 for some reason skips the byte following BRK
		case RTI_40: RTI( cpu ); break;
		case NOP_EA: Implied(); break;

		default:
			printf( "Opcode $%02X not implemented\n", opcode );
			operand = operand;
	}

	#undef Implied
	#undef Immediate
	#undef ZeroPage
	
	#ifdef DEBUG
		assert( old_pc != cpu->pc ); // make sure pc gets updated
	#endif
}






