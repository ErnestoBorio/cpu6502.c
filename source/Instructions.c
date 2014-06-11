
#include "Cpu6502.h"
/* All instructions are static to mimic private methods that are not needed in the 
	public interface of Cpu6502 */

// -------------------------------------------------------------------------------
static void LDr( Cpu6502 *cpu, byte *registre, byte value ) // LDA, LDX, LDY
{
	*registre = value;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void DeInXY( Cpu6502 *cpu, byte *registre, signed int delta ) // INX, DEX, INY, DEY
{
	*registre += delta;
	cpu->status.zero = ( *registre == 0 );
	cpu->status.negative = ( *registre & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void CPr( Cpu6502 *cpu, byte registre, byte value ) // CMP, CPX, CPY
{
	cpu->status.zero  = ( registre == value );
	cpu->status.carry = ( registre >= value );
	cpu->status.negative = ( ( registre - value ) & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void AND( Cpu6502 *cpu, byte value )
{
	cpu->a &= value;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void EOR( Cpu6502 *cpu, byte value )
{
	cpu->a ^= value;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void ORA( Cpu6502 *cpu, byte value )
{
	cpu->a |= value;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void BIT( Cpu6502 *cpu, byte value )
{
	cpu->status.zero = ( ( value & cpu->a ) == 0 );
	cpu->status.overflow = ( value & bit6 ) != 0;
	cpu->status.negative = ( value & sign_bit ) != 0;
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
	cpu->status.negative = ( reg_from & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------