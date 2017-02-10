#pragma once

#include <cstdint>
#include <string>

namespace x3 {
namespace gltex {

	uint32_t gettexID    (const std::string& name);
	uint32_t generate    (const std::string& name);
	uint32_t generateall ();

} // end gltex
} // end x3