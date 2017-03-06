#include <string>
#include <vector>
#include <fstream>
#include "basicp.h"
#include "basiccpu.h"

using namespace std;
using namespace bc;


namespace basic {

	struct Head {
		static const uint16_t 
			H_SIZE  = 32,
			FN_SIZE = 5,
			FN_MAIN = 6;
		vector<uint16_t> prog;
		uint16_t func_count = 0;
		int write() {
			prog = {
				0,  // NOOP
				imerge(OP_JSR, ADRW_NWD, 0), FN_MAIN,
				imerge(OP_SET, ADR_PC, ADR_NWD), 0,  // die
				func_count, // [5] FN_SIZE
			};
			// padding
			while (prog.size() < H_SIZE)
				prog.push_back(0);
			return 0;
		}
		string display() {
			// boot code
			uint16_t PC = 0;
			string s1 = "boot:  ";
			for (int i = 0; i < 3; i++)
				s1 += strfmt("[%s]  ", iline(&prog[0], PC).c_str());
			// data block
			string s2 = strfmt("data:  [FN_SIZE  %d]", func_count);
			return s1 + "\n" + s2;
		}
	};

	struct FuncList {
		vector<pair<string, uint16_t>> fnlist = { {"main",0} };
		vector<uint16_t> prog;
		int write() {
			prog = { };
			for (const auto& f : fnlist)
				prog.push_back(f.second);
			for (const auto& f : fnlist) {
				prog.push_back(f.first.length());
				for (auto c : f.first)
					prog.push_back(c);
			}
			return 0;
		}
		string display() {
			string s = "func:  ";
			for (const auto& fn : fnlist)
				s += strfmt("[%s  %d]  ", fn.first.c_str(), fn.second);
			return s;
		}
	};

	int load(const string& fname, vector<uint16_t>& prog) {
		Head head;
		head.write();
		FuncList func;
		func.write();

		// join program
		prog = { };
		prog.insert(prog.end(), head.prog.begin(), head.prog.end());
		prog.insert(prog.end(), func.prog.begin(), func.prog.end());
		// display
		for (int i = 0; i < prog.size(); i++) {
			const char* s = "";
			if      (i > 0 && i % 8 == 7)  s = "\n";
			else if (i > 0 && i % 4 == 3)  s = "   ";
			printf("%08x %s", prog[i], s);
		}
		printf("\n\n");
		cout << head.display() << endl;
		cout << func.display() << endl;
		return 0;
	}

} // end basic