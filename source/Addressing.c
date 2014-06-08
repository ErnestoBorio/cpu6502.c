
#include "Cpu6502.h"
/* All addressing modes are static to mimic private methods that are 
	not needed in the public interface of Cpu6502 */

// -------------------------------------------------------------------------------
static word Relative( Cpu6502 *cpu, byte jump )
{
	word address = cpu->pc + 2;  // The branch is relative to the next instruction's address
	if( jump & sign_bit ) { // relative jump is negative
		address += ( (word)jump - 0x100 ); // subtract jump's 2's complement
	}
	else {
		address += jump;
	}
	return address;
}

// -------------------------------------------------------------------------------
static word Absolute( Cpu6502 *cpu, byte address_lowbyte )
{
	word address = address_lowbyte;
	address |= cpu->read_memory( cpu->sys, cpu->pc + 2 ) <<8;
	cpu->pc += 3;
	return address;
}

// -------------------------------------------------------------------------------
