#pragma once

#include <string>

#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "zn_exception.hpp"

class zn_message_digest
{
	private:
		SHA_CTX _sha;

	public:
		zn_message_digest();
		~zn_message_digest();

		void feed(const std::string &message);
		std::string get_digest();
		void clear();
};

class zn_soap_digest
{
	public:
		static std::string md_ssha(const std::string& content);
};
