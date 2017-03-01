#include <string>
#include <vector>

namespace bc {
namespace parse {
	int  parse    (const std::string& fname, std::vector<uint16_t>& prog);
	int  parse    (const std::vector<std::string>& tok, std::vector<uint16_t>& prog);
	void showprog (const std::vector<uint16_t>& prog);
} // end parse
} // end bc