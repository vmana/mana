#ifndef imap_H
#define imap_H

#include "common.h"
#include "cypher.h"

#ifndef NO_MANA_IMAP
#include <curl/curl.h>
#endif // NO_MANA_IMAP

using namespace mana;

class imap
{
	protected:
		static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
		void prepare_curl(CURL *curl, string url);

	public:
		vector<string> error;

		bool verbose = false;
		string server;
		string user;
		string pass;
		string path;
		bool ssl = false;

		imap();
		vector<string> list();
		vector<int> search(string S);
		string fetch(int uid);
		string subject(int uid);
		string from(int uid);
		string text(int uid);
		// if delay is set, do not close upon completion
		bool move(string src, string dst, int uid, bool delay = false);
		bool close();
		static string utf7_decode(string S);

		vector<string> header_fields(int uid, vector<string> fields);
};

#endif // imap_H
