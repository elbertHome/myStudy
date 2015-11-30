#pragma once

#include "zlib.h"

#include "zn_stream.hpp"

class zn_gunzip_stream : public zn_buffered_stream
{
	private:
		z_stream _stream;

	public:
		zn_gunzip_stream();
		~zn_gunzip_stream();

		virtual std::string consume();
};
