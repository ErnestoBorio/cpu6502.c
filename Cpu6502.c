
#include <stdlib.h>
#include "Cpu6502.h"

// -------------------------------------------------------------------------------
void Cpu6502_Free( Cpu6502 *cpu )
{
	free( cpu );
}

// -------------------------------------------------------------------------------
Cpu6502* Cpu6502_Create(
	void *parent_system,
	byte *stack_pointer,
	byte (*read_memory)( void *sys, word address ),
	void (*write_memory)( void *sys, word address, byte value )
#ifdef _Cpu6502_Disassembler								
	, byte (*read_memory_disasm)( void *parent_system, word address )
#endif
	)
{
	Cpu6502 *cpu = (Cpu6502*) malloc( sizeof( Cpu6502 ) );
	if( cpu == NULL ) {
		return NULL;
	}

	cpu->sys = parent_system;
	cpu->read_memory = read_memory;
	cpu->write_memory = write_memory;
	cpu->stack = stack_pointer;

#ifdef _Cpu6502_Disassembler
	cpu->read_memory_disasm = read_memory_disasm;
#endif

	return cpu;
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
	cpu->pc = cpu->read_memory( cpu->sys, 0xFFFC ); // Jump to reset vector
	cpu->pc |= cpu->read_memory( cpu->sys, 0xFFFD ) <<8;
}

// -------------------------------------------------------------------------------
void Cpu6502_Reset( Cpu6502 *cpu )
{
	cpu->pc = cpu->read_memory( cpu->sys, 0xFFFC ); // Jump to reset vector
	cpu->pc |= cpu->read_memory( cpu->sys, 0xFFFD ) <<8;
}

// -------------------------------------------------------------------------------