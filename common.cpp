#include "common.h"
#include <iconv.h>
#include <iomanip>
#include <boost/locale.hpp>

namespace mana
{

string trim(char *str)
{
	return trim(string(str));
}

string trim(string str)
{
	string ret = str;
	while (ret.length() > 0)
	{
		int size = ret.length() - 1;
		if (ret[size] == '\r' || ret[size] == '\n') ret.resize(ret.size() - 1);
		else break;
	}
	while (ret.length() > 0)
	{
		if (ret[0] == '\r' || ret[0] == '\n') ret = ret.substr(1);
		else break;
	}
	return ret;
}

string trim_space(string str)
{
	string ret = str;
	while (ret.length() > 0)
	{
		int size = ret.length() - 1;
		if (ret[size] == ' ' || ret[size] == '\t') ret.resize(ret.size() - 1);
		else break;
	}
	while (ret.length() > 0)
	{
		if (ret[0] == ' ' || ret[0] == '\t') ret = ret.substr(1);
		else break;
	}
	return ret;
}

string strlower(string str)
{
	for (int i = 0; i < str.size(); ++i)
		str[i] = tolower(str[i]);
	return str;
}

string strupper(string str)
{
	for (int i = 0; i < str.size(); ++i)
		str[i] = toupper(str[i]);
	return str;
}

string inc(string &str)
{
	stringstream ss;
	ss << (int)(atoi(str.c_str()) + 1);
	ss >> str;
	return str;
}

int strpos(string str, string needle, int pos)
{
	return str.find(needle, pos);
}

string substr(string str, int pos, int n)
{
	string ret = "";
	// negative pos correction
	if (pos < 0) pos = str.length() + pos;
	if (pos < 0) pos = 0;

	if (n < 0) n = str.length() + n - pos;
	if (n < 0) n = 0;

	if (pos > str.length()) return ret; // out of range
	ret = (n) ? str.substr(pos, n) : str.substr(pos);
	return ret;
}

vector<string> explode(string separator, string str, bool keep_empty)
{
	vector<string> ret;
	string::size_type found;

	int separator_len = strlen(separator.c_str());
	if (separator_len == 0) return ret;

	found = str.find(separator);
	while (found != string::npos)
	{
		if (!(!keep_empty && found == 0)) // don't add if (!keep_empty && found is start of string)
		{
			ret.push_back(str.substr(0, found));
		}
		str = str.substr(found + separator_len);
		found = str.find(separator);
	}

	if (keep_empty || str.length() != 0) ret.push_back(str);

	return ret;
}

string str_replace(const string &search, const string &replace, const string &S)
{
	string ret;
	int nsearch = search.length();
	int nS = S.length();

	if (nsearch == 0) return S;

	for (int i = 0, j = 0; i < nS; j = 0)
	{
		while (i + j < nS && j < nsearch && S[i+j] == search[j])
			j++;
		if (j == nsearch) //found 'search'
		{
			ret.append(replace);
			i += nsearch;
		}
		else
		{
			ret.append(&S[i++], 1);
		}
	}

	return ret;
}

int common(const string &A, const string &B)
{
	int min = A.length();
	if (B.length() < min) min = B.length();
	for (int i = 0; i < min; ++i)
	{
		if (A[i] != B[i]) return i;
	}
	return 0;
}

string randstring(int n)
{
	string ret = "";
	if (n <= 0) return ret;
	#ifndef _WIN32
		FILE *f = fopen("/dev/urandom", "rb");
		if (!f)
		{
			srand((unsigned int) (time(NULL) + getpid()));
			for (int i = 0; i < n; i++)
				ret += (char)('a' + rand() % 26);
			return ret;
		}
		unsigned char *buf = new unsigned char[n+1];
		bzero(buf, n+1);
		fread(buf, 1, n, f);
		for (int i = 0; i < n; i++)
			ret += (char)('a' + (buf[i] % 26));
		delete [] buf;
		fclose(f);
	#else
	srand((unsigned int) (time(NULL) + getpid()));
	for (int i = 0; i < n; i++)
		ret += (char)('a' + rand() % 26);
	#endif
	return ret;
}

string rand_int_string(int n)
{
	string ret = "";
	if (n <= 0) return ret;
	#ifndef _WIN32
		FILE *f = fopen("/dev/urandom", "rb");
		if (!f)
		{
			srand((unsigned int) (time(NULL) + getpid()));
			for (int i = 0; i < n; i++)
				ret += (char)('0' + rand() % 10);
			return ret;
		}
		unsigned char *buf = new unsigned char[n+1];
		bzero(buf, n+1);
		fread(buf, 1, n, f);
		for (int i = 0; i < n; i++)
			ret += (char)('0' + (buf[i] % 10));
		delete [] buf;
		fclose(f);
	#else
	srand((unsigned int) (time(NULL) + getpid()));
	for (int i = 0; i < n; i++)
		ret += (char)('0' + rand() % 10);
	#endif
	return ret;
}

/**********		file		**********/

file::file()
{
	handle = NULL;
	path = "";
}

file::file(string path)
{
	file();
	this->path = path;
}

bool file::open(string mode)
{
	bool ret = true;
	handle = fopen(path.c_str(), mode.c_str());
	if (!handle) ret = false;
	return ret;
}

bool file::close()
{
	bool ret = false;
	if (!handle) return ret;
	if (!fclose(handle)) ret = true;
	return ret;
}

bool file::isdir() { return isdir(path); }
bool file::exists() { return exists(path); }
int file::size() { return size(path); }
bool file::copy(string dst) { return copy (path, dst); }
bool file::move(string dst) { return move (path, dst); }
string file::filename() { return filename(path); }
string file::pathname() { return pathname(path); }
string file::last_modification(string format) { return last_modification(path, format); }
string file::add_trailing_slash() { return add_trailing_slash(path); }
string file::basename() { return basename(path); }
string file::extension() { return extension(path); }
bool file::remove() { return remove(path); }
int file::readline(string *s) { return readline(handle, s, line_size); }
bool file::read_content(string *content) { return read_content(path, content); }
string file::read_content() { return read_content(path); }
bool file::writeline(string content)  { return writeline(handle, content); }
bool file::write_content(string &content)  { return write_content(path, content); }
bool file::append(string content) { return append(path, content); }
vector<string> file::read_vector() { return read_vector(path); }
bool file::read_vector(vector<string> *V) { return read_vector(path, V); }
bool file::write_vector(vector<string> *V) { return write_vector(path, V); }

bool file::create_symlink(string dst) { return create_symlink(path, dst); }
string file::read_symlink() { return read_symlink(path); }

bool file::isdir(string filename)
{
	bool ret = false;
	if (!exists(filename)) return false;
	struct stat buf;
	if (! stat(string(filename).c_str(), &buf)) ret = (((buf.st_mode & S_IFDIR) == S_IFDIR) == 1);
	return ret;
}

bool file::exists(string filename)
{
	FILE *tmphandle = fopen(filename.c_str(), "rb");
	if (tmphandle)
	{
		fclose(tmphandle);
		return true;
	}
	return false;
}

int file::size(string filename)
{
	if (!exists(filename)) return 0;
	struct stat st;
	stat(filename.c_str(), &st);
	return st.st_size;
}

bool file::copy(string src, string dst)
{
	FILE *f = fopen(src.c_str(), "rb");
	if (!f) return false;
	FILE *f2 = fopen(dst.c_str(), "wb+");
	if (!f2) { fclose(f); return false; }
	int nread = 0;
	char buf[256];
	while ((nread = fread(buf, 1, 256, f)))
		fwrite(buf, 1, nread, f2);
	fclose(f);
	fclose(f2);
	return true;
}

bool file::move(string src, string dst)
{
	bool ret = copy(src, dst);
	if (ret) ret = remove(src);
	return ret;
}

string file::filename(string path)
{
	string ret = "";
	if (path.length() == 0) return ret;
	int pos = path.length() - 1;

	while (pos-- > 0)
		if (path[pos] == '/' || path[pos] == '\\') break;

	ret = path.substr(pos+1);
	return ret;
}

string file::pathname(string path)
{
	string ret = "";
	if (path.length() == 0) return ret;
	int pos = path.length();

	while (pos-- > 0)
		if (path[pos] == '/' || path[pos] == '\\') break;

	ret = path.substr(0, pos + 1);
	return ret;
}

string file::last_modification(string path, string format)
{
	string ret = "";
	#ifndef _WIN32
	if (!exists(path)) return ret;
	struct stat st;
	stat(path.c_str(), &st);
	struct tm *tstruct;
	char buf[80];
	tstruct = localtime(&st.st_mtim.tv_sec);
	strftime(buf, sizeof(buf), format.c_str(), tstruct);
	return string(buf);
	#else
	// no implemented
	return ret;
	#endif
}

string file::add_trailing_slash(string path)
{
	string ret = path;
	#ifdef _WIN32
	if (ret == "") return "\\";
	if (ret[ret.length() - 1] != '\\') ret += '\\';
	#else
	if (ret == "") return "/";
	if (ret[ret.length() - 1] != '/') ret += '/';
	#endif
	return ret;
}

string file::system_convert(string path)
{
	string ret = path;
	#ifdef _WIN32
	for (int i = 0; i < path.length(); i++)
		if (ret[i] == '/') path[i] = '\\';
	#else
	for (uint i = 0; i < path.length(); i++)
		if (ret[i] == '\\') path[i] = '/';
	#endif
	return ret;
}

string file::basename(string path)
{
	if (path.length() < 4) return "";
	vector<string> exp = explode(".", path);
	if (exp.size() == 1) return path;
	string ret = "";

	for (int i = 0; i < exp.size() -1; ++i)
		ret += exp[i] + '.';

	ret = substr(ret, 0, -1);
	return ret;
}

string file::extension(string path)
{
	if (path.length() < 4) return "";
	vector<string> exp = explode(".", path);
	string ret = exp.back();
	return strlower(ret);
}

bool file::remove(string path)
{
	path = system_convert(path);
	#ifdef _WIN32
	return DeleteFileA(path.c_str());
	#else
	return !(::remove(path.c_str()));
	#endif
}

int file::read(char *buffer, int size)
{
	int ret = 0;
	ret = fread(buffer, 1, size, handle);
	return ret;
}

int file::readline(FILE *f, string *s, int line_size)
{
	if (s) *s = "";
	int size = line_size;
	char *buf = new char[size];
	bzero(buf, size);
	if (fgets(buf, size, f))
	{
		if (s) *s = trim(buf);
	}
	else
	{
		delete [] buf;
		return -1;
	}

	delete [] buf;
	if (s) return s->length();
	return 0;
}

bool file::read_content(string filename, string *content)
{
	bool ret = false;
	*content = "";
	FILE *f = fopen(filename.c_str(), "rb");
	if (!f) return ret;
	char buffer[1024];
	bzero(buffer, sizeof(buffer));
	int size;
	while ((size = fread(buffer, 1, sizeof(buffer), f)))
	{
		content->append(buffer, size);
		bzero(buffer, sizeof(buffer));
	}
	fclose(f);
	ret = true;
	return ret;
}

string file::read_content(string filename)
{
	string ret = "";
	if (!read_content(filename, &ret)) ret = "";
	return ret;
}

bool file::writeline(FILE *f, string content)
{
	bool ret = true;
	if (substr(content, -1) != "\n") content += "\n";
	if (!fputs(content.c_str(), f)) ret = false;
	return ret;
}

bool file::write_content(string filename, string &content)
{
	bool ret = false;
	FILE *f = fopen(filename.c_str(), "wb+");
	if (!f) return ret;
	uint size = fwrite(content.c_str(), 1, content.size(), f);
	fclose(f);
	ret = (size == content.size());
	return ret;
}

bool file::append(string filename, string content)
{
	bool ret = false;
	FILE *f = fopen(filename.c_str(), "a");
	if (!f) return ret;
	uint size = fwrite((char*)content.c_str(), 1, content.size(), f);
	fclose(f);
	ret = (size == content.size());
	return ret;
}

vector<string> file::read_vector(string filename)
{
	vector<string> ret;
	read_vector(filename, &ret);
	return ret;
}

bool file::read_vector(string filename, vector<string> *V)
{
	FILE *f = fopen(filename.c_str(), "r");
	if (!f) return false;
	V->clear();
	string line;
	while (readline(f, &line) >= 0)
	{
		V->push_back(line);
		if (V->size() == V->capacity()) V->reserve(V->capacity() + 100);
	}
	fclose(f);

	V->resize(V->size());
	return true;
}

bool file::write_vector(string filename, vector<string> *V)
{
	bool ret = true;
	FILE *f = fopen(filename.c_str(), "w+");
	if (!f) return false;
	for (int i = 0; i < V->size(); ++i)
	{
		ret = writeline(f, (*V)[i]) && ret;
	}
	fclose(f);
	return ret;
}

bool file::create_symlink(string src, string dst)
{
	#ifndef _WIN32
	return (symlink(src.c_str(), dst.c_str()) == 0);
	#endif
	return false;
}

string file::read_symlink(string path)
{
	string ret = "";
	#ifndef _WIN32
	char *buf = new char[1024];
	bzero(buf, 1024);
	if (readlink(path.c_str(), buf, 1023) >= 0) ret += buf;
	delete [] buf;
	#else
	ret = path;
	#endif
	return ret;
}

/********	System()	********/

system::system()
{
	path = "";
}

system::system(string path)
{
	this->path = path;
}

void system::undefunct()
{
	#ifndef _WIN32
	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sa, NULL);
	#endif
}

vector<string> system::ls(string path, bool absolute_path)
{
	vector<string> ret;
	DIR *dp;
	struct dirent *dirp;
	if (path.length() == 0) return ret;

	#ifdef WIN32
	if (substr(path, -1) != "\\") path += '\\';
	#else
	if (substr(path, -1) != "/") path += '/';
	#endif

	if (!(dp  = opendir(path.c_str()))) return ret;

	while ((dirp = readdir(dp)))
	{
		if (!strncmp(dirp->d_name, (char*)".", 1)) continue;
		ret.push_back(((absolute_path) ? path : "") + string(dirp->d_name));
	}
	closedir(dp);
	return ret;
}

vector<string> system::rls(string path)
{
	vector<string> ret, rootres;
	if (substr(path, -1) != "/") path += "/";
	rootres = ls(path, true);
	int size = rootres.size();
	for (int i = 0; i < size; ++i)
	{
		if (file::isdir(rootres[i]))
		{
			rootres[i] += "/";
			vector<string> res = rls(rootres[i]);
			if (res.size() > 0) ret = fusion(ret, res);
		}
	}
	ret = fusion(ret, rootres);
	return ret;
}

string system::shellexec(string cmd, bool wait)
{
	string ret = "";
	#ifdef _WIN32
	STARTUPINFOA sinfo;
	PROCESS_INFORMATION pinfo;
	bzero(&sinfo, sizeof(STARTUPINFOA));
	bzero(&pinfo, sizeof(PROCESS_INFORMATION));

	if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS | DETACHED_PROCESS,
	NULL, NULL, (LPSTARTUPINFOA)&sinfo, (LPPROCESS_INFORMATION)&pinfo))
	{
		CloseHandle(pinfo.hProcess);
		CloseHandle(pinfo.hThread);
	}
	#else

	if (!wait)
	{
		int pid = fork();
		if (pid == 0)
		{
			::system(cmd.c_str());
			exit(0);
		}
	}
	else // wait for the output
	{
		FILE *pipe = popen(cmd.c_str(), "r");
		if (!pipe) return "mana::system::shellexec::error";

		char buffer[128];
		while (!feof(pipe))
		{
			if(fgets(buffer, 128, pipe) != NULL)
			ret += buffer;
		}
		pclose(pipe);
	}
	#endif
	return ret;
}

void system::killprocess(string name)
{
	#ifdef _WIN32
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof (pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp((char*)pEntry.szExeFile, name.c_str()) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
			(DWORD) pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	#else
	shellexec("killall " + name + " 1>/dev/null 2>/dev/null");
	#endif
}

string system::pwd()
{
	string ret = "";
	#ifndef _WIN32
		char path[MAXPATHLEN];
		getcwd(path, MAXPATHLEN);
		ret = path;
		ret += '/';
	#else
		int pathsize = GetCurrentDirectory(0, NULL);
		char *path = new char[pathsize + 1];
		GetCurrentDirectoryA(pathsize, path);
		ret = path;
		delete [] path;
		ret += '\\';
	#endif
		return ret;
}

void system::chdir(string path)
{
	#ifdef _WIN32
	SetCurrentDirectoryA(path.c_str());
	#else
	::chdir(path.c_str());
#endif
}

void system::mkdir(string path, int mode)
{
	#ifdef _WIN32
	CreateDirectoryA(path.c_str(), NULL);
	#else
	::mkdir(path.c_str(), mode);
	#endif
}

void system::move(string path, string new_path)
{
	#ifdef _WIN32
	#else
	::rename(path.c_str(), new_path.c_str());
	#endif
}

bool system::daemonlock(string lockname)
{
	#ifndef _WIN32
	int fdlock;
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	if ((fdlock = open(lockname.c_str(), O_WRONLY|O_CREAT, 0666)) == -1) return false;
	if (fcntl(fdlock, F_SETLK, &fl) == -1) return false;
	#else
	if (WaitForSingleObject(CreateMutexA(NULL, FALSE, lockname.c_str()), 10) == WAIT_TIMEOUT) return false;
	#endif
	return true;
}

string system::date(string format)
{
	return date(format, time(NULL));
}

string system::date(string format, time_t ts)
{
	time_t now = ts;
	struct tm *tstruct;
	char buf[80];
	tstruct = localtime(&now);
	strftime(buf, sizeof(buf), format.c_str(), tstruct);
	return string(buf);
}

unsigned long system::timestamp_milliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int system::timestamp()
{
	return time(NULL);
}

bool system::log(string data)
{
	if (!enable_log) return true;
	return log(path, data);
}

bool system::log(vector<string> data)
{
	if (!enable_log) return true;
	return log(path, data);
}

bool system::log(string filename, string data)
{
	bool ret = false;
	FILE *f = fopen(filename.c_str(), "a");
	if (!f) return ret;
	string content = date() + " : " + data;
	if (substr(data, -1) != "\n") content += '\n';
	uint size = fwrite(content.c_str(), 1, content.size(), f);
	fclose(f);
	ret = (size == content.size());
	return ret;
}

bool system::log(string filename, vector<string> data)
{
	bool ret = true;
	for (int i = 0; i < data.size(); ++i)
	{
		ret = ret && log(filename, data[i]);
	}
	return ret;
}

void system::sleep(int mseconds)
{
	#ifdef _WIN32
	Sleep(mseconds);
	#else
	struct timespec tsp; tsp.tv_sec = (mseconds/1000); tsp.tv_nsec = (mseconds%1000)*1000000; nanosleep(&tsp, &tsp);
	#endif
}

void system::init_system()
{
	#if defined(_WIN32)
	static WSADATA wsaData;
	int wsaerr = WSAStartup(MAKEWORD(1, 0), &wsaData);
	atexit((void(*)())WSACleanup);
	#endif
}

/********	Convert()	********/

convert::convert()
{
}

string convert::int_string(int value)
{
	string ret;
	stringstream ss;
	ss << value;
	ss >> ret;
	return ret;
}
string convert::ulong_string(unsigned long value)
{
	string ret;
	stringstream ss;
	ss << value;
	ss >> ret;
	return ret;
}

int convert::string_int(const string &value)
{
	return atoi(value.c_str());
}

float convert::string_float(const string &value)
{
	float ret = 0;
	try
	{
		string convert = str_replace(",", ".", value);
		ret = std::stof(convert);
	} catch (const std::invalid_argument& ia) { }
	return ret;
}

string convert::float_string(float value)
{
	string ret;
	stringstream ss;
	ss << value;
	ss >> ret;
	return ret;
}

double convert::string_double(const string &value)
{
	double ret = 0;
	try
	{
		string convert = str_replace(",", ".", value);
		ret = std::stod(convert);
	} catch (const std::invalid_argument& ia) { }
	return ret;
}

string convert::double_string(double value)
{
	string ret;
	stringstream ss;
	ss << value;
	ss >> ret;
	return ret;
}

string convert::encoding(const string &value, const string &src_encode, const string &dst_encode)
{
	string ret;
	iconv_t cd = iconv_open(src_encode.c_str(), dst_encode.c_str());
	if (cd == (iconv_t)-1) return ret;
	char *in_ptr = (char*)value.c_str();
	size_t in_left = value.length();

	size_t out_left = 2 * in_left;
	char *output = new char[out_left + 1];
	bzero(output, out_left + 1);
	char *out_ptr = output;

	if (iconv(cd, &in_ptr, &in_left, &out_ptr, &out_left) != (size_t)-1)
	{
		ret = string(output);
	}

	delete [] output;
	iconv_close(cd);
	return ret;
}

string convert::iso88591_utf8(const string &str)
{
#ifdef BOOST_LOCALE
	return boost::locale::conv::to_utf<char>(str,"Latin1");
#else
	return str;
#endif
}

string convert::utf8_iso88591(const string &str)
{
#ifdef BOOST_LOCALE
	return boost::locale::conv::from_utf<char>(str,"Latin1");
#else
	return str;
#endif
}

string convert::hex_char(const char &value)
{
	string ret;
	stringstream ss;
	ss << hex << (unsigned int)value;
	ret = ss.str();
	return ret;
}

string convert::hex_string(const string &str, const string &prefix)
{
	string ret;
	stringstream ss;
	for (int i = 0; i < str.size(); i++)
	{
		ss << dec << prefix << hex << setw(2) << setfill('0') << (int)(unsigned char)str[i];
	}
	ret = ss.str();
	return ret;
}

/********	Regexp	********/

#ifndef NO_REGEX

regexp::regexp()
{
}

regexp::regexp(string str)
{
	S = str;
}

bool regexp::match(string reg)
{
	return match(S, reg);
}

bool regexp::match(string str, string reg)
{
	try
	{
#ifdef BOOST_REGEX
		return boost::regex_match(str, boost::regex(reg));
#else
		return std::regex_match(str, std::regex(reg));
#endif
	}
	catch (...) { cout << "regexp::match exception : " << str << "::" << reg << endl; }
	return false;
}

string regexp::search(string reg, string format)
{
	return search(S, reg, format);
}

string regexp::search(string str, string reg, string format)
{
	string ret = "";
	try
	{
		if (!match(str, reg)) return ret;
#ifdef BOOST_REGEX
		ret = boost::regex_replace(str, boost::regex(reg), format);
#else
		ret = std::regex_replace(str, std::regex(reg), format);
#endif
	}
	catch (...) { cout << "regexp::search exception : " << str << "::" << reg << "::" << format << endl; }
	return ret;
}

#endif



} // namespace

int load_so(int i)
{
	return i+1;
}




