#include "zn_gunzip.hpp"

#include <vector>

#include <iostream>
#include <fstream>

zn_gunzip_stream::zn_gunzip_stream()
{
	/* allocate inflate state */
    _stream.zalloc = Z_NULL;
    _stream.zfree = Z_NULL;
    _stream.opaque = Z_NULL;
    _stream.next_in = Z_NULL;
	_stream.avail_in = Z_NULL;

	//Initializes the internal stream state for decompression
	inflateInit2(&_stream, 16 + MAX_WBITS);
}

zn_gunzip_stream::~zn_gunzip_stream()
{
	inflateEnd(&_stream);
}

std::string zn_gunzip_stream::consume()
{
	std::string in_buf = shift();
	std::vector<char> out_buf(4096);

	_stream.avail_in = in_buf.length();
	_stream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(in_buf.c_str()));
	_stream.avail_out = 4096;
	_stream.next_out = reinterpret_cast<Bytef *>(&out_buf[0]);

	std::string deflated_data;
	while (1)
	{
		int ret = inflate(&_stream, Z_NO_FLUSH);
		if (ret < 0)
		{
			return std::string();
		}

		// compress data left
		if (_stream.avail_out == 0 && _stream.avail_in != 0)
		{
			deflated_data += std::string(&out_buf[0], 4096);

			_stream.avail_out = 4096;
			_stream.next_out = reinterpret_cast<Bytef *>(&out_buf[0]);
		}
		else
		{
			deflated_data += std::string(&out_buf[0], 4096 - _stream.avail_out);
			break;
		}
	}

	if (_stream.avail_in != 0)
	{
		unshift(std::string(reinterpret_cast<char *>(_stream.next_in), _stream.avail_in));
	}

	return deflated_data;
}

/*
int main()
{
	std::ifstream ifs ("a.gz", std::ifstream::in|std::ifstream::binary);
	std::ofstream ofs ("a.bin", std::ofstream::out|std::ofstream::binary);
	zn_gunzip_stream gunzip_stream;

	char buf[512];
	while (ifs.good())
	{
		ifs.read(buf, 512);
		gunzip_stream.feed(std::string(buf, ifs.gcount()));
		ofs << gunzip_stream.consume();
	}
}
*/
