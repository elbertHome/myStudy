#include "zn_message_digest.hpp"

#include <cstring>
#include <cmath>
#include <cstdlib>

static char *base64_encode(const char *message, char *buf, size_t n);
static std::string _get_saltbytes();

zn_message_digest::zn_message_digest()
{
	if (!SHA1_Init(&_sha))
	{
		ZN_THROW(zn_digest_exception, "SHA1 init error.");
	}
}

zn_message_digest::~zn_message_digest()
{
	char sha1_buf[SHA_DIGEST_LENGTH];
	SHA1_Final(reinterpret_cast<unsigned char *>(sha1_buf), &_sha);
}

void zn_message_digest::feed(const std::string &message)
{
	if (!SHA1_Update(&_sha, message.c_str(), message.length()))
	{
		ZN_THROW(zn_digest_exception, "SHA1 update error.");
	}
}

std::string zn_message_digest::get_digest()
{
	char sha1_buf[SHA_DIGEST_LENGTH];
	char base64_buf[SHA_DIGEST_LENGTH * 2];
	memset(sha1_buf, 0, SHA_DIGEST_LENGTH);
	memset(base64_buf, 0, SHA_DIGEST_LENGTH * 2);

	// sha1
	if (!SHA1_Final(reinterpret_cast<unsigned char *>(sha1_buf), &_sha))
	{
		ZN_THROW(zn_digest_exception, "SHA1 final error.");
	}

	// base64
	base64_encode(sha1_buf, base64_buf, SHA_DIGEST_LENGTH);

	// replace '/' to ','
	for (char *p = base64_buf; *p != 0; ++p)
	{
		if (*p == '/')
		{
			*p = ',';
		}
	}

	return std::string(base64_buf);
}

void zn_message_digest::clear()
{
	if (!SHA1_Init(&_sha))
	{
		ZN_THROW(zn_digest_exception, "SHA1 init error.");
	}
}

// ssha hash used by MD
// {SSHA}base64(sha1(password + salt) + salt)
std::string zn_soap_digest::md_ssha(const std::string &content)
{
	char sha1_buf[SHA_DIGEST_LENGTH];
	char base64_buf[SHA_DIGEST_LENGTH * 2];
	memset(sha1_buf, 0, SHA_DIGEST_LENGTH);
	memset(base64_buf, 0, SHA_DIGEST_LENGTH * 2);

	std::string salted_password = content + _get_saltbytes();
	if (SHA1(reinterpret_cast<const unsigned char *>(salted_password.c_str()),
				salted_password.length(),
				reinterpret_cast<unsigned char *>(sha1_buf)) == NULL)
	{
		ZN_THROW(zn_digest_exception, "SHA1 error.");
	}

	std::string sha1_digest(sha1_buf, SHA_DIGEST_LENGTH);
	sha1_digest += _get_saltbytes();

	base64_encode(sha1_digest.c_str(), base64_buf, SHA_DIGEST_LENGTH + 4);

	std::string ret("{SSHA}");
	ret += base64_buf;

	return ret;
}

char *base64_encode(const char *message, char *buf, size_t n)
{
	int encode_size = 4 * ceil(n / 3.0f);
	FILE *mstream = fmemopen(buf, encode_size + 1, "w");

	BIO *bio, *b64;
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new_fp(mstream, BIO_NOCLOSE);
	BIO_push(b64, bio);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(b64, message, n);
	(void)BIO_flush(b64);
	BIO_free_all(b64);
	fclose(mstream);

	return buf;
}

static std::string _get_saltbytes()
{
	static const size_t SALT_SIZE = 4;
	char buf[SALT_SIZE];
	//srand(time(NULL));
	srand(0);
	for (size_t i = 0; i != SALT_SIZE; ++i)
	{
		buf[i] = rand() % 256;
	}

	return std::string(buf, SALT_SIZE);
}

/*
int main()
{
	std::string message = "Original String";
	zn_message_digest d;
	d.feed(message);
	std::string digest = d.get_digest();
	std::cout << digest << std::endl;
	d.clear();
	d.feed("adf");
	std::string digest1 = d.get_digest();
	std::cout << digest1 << std::endl;
}
*/
