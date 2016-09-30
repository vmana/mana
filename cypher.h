#ifndef MANACRYPT_H
#define MANACRYPT_H

#include "common.h"
#include "polarssl/mbedtls/config.h"
#include "polarssl/mbedtls/base64.h"
#include "polarssl/mbedtls/rsa.h"
#include "polarssl/mbedtls/sha1.h"

namespace mana
{

#define rsa_private_key_file	"private.txt"
#define rsa_public_key_file		"public.txt"

class cypher
{
	private :
		static mbedtls_rsa_context private_key;
		static bool is_private_key;
		static mbedtls_rsa_context public_key;
		static bool is_public_key;
	
	public :
		
		cypher();
		static void init();
		static string sha_file(string filename);
		static string sha_string(string str);
		static bool sha_check(string filename, string sha);
		static string base64_encode(string S);
		static string base64_decode(string S);
		/*
		static bool sign_file(string filename, string signame = "");
		static bool verif_file(string filename, string signame = "");
		*/
};

} // namespace

#endif // MANACRYPT_H
