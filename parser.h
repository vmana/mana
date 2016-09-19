#ifndef LINEPARSER_H
#define LINEPARSER_H

#include "common.h"

namespace mana
{

class parser
{
	private :
		vector<string> opt_value;
		vector<string> opt_name;
		vector<string> args;
		
	public :
		parser();
		parser(string);
		bool parse(string S);
		string option(string);
		bool option(string, string*);
		bool option(int, string*, string*);
		bool is(string);
		string operator[](int i);
		int count();
		int size();
		int opt_count();
		bool mask(string str); // +a +b -c
		string raw;
		string parse_error;
		~parser();
};

} // namespace

#endif // LINEPARSER_H
