
#include <stdio.h>
#include <string.h>
#include "Cpu6502.h"

#ifdef _Cpu6502_Disassembler

static const char opcode_mnemonic[0x100][4] = {
//         0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
/* $00 */ "BRK\0","ORA\0","...\0","...\0","...\0","ORA\0","ASL\0","...\0","PHP\0","ORA\0","ASL\0","...\0","...\0","ORA\0","ASL\0","...\0",
/* $10 */ "BPL\0","ORA\0","...\0","...\0","...\0","ORA\0","ASL\0","...\0","CLC\0","ORA\0","...\0","...\0","...\0","ORA\0","ASL\0","...\0",
/* $20 */ "JSR\0","AND\0","...\0","...\0","BIT\0","AND\0","ROL\0","...\0","PLP\0","AND\0","ROL\0","...\0","BIT\0","AND\0","ROL\0","...\0",
/* $30 */ "BMI\0","AND\0","...\0","...\0","...\0","AND\0","ROL\0","...\0","SEC\0","AND\0","...\0","...\0","...\0","AND\0","ROL\0","...\0",
/* $40 */ "RTI\0","EOR\0","...\0","...\0","...\0","EOR\0","LSR\0","...\0","PHA\0","EOR\0","LSR\0","...\0","JMP\0","EOR\0","LSR\0","...\0",
/* $50 */ "BVC\0","EOR\0","...\0","...\0","...\0","EOR\0","LSR\0","...\0","CLI\0","EOR\0","...\0","...\0","...\0","EOR\0","LSR\0","...\0",
/* $60 */ "RTS\0","ADC\0","...\0","...\0","...\0","ADC\0","ROR\0","...\0","PLA\0","ADC\0","ROR\0","...\0","JMP\0","ADC\0","ROR\0","...\0",
/* $70 */ "BVS\0","ADC\0","...\0","...\0","...\0","ADC\0","ROR\0","...\0","SEI\0","ADC\0","...\0","...\0","...\0","ADC\0","ROR\0","...\0",
/* $80 */ "...\0","STA\0","...\0","...\0","STY\0","STA\0","STX\0","...\0","DEY\0","...\0","TXA\0","...\0","STY\0","STA\0","STX\0","...\0",
/* $90 */ "BCC\0","STA\0","...\0","...\0","STY\0","STA\0","STX\0","...\0","TYA\0","STA\0","TXS\0","...\0","...\0","STA\0","...\0","...\0",
/* $A0 */ "LDY\0","LDA\0","LDX\0","...\0","LDY\0","LDA\0","LDX\0","...\0","TAY\0","LDA\0","TAX\0","...\0","LDY\0","LDA\0","LDX\0","...\0",
/* $B0 */ "BCS\0","LDA\0","...\0","...\0","LDY\0","LDA\0","LDX\0","...\0","CLV\0","LDA\0","TSX\0","...\0","LDY\0","LDA\0","LDX\0","...\0",
/* $C0 */ "CPX\0","CMP\0","...\0","...\0","CPX\0","CMP\0","DEC\0","...\0","INY\0","CMP\0","DEX\0","...\0","CPX\0","CMP\0","DEC\0","...\0",
/* $D0 */ "BNE\0","CMP\0","...\0","...\0","...\0","CMP\0","DEC\0","...\0","CLD\0","CMP\0","...\0","...\0","...\0","CMP\0","DEC\0","...\0",
/* $E0 */ "CPX\0","SBC\0","...\0","...\0","CPX\0","SBC\0","INC\0","...\0","INX\0","SBC\0", "~\0" ,"...\0","CPX\0","SBC\0","INC\0","...\0",
/* $F0 */ "BEQ\0","SBC\0","...\0","...\0","...\0","SBC\0","INC\0","...\0","SED\0","SBC\0","...\0","...\0","...\0","SBC\0","INC\0","..."
};

static const int opcode_length[ 0x100 ] = {
//        0 1 2 3 4 5 6 7 8 9 A B C D E F
/* $00 */ 1,2,0,0,0,2,2,0,1,2,1,0,0,3,3,0,
/* $10 */ 2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,
/* $20 */ 3,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0,
/* $30 */ 2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,
/* $40 */ 1,2,0,0,0,2,2,0,1,2,1,0,3,3,3,0,
/* $50 */ 2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,
/* $60 */ 1,2,0,0,0,2,2,0,1,2,1,0,3,3,3,0,
/* $70 */ 2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,
/* $80 */ 0,2,0,0,2,2,2,0,1,0,1,0,3,3,3,0,
/* $90 */ 2,2,0,0,2,2,2,0,1,3,1,0,0,3,0,0,
/* $A0 */ 2,2,2,0,2,2,2,0,1,2,1,0,3,3,3,0,
/* $B0 */ 2,2,0,0,2,2,2,0,1,3,1,0,3,3,3,0,
/* $C0 */ 2,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0,
/* $D0 */ 2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0,
/* $E0 */ 2,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0,
/* $F0 */ 2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0
};

static inline char* Cpu6502_Get_addressing( Cpu6502 *cpu, char *string );

// ----------------------------------------------------------------------------------------------------------------
void Cpu6502_Disassemble( Cpu6502 *cpu, byte hide_instruction )
{
	cpu->instruction_count++;

	char string[256];
	byte opcode = cpu->read_memory( cpu->sys, cpu->pc );
	printf( "pc:%04X  %02X ", cpu->pc, opcode );

	if( opcode_length[opcode] > 1 ) {
		printf( "%02X ", cpu->read_memory( cpu->sys, cpu->pc+1 ) );
		if( opcode_length[opcode] > 2 ) {
			printf( "%02X ", cpu->read_memory( cpu->sys, cpu->pc+2 ) );
		} else {
			printf( ".. " );
		}
	} else {
		printf( ".. .. " );
	}

	printf( " a:%02X x:%02X y:%02X sp:%02X  %c%c%c%c%c%c [%02X %02X %02X] ",
			 cpu->a, cpu->x, cpu->y, cpu->sp,
			 ( cpu->status.zero ? 'z' : '.' ),
			 ( cpu->status.negative ? 'n' : '.' ),
			 ( cpu->status.carry ? 'c' : '.' ),
			 ( cpu->status.overflow ? 'v' : '.' ),
			 ( cpu->status.interrupt_disable ? 'i' : '.' ),
			 ( cpu->status.decimal_mode ? 'd' : '.' ),
			 cpu->stack[0xFF],
			 cpu->stack[0xFE],
			 cpu->stack[0xFD] );

	if( ! hide_instruction ) {
		printf( "%s %s\n", opcode_mnemonic[opcode], Cpu6502_Get_addressing( cpu, string ) );
	}
	else {
		printf( "...\n" );
	}

	if( opcode == RTI_40 ) {
		printf( "Return from interrupt\n" );
	}
}

// ----------------------------------------------------------------------------------------------------------------
static inline char* Cpu6502_Get_addressing( Cpu6502 *cpu, char *string )
{
	byte opcode = cpu->read_memory( cpu->sys, cpu->pc );
	byte operand = cpu->read_memory( cpu->sys, cpu->pc+1 );
	byte operand2 = cpu->read_memory( cpu->sys, cpu->pc+2 );
	word address = ((word)operand2) <<8 | operand;

	switch( opcode )
	{
		case AND_Immediate_29:		case LDA_Immediate_A9:		case LDX_Immediate_A2:		case LDY_Immediate_A0:		case EOR_Immediate_49:
		case ORA_Immediate_09:		case ADC_Immediate_69:		case SBC_Immediate_E9:		case CMP_Immediate_C9:		case CPX_Immediate_E0:
		case CPY_Immediate_C0:
			sprintf( string, "#$%02X", operand ); break;

		case LDA_Zero_page_A5:		case LDX_Zero_page_A6:		case LDY_Zero_page_A4:		case AND_Zero_page_25:		case EOR_Zero_page_45:
		case ORA_Zero_page_05:		case BIT_Zero_page_24:		case ADC_Zero_page_65:		case SBC_Zero_page_E5:		case CMP_Zero_page_C5:
		case CPX_Zero_page_E4:		case CPY_Zero_page_C4:		case INC_Zero_page_E6:		case DEC_Zero_page_C6:		case ASL_Zero_page_06:
		case LSR_Zero_page_46:		case ROL_Zero_page_26:		case ROR_Zero_page_66:		case STA_Zero_page_85:		case STX_Zero_page_86:
		case STY_Zero_page_84:
			sprintf( string, "$%02X", operand ); break;

		case LDA_Zero_page_X_B5:		case LDY_Zero_page_X_B4:		case AND_Zero_page_X_35:		case EOR_Zero_page_X_55:		case ORA_Zero_page_X_15:
		case ADC_Zero_page_X_75:		case SBC_Zero_page_X_F5:		case CMP_Zero_page_X_D5:		case INC_Zero_page_X_F6:		case DEC_Zero_page_X_D6:
		case ASL_Zero_page_X_16:		case LSR_Zero_page_X_56:		case ROL_Zero_page_X_36:		case ROR_Zero_page_X_76:		case STA_Zero_page_X_95:
		case STY_Zero_page_X_94:
			sprintf( string, "$%02X,X", operand ); break;

		case LDX_Zero_page_Y_B6:	case STX_Zero_page_Y_96:
			sprintf( string, "$%02X,Y", operand ); break;

		case BCC_Relative_90:		case BCS_Relative_B0:		case BEQ_Relative_F0:		case BMI_Relative_30:		case BNE_Relative_D0:
		case BPL_Relative_10:		case BVC_Relative_50:		case BVS_Relative_70:
			address = cpu->pc + 2 + ( (signed char) operand );
			sprintf( string, "$%04X", address );
			break;

		case LDA_Absolute_AD:		case LDX_Absolute_AE:		case LDY_Absolute_AC:		case AND_Absolute_2D:		case EOR_Absolute_4D:
		case ORA_Absolute_0D:		case BIT_Absolute_2C:		case ADC_Absolute_6D:		case SBC_Absolute_ED:		case CMP_Absolute_CD:
		case CPX_Absolute_EC:		case CPY_Absolute_CC:		case INC_Absolute_EE:		case DEC_Absolute_CE:		case ASL_Absolute_0E:
		case LSR_Absolute_4E:		case ROL_Absolute_2E:		case ROR_Absolute_6E:		case JSR_20:		case JMP_Absolute_4C:
		case STA_Absolute_8D:		case STX_Absolute_8E:		case STY_Absolute_8C:
			sprintf( string, "$%04X", address ); break;

		case LDA_Absolute_X_BD:		case LDY_Absolute_X_BC:		case AND_Absolute_X_3D:		case EOR_Absolute_X_5D:		case ORA_Absolute_X_1D:
		case ADC_Absolute_X_7D:		case SBC_Absolute_X_FD:		case CMP_Absolute_X_DD:		case INC_Absolute_X_FE:		case DEC_Absolute_X_DE:
		case ASL_Absolute_X_1E:		case LSR_Absolute_X_5E:		case ROL_Absolute_X_3E:		case ROR_Absolute_X_7E:		case STA_Absolute_X_9D:
			sprintf( string, "$%04X,X", address ); break;

		case LDA_Absolute_Y_B9:		case LDX_Absolute_Y_BE:		case AND_Absolute_Y_39:		case EOR_Absolute_Y_59:		case ORA_Absolute_Y_19:
		case ADC_Absolute_Y_79:		case SBC_Absolute_Y_F9:		case CMP_Absolute_Y_D9:		case STA_Absolute_Y_99:
			sprintf( string, "$%04X,Y", address ); break;

		case LDA_Indexed_Indirect_X_A1:		case AND_Indexed_Indirect_X_21:		case EOR_Indexed_Indirect_X_41:		case ORA_Indexed_Indirect_X_01:
		case ADC_Indexed_Indirect_X_61:		case SBC_Indexed_Indirect_X_E1:		case CMP_Indexed_Indirect_X_C1:		case STA_Indexed_Indirect_X_81:
			sprintf( string, "($%02X,X)", operand ); break;

		case LDA_Indirect_Indexed_Y_B1:		case AND_Indirect_Indexed_Y_31:		case EOR_Indirect_Indexed_Y_51:		case ORA_Indirect_Indexed_Y_11:
		case ADC_Indirect_Indexed_Y_71:		case SBC_Indirect_Indexed_Y_F1:		case CMP_Indirect_Indexed_Y_D1:		case STA_Indirect_Indexed_Y_91:
			sprintf( string, "($%02X),Y", operand ); break;

		case JMP_Indirect_6C:
			sprintf( string, "($%04X)", address ); break;

		case ASL_Accumulator_0A:		case LSR_Accumulator_4A:		case ROL_Accumulator_2A:		case ROR_Accumulator_6A:
			sprintf( string, "A" ); break;

		default: // Default cases includes both implied addressing and illegal opcodes. For now just return an empty string
			string[0] = '\0';
	}

	return string;
}

#endif // #ifdef _Cpu6502_Disassembler










