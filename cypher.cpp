#include "cypher.h"

namespace mana
{

bool cypher::is_private_key = false;
bool cypher::is_public_key = false;
mbedtls_rsa_context cypher::private_key;
mbedtls_rsa_context cypher::public_key;

cypher::cypher()
{
}

void cypher::init()
{
	// init private_key and public_key
	is_private_key = false;
	is_public_key = false;

	FILE *f;
	if ((f = fopen(rsa_private_key_file, "rb")))
	{
		mbedtls_rsa_init(&private_key, MBEDTLS_RSA_PKCS_V15, 0);
		int ret = 0;
		ret += mbedtls_mpi_read_file(&private_key.N, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.E, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.D, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.P, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.Q, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.DP, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.DQ, 16, f);
		ret += mbedtls_mpi_read_file(&private_key.QP, 16, f);
		if (!ret)
		{
			private_key.len = (mbedtls_mpi_bitlen(&private_key.N) + 7) >> 3;
			is_private_key = true;
		}

		fclose(f);
	}

	if ((f = fopen(rsa_public_key_file, "rb")))
	{
		mbedtls_rsa_init(&public_key, MBEDTLS_RSA_PKCS_V15, 0);
		int ret = 0;
		ret += mbedtls_mpi_read_file(&public_key.N, 16, f);
		ret += mbedtls_mpi_read_file(&public_key.E, 16, f);
		if (!ret)
		{
			public_key.len = (mbedtls_mpi_bitlen(&public_key.N) + 7) >> 3;
			is_public_key = true;
		}

		fclose(f);
	}
	//cout << is_private_key <<":" << is_public_key << endl;
}

string cypher::sha_file(string filename)
{
	stringstream ret;
	if (! file::exists(filename)) return ret.str();

	unsigned char sha1sum[20];
	char buffer[1000];
	int size = 0;

	mbedtls_sha1_context ctx;
	mbedtls_sha1_init(&ctx);

	mbedtls_sha1_starts(&ctx);

	bzero(sha1sum, sizeof(sha1sum));

	file f(filename);
	f.open();

	while ((size = f.read(buffer, 1000)))
	{
		mbedtls_sha1_update(&ctx, (const unsigned char*)buffer, (size_t)size);
	}
	f.close();

	mbedtls_sha1_finish(&ctx, sha1sum);

	for (int i = 0; i < sizeof(sha1sum); i++)
	{
		ret << (((int)sha1sum[i] < 16) ? "0" : "") << uppercase << hex << (int)sha1sum[i];
	}

	mbedtls_sha1_free(&ctx);
	return ret.str();
}

string cypher::sha_string(string str)
{
	stringstream ret;
	unsigned char sha1sum[20];
	mbedtls_sha1_context ctx;
	mbedtls_sha1_init(&ctx);

	mbedtls_sha1_starts(&ctx);

	bzero(sha1sum, sizeof(sha1sum));

	mbedtls_sha1_update(&ctx, (const unsigned char*)str.c_str(), (size_t)str.length());
	mbedtls_sha1_finish(&ctx, sha1sum);
	for (int i = 0; i < sizeof(sha1sum); i++)
	{
		ret << (((int)sha1sum[i] < 16) ? "0" : "") << uppercase << hex << (int)sha1sum[i];
	}

	mbedtls_sha1_free(&ctx);
	return ret.str();
}

bool cypher::sha_check(string filename, string sha)
{
	if (sha.length() <= 0) return false;
	if (sha == sha_file(filename)) return true;
	return false;
}


string cypher::base64_encode(string S)
{
	string ret = "";
	size_t written_bytes;
	unsigned char *dst = NULL;
	size_t dstlen = 0;
	mbedtls_base64_encode(dst, dstlen, &written_bytes, (unsigned char*)S.c_str(), S.length());
	dstlen = written_bytes;
	dst = new unsigned char[dstlen + 1];
	bzero(dst, dstlen + 1);
	mbedtls_base64_encode(dst, dstlen, &written_bytes, (unsigned char*)S.c_str(), S.length());
	ret.assign((char*)dst);
	delete [] dst;
	return ret;
}

string cypher::base64_decode(string S)
{
	string ret = "";
	size_t written_bytes;
	unsigned char *dst = NULL;
	size_t dstlen = 0;
	int res = mbedtls_base64_decode(dst, dstlen, &written_bytes, (unsigned char*)S.c_str(), S.length());
	if (res == MBEDTLS_ERR_BASE64_INVALID_CHARACTER) return ""; // an error occured, don't go further

	dstlen = written_bytes;
	dst = new unsigned char[dstlen + 1];
	bzero(dst, dstlen + 1);
	mbedtls_base64_decode(dst, dstlen, &written_bytes, (unsigned char*)S.c_str(), S.length());
	ret.assign((char*)dst, dstlen);
	delete [] dst;
	return ret;
}
/*
bool cypher::sign_file(string filename, string signame)
{
	if (!is_private_key) return false;
	unsigned char hash[20];

	if (sha1_file(filename.c_str(), hash)) return false; // error when reading

	char signature[msiglen];
	bzero(signature, sizeof(signature));
	int ret = rsa_pkcs1_sign(&private_key, NULL, NULL, RSA_PRIVATE, SIG_RSA_SHA1, sizeof(hash), hash, (unsigned char*) signature); // sign sha
	if (ret) return false;

	// write signature
	string outname;
	if (signame == "") outname = filename + ".sig";
	else outname = signame;

	FILE *f;
	if (!(f = fopen(outname.c_str(), "wb+")))	return false;

	for (int i = 0; i < msiglen; i++)
		fprintf(f, "%c", signature[i]);

	fclose(f);
	return true;
}

bool cypher::verif_file(string filename, string signame)
{
	if (!is_public_key) return false;
	if (signame == "") signame = filename + ".sig";

	unsigned char hash[20];
	if (sha1_file(filename.c_str(), hash)) return false; // error when reading
	FILE *f;
	if (!(f = fopen(signame.c_str(), "rb"))) return false;

	char signature[msiglen];
	bzero(signature, sizeof(signature));
	for (int i = 0; i < msiglen; i++)
		signature[i] = fgetc(f);
	fclose(f);

	if (rsa_pkcs1_verify(&public_key, RSA_PUBLIC, SIG_RSA_SHA1, sizeof(hash), hash, (unsigned char*) signature))
		return false;
	return true;
}
*/
} // namepace
