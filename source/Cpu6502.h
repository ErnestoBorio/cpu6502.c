
#ifndef _Cpu6502_h_
	#define _Cpu6502_h_

#define _Cpu6502_Disassembler // compiles the disassembler

#include <assert.h>
#include "Opcodes.h"

typedef unsigned char byte;
typedef unsigned short int word;

#define sign_bit 1<<7 // Most significant bit of a byte is the sign bit, 1 means negative

inline byte lowbyte( word value ) {
    return value & 0xFF;
}

inline byte highbyte( word value ) {
    return ( value & 0xFF00 ) >>8;
}

// -------------------------------------------------------------------------------
typedef struct // Cpu6502
{
	word pc; // Program counter
	byte sp; // Stack pointer
	byte a; // Accumulator register
	byte x; // Index register x
	byte y; // Index register y
	
	struct Cpu6502_status { // *warning* !=0 means true, not guaranteed to be exactly = 1
		byte carry;
		byte zero;
		byte interrupt_disable;
		byte decimal_mode;
		byte break_command;
		byte overflow;
		byte negative;
	} status;

	void *sys; // Pointer to the parent system, E.G.: C64, NES, VCS, BBC

	// Pointers to external memory access functions provided by the parent system
	byte (*read_memory)( void *parent_system, word address );
	void (*write_memory)( void *parent_system, word address, byte value );

	#ifdef _Cpu6502_Disassembler
		unsigned long int instruction_count;
	#endif

} Cpu6502;

// -------------------------------------------------------------------------------
// Factory constructor
Cpu6502 *Cpu6502_Create(
	void *parent_system,
	byte (*read_memory)( void *parent_system, word address ),
	void (*write_memory)( void *parent_system, word address, byte value ) );

// Destructor
void Cpu6502_Free( Cpu6502 *cpu );

// Executes the next instruction pointed to by the program counter
void Cpu6502_CpuStep( Cpu6502 *cpu );

#ifdef _Cpu6502_Disassembler
	void Cpu6502_Disassemble( Cpu6502 *cpu );
	char* Cpu6502_Get_addressing( Cpu6502 *cpu, char *string );
#endif

#endif // #ifndef _Cpu6502_h_















