
#include <stdio.h>
#include <string.h>
#include "Cpu6502.h"
#include "symbols.h"

#ifdef _Cpu6502_Disassembler

static const char opcode_mnemonic[0x100][4] = {
//   0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F
   "BRK", "ORA", "...", "...", "...", "ORA", "ASL", "...", "PHP", "ORA", "ASL", "...", "...", "ORA", "ASL", "...", // 00
   "BPL", "ORA", "...", "...", "...", "ORA", "ASL", "...", "CLC", "ORA", "...", "...", "...", "ORA", "ASL", "...", // 10
   "JSR", "AND", "...", "...", "BIT", "AND", "ROL", "...", "PLP", "AND", "ROL", "...", "BIT", "AND", "ROL", "...", // 20
   "BMI", "AND", "...", "...", "...", "AND", "ROL", "...", "SEC", "AND", "...", "...", "...", "AND", "ROL", "...", // 30
   "RTI", "EOR", "...", "...", "...", "EOR", "LSR", "...", "PHA", "EOR", "LSR", "...", "JMP", "EOR", "LSR", "...", // 40
   "BVC", "EOR", "...", "...", "...", "EOR", "LSR", "...", "CLI", "EOR", "...", "...", "...", "EOR", "LSR", "...", // 50
   "RTS", "ADC", "...", "...", "...", "ADC", "ROR", "...", "PLA", "ADC", "ROR", "...", "JMP", "ADC", "ROR", "...", // 60
   "BVS", "ADC", "...", "...", "...", "ADC", "ROR", "...", "SEI", "ADC", "...", "...", "...", "ADC", "ROR", "...", // 70
   "...", "STA", "...", "...", "STY", "STA", "STX", "...", "DEY", "...", "TXA", "...", "STY", "STA", "STX", "...", // 80
   "BCC", "STA", "...", "...", "STY", "STA", "STX", "...", "TYA", "STA", "TXS", "...", "...", "STA", "...", "...", // 90
   "LDY", "LDA", "LDX", "...", "LDY", "LDA", "LDX", "...", "TAY", "LDA", "TAX", "...", "LDY", "LDA", "LDX", "...", // A0
   "BCS", "LDA", "...", "...", "LDY", "LDA", "LDX", "...", "CLV", "LDA", "TSX", "...", "LDY", "LDA", "LDX", "...", // B0
   "CPY", "CMP", "...", "...", "CPY", "CMP", "DEC", "...", "INY", "CMP", "DEX", "...", "CPY", "CMP", "DEC", "...", // C0
   "BNE", "CMP", "...", "...", "...", "CMP", "DEC", "...", "CLD", "CMP", "...", "...", "...", "CMP", "DEC", "...", // D0
   "CPX", "SBC", "...", "...", "CPX", "SBC", "INC", "...", "INX", "SBC", "NOP", "...", "CPX", "SBC", "INC", "...", // E0
   "BEQ", "SBC", "...", "...", "...", "SBC", "INC", "...", "SED", "SBC", "...", "...", "...", "SBC", "INC", "..."  // F0
};

extern const int opcode_addressing[ 0x100 ];
extern const int opcode_legality[ 0x100 ];

void Cpu6502_Disassemble( Cpu6502 *cpu, int cycles )
{
   byte opcode = cpu->disasm.op_bytes[0];
   printf( "%04X  %02X ", cpu->disasm.pc, opcode );
   
   switch( opcode_addressing[opcode] )
   {
      case IMP: case ACU:
         printf( "     " );
         break;
      case IMM: case REL: case ZRP: case ZPX: case ZPY: case IIX: case IIY:
         printf( "%02X   ", cpu->disasm.op_bytes[1] );
         break;
      default: // Absolute based addressing modes or illegal instructions
         printf( "%02X %02X", cpu->disasm.op_bytes[1], cpu->disasm.op_bytes[2] );
         break;
   }
   
   int adr_len = 0; // How many chars does the addressing mode log take

   if( opcode_legality[opcode] == 1 ) {
      printf(" *NOP ");
   }
   else
   {
      printf("  %s ", opcode_mnemonic[opcode] );
      switch( opcode_addressing[opcode] )
      {
         case ABS:
            adr_len = printf( "$%04X", cpu->disasm.address );
            if( ( opcode != JMP_Absolute_4C ) && ( opcode != JSR_20 ) ) {
               adr_len += printf( " = %02X", cpu->disasm.value );
            }
            break;
         case REL: adr_len = printf( "$%04X", cpu->disasm.address ); break;
         case IMM: adr_len = printf( "#$%02X", cpu->disasm.op_bytes[1] ); break;
         case ZRP: adr_len = printf( "$%02X = %02X", cpu->disasm.op_bytes[1], cpu->disasm.value ); break;
         case ZPX: adr_len = printf( "$%02X,X @ %02X = %02X", cpu->disasm.op_bytes[1], cpu->disasm.address, cpu->disasm.value ); break;
         case ZPY: adr_len = printf( "$%02X,Y @ %02X = %02X", cpu->disasm.op_bytes[1], cpu->disasm.address, cpu->disasm.value ); break;
         case ABX: adr_len = printf( "$%02X%02X,X @ %04X = %02X", cpu->disasm.op_bytes[2], cpu->disasm.op_bytes[1], cpu->disasm.address, cpu->disasm.value ); break;
         case ABY: adr_len = printf( "$%02X%02X,Y @ %04X = %02X", cpu->disasm.op_bytes[2], cpu->disasm.op_bytes[1], cpu->disasm.address, cpu->disasm.value ); break;
         case IIX: adr_len = printf( "($%02X,X) @ %02X = %04X = %02X", cpu->disasm.op_bytes[1], cpu->disasm.temp_address, cpu->disasm.address, cpu->disasm.value ); break;
         case IIY: adr_len = printf( "($%02X),Y = %04X @ %04X = %02X", cpu->disasm.op_bytes[1], cpu->disasm.temp_address, cpu->disasm.address, cpu->disasm.value ); break;
         case IND: adr_len = printf( "($%02X%02X) = %04X", cpu->disasm.op_bytes[2], cpu->disasm.op_bytes[1], cpu->disasm.temp_address ); break;
         case ACU: adr_len = printf( "A" ); break;
         case IMP: adr_len = 0; break;
      }
   }
   for( int i = adr_len; i < 28; ++i ) {
      printf(" "); // Complete addressing mode log to 26 characters for alignment
   }

   printf( "A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\r\n",
      cpu->disasm.a, cpu->disasm.x, cpu->disasm.y, cpu->disasm.status, cpu->disasm.sp, cycles );
}

#endif // #ifdef _Cpu6502_Disassembler