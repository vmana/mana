#ifndef MANACRYPT_H
#define MANACRYPT_H

#include "common.h"
#include "polarssl/mbedtls/config.h"
#include "polarssl/mbedtls/base64.h"
#include "polarssl/mbedtls/rsa.h"
#include "polarssl/mbedtls/sha1.h"

#include "polarssl/mbedtls/aes.h"

namespace mana
{

/* #define rsa_private_key_file	"private.txt" */
/* #define rsa_public_key_file	"public.txt" */

class cypher
{
	private :
		/* static mbedtls_rsa_context private_key; */
		/* static bool is_private_key; */
		/* static mbedtls_rsa_context public_key; */
		/* static bool is_public_key; */

	public :

		cypher();
		static void init();
		static string sha_file(string filename);
		static string sha_string(string str);
		static bool sha_check(string filename, string sha);
		static string base64_encode(string S, int length = -1);
		static string base64_decode(string S);

		// all AES operations are done in base64
		// this way we don't deal with char* but with string
		static string generate_aes_key(); // base64 key

		// iv : base64 16 bytes
		// key : base64 AES key
		// size : multiple of 16 (16 bytes)
		// on error : return an empty string
		// on success : return base64 encoded data
		static string aes256_encode(string data, string key, string iv, int size = -1);

		// data : base64 encoded data
		static string aes256_decode(string data, string key, string iv, int size = -1);

		/*
		static bool sign_file(string filename, string signame = "");
		static bool verif_file(string filename, string signame = "");
		*/
};

} // namespace

#endif // MANACRYPT_H
