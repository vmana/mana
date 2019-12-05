#ifndef MANAHTTP_H
#define MANAHTTP_H

#include "common.h"

#ifndef NO_MANA_HTTP
#ifdef MANA_CURL_ANDROID
#include "libcurl.arm/curl.h"
#else
#include <curl/curl.h>
#endif // MANA_CURL_ANDROID
#endif // NO_MANA_HTTP

namespace mana
{

class http_cookie
{
	public :
		string key;
		string data;

		http_cookie();
		~http_cookie();
};

class http
{
	protected :
		CURL *curl = NULL;
		static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
		static size_t write_datafile(void *ptr, size_t size, size_t nmemb, void *stream);
		void prepare_curl(string url);
		void update_cookie_value();
		int status_code = -1;

	public :
		int timeout = 10;
		int request_timeout = 120;
		string fieldname_upload; // default : file
		string useragent;
		bool follow_location = false;
		bool allow_gzip = true;
		string post_data = ""; // perform a POST if non-empty, a=1&b=2&c=3...
		string force_cookie = ""; // a=1; b=2; c=3; ...
		string jar = ""; // cookie jar file name
		string user_pass = ""; // user:pass
		vector<http_cookie> cookie;

		vector<string> error;

		http();
		string get(string url);
		bool download(string url, string file = "");
		bool upload(string url, string file, string filename = "");
		string search_cookie(string key);
		int get_status_code();
		void clean();
		~http();

};

} // namespace



#endif // MANAHTTP_H
