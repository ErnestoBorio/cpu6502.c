
#include <stdlib.h>
#include "Cpu6502.h"
#include "symbols.h"

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

const int opcode_addressing[ 0x100 ] = {
//  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
   IMP, IIX,  0 ,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, ACU,  0 , ABS, ABS, ABS,  0 , // 00
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPX,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABX,  0 , // 10
   ABS, IIX,  0 ,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, ACU,  0 , ABS, ABS, ABS,  0 , // 20
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPX,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABX,  0 , // 30
   IMP, IIX,  0 ,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, ACU,  0 , ABS, ABS, ABS,  0 , // 40
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPX,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABX,  0 , // 50
   IMP, IIX,  0 ,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, ACU,  0 , IND, ABS, ABS,  0 , // 60
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPX,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABX,  0 , // 70
   IMM, IIX, IMM,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, IMP,  0 , ABS, ABS, ABS,  0 , // 80
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPY,  0 , IMP, ABY, IMP,  0 ,  0 , ABX,  0 ,  0 , // 90
   IMM, IIX, IMM,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, IMP,  0 , ABS, ABS, ABS,  0 , // A0
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPY,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABY,  0 , // B0
   IMM, IIX, IMM,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, IMP,  0 , ABS, ABS, ABS,  0 , // C0
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPX,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABX,  0 , // D0
   IMM, IIX, IMM,  0 , ZRP, ZRP, ZRP,  0 , IMP, IMM, IMP,  0 , ABS, ABS, ABS,  0 , // E0
   REL, IIY,  0 ,  0 , ZPX, ZPX, ZPX,  0 , IMP, ABY, IMP,  0 , ABX, ABX, ABX,  0   // F0
};

// 0 = illegal, 1 = illegal NOP (semi-legal), 2 = legal
const int opcode_legality[ 0x100 ] = {
// 0 1 2 3 4 5 6 7 8 9 A B C D E F
   2,2,0,0,1,2,2,0,2,2,2,0,1,2,2,0, //00
   2,2,0,0,1,2,2,0,2,2,1,0,1,2,2,0, //10
   2,2,0,0,2,2,2,0,2,2,2,0,2,2,2,0, //20
   2,2,0,0,1,2,2,0,2,2,1,0,1,2,2,0, //30
   2,2,0,0,1,2,2,0,2,2,2,0,2,2,2,0, //40
   2,2,0,0,1,2,2,0,2,2,1,0,1,2,2,0, //50
   2,2,0,0,1,2,2,0,2,2,2,0,2,2,2,0, //60
   2,2,0,0,1,2,2,0,2,2,1,0,1,2,2,0, //70
   1,2,1,0,2,2,2,0,2,1,2,0,2,2,2,0, //80
   2,2,0,0,2,2,2,0,2,2,2,0,0,2,0,0, //90
   2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0, //A0
   2,2,0,0,2,2,2,0,2,2,2,0,2,2,2,0, //B0
   2,2,1,0,2,2,2,0,2,2,2,0,2,2,2,0, //C0
   2,2,0,0,1,2,2,0,2,2,1,0,1,2,2,0, //D0
   2,2,1,0,2,2,2,0,2,2,2,0,2,2,2,0, //E0
   2,2,0,0,1,2,2,0,2,2,1,0,1,2,2,0  //F0
};