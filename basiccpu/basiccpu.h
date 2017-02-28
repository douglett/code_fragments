#pragma once

namespace bc {
	enum OPCODE {
		OP_NOOP=0,
		OP_ADD =1,  OP_SUB, OP_MUL, OP_DIV,
		OP_IFE =5,  OP_IFN, OP_IFL, OP_IFG,
		OP_SET =10, OP_JSR, OP_RET
	};
	enum ADDRCODE {
		ADR_A  =0,  ADR_B, ADR_C, ADR_X, ADR_Y, ADR_Z, ADR_I, ADR_J,
		ADR_PC =10, ADR_SP, ADR_NWD,
		ADRW_A =15, ADRW_B, ADRW_C, ADRW_X, ADRW_Y, ADRW_Z, ADRW_I, ADRW_J,
		ADRW_PC=25, ADRW_SP, ADRW_NWD
	};
	struct CPU {
		uint16_t A,B,C,X,Y,Z,I,J;
		uint16_t PC,SP,CYC;
		uint16_t ram[0x10000];
	};
	void      isplit(uint16_t in, char* o, char* b, char* a);
	uint16_t  imerge(char o, char b, char a);
	int       ilen(uint16_t in);
	uint16_t* iptr(CPU& cpu, char adr);
	int       reset(CPU& cpu);
	int       step(CPU& cpu);
} // end bc