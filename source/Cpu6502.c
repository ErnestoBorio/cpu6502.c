
#include <stdlib.h>
#include "Cpu6502.h"

// -------------------------------------------------------------------------------
void Cpu6502_Free( Cpu6502 *cpu )
{
	free( cpu );
}

// -------------------------------------------------------------------------------
Cpu6502 *Cpu6502_Create(
	void *parent_system,
	byte (*read_memory)( void *sys, word address ),
	void (*write_memory)( void *sys, word address, byte value ) )
{
	Cpu6502 *cpu = (Cpu6502*) malloc( sizeof( Cpu6502 ) );
	if( cpu == NULL ) {
		return NULL;
	}

	cpu->sys = parent_system;
	cpu->read_memory = read_memory;
	cpu->write_memory = write_memory;

	return cpu;
}
// -------------------------------------------------------------------------------
