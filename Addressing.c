
#include "Cpu6502.h"
/* 
 All addressing modes are static to mimic private methods that are 
	not needed in the public interface of Cpu6502.
 
 This file is to be included in CpuStep.c
 
 Instructions as seen in http://www.obelisk.demon.co.uk/6502/
 
 *_adr functions return an address for writing operations like STA
 other funcionts return the value in the specified memory address for reading operations like LDA
 
 */

#define get_operand2_high() ( cpu->read_memory[cpu->pc + 2]( cpu->sys, cpu->pc + 2 ) <<8 )

// -------------------------------------------------------------------------------
static word Absolute_adr( Cpu6502 *cpu, byte address_lowbyte ) // STA $HHHH
{
	word address = address_lowbyte;
	address |= get_operand2_high();
	cpu->pc += 3;
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
      
	return address;
}

// -------------------------------------------------------------------------------
static byte Absolute( Cpu6502 *cpu, byte address_lowbyte ) // LDA $HHHH
{
	word address = address_lowbyte;
	address |= get_operand2_high();
	cpu->pc += 3;
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
      
	return cpu->read_memory[address]( cpu->sys, address );
}

// -------------------------------------------------------------------------------
static word Absolute_Indexed_adr( Cpu6502 *cpu, byte address_lowbyte, byte index ) // STA $HHHH,X
{
	word address = address_lowbyte + index;
	if( address > 0xFF ) {
		cpu->addressing_page_cross = 1;
	}
	address += get_operand2_high();
	cpu->pc += 3;
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
   
	return address;
}

// -------------------------------------------------------------------------------
static byte Absolute_Indexed( Cpu6502 *cpu, byte address_lowbyte, byte index ) // LDA $HHHH,Y
{
	word address = address_lowbyte + index;
	if( address > 0xFF ) {
		cpu->addressing_page_cross = 1;
	}
	address += get_operand2_high();
	cpu->pc += 3;
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
   
	return cpu->read_memory[address]( cpu->sys, address );
}

// -------------------------------------------------------------------------------
static word Indexed_Indirect_X_adr( Cpu6502 *cpu, byte base ) // STA ($HH,X)
{
	cpu->pc += 2;
	byte pointer = ((byte)(base + cpu->x ));
	word address = cpu->read_memory[pointer]( cpu->sys, pointer );
	pointer++;
	address |= cpu->read_memory[pointer]( cpu->sys, pointer ) <<8;
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.temp_address = (byte)(pointer-1);
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
   
	return address;
}

// -------------------------------------------------------------------------------
static byte Indexed_Indirect_X( Cpu6502 *cpu, byte base ) // LDA ($HH,X)
{
	cpu->pc += 2;
	byte pointer = ((byte)(base + cpu->x ));
	word address = cpu->read_memory[pointer]( cpu->sys, pointer );
	pointer++;
	address |= cpu->read_memory[pointer]( cpu->sys, pointer ) <<8;
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.temp_address = (byte)(pointer-1);
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
   
	return cpu->read_memory[address]( cpu->sys, address );
}

// -------------------------------------------------------------------------------
static word Indirect_Indexed_Y_adr( Cpu6502 *cpu, byte base ) // STA ($HH),Y
{
	cpu->pc += 2;
	word address = cpu->read_memory[base]( cpu->sys, base ) + cpu->y;
	if( address > 0xFF ) {
		cpu->addressing_page_cross = 1;
	}
	base++;
	address += ( cpu->read_memory[base]( cpu->sys, base ) <<8 );
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.temp_address = cpu->read_memory_disasm( cpu->sys, (byte)(base-1) );
      cpu->disasm.temp_address |= cpu->read_memory_disasm( cpu->sys, base ) <<8;
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
   
	return address;
}

// -------------------------------------------------------------------------------
static byte Indirect_Indexed_Y( Cpu6502 *cpu, byte base ) // LDA ($HH),Y
{
	cpu->pc += 2;
	word address = cpu->read_memory[base]( cpu->sys, base ) + cpu->y;
	if( address > 0xFF ) {
		cpu->addressing_page_cross = 1;
	}
	base++;
	address += ( cpu->read_memory[base]( cpu->sys, base ) <<8 );
   
   #ifdef _Cpu6502_Disassembler
      cpu->disasm.temp_address = cpu->read_memory_disasm( cpu->sys, (byte)(base-1) );
      cpu->disasm.temp_address |= cpu->read_memory_disasm( cpu->sys, base ) <<8;
      cpu->disasm.address = address;
      cpu->disasm.value = cpu->read_memory_disasm( cpu->sys, address );
   #endif
   
	return cpu->read_memory[address]( cpu->sys, address );
}

// -------------------------------------------------------------------------------