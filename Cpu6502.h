
#ifndef _Cpu6502_h_
	#define _Cpu6502_h_

// This file should be present in the project that uses Cpu6502
//#include "Cpu6502_settings.h"

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
	
	// Temporal data used by CpuStep()
	byte cycles; // Cycle count of the last executed instruction [1..7]
	byte addressing_page_cross; // Flag when indexed modes cross a page boundary

	// Tables of pointers to external memory access functions provided by the parent system
	byte (*read_memory[0x10000])( void *parent_system, word address );
	void (*write_memory[0x10000])( void *parent_system, word address, byte value );

#ifdef _Cpu6502_Disassembler
		// this is used by the disassembler, should not produce side effects:
	byte (*read_memory_disasm)( void *parent_system, word address );
	unsigned long int instruction_count;
   struct {
      byte op_bytes[3];
      word pc; // Program counter
      byte sp; // Stack pointer
      byte a; // Accumulator register
      byte x; // Index register x
      byte y; // Index register y   
      // struct { // Flags, 0 = false, 1 = true
      //    byte carry;
      //    byte zero;
      //    byte interrupt_disable;
      //    byte decimal_mode;
      //    byte overflow;
      //    byte negative;
      // } status;
      byte status;
      // byte op_length; Not needed?
      word address; // address resolved by addressing mode
      word temp_address; // temporal address calculated by indirect addressing modes
      byte value; // value that was on that address prior to changing
   } disasm;
#endif

} Cpu6502;

// -------------------------------------------------------------------------------
// Factory constructor
Cpu6502* Cpu6502_Create( void *parent_system );

// Destructor
void Cpu6502_Free( Cpu6502 *cpu );

void Cpu6502_Initialize( Cpu6502 *cpu );
void Cpu6502_Reset( Cpu6502 *cpu );

// Executes the next instruction pointed to by the program counter
int Cpu6502_CpuStep( Cpu6502 *cpu );

int Cpu6502_IRQ( Cpu6502 *cpu );
int Cpu6502_NMI( Cpu6502 *cpu );

#ifdef _Cpu6502_Disassembler
	void Cpu6502_Disassemble( Cpu6502 *cpu, int cycles );
#endif

#endif // #ifndef _Cpu6502_h_