#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>

#include "tinylisp.h"

using namespace std;
using namespace lisp;



namespace env {

	// const val nil;
	const vector<string> keywords = {
		"nil", 
		"+", "-", "*", "/",
		"=", ">", "<", ">=", "<=",
		"let", "defun", "print", "len", "while"
	};

	map<string, val> def;

	int define(const string& name, const val& v) {
		// don't redefine keywords
		string namel = strtolower(name);
		for (auto &s : keywords)
			if (namel == s) {
				lerror("runtime", string("tried to redefine keyword: ")+namel, v.tok);
				return 1;
			}
		// set def
		def[name] = v;
		return 0;
	}

	const val& getdef(const string& name) {
		string namel = strtolower(name);
		for (const auto &d : def)
			if (d.first == namel)
				return d.second;
		return nil;
	}

	const val& getdef(const val& name) {
		auto& v = getdef(name.sval);
		if (&v == &nil)
			lerror("runtime", string("unknown definition: ")+strtolower(name.sval), name.tok);
		return v;
	}

	void undef(const string& name) {
		string namel = strtolower(name);
		auto &v = getdef(namel);
		if (&v != &nil)
			def.erase(namel);
		// v invalid here!
	}

} // end env




namespace lisp {

	// public helpers
	int isnil(const val& v) {
		return v.type == val::T_LIST && v.lval.size() == 0;
	}
	
	// list tools
	const val& firstitem(const val& v) {
		if (v.type == val::T_LIST && v.lval.size() > 0)
			return firstitem(v.lval[0]);
		return v;
	}
	const val& lastitem(const val& v) {
		if (v.type == val::T_LIST && v.lval.size() > 0)
			return lastitem(v.lval.back());
		return v;
	}
	const val& atomize(const val& v) {
		if (v.type == val::T_LIST && v.lval.size() == 1)
			return atomize(v.lval[0]);
		return v;
	}

	// create sublist
	val sublist(const val& v, int start, int len) {
		int last = start + len;
		if (len < 1)
			last = v.lval.size() + len;
		val rval;
		for (int i = start; i < last; i++)
			rval.lval.push_back(v.lval[i]);
		return atomize(rval);
	}

	int compare(const val& v1, const val& v2) {
		if (v1.type != v2.type)
			return 0;
		else if (v1.type == val::T_INT && v1.ival == v2.ival)
			return 1;
		else if (v1.type == val::T_STRING && v1.sval == v2.sval)
			return 1;
		else if (isnil(v1) && isnil(v2))
			return 1;
		return 0;
	}


	// execute a function
	val execute_func(const val& name, const val& args) {
		const val& func = env::getdef(name);
		if (haserror())
			return nil;
		// assert( func.lval.size() == 2 );
		if (func.lval.size() != 2) {
			lerror("runtime", "expected 2 value list", func.tok);
			return nil;
		}
		// define ids
		auto &argnames = func.lval[0].lval;
		for (int i = 0; i < argnames.size(); i++) 
			if (argnames[i].type == val::T_IDENT && i < args.lval.size()) {
				int err = env::define( argnames[i].sval, args.lval[i] );
				if (err)
					return nil;
			}
		// run
		val rval;
		for (int i = 0; i < func.lval[1].lval.size(); i++) {
			rval = eval(func.lval[1].lval[i]);
			if (haserror())
				break;
		}
		// undefine ids
		for (int i = 0; i < argnames.size(); i++)
			env::undef(argnames[i].sval);
		return rval;
	}


	// public: evaluate list
	val eval(const val& v) {
		switch (v.type) {
		case val::T_INT:
		case val::T_STRING:
			return v;

		case val::T_IDENT:
			// cout << "get " << v.sval << " " << parser::show_val(env::get(v.sval)) << endl;
			if (v.sval == "nil")
				return nil;
			return env::getdef(v);

		case val::T_LIST:
			if (isnil(v))
				return v;  // test for nil
			if (v.lval[0].type != val::T_IDENT)
				return v;  // not a function: return entire list

			// otherwise, try and run list as function
			auto name = v.lval[0].sval;

			// math functions
			if (name == "+" || name == "-" || name == "*" || name == "/") {
				assert(v.lval.size() >= 2);  // lazy error checking
				val rval(val::T_INT);
				for (int i = 1; i < v.lval.size(); i++) {
					int res = eval(v.lval[i]).ival;
					if (haserror())
						return rval;  // stop on error
					else if (i == 1)
						rval.ival = res;  // set start value
					else if (name == "+")
						rval.ival += res;
					else if (name == "-")
						rval.ival -= res;
					else if (name == "*")
						rval.ival *= res;
					else if (name == "/")
						rval.ival /= res;
				}
				return rval;
			}
			// equality (numeric and string)
			else if (name == "=") {
				assert(v.lval.size() >= 3);
				val rval(val::T_INT);
				val comp;
				// check against each comparison value
				for (int i = 1; i < v.lval.size(); i++) {
					val vv = eval(v.lval[i]);
					if (haserror())
						return rval;  // stop on error
					else if (i == 1)
						comp = vv;  // get comparison value
					else if (!compare(comp, vv))
						return rval;  // not equal
				}
				rval.ival = 1;
				return rval;
			}
			// compare (numeric)
			else if (name == ">" || name == "<" || name == ">=" || name == "<=") {
				assert(v.lval.size() >= 3);
				val rval(val::T_INT);
				int subject = 0;
				// check against each comparison value
				for (int i = 1; i < v.lval.size(); i++) {
					int vv = eval(v.lval[i]).ival;
					if (haserror())
						return rval;  // stop on error
					else if (i == 1)
						subject = vv;  // comparison value
					else if (name == ">" && vv > subject)
						continue;
					else if (name == "<" && vv < subject)
						continue;
					else if (name == ">=" && vv >= subject)
						continue;
					else if (name == "<=" && vv <= subject)
						continue;
					else
						return rval;  // bad comparison
				}
				// all comparisons correct
				rval.ival = 1;
				return rval;
			}
			// define value
			else if (name == "let") {
				assert(v.lval.size() == 3);
				assert(v.lval[1].type == val::T_IDENT);
				const string &name = v.lval[1].sval;
				val setval = eval(v.lval[2]);
				if (haserror())
					return setval;
				env::define(name, setval);  // haserror() is set here
				return setval;
			}
			// define function
			else if (name == "defun") {
				// lazy errors
				assert(v.lval.size() == 4);
				assert(v.lval[1].type == val::T_IDENT);
				assert(v.lval[2].type == val::T_LIST);
				assert(v.lval[3].type == val::T_LIST);
				// define without
				const string &name = v.lval[1].sval;
				val deflist = sublist(v, 2, 0);
				env::define(name, deflist);  // haserror() is set here
				return lastitem(v);
			}
			// print evaluated list contents
			else if (name == "print") {
				stringstream ss;  // cache line output
				for (int i = 1; i < v.lval.size(); i++) {
					val vv = eval(v.lval[i]);
					if (haserror())
						return nil;  // stop on error
					ss << parser::show_val(vv) << " ";
				}
				cout << ss.str() << endl;
				return lastitem(v);
			}
			// list length
			else if (name == "len") {
				assert(v.lval.size() == 2);
				val len(val::T_INT);
				val arg = eval(v.lval[1]);
				if (haserror())
					return len;
				assert(arg.type == val::T_LIST);
				len.ival = arg.lval.size();
				return len;
			}
			// while loop
			else if (name == "while") {
				// cout << ">> while" << endl;
				assert(v.lval.size() == 3);
				val rval;
				auto& condition = v.lval[1];
				auto& body = v.lval[2].lval;
				while (eval(condition).ival != 0) {
					if (haserror())
						return nil;
					for (int i = 0; i < body.size(); i++) {
						rval = eval(body[i]);
						if (haserror())
							return nil;
					}
				}
				return rval;  // return last result
			}
			// user defined functions
			else {
				// cout << ">> calling: " << name << endl;
				val args(val::T_LIST);
				for (int i = 0; i < v.lval.size(); i++) {
					val vv = eval(v.lval[i]);
					if (haserror())
						return nil;  // error check everything, including first item
					if (i >= 1)
						args.lval.push_back(vv);
				}
				return execute_func(v.lval[0], args);
			}
		} // end switch

		cout << "should never get here" << endl;
		return nil;
	}

} // end lisp
