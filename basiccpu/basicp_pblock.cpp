#include <string>
#include <vector>
#include <fstream>
#include "basicp.h"
#include "basiccpu.h"

using namespace std;
using namespace bc;


namespace basic {

	// basic program block
	struct pblock {
		vector<uint16_t> prog;
		virtual int size() const = 0;
		virtual int write() = 0;
		virtual string display() const = 0;
	};


	struct Head : pblock {
		static const uint16_t 
			HEAD_SIZE    = 32,
			FN_SIZE_ADDR = 5,
			FN_MAIN_ADDR = 6;
		uint16_t func_count = 0;
		int size() const {
			return HEAD_SIZE;
		}
		int write() {
			prog = {
				0,  // NOOP
				imerge(OP_JSR, ADRW_NWD, 0), FN_MAIN_ADDR,
				imerge(OP_SET, ADR_PC, ADR_NWD), 0,  // die
				func_count, // [5] FN_SIZE
			};
			// padding
			while (prog.size() < HEAD_SIZE)
				prog.push_back(0);
			return 0;
		}
		string display() const {
			// boot code
			uint16_t PC = 0;
			string s1 = "boot:  ";
			for (int i = 0; i < 3; i++)
				s1 += strfmt("[%s]  ", iline(&prog[0], PC).c_str());
			// data block
			string s2 = strfmt("head:  [FN_SIZE  %d]", func_count);
			return s1 + "\n" + s2;
		}
	};


	struct FuncList : pblock {
		vector<pair<string, uint16_t>> fnlist = { {"main",0} };
		int size() const {
			int sz = 0;
			for (const auto& f : fnlist)
				sz += 2 + f.first.length();
			return sz;
		}
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
		string display() const {
			string s = "func:  ";
			for (const auto& fn : fnlist)
				s += strfmt("[%s  %d]  ", fn.first.c_str(), fn.second);
			return s;
		}
	};


	struct DataList : pblock {
		static const uint16_t
			DAT_NUM = 0,
			DAT_ARR = 1,
			DAT_STR = 2;
		vector<vector<uint16_t>> data;
		int size() const {
			int sz = 0;
			for (const auto& d : data)
				sz += d.size();
			return sz;
		}
		int write() {
			prog = { };
			for (const auto& d : data)
				prog.insert(prog.end(), d.begin(), d.end());
			return 0;
		}
		string showdata(int i) const {
			string s;
			const auto& d = data[i];
			switch (d[0]) {
			case DAT_NUM:  s += strfmt("%d", d[1]);  break;
			case DAT_ARR:  s += "[";  for (int i=0; i<d[1]; i++)  s += strfmt("%d ", d[2+i]);  s += "]";  break;
			case DAT_STR:  s += "[";  for (int i=0; i<d[1]; i++)  s += char(d[2+i]&0xffff);  s += "]";  break;
			}
			return s;
		}
		string display() const {
			string s = "data:  ";
			for (int i = 0; i < data.size(); i++)
				s += "\n  :" + showdata(i);
			return s;
		}
		// add new data blocks
		int add(uint16_t i) {
			data.emplace_back();
			data.back().insert(data.back().end(), { DAT_NUM, i });
			return 0;
		}
		int addvec(const vector<uint16_t>& v) {
			data.emplace_back();
			data.back().push_back((uint16_t)DAT_ARR);  // array identifier
			data.back().push_back(v.size());  // array length
			data.back().insert(data.back().end(), v.begin(), v.end());  // copy array
			return 0;
		}
		int addstr(const string& s) {
			data.emplace_back();
			data.back().push_back((uint16_t)DAT_STR);  // string identifier (lol casting)
			data.back().push_back(s.size());  // string length
			for (int i=0; i<s.length(); i++)
				data.back().push_back( s[i] );  // copy string
			return 0;
		}
	};


	struct Program : pblock {
		int size() const {
			return prog.size();
		}
		int write() {
			return 0;
		}
		string display() const {
			return "prog:  \n";
		}
	};


	int pblock_test() {
		Head head;
		FuncList func;
		DataList data;
		data.add(10);
		data.addstr("hello world");
		data.addvec({ 10, 10, 10 });

		// join program
		prog = { };
		for (const auto& p : vector<pblock*>{ &head, &func, &data })
			p->write(),  prog.insert(prog.end(), p->prog.begin(), p->prog.end());
		showprog(prog);  cout << endl;
		cout << head.display() << endl;
		cout << func.display() << endl;
		cout << data.display() << endl;
		return 0;
	}

} // end basic