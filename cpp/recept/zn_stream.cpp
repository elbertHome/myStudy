#include "zn_stream.hpp"

// dummy
void zn_buffered_stream::feed(const std::string &buf)
{
	append(buf);
}

std::string zn_buffered_stream::consume()
{
	return shift();
}
