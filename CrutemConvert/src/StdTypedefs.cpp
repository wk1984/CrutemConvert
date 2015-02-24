#include "StdTypedefs.h"

#include <iostream>
#include <sstream>
#include <string>

std::string to_string(int32 i)
{
	std::ostringstream stream;
	stream << std::dec << i;
	return stream.str().c_str();
}

