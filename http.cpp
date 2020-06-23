#include "http.h"

#define mana_useragent	"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:21.0) Gecko/20100101 Firefox/21.0"
// Errors
#define mana_error_http_get			"mana.error : unable to perform an url get"
#define mana_error_http_upload		"mana.error : unable to perform an url upload"
#define mana_error_file_open		"mana.error : unable to open file "

namespace mana
{

http_cookie::http_cookie()
{
}

http_cookie::~http_cookie()
{
}

http::http()
{
	fieldname_upload = "file";
	useragent = mana_useragent;
}

string http::get(string url)
{
	string ret = "";
	#ifndef NO_MANA_HTTP

	if (curl) curl_easy_cleanup(curl);
	curl = curl_easy_init();
	if (curl)
	{
		prepare_curl(url);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http::write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);

		CURLcode res = curl_easy_perform(curl);
		update_cookie_value();

		// error gestion
		if (res)
		{
			error.push_back((string)mana_error_http_get + "(" + convert::int_string(res) + ")");
			ret = "";
		}
		else
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
		}

		curl_easy_cleanup(curl);
		curl = NULL;
	}
	#endif // NO_MANA_HTTP
	return ret;
}

bool http::download(string url, string file)
{
	bool ret = true;
	#ifndef NO_MANA_HTTP
	CURLcode res;

	// use real filename if empty
	if (file == "") file = mana::file::filename(file);

	FILE *f = fopen(file.c_str(), "wb+");
	if (!f)
	{
		error.push_back(string(mana_error_file_open + file));
		return false;
	}

	if (curl) curl_easy_cleanup(curl);
	curl = curl_easy_init();
	if (curl)
	{
		prepare_curl(url);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http::write_datafile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

		res = curl_easy_perform(curl);
		update_cookie_value();

		if (res)
		{
			error.push_back((string)mana_error_http_get + "(" + convert::int_string(res) + ")");
			ret = false;
		}
		else
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
		}

		curl_easy_cleanup(curl);
		curl = NULL;
	}
	fclose(f);
	#endif // NO_MANA_HTTP
	return ret;
}

bool http::upload(string url, string file, string filename)
{
	bool ret = true;
	#ifndef NO_MANA_HTTP
	CURLcode res;

	if (curl) curl_easy_cleanup(curl);
	curl = curl_easy_init();
	if (curl)
	{
		// use real filename if empty
		if (filename == "") filename = mana::file::filename(file);
		string writed = "";

		prepare_curl(url);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http::write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writed);

		struct curl_httppost *post = NULL;
		struct curl_httppost *last = NULL;
		curl_formadd
		(
			&post,
			&last,
			CURLFORM_COPYNAME, (char*)fieldname_upload.c_str(),
			CURLFORM_FILE, (char*)file.c_str(),
			CURLFORM_FILENAME, (char*)filename.c_str(),
			CURLFORM_END
		);

		if (post_data != "")
		{
			vector<string> post_explode = explode("&", post_data);
			for (int i = 0; i< post_explode.size(); ++i)
			{
				string post_arg = post_explode[i];
				int eq = strpos(post_arg, "=");
				if (eq == string::npos)
				{
					curl_formadd(&post, &last, CURLFORM_COPYNAME, post_arg.c_str(), CURLFORM_COPYCONTENTS, "",CURLFORM_END);
				}
				else
				{
					string post_name = substr(post_arg, 0, eq);
					string post_value = substr(post_arg, eq + 1);
					curl_formadd(&post, &last, CURLFORM_COPYNAME, post_name.c_str(), CURLFORM_COPYCONTENTS, post_value.c_str(),CURLFORM_END);
				}
			}
		}

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

		res = curl_easy_perform(curl);
		update_cookie_value();
		curl_formfree(post);

		if (res)
		{
			error.push_back((string)mana_error_http_upload + "(" + convert::int_string(res) + ")");
			ret = false;
		}
		else
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
		}

		curl_easy_cleanup(curl);
		curl = NULL;
	}
	#endif // NO_MANA_HTTP
	return ret;
}

string http::search_cookie(string key)
{
	for (int i = 0; i < cookie.size(); ++i)
	{
		if (cookie[i].key == key) return cookie[i].data;
	}
	return "";
}

void http::clean()
{
	if (jar == "") return;
	if (file::exists(jar)) file::remove(jar);
}

size_t http::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int ret = size * nmemb;
	((string*)stream)->append((char*)ptr, ret);
	return ret;
}

size_t http::write_datafile(void *ptr, size_t size, size_t nmemb, void *stream)
{
	fwrite(ptr, size, nmemb, (FILE*)stream);
	return nmemb;
}

void http::prepare_curl(string url)
{
	#ifndef NO_MANA_HTTP
	curl_easy_setopt(curl, CURLOPT_URL, (char*)url.c_str());
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, request_timeout);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); // multi-threading issues
	curl_easy_setopt(curl, CURLOPT_USERAGENT, mana_useragent);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	if (follow_location) curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
	if (post_data != "") curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
	if (user_pass != "") curl_easy_setopt(curl, CURLOPT_USERPWD, user_pass.c_str());

	if (jar != "")
	{
		curl_easy_setopt(curl,  CURLOPT_COOKIEJAR, jar.c_str());
		curl_easy_setopt(curl,  CURLOPT_COOKIEFILE, jar.c_str());
	}
	else curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // enable cookie engine
	if (force_cookie != "") curl_easy_setopt(curl, CURLOPT_COOKIE, force_cookie.c_str());
	if (allow_gzip) curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
	#endif
}

void http::update_cookie_value()
{
	#ifndef NO_MANA_HTTP
	struct curl_slist *cookies;
	struct curl_slist *nc;
	curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);

	cookie.clear();
	nc = cookies;
	while (nc)
	{
		// parse data
		string data = string(nc->data);
		auto split = explode("\t", data);
		nc = nc->next;
		if (split.size() > 6)
		{
			http_cookie C;
			C.key = split[5];
			C.data = split[6];
			cookie.push_back(C);
		}
	}

	curl_slist_free_all(cookies);
	#endif
}

int http::get_status_code()
{
	return status_code;
}

http::~http()
{
	if (curl) curl_easy_cleanup(curl);
}

} // namespace


