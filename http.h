#ifndef MANAHTTP_H
#define MANAHTTP_H

#include "common.h"

#ifndef NO_MANA_CURL_HTTP
#include <curl/curl.h>
#else
#include "libcurl.arm/curl.h"
#ifndef NO_MANA_HTTP
#endif // NO_MANA_HTTP
#endif // NO_MANA_CURL_HTTP

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
		static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
		static size_t write_datafile(void *ptr, size_t size, size_t nmemb, void *stream);
		void prepare_curl(CURL *curl, string url);
		void update_cookie_value(CURL *curl);
	
	public :
		int timeout;
		string fieldname_upload; // default : file
		string useragent;
		bool follow_location = false;
		string post_data = ""; // perform a POST if non-empty, a=1&b=2&c=3...
		string force_cookie = ""; // a=1; b=2; c=3; ...
		string jar = ""; // cookie jar file name
		vector<http_cookie> cookie;
		
		vector<string> error;

		http();
		string get(string url);
		bool download(string url, string file = "");
		bool upload(string url, string file, string filename = "");
		string search_cookie(string key);
		void clean();
		~http();
		
};

} // namespace



#endif // MANAHTTP_H
