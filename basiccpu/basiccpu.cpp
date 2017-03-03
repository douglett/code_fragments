#include <iostream>
#include <vector>
#include <map>
#include "basiccpu.h"

using namespace std;


namespace bc {
	static const uint8_t 
		BITMASK_5 = 0b00011111, 
		BITMASK_6 = 0b00111111;

	void isplit(uint16_t in, char* o, char* b, char* a) {
		// format:  aaaaaabbbbbooooo
		if (a != NULL)  *a = (in >> 10) & BITMASK_6;
		if (b != NULL)  *b = (in >> 5 ) & BITMASK_5;
		if (o != NULL)  *o = in & BITMASK_5;
	}
	uint16_t imerge(char o, char b, char a) {
		// format:  aaaaaabbbbbooooo
		return ((a & BITMASK_6) << 10) | ((b & BITMASK_5) << 5) | (o & BITMASK_5);
	}
	int ilen(uint16_t in) {
		char len=1, o, a, b;
		isplit(in, &o, &a, &b);
		if (a == ADR_NWD || a == ADRW_NWD)  len++;
		if (b == ADR_NWD || b == ADRW_NWD)  len++;
		return len;
	}
	uint16_t* iptr(CPU& cpu, char adr) {
		static uint16_t nil;
		switch (ADDRCODE(adr)) {
		case ADR_A: return &cpu.A;  case ADR_B: return &cpu.B;  case ADR_C: return &cpu.C;
		case ADR_X: return &cpu.X;  case ADR_Y: return &cpu.Y;  case ADR_Z: return &cpu.Z;
		case ADR_I: return &cpu.I;  case ADR_J: return &cpu.J;
		case ADRW_A: return &cpu.ram[cpu.A];  case ADRW_B: return &cpu.ram[cpu.B];  case ADRW_C: return &cpu.ram[cpu.C];
		case ADRW_X: return &cpu.ram[cpu.X];  case ADRW_Y: return &cpu.ram[cpu.Y];  case ADRW_Z: return &cpu.ram[cpu.Z];
		case ADRW_I: return &cpu.ram[cpu.I];  case ADRW_J: return &cpu.ram[cpu.J];
		case ADR_PC:    return &cpu.PC;
		case ADR_SP:    return &cpu.SP;
		case ADR_NWD:   return &cpu.ram[cpu.PC++];
		case ADRW_PC:   return &cpu.ram[cpu.PC];
		case ADRW_SP:   return &cpu.ram[cpu.SP];
		case ADRW_NWD:  return &cpu.ram[ cpu.ram[cpu.PC++] ];
		default:  return &nil;
		}
	}
	string inameop(char o) {
		static const map<int, string> OP_STR = {
			{ OP_NOOP, "NOOP" },
			{ OP_ADD,  "ADD"  }, { OP_SUB, "SUB" }, { OP_MUL, "MUL" }, { OP_DIV, "DIV" },
			{ OP_IFE,  "IFE"  }, { OP_IFN, "IFN" }, { OP_IFL, "IFL" }, { OP_IFG, "IFG" },
			{ OP_SET,  "SET"  }, { OP_JSR, "JSR" }, { OP_RET, "RET" }
		};
		if (OP_STR.count(o))  return OP_STR.at(o);
		return  "?";
	}
	string inameaddr(char a) {
		const static string         reg  = "ABCXYZIJ";
		const static vector<string> reg2 = { "PC", "SP", "NWD" };
		if (a >= ADR_A   && a <= ADR_J   )  return reg.substr(a - ADR_A, 1);
		if (a >= ADRW_A  && a <= ADRW_J  )  return "[" + reg.substr(a - ADRW_A, 1) + "]";
		if (a >= ADR_PC  && a <= ADR_NWD )  return reg2[a - ADR_PC];
		if (a >= ADRW_PC && a <= ADRW_NWD)  return "[" + reg2[a - ADR_PC] + "]";
		return  "?";
	}
	string iline(const CPU& cpu) {
		// init
		char o, a, b;
		uint16_t i = cpu.PC;
		string s;
		// setup
		isplit(cpu.ram[i], &o, &a, &b);
		s = inameop(o);
		// get first argument
		if      (a == ADR_NWD )  s += strfmt( " %d",   cpu.ram[++i] );
		else if (a == ADRW_NWD)  s += strfmt( " [%d]", cpu.ram[++i] );
		else    s += " " + inameaddr(a);
		// get second argument
		if      (b == ADR_NWD )  s += strfmt( " %d",   cpu.ram[++i] );
		else if (b == ADRW_NWD)  s += strfmt( " [%d]", cpu.ram[++i] );
		else    s += " " + inameaddr(b);
		// print
		return s;
	}

	int reset(CPU& cpu) {
		cpu.A = cpu.B = cpu.C = cpu.X = cpu.Y = cpu.Z = cpu.I = cpu.J = 0;
		cpu.PC  = 1;
		cpu.CYC = 0;
		cpu.SP  = 0xffff;
		fill_n(cpu.ram, 0x10000, 0);
		return 0;
	}

	int step(CPU& cpu) {
		if (cpu.PC == 0)  return 1;  // stop
		char     o, a, b;
		uint16_t in = cpu.ram[cpu.PC++];
		isplit(in, &o, &a, &b);
		switch (OPCODE(o)) {
		// switch (o) {
		case OP_NOOP:  return 1;  // end execution ok
		case OP_ADD:  *iptr(cpu,a) += *iptr(cpu,b);  break;
		case OP_SUB:  *iptr(cpu,a) -= *iptr(cpu,b);  break;
		case OP_MUL:  *iptr(cpu,a) *= *iptr(cpu,b);  break;
		case OP_DIV:  *iptr(cpu,a) /= *iptr(cpu,b);  break;
		case OP_IFE:  if (!(*iptr(cpu,a) == *iptr(cpu,b)))  cpu.PC++;  break;
		case OP_IFN:  if (!(*iptr(cpu,a) != *iptr(cpu,b)))  cpu.PC++;  break;
		case OP_IFL:  if (!(*iptr(cpu,a) <  *iptr(cpu,b)))  cpu.PC++;  break;
		case OP_IFG:  if (!(*iptr(cpu,a) >  *iptr(cpu,b)))  cpu.PC++;  break;
		case OP_SET:  *iptr(cpu,a) = *iptr(cpu,b);  break;
		case OP_JSR:  cpu.ram[cpu.SP--] = cpu.PC;  cpu.PC = *iptr(cpu,a);  break;
		case OP_RET:  cpu.PC = cpu.ram[cpu.SP++];  break;
		}
		cpu.CYC += ilen(in);  // increase cycle count
		return 0;
	}
} // end bc