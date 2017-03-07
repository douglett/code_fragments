namespace basic {

	static vector<string> toklist;

	static int parse() {
		for (int i=0; i<toklist.size(); i++) {
			if (toklist[i][0] == '\'') 
				continue;
			else if (toklist[i] == "func") {
				string name = toklist[++i];
			}
			else if (toklist[i] == "end" && toklist[i+1] == "func") {
				i++;
			}
		}
		return 0;
	}

	int fparse(const string& fname) {
		// open
		toklist = { };
		fstream fs(fname, fstream::in);
		if (!fs.is_open()) {
			fprintf(stderr, "error loading file: %s\n", fname.c_str());
			return 1;
		}
		// load tokens
		string s, s2;
		while (fs >> s) {
			if (s[0] == '\'')  getline(fs, s2),  s += s2;
			toklist.push_back(s);
			cout << ">> " << s << endl;
		}
		// parse
		return  parse();
	}

} // end basic