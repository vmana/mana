#ifndef COMMON_H
#define COMMON_H
#include <errno.h>
#include <cctype>
#include <time.h>
#include <chrono>
#include "lib.h"
#include <algorithm>
#ifndef NO_REGEX
#ifdef BOOST_REGEX
#include <boost/regex.hpp>
#else
#include <regex>
#endif
#endif

#define default_line_size 4096
#define debug_line(s) cout << "*** [" << system::date() << "] " << __FILE__ << " function " << __PRETTY_FUNCTION__ << " line " << __LINE__ << " : " << s << endl;
#define cpp_line(s) string("file ") + __FILE__ + string(" @ ") + __PRETTY_FUNCTION__ + string(" line ") + mana::convert::int_string(__LINE__) + string(" : ") + string(s)

namespace mana
{

	class file
	{
		private :
		public :
			file();
			file(string path);

			string path;
			FILE *handle;
			unsigned int line_size = default_line_size;
			bool open(string mode = "rb");
			bool close();

			bool isdir();
			bool exists();
			int size();
			bool copy(string dst);
			bool move(string dst);
			string filename();
			string pathname();
			string last_modification(string format);
			string add_trailing_slash();
			string system_convert();
			string basename();
			string extension();
			bool remove();
			int read(char *buffer, int size);
			int readline(string *s = NULL);
			bool read_content(string *content);
			string read_content();
			bool writeline(string content);
			bool write_content(string &content);
			bool append(string content);
			vector<string> read_vector();
			bool read_vector(vector<string> *V);
			bool write_vector(vector<string> *V);

			bool create_symlink(string dst);
			string read_symlink();

			static bool isdir(string filename);
			static bool exists(string filename);
			static int size(string filename);
			static bool copy(string src, string dst);
			static bool move(string src, string dst);
			static string filename(string path);
			static string pathname(string path);
			static string last_modification(string path, string format);
			static string add_trailing_slash(string path);
			static string system_convert(string path);
			static string basename(string path);
			static string extension(string path);
			static bool remove(string path);
			static int readline(FILE *f, string *s = NULL, int line_size = default_line_size);
			static bool read_content(string filename, string *content);
			static string read_content(string filename);
			static bool writeline(FILE *f, string content);
			static bool write_content(string filename, string &content);
			static bool append(string filename, string content);
			static vector<string> read_vector(string filename);
			static bool read_vector(string filename, vector<string> *V);
			static bool write_vector(string filename, vector<string> *V);

			bool create_symlink(string src, string dst);
			string read_symlink(string path);

	};


	class system
	{
		public :
			system();
			system(string path);

			string path;
			bool enable_log = true;

			bool log(string data);
			bool log(vector<string> data);

			static void undefunct();
			static vector<string> ls(string path, bool absolute_path = false);
			static vector<string> rls(string path); // recursive
			static string shellexec(string cmd, bool wait = true); // wait : get output, else simple fork and ret = ""
			static void killprocess(string name);
			static string pwd();
			static void chdir(string path);
			static void mkdir(string path, int mode = S_IRWXU | S_IRWXG | S_IRWXO);
			static void move(string path, string new_path);
			static bool daemonlock(string lockname = "daemon.lock");
			static string date(string format = "%Y-%m-%d %X");
			static string date(string format, time_t ts);
			static unsigned long timestamp_milliseconds();
			static int timestamp();
			static bool log(string filename, string data);
			static bool log(string filename, vector<string> data);
			static void sleep(int mseconds);
			static void init_system();
	};

	class convert
	{
		public :
			convert();
			static string int_string(int value);
			static string ulong_string(unsigned long value);
			static int string_int(const string &value);
			static float string_float(const string &value);
			static string float_string(float value);
			static double string_double(const string &value);
			static string double_string(double value);
			static string encoding(const string &value, const string &src_encode, const string &dst_encode);
			static string iso88591_utf8(const string &str);
			static string utf8_iso88591(const string &str);
			static string hex_char(const char &value);
			static string hex_string(const string &str, const string &prefix = "");
	};

#ifndef NO_REGEX

	class regexp
	{
		public :
			string S;

			regexp();
			regexp(string str);
			bool match(string reg);
			static bool match(string str, string reg);

			// search example : str="a b c" reg="a (.*) .*" format="found:$1"
			// output : found:b
			string search(string reg, string format);
			static string search(string str, string reg, string format);
	};

#endif

	string trim(char *str);
	string trim(string str);
	string trim_space(string str);
	string strlower(string str);
	string strupper(string str);
	string inc(string &str); // increment
	int strpos(string str, string needle, int pos = 0);
	string substr(string str, int pos = 0, int n = 0);
	vector<string> explode(string separator, string str, bool keep_empty = false);
	string str_replace(const string &search, const string &replace, const string &S);
	int common(const string &A, const string &B);
	string randstring(int n = 10);
	string rand_int_string(int n = 4);

	#ifdef _WIN32
	void mkdir(string path, int mode = 0);
	void chdir(char* path);
	#endif

// Declarations

template<class T>
void print_r(vector<T> &V, string *res = NULL);
template<class T>
void print_r(vector<vector<T> > &V, string *res = NULL);
template<class T>
vector<T> unique(const vector<T> &V);

// Definitions

template<class T>
vector<T> unique(const vector<T> &V)
{
	vector<T> ret = V;
	std::sort(ret.begin(), ret.end());
	auto end = std::unique(ret.begin(), ret.end());
	ret.erase(end, ret.end());
	return ret;
}

template<class T>
vector<T> concat(vector<T> &L, vector<T> &L2)
{
	vector<T> ret = L;
	ret.insert(ret.end(), L2.begin(), L2.end());
	return ret;
}

template<class T>
vector<T> fusion(vector<T> &L, vector<T> &L2)
{
	vector<T> ret = concat(L, L2);
	std::sort(ret.begin(), ret.end());
	ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
	return ret;
}

template<class T>
vector<T> remove(vector<T> &L, T value)
{
	vector<T> ret;
	for (int i = 0; i < L.size(); ++i)
		if (L[i] != value) ret.push_back(L[i]);
	return ret;
}

template<class T>
void print_r(vector<T> &V, string *res)
{
	if (V.size() == 0) return;
	if (res) *res += "{ ";
	else cout << "{ ";

	for (int i = 0; i < V.size(); ++i)
	{
		if (res == NULL)
		{
			cout << V[i] ;
			if (i == V.size() - 1) cout << " }" << endl;
			else cout << ", ";
		}
		else
		{
			*res += V[i];
			if (i == V.size() - 1) *res += " }\n";
			else *res +=  ", ";
		}
	}
}

template<class T>
void print_r(vector<vector<T> > &V, string *res)
{
	for (int i = 0; i < V.size(); ++i)
	{
		if (res == NULL) cout << "[" << convert::int_string(i) << "] ";
		else *res += "[" + convert::int_string(i) + "] ";
		print_r(V[i], res);
	}
}

/*
template<class T>
T vectorRand(vector<T> &L)
{
	T ret;
	if (L.size() <= 0) return ret; // error
	srand((unsigned int) (time(NULL) + getpid() + pthread_self()));
	int value = rand() % L.size();
	ret = L[value];
	return ret;
}



template<class T>
vector<T> vectorRemove(vector<T> &L, vector<T> &L2)
{
	vector<T> ret;
	for (int i = 0; i < L.size(); i++)
	{
		bool toadd = true;
		for (int i2 = 0; i2 < L2.size(); i2++)
		{
			if (L[i] == L2[i2])
			{
				toadd = false;
				break;
			}
		}
		if (toadd) ret.push_back(L[i]);
	}
	return ret;
}

*/
} // namespace

extern "C"
{
int load_so(int i);
}
#endif // COMMON_H
