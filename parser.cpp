#include "parser.h"

namespace mana
{

parser::parser()
{
	raw = "";
	parse_error = "";
}

parser::parser(string S)
{
	parser();
	parse(S);
}

bool parser::parse(string S)
{
	bool ret = true;
	enum states { none, arg, opt, opt_plus, opt_minus, error, parsed };
	string option = ""; // current option name
	string argument = ""; // current argument value
	char quote = '\0'; // current quote char
	
	S += " "; // ensure we will reset state to none
	
	int n = 0; // current position in S
	char p; // current char
	int size = S.length();
	
	states state = none;
	while (state != error && state != parsed)
	{
		if (n >= size)
		{
			if (state == none) state = parsed;
			else state = error;
			break;
		}
		
		p = S[n];
		
		switch (state)
		{
			case none :
				if (p != ' ' && p != '\t')
				{
					// check if we have an arg or an opt
					if ((p != '-' && p != '+') || option != "")
					{
						state = arg;
					}
					else
					{
						state = opt;
					}
				} else n++; // next char
				break;
			case opt :
				if (p == '+') state = opt_plus;
				else state = opt_minus;
				n++;
				break;
			case opt_plus :
				if (p != ' ' && p != '\t')
				{
					option += p;
					n++;
				}
				else
				{
					// end of opt_plus state
					opt_name.push_back(option);
					opt_value.push_back("");
					option = "";
					state = none;
					n++;
				}
				break;
			case opt_minus :
				if (p != ' ' && p != '\t')
				{
					option += p;
					n++;
				}
				else
				{
					// end of opt_minus state
					state = none;
					n++;
				}
				break;
			case arg :
				
				// assign quote if needed
				if ((p == '\'' || p == '"') && argument == "")
				{
					quote = p;
					n++;
					break; // switch break
				}
				
				if (p == '\\')
				{
					argument += S[n+1]; // add next char
					n += 2;
					break; // switch break
				}
				
				if (p == ' ' || p == '\t')
				{
					if (quote != '\0')
					{
						argument += p;
						n++;
						break; // switch break
					}
					
					// check for option
					if (option == "")
					{
						args.push_back(argument);
					}
					else
					{
						// an option name was already set
						opt_name.push_back(option);
						opt_value.push_back(argument);
						option = "";
					}
					argument = "";
					state = none;
					n++;
				}
				else
				{
					// an non-empty char was found
					// check for quote char
					if (p == quote)
					{
						quote = '\0';
					}
					else
					{
						// standard char
						argument += p;
					}
					n++;
				}
				break;
		}
	}
	
	if (state == error)
	{
		parse_error = "parse error on character " + convert::int_string(n);
		ret = false;
	}
	return ret;
}

string parser::option(string S)
{
	if (opt_name.size() != opt_value.size()) return "";
	for (int i = 0; i < opt_name.size(); i++)
	{
		if (opt_name[i] == S) return opt_value[i];
	}
	return "";
}

bool parser::option(string name, string *value)
{
	for (int i = 0; i < opt_name.size(); i++)
	{
		if (opt_name[i] == name)
		{
			*value = opt_value[i];
			return true;
		}
	}
	return false;
}

bool parser::option(int pos, string *name, string *value)
{
	if (pos >= opt_name.size() || pos >= opt_value.size()) return false;
	*name = opt_name[pos];
	*value = opt_value[pos];
	return true;
}

bool parser::is(string S)
{
	for (int i = 0; i < opt_name.size(); i++)
	{
		if (opt_name[i] == S) return true;
	}
	return false;
}

string parser::operator[](int i)
{
	if (i < count()) return args[i];
	else return "";
}

int parser::count()
{
	return args.size();
}

int parser::size()
{
	return count();
}

int parser::opt_count()
{
	return opt_value.size();
}

bool parser::mask(string str)
{
	bool ret = true;
	
	int found;
	found = str.find_first_of(" ");
	while (found != string::npos)
	{
		if (found != 0)
		{
			string val = str.substr(0, found);
			if ((val[0] == '+') && !is(val.substr(1))) { ret = false; break; }
			if ((val[0] == '-') && is(val.substr(1))) { ret = false; break; }
		}
		str = str.substr(found + 1);
		found = str.find_first_of(" ");
	}
	
	if (str.length() != 0)
	{
		string val = str.substr(0, found);
		if ((val[0] == '+') && !is(val.substr(1))) ret = false;
		if ((val[0] == '-') && is(val.substr(1))) ret = false;
	}
	
	return ret;
}

parser::~parser()
{
}

} // namespace
