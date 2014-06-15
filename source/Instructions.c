
#include "Cpu6502.h"
/* All instructions are static to mimic private methods that are not needed in the 
	public interface of Cpu6502 */

static inline void push( Cpu6502 *cpu, byte value );
static inline byte pull( Cpu6502 *cpu );

// -------------------------------------------------------------------------------
static void LDr( Cpu6502 *cpu, byte *registre, byte value ) // LDA, LDX, LDY
{
	*registre = value;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void DeInXY( Cpu6502 *cpu, byte *registre, signed int delta ) // INX, DEX, INY, DEY
{
	*registre += delta;
	cpu->status.zero = ( *registre == 0 );
	cpu->status.negative = ( *registre & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void ADC( Cpu6502 *cpu, byte value )
{
	word sum = cpu->a + value + cpu->status.carry;
	cpu->status.carry = ( sum > 0xFF );
	cpu->status.overflow = ( ( ( cpu->a ^ sum ) & ( value ^ sum ) & sign_bit ) != 0 );
	cpu->a = sum & 0xFF;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;

}
// -------------------------------------------------------------------------------
static void SBC( Cpu6502 *cpu, byte value )
{
	word diff = cpu->a - value - ( ! cpu->status.carry );
	cpu->status.carry = ( diff < 0x100 );
	cpu->status.overflow = ( ( ( cpu->a ^ value ) & ( cpu->a ^ diff ) & sign_bit ) != 0 );
	cpu->a = diff & 0xFF;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void ASL( Cpu6502 *cpu, word address )
{
	byte value = cpu->read_memory( cpu->sys, address );
	cpu->status.carry = value & bit7;
	value = value <<1;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) != 0;
	cpu->write_memory( cpu->sys, address, value );
}
// -------------------------------------------------------------------------------
static void LSR( Cpu6502 *cpu, word address )
{
	byte value = cpu->read_memory( cpu->sys, address );
	cpu->status.carry = value & bit0;
	value = value >>1;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) != 0;
	cpu->write_memory( cpu->sys, address, value );
}
// -------------------------------------------------------------------------------
static void ROL( Cpu6502 *cpu, word address )
{
	byte value = cpu->read_memory( cpu->sys, address );
	byte old_carry = cpu->status.carry;
	cpu->status.carry = value & bit7;
	value = ( value <<1 ) | old_carry;
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) != 0;
	cpu->write_memory( cpu->sys, address, value );
}
// -------------------------------------------------------------------------------
static void ROR( Cpu6502 *cpu, word address )
{
	byte value = cpu->read_memory( cpu->sys, address );
	byte old_carry = cpu->status.carry;
	cpu->status.carry = value & bit7;
	value = ( value >>1 ) | ( old_carry <<7 );
	cpu->status.zero = ( value == 0 );
	cpu->status.negative = ( value & sign_bit ) != 0;
	cpu->write_memory( cpu->sys, address, value );
}
// -------------------------------------------------------------------------------
static void CPr( Cpu6502 *cpu, byte registre, byte value ) // CMP, CPX, CPY
{
	cpu->status.zero  = ( registre == value );
	cpu->status.carry = ( registre >= value );
	cpu->status.negative = ( ( registre - value ) & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void AND( Cpu6502 *cpu, byte value )
{
	cpu->a &= value;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void EOR( Cpu6502 *cpu, byte value )
{
	cpu->a ^= value;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void ORA( Cpu6502 *cpu, byte value )
{
	cpu->a |= value;
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static void BIT( Cpu6502 *cpu, byte value )
{
	cpu->status.zero = ( ( value & cpu->a ) == 0 );
	cpu->status.overflow = ( value & bit6 ) != 0;
	cpu->status.negative = ( value & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
// BEQ, BNE, BPL, BMI, BVS, BVC, BCS, BCC
static void Branch( Cpu6502 *cpu, byte flag, byte condition, byte jump )
{
	if( flag == condition )
		cpu->pc = Relative( cpu, jump );
	else
		cpu->pc += 2;
}
// -------------------------------------------------------------------------------
static void JMP( Cpu6502 *cpu, byte address_lowbyte )
{
	word address = address_lowbyte;
	address |= cpu->read_memory( cpu->sys, cpu->pc + 2 ) <<8; // high byte
	cpu->pc = address;
}
// -------------------------------------------------------------------------------
static void Trr( Cpu6502 *cpu, byte reg_from, byte *reg_to ) // TAX, TAY, TXA, TYA
{
	*reg_to = reg_from;
	cpu->status.zero = ( reg_from == 0 );
	cpu->status.negative = ( reg_from & sign_bit ) != 0;
}
// -------------------------------------------------------------------------------
static inline void push( Cpu6502 *cpu, byte value )
{
	cpu->write_memory( cpu->sys, 0x100 + cpu->sp, value ); // Stack sits on $100-$1FF
	cpu->sp--;
	assert( ( cpu->sp >= 0 ) && ( cpu->sp <= 0xFF ) );
	// WIP make sure that type byte is exactly 8 bits wide.
}
// ---------------------------------------
static inline byte pull( Cpu6502 *cpu )
{
	cpu->sp++;
	assert( ( cpu->sp >= 0 ) && ( cpu->sp <= 0xFF ) );
	return cpu->read_memory( cpu->sys, 0x100 + cpu->sp ); // Stack sits on $100-$1FF
}
// ---------------------------------------
static void PLA( Cpu6502 *cpu )
{
	cpu->a = pull( cpu );
	cpu->status.zero = ( cpu->a == 0 );
	cpu->status.negative = ( cpu->a & sign_bit ) != 0;
}
// ---------------------------------------
static inline byte pack_status( Cpu6502 *cpu, byte break_flag )
{
	return
		cpu->status.carry |
		cpu->status.zero <<1 |
		cpu->status.interrupt_disable <<2 |
		cpu->status.decimal_mode <<3 |
		break_flag <<4 | // virtual break flag
		1 <<5 | // Unused flag, is always 1
		cpu->status.overflow <<6 |
		cpu->status.negative <<7;
}
// ---------------------------------------
static void PHP( Cpu6502 *cpu )
{
	// WIP: According to Brad Taylor's http://nesdev.com/the%20'B'%20flag%20&%20BRK%20instruction.txt
	// PHP pushes the break flag as 1
	push( cpu, pack_status( cpu, 1 ) );
}
// ---------------------------------------
static inline void unpack_status( Cpu6502 *cpu, byte status )
{
	cpu->status.carry = ( status & ( 1 <<0 ) ) != 0;
	cpu->status.zero = ( status & ( 1 <<1 ) ) != 0;
	cpu->status.interrupt_disable = ( status & ( 1 <<2 ) ) != 0;
	cpu->status.decimal_mode = ( status & ( 1 <<3 ) ) != 0;
	// break flag, bit 4 is discarded as it's not an actual flag
	cpu->status.overflow = ( status & ( 1 <<6 ) ) != 0;
	cpu->status.negative = ( status & ( 1 <<7 ) ) != 0;
}
// ---------------------------------------
static void PLP( Cpu6502 *cpu )
{
	unpack_status( cpu, pull( cpu ) );
}
// -------------------------------------------------------------------------------
static void JSR( Cpu6502 *cpu, byte address_lowbyte )
{
	cpu->pc += 2; // PC is off by -1, pointing to JSR's last byte. Will be corrected on RTS
	push( cpu, cpu->pc >>8 ); // pc's highbyte
	push( cpu, cpu->pc & 0xFF ); // pc's lowbyte

	word address = address_lowbyte;
	address |= cpu->read_memory( cpu->sys, cpu->pc ) <<8; // pc here is at JSR's 3rd byte
	cpu->pc = address;
}
// -------------------------------------------------------------------------------
static void RTS( Cpu6502 *cpu )
{
	cpu->pc = pull( cpu ); // pc's lowbyte
	cpu->pc |= pull( cpu ) <<8; // pc's highbyte
	cpu->pc++; // Fix JSR's off by -1 error
}
// -------------------------------------------------------------------------------
static void IRQ( Cpu6502 *cpu, byte brk )
{
	// WIP: If it's an IRQ, check for the interrupt disable flag
	push( cpu, cpu->pc >>8 ); // pc's highbyte
	push( cpu, cpu->pc & 0xFF ); // pc's lowbyte
	push( cpu, pack_status( cpu, brk ) );
	// WIP: DarcNES unsets decimal flag here, other sources don't
	// "NMOS 6502 do not clear the decimal mode flag when an interrupt occurs". Other 6502s do?
	cpu->status.interrupt_disable = 1;
	cpu->pc = cpu->read_memory( cpu->sys, 0xFFFE ); // Jump to IRQ/BRK vector
	cpu->pc |= cpu->read_memory( cpu->sys, 0xFFFF ) <<8;
}
// -------------------------------------------------------------------------------
static void RTI( Cpu6502 *cpu )
{
	unpack_status( cpu, pull( cpu ) );
	cpu->pc = pull( cpu ); // pc's lowbyte
	cpu->pc |= pull( cpu ) <<8; // pc's highbyte
}
// -------------------------------------------------------------------------------






















