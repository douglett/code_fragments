#pragma once

#include <cstdint>
#include <string>

namespace x3 {
namespace gltex {

	uint32_t generate(const std::string& name, const std::string& type);
	uint32_t gettexID(const std::string& name);

} // end gltex
} // end x3