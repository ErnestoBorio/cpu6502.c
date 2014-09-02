
#include <stdlib.h>
#include "Cpu6502.h"

// -------------------------------------------------------------------------------
void Cpu6502_Free( Cpu6502 *cpu )
{
	free( cpu );
}

// -------------------------------------------------------------------------------
Cpu6502* Cpu6502_Create( void *parent_system )
{
	Cpu6502 *this = (Cpu6502*) malloc( sizeof( Cpu6502 ) );
	if( this == NULL ) {
		return NULL;
	}
	this->sys = parent_system;
	return this;
}

// -------------------------------------------------------------------------------
void Cpu6502_Initialize( Cpu6502 *cpu )
{
	// as stated in http://wiki.nesdev.com/w/index.php/CPU_power_up_state
	cpu->sp = 0xFD;
	cpu->a = cpu->x = cpu->y = 0;	
	cpu->status.carry = 0;
	cpu->status.zero = 0;
	cpu->status.decimal_mode = 0;
	cpu->status.overflow = 0;
	cpu->status.negative =	0;
	cpu->status.interrupt_disable = 1;
}

// -------------------------------------------------------------------------------
void Cpu6502_Reset( Cpu6502 *cpu )
{
	cpu->pc = cpu->read_memory[0xFFFC]( cpu->sys, 0xFFFC ); // Jump to reset vector
	cpu->pc |= cpu->read_memory[0xFFFD]( cpu->sys, 0xFFFD ) <<8;
}

// -------------------------------------------------------------------------------