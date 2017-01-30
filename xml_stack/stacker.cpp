#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <regex>

using namespace std;


class StrFeed {
public:
	virtual int next(std::string& s) = 0;
};

class FileFeed : public StrFeed {
public:
	fstream fs;

	int load(const string& fname) {
		fs.open(fname, fstream::in);
		return fs.is_open();
	}

	virtual int next(string& s) {
		s = "";
		while (fs.peek() != EOF) {
			switch (fs.peek()) {
			case '<':   if (s.length())  return 1;  s += fs.get();  break;  // break if not at start
			case '>':   s += fs.get();  return 1;  break;  // add and break
			case '\f':
			case '\r':
			case '\n':  s += "\\n";  fs.get();  break;
			default:    s += fs.get();  break;
			}
		}
		return (s.length() > 0);
	}
};


namespace stacker {
	struct Node {
		string text;
		string tag;
		int tagtype = 0;
		list<Node> children;
	};
	Node head;
	// vector<Node*> nstack;

	static string gettagname(const string& s, int start) {
		if (start < 1 || start >= s.length())  return "";
		string str;
		regex reg("^[A-Za-z0-9_\\-]+$");
		for (int pos = start; pos < s.length(); pos++)
			if    (regex_match(str + s[pos], reg))  str += s[pos];
			else  break;
		for (auto& c : str)  c = tolower(c);  // make string lower case
		return str;
	}

	static const vector<string> empty_tags = {
		"meta", "link", "br" };

	static void setnodetext(Node& node, const string& s) {
		node.text = s;
		node.tag = "";
		node.tagtype = 0;
		if      (s.substr(0, 2) == "</") { node.tag = gettagname(s, 2);  node.tagtype = 2; }
		else if (s.substr(0, 1) == "<")  { node.tag = gettagname(s, 1);  node.tagtype = 1;
			if      (s.substr(s.size()-3) == "/>")  node.tagtype = 3;
			else if (find(empty_tags.begin(), empty_tags.end(), node.tag) != empty_tags.end())  node.tagtype = 3;
		}
	}

	void mktree(StrFeed& feed) {
		// init
		head = {};
		head.text = head.tag = "[document]";
		vector<Node*> nstack = { &head };
		string s;
		// loop
		while (feed.next(s)) {
			Node node;
			setnodetext(node, s);
			// handle stack
			if (node.tagtype == 1) {
				nstack.back()->children.push_back( node );
				nstack.push_back( &nstack.back()->children.back() );
			}
			else if (node.tagtype == 2) {
				// get last index of nodename
				int lastindex = -1;
				for (int i = 0; i < nstack.size(); i++)
					if (nstack[i]->tag == node.tag && nstack[i]->tagtype == 1)
						lastindex = i;
				// if something was found, erase stack to that point
				if (lastindex > 0)
					nstack.erase( nstack.begin()+lastindex, nstack.end() );
				nstack.back()->children.push_back( node );
			}
			else {
				nstack.back()->children.push_back( node );
			}
		}
		// show completed
		printf("mktree ran. doclen: %d\n", (int)head.children.size());
	}

	void showdoc(const Node& node) {
		static string ind = "";
		printf("%s(%s)  [%.20s]\n", ind.c_str(), node.tag.c_str(), node.text.c_str());
		for (const auto& n : node.children) {
			ind += "|   ";
			showdoc(n);
			ind.erase(ind.size()-4);
		}
	}

	void showdoc() {
		showdoc(head);
	}

} // end stacker


int main() {
	printf("start\n");
	FileFeed feed;
	feed.load("cppref.html");
	stacker::mktree(feed);
	stacker::showdoc();

	return 0;
}