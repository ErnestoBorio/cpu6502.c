
#ifndef _Cpu6502_h_
	#define _Cpu6502_h_

#define _Cpu6502_Disassembler // compiles the disassembler

#ifndef false
	#define false 0
	#define true ! false
#endif

#include <assert.h>
#include "Opcodes.h"

typedef unsigned char byte;
typedef unsigned short int word;

// These will generate compile time errors if byte and word are not the appropriate size
typedef char _static_assert_type_byte_is_1_byte_wide_ [ ( sizeof(byte) == 1 )*2-1 ];
typedef char _static_assert_type_word_is_2_bytes_wide_[ ( sizeof(word) == 2 )*2-1 ];

#define bit0 1<<0
#define bit6 1<<6
#define bit7 1<<7
#define sign_bit bit7 // Most significant bit of a byte is the sign bit, 1 means negative

// -------------------------------------------------------------------------------
typedef struct // Cpu6502
{
	word pc; // Program counter
	byte sp; // Stack pointer
	byte a; // Accumulator register
	byte x; // Index register x
	byte y; // Index register y
	
	struct { // Flags, 0 = false, 1 = true
		byte carry;
		byte zero;
		byte interrupt_disable;
		byte decimal_mode;
		byte overflow;
		byte negative;
	} status;

	void *sys; // Pointer to the parent system, E.G.: C64, NES, VCS, BBC

	byte *stack; // Pointer to the stack, at memory page 1 [$100..$1FF]

	// Pointers to external memory access functions provided by the parent system
	byte (*read_memory)( void *parent_system, word address );
	void (*write_memory)( void *parent_system, word address, byte value );

	#ifdef _Cpu6502_Disassembler
		unsigned long int instruction_count;
	#endif

} Cpu6502;

// -------------------------------------------------------------------------------
// Factory constructor
Cpu6502* Cpu6502_Create(
	void *parent_system,
	byte *stack_pointer,
	byte (*read_memory)( void *parent_system, word address ),
	void (*write_memory)( void *parent_system, word address, byte value ) );

// Destructor
void Cpu6502_Free( Cpu6502 *cpu );

void Cpu6502_Initialize( Cpu6502 *cpu );
void Cpu6502_Reset( Cpu6502 *cpu );

// Executes the next instruction pointed to by the program counter
void Cpu6502_CpuStep( Cpu6502 *cpu );

void Cpu6502_IRQ( Cpu6502 *cpu );
void Cpu6502_NMI( Cpu6502 *cpu );

#ifdef _Cpu6502_Disassembler
	void Cpu6502_Disassemble( Cpu6502 *cpu, byte hide_instruction );
#endif

#endif // #ifndef _Cpu6502_h_















