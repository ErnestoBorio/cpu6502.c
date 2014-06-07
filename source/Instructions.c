
#include "Cpu6502.h"
/* All instructions are static to mimic private methods that are not needed in the 
	public interface of Cpu6502 */

// -------------------------------------------------------------------------------
static void LDA( Cpu6502 *cpu, byte value )
{
	cpu->a = value;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) > 0;
}
// -------------------------------------------------------------------------------
static void STA( Cpu6502 *cpu, word address )
{
	cpu->write_memory( cpu->sys, address, cpu->a );
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) > 0;
}
// -------------------------------------------------------------------------------
static void CMP( Cpu6502 *cpu, byte value )
{
	cpu->status.zero  = ( cpu->a == value );
	cpu->status.carry = ( cpu->a >= value );
	cpu->status.negative = ( ( cpu->a - value ) & sign_bit ) > 0;
}
// -------------------------------------------------------------------------------
static void BEQ( Cpu6502 *cpu, byte jump )
{
	if( cpu->status.zero ) {
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
