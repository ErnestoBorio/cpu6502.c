
#include <assert.h>
#include "Cpu6502.h"

/* All instructions and addressing modes are static to this translation unit to keep
	them hidden from the public interface of Cpu6502, behaving as private methods.
	That's why their implementations are included here, so to make them all a single
	translation unit. */
#include "Addressing.c"
#include "Instructions.c"

void Cpu6502_CpuStep( Cpu6502 *cpu )
{
	byte opcode = cpu->read_memory( cpu->sys, cpu->pc );

	// The 6502 reads the next byte in advance to gain time, this could have side effects, so it's not trivial
	byte operand = cpu->read_memory( cpu->sys, cpu->pc + 1 );

	#ifdef _Disassembler
		Cpu6502_Disassemble( cpu );
	#endif

	// This addressing modes don't have a function, so this just updates the PC
	#define Implied()		cpu->pc += 1
	#define Immediate()	cpu->pc += 2
	#define ZeroPage()	cpu->pc += 2

	switch( opcode )
	{
		case LDA_Immediate_A9: LDA( cpu, operand ); Immediate(); break;
		case STA_Zero_page_85: STA( cpu, cpu->read_memory( cpu->sys, operand ) ); ZeroPage(); break;
		case CMP_Immediate_C9: CMP( cpu, operand ); Immediate(); break;
		case BEQ_Relative_F0: BEQ( cpu, operand ); break;
		case JMP_Absolute_4C: JMP( cpu, operand ); break;


		default:
			assert( 0 && "Opcode not implemented" );
	}

	#undef Implied
	#undef Immediate
	#undef ZeroPage
}