
#include "Cpu6502.h"
/* 
 All addressing modes are static to mimic private methods that are 
	not needed in the public interface of Cpu6502.
 
 This file is to be included in CpuStep.c
 
 Instructions as seen in http://www.obelisk.demon.co.uk/6502/
 */

#define get_operand2_high() ( cpu->read_memory( cpu->sys, cpu->pc + 2 ) <<8 )

// -------------------------------------------------------------------------------
static word Relative( Cpu6502 *cpu, byte jump ) // BEQ $HH
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
static word Absolute_adr( Cpu6502 *cpu, byte address_lowbyte ) // STA $HHHH
{
	cpu->cycles = 4;
	word address = address_lowbyte;
	address |= get_operand2_high();
	cpu->pc += 3;
	return address;
}

// -------------------------------------------------------------------------------
static byte Absolute( Cpu6502 *cpu, byte address_lowbyte ) // LDA $HHHH
{
	cpu->cycles = 4;
	word address = address_lowbyte;
	address |= get_operand2_high();
	cpu->pc += 3;
	return cpu->read_memory( cpu->sys, address );
}

// -------------------------------------------------------------------------------
static word Absolute_Indexed_adr( Cpu6502 *cpu, byte address_lowbyte, byte index ) // STA $HHHH,X
{
	cpu->cycles = 5;
	word address = address_lowbyte + index;
	//WIP: if( address_lowbyte + index > 0xFF ) then cycles++
	address += get_operand2_high();
	cpu->pc += 3;
	return address;
}

// -------------------------------------------------------------------------------
static word Absolute_Indexed( Cpu6502 *cpu, byte address_lowbyte, byte index ) // LDA $HHHH,Y
{
	cpu->cycles = 5;
	word address = address_lowbyte + index;
	//WIP: if( address_lowbyte + index > 0xFF ) then cycles++
	address += get_operand2_high();
	cpu->pc += 3;
	return cpu->read_memory( cpu->sys, address );
}

// -------------------------------------------------------------------------------
static word Indexed_Indirect_X_adr( Cpu6502 *cpu, byte base ) // STA ($HH,X)
{
	cpu->cycles = 6;
	cpu->pc += 2;
	word address = cpu->read_memory( cpu->sys, ((byte)(base + cpu->x )) );
	address |= cpu->read_memory( cpu->sys, ((byte)(base + cpu->x + 1)) ) <<8;
	return address;
}

// -------------------------------------------------------------------------------
static byte Indexed_Indirect_X( Cpu6502 *cpu, byte base ) // LDA ($HH,X)
{
	cpu->cycles = 6;
	cpu->pc += 2;
	word address = cpu->read_memory( cpu->sys, ((byte)(base + cpu->x )) );
	address |= cpu->read_memory( cpu->sys, ((byte)(base + cpu->x + 1)) ) <<8;
	return cpu->read_memory( cpu->sys, address );
}

// -------------------------------------------------------------------------------
static byte Indirect_Indexed_Y_adr( Cpu6502 *cpu, byte base ) // STA ($HH),Y
{
	cpu->cycles = 6;
	cpu->pc += 2;
	byte ptr_low = cpu->read_memory( cpu->sys, base ) + cpu->y;
	word address = ptr_low | ( cpu->read_memory( cpu->sys, (byte)( base + 1 ) ) <<8 );
	return address;
}

// -------------------------------------------------------------------------------
static byte Indirect_Indexed_Y( Cpu6502 *cpu, byte base ) // LDA ($HH),Y
{
	cpu->cycles = 6;
	cpu->pc += 2;
	byte ptr_low = cpu->read_memory( cpu->sys, base ) + cpu->y;
	word address = ptr_low | ( cpu->read_memory( cpu->sys, (byte)( base + 1 ) ) <<8 );
	return cpu->read_memory( cpu->sys, address );
}

// -------------------------------------------------------------------------------






















