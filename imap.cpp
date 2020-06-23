#include "imap.h"

#define mana_error_imap				"mana.error : unable to perform an imap"

imap::imap()
{
}

void imap::prepare_curl(CURL *curl, string &url)
{
	curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, pass.c_str());
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, imap::write_data);
}

vector<string> imap::list()
{
	vector<string> ret;
	#ifndef NO_MANA_IMAP
	string res = "";
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{

		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path;
		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " list (" + convert::int_string(ret_code) + ")");
			return ret;
		}
		ret = explode("\n", res);
		// remove left part berfore "/"
		for (int i = 0; i < ret.size(); ++i)
		{
			auto xplode = explode("\"/\" ", ret[i]);
			if (xplode.size() != 2) continue; // parse error
			ret[i] = xplode[1];
		}
	}
	#endif // NO_MANA_IMAP
	return ret;
}

vector<int> imap::search(string S)
{
	vector<int> ret;
	#ifndef NO_MANA_IMAP
	string res = "";
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path;

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, ("SEARCH " + S).c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " search '" + S + "' (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		res = trim(res); // remove any \n \r
		if (substr(res, 0, 8) == "* SEARCH")
		{
			res = substr(res, 8);
		}
		else
		{
			error.push_back((string)mana_error_imap + " search : not starting with '* SEARCH'");
			return ret;
		}

		res = trim_space(res);

		auto xplode = explode(" ", res);
		// convert string to int
		for (int i = 0; i < xplode.size(); ++i)
			ret.push_back(convert::string_int(xplode[i]));

	}
	#endif // NO_MANA_IMAP
	return ret;
}
string imap::fetch(int uid)
{
	string ret;
	#ifndef NO_MANA_IMAP
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path + "/;UID=" + convert::int_string(uid);

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " fetch (" + convert::int_string(ret_code) + ")");
			return ret;
		}
	}
	#endif // NO_MANA_IMAP
	return ret;
}

string imap::subject(int uid)
{
	string ret = "";
	#ifndef NO_MANA_IMAP
	string res = "";
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path + "/;UID=" + convert::int_string(uid) + ";SECTION=HEADER.FIELDS%20(SUBJECT)";

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " subject UID " + convert::int_string(uid) + " (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		// remove it
		if (strlower(substr(res, 0, 9)) == "subject: ") res = substr(res, 9);

		res = str_replace("\r", "", res);
		auto xplode = explode("\n", res);
		for (string line : xplode)
		{
			// search for utf-8 encoded string
			string utf8_data = regexp(line).search(" *=\\?[uU][tT][fF]-8\\?B\\?([^\\?]*)\\?=", "$1");
			if (utf8_data != "")
			{
				line = cypher::base64_decode(utf8_data);
			}

			ret += line;
		}

	}
	#endif // NO_MANA_IMAP
	return ret;
}

string imap::from(int uid)
{
	string ret = "";
	#ifndef NO_MANA_IMAP
	string res = "";
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path + "/;UID=" + convert::int_string(uid) + ";SECTION=HEADER.FIELDS%20(FROM)";

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " from UID " + convert::int_string(uid) + " (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		// remove it
		if (strlower(substr(ret, 0, 6)) == "from: ") ret = substr(ret, 6);

		ret = trim(ret);

	}
	#endif // NO_MANA_IMAP
	return ret;
}

string imap::text(int uid)
{
	string ret;
	#ifndef NO_MANA_IMAP
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path + "/;UID=" + convert::int_string(uid) + ";SECTION=TEXT";

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " text UID " + convert::int_string(uid) + " (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		// remove \r
		ret = str_replace("\r", "", ret);
	}
	#endif // NO_MANA_IMAP
	return ret;
}

vector<string> imap::header_fields(int uid, vector<string> fields)
{
	vector<string> ret;
	#ifndef NO_MANA_IMAP
	string res = "";
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path + "/;UID=" + convert::int_string(uid) + ";SECTION=HEADER.FIELDS%20(";
		// add each field + %20
		for (auto field : fields)
			url += field + "%20";

		// remove last %20 and add )
		url = substr(url, 0, -3) + ")";

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " header_fields (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		auto xplode = explode("\n", res);
		// search for fields and reorder them
		for (auto field : fields)
		{
			string search_field = strlower(field) + ": ";
			bool found = false;
			for (auto x : xplode)
			{
				if (strlower(substr(x, 0, search_field.length())) == search_field)
				{
					// found so add it
					found = true;
					ret.push_back(substr(x, search_field.length()));
					break;
				}
			}
			if (!found) error.push_back((string)mana_error_imap + " header_fields missing field : " + field);
		}
	}
	#endif // NO_MANA_IMAP
	return ret;
}

bool imap::move(string src, string dst, int uid, bool delay)
{
	bool ret = false;
	#ifndef NO_MANA_IMAP
	string res = "";
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(src, 0, 1) != "/") src = "/" + src;
		string url = "imap://" + server + src + "/;UID=" + convert::int_string(uid);
		prepare_curl(curl, url);

		string request = "COPY " + convert::int_string(uid) + " " + dst;
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		CURLcode ret_code = curl_easy_perform(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " move:copy UID " + convert::int_string(uid) + " (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		request = "STORE " + convert::int_string(uid) + " +FLAGS.SILENT (\\Deleted)";
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		ret_code = curl_easy_perform(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " move:store UID " + convert::int_string(uid) + " (" + convert::int_string(ret_code) + ")");
			return ret;
		}

		if (!delay)
		{
			request = "CLOSE";
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
			ret_code = curl_easy_perform(curl);

			if (ret_code)
			{
				error.push_back((string)mana_error_imap + " move:close UID " + convert::int_string(uid) + " (" + convert::int_string(ret_code) + ")");
				return ret;
			}
		}

		curl_easy_cleanup(curl);
		ret= true;
	}
	#endif // NO_MANA_IMAP
	return ret;
}

bool imap::close()
{
	bool ret = false;
	#ifndef NO_MANA_IMAP
	CURL *curl;
	curl = curl_easy_init();
	if (curl)
	{
		// correct path if it doesn't start with "/"
		if (substr(path, 0, 1) != "/") path = "/" + path;
		string url = "imap://" + server + path + "/";

		prepare_curl(curl, url);

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "CLOSE");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
		CURLcode ret_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (ret_code)
		{
			error.push_back((string)mana_error_imap + " close (" + convert::int_string(ret_code) + ")");
			return ret;
		}
		ret = true;
	}
	#endif // NO_MANA_IMAP
	return ret;
}

size_t imap::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int ret = size * nmemb;
	((string*)stream)->append((char*)ptr, ret);
	return ret;
}

imap::~imap()
{
}

