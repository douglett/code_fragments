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
		int write() {
			prog = {
				0,  // NOOP
				imerge(OP_JSR, ADRW_NWD, 0), FN_MAIN,
				imerge(OP_SET, ADR_PC, ADR_NWD), 0,  // die
				0, // [5] FN_SIZE
			};
			// padding
			while (prog.size() < H_SIZE)
				prog.push_back(0);
			return 0;
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
	};

	int load(const string& fname, vector<uint16_t>& prog) {
		Head head;
		head.write();
		FuncList func;
		func.write();

		prog = { };
		prog.insert(prog.end(), head.prog.begin(), head.prog.end());
		prog.insert(prog.end(), func.prog.begin(), func.prog.end());
		for (auto p : prog)
			printf("%08x ", p);
		printf("\n");
		return 0;
	}

} // end basic