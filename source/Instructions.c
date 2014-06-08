
#include "Cpu6502.h"
/* All instructions are static to mimic private methods that are not needed in the 
	public interface of Cpu6502 */

// -------------------------------------------------------------------------------
static void LDr( Cpu6502 *cpu, byte *register_, byte value ) // LDA, LDX, LDY
{
	*register_ = value;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) > 0;
}
// -------------------------------------------------------------------------------
static void CPr( Cpu6502 *cpu, byte register_, byte value ) // CMP, CPX, CPY
{
	cpu->status.zero  = ( register_ == value );
	cpu->status.carry = ( register_ >= value );
	cpu->status.negative = ( ( register_ - value ) & sign_bit ) > 0;
}
// -------------------------------------------------------------------------------
// BEQ, BNE, BPL, BMI, BVS, BVC, BCS, BCC
static void Branch( Cpu6502 *cpu, byte flag, byte condition, byte jump )
{
	if( flag == condition ) {
		cpu->pc = Relative( cpu, jump );
	}
	else {
		cpu->pc += 2;
	}
}
// -------------------------------------------------------------------------------
static void JMP( Cpu6502 *cpu, byte address_lowbyte )
{
	word address = address_lowbyte;
	// [pc+2] is the high byte of the address:
	address |= cpu->read_memory( cpu->sys, cpu->pc + 2 ) <<8;
	cpu->pc = address;
}
// -------------------------------------------------------------------------------
static void Trr( Cpu6502 *cpu, byte reg_from, byte *reg_to ) // TAX, TAY, TXA, TYA
{
	*reg_to = reg_from;
	cpu->status.zero = ( reg_from == 0 );
	cpu->status.negative = ( reg_from & sign_bit ) > 0;
}
// -------------------------------------------------------------------------------