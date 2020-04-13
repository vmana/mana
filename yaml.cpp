#include "yaml.h"
namespace mana
{

yaml_data::yaml_data()
{
}

yaml_data::yaml_data(string path)
{
	load(path);
}

void yaml_data::load(string path)
{
	this->path = path;
}

vector<string> yaml_data::split_key_value() const
{
	vector<string> ret;
	auto split = explode(":", path);
	switch (split.size())
	{
		case 0:
		{
			ret.push_back("");
			ret.push_back("");
			return ret;
		}
		case 1:
		{
			// no value
			ret.push_back(split[0]);
			ret.push_back("");
			break;
		}
		default:
		{
			ret.push_back(split[0]);
			string val = "";
			for (int i = 1; i < split.size(); ++i)
			{
				val += split[i] + ":";
			}
			val = substr(val, 0, -1); // remove last :
			ret.push_back(val);
		}
	}

	return ret;
}

bool operator==(const yaml_data &A, const yaml_data &B)
{
	bool ret = false;
	if (A.path == B.path) ret = true;
	return ret;
}

bool operator!=(const yaml_data &A, const yaml_data &B)
{
	return !(A == B);
}

bool operator<(const yaml_data &A, const yaml_data &B)
{
	return (A.path < B.path);
}

bool operator>(const yaml_data &A, const yaml_data &B)
{
	return (A.path > B.path);
}

yaml::yaml()
{

}

yaml::yaml(const yaml &Y)
{
	this->data = Y.data;
}

yaml::yaml(string S)
{
	load_string(S);
}

int yaml::space_count(string &S)
{
	int ret = 0;

	while (ret < S.size())
	{
		if (S[ret] != ' ') break;
		ret++;
	}
	return ret;
}

bool yaml::load(string filename)
{
	bool ret = false;
	data.clear();
	vector<string> lines;
	if (!file::read_vector(filename, &lines)) return ret;
	return load_vector(lines);
}

bool yaml::load_string(string &S)
{
	vector<string> lines = explode("\n", S);
	return load_vector(lines);
}

bool yaml::load_vector(vector<string> &lines)
{
	bool ret = false;
	data.clear();

	// parse it
	int previous_spaces = 0; // previous path space
	bool add_list_space = false; // special cases
	vector<string> path;
	vector<int> path_space; // space taken by path. Usually 2 per elt, but some lists elt count for 4
	bool is_list;
	string comment = "";

	for (int i = 0; i < lines.size(); ++i)
	{
		string line = lines[i];
		int line_space = space_count(line);

		// remove spaces
		line = trim_space(line);

		if (line.length() <= 1)
		{
			comment += "\n";
			continue;
		}

		// check for comments
		if (line[0] == '#')
		{
			comment += lines[i] + "\n";
			continue;
		}

		is_list = (substr(line, 0, 2) == "- ");

		if (is_list) line = "-" + substr(line, 2);

		bool found_colon = (strpos(line, ":") != string::npos);

		// check if it's a list element : - xxxx
		// todo: single value like aaa.bbb
		if (! found_colon)
		{
			if (is_list)
			{
				// Correct indentation in some cases
				// if the parent is at the same level as this list, indent with 2 spaces
				// example : salt yaml output of grains.items
				if (line_space == previous_spaces)
				{
					add_list_space = true;
				}

				if (add_list_space) line_space += 2;

				// get parent's path
				int count_space = 0;
				int count;
				for (count = 0; count < path_space.size(); ++count)
				{
					if (count_space >= line_space) break;
					count_space += path_space[count]; // increment based on saved values
				}

				// reduce path and path_space to <count>
				path.resize(count);
				path_space.resize(count);

				// add it
				yaml_data D;
				for (int n = 0; n < path.size(); ++n)
					D.path += path[n] + "/";

				D.path += line + ":";
				D.comment = comment;
				comment = ""; // reset
				data.push_back(D);
			}
			else
			{
				// todo: aaa.bbb
			}

			previous_spaces = line_space;
			continue;
		}

		add_list_space = false; // reset when not in a list element

		// check if it's a key:value, or just a path
		// we will check the last char of line for that
		bool is_path = (substr(line, -1) == ":");

		if (is_path)
		{
			// it's a path
			string key = substr(line, 0, -1); // key without the last ':'

			// check for ':' or '/' in key, if so it'll be base64 encoded
			found_colon = (strpos(key, ":") != string::npos);
			bool found_slash = (strpos(key, "/") != string::npos);
			if (found_colon || found_slash) key = "base64=" + cypher::base64_encode(key);

			// where do we move in path ?
			if (line_space > previous_spaces)
			{
				// to the right
				path.push_back(key);

				if (is_list) path_space.push_back(4);
				else path_space.push_back(2);
			}
			else
			{
				// to the left
				// get parent's path
				int count_space = 0;
				int count;
				for (count = 0; count < path_space.size(); ++count)
				{
					if (count_space >= line_space) break;
					count_space += path_space[count]; // increment based on saved values
				}

				// reduce path and path_space to <count>
				path.resize(count);
				path_space.resize(count);

				path.push_back(key);

				if (is_list) path_space.push_back(4);
				else path_space.push_back(2);
			}
		}
		else
		{
			// not a path => key:value
			vector<string> split = explode(":", line);
			int nsplit = split.size();

			// check if we need to correct path
			if (line_space < previous_spaces)
			{
				int count_space = 0;
				int count;
				for (count = 0; count < path_space.size(); ++count)
				{
					if (count_space >= line_space) break;
					count_space += path_space[count]; // increment based on saved values
				}

				// reduce path and path_space to <count>
				path.resize(count);
				path_space.resize(count);
			}

			yaml_data D;
			for (int n = 0; n < path.size(); ++n)
				D.path += path[n] + "/";

			D.path += split[0] + ":";

			for (int n = 1; n < nsplit; ++n)
				D.path += trim_space(split[n]) + ":";

			D.path = substr(D.path, 0, -1); // remove last ':'

			D.comment = comment;
			comment = ""; // reset
			data.push_back(D);
		}

		previous_spaces = line_space;
	}

	ret = true;
	return ret;
}

bool yaml::save(string filename, bool add_comments)
{
	bool ret = true;
	string res = generate(add_comments);
	ret = file::write_content(filename, res);
	return ret;
}

void yaml::print(string *S)
{
	string res = "";
	for (int i = 0; i < data.size(); ++i)
	{
		res += data[i].path + "\n";
	}
	if (S) *S = res;
	else cout << res;
}

void yaml::sort()
{
	std::sort(data.begin(), data.end());
}

void yaml::add_top_comment(string S)
{
	if (data.size() == 0) return;
	if (substr(S, -1) != "\n") S += "\n";
	data[0].comment = S;
}

void yaml::remove_comments()
{
	for (int i = 0; i < data.size(); ++i)
		data[i].comment = "";
}

string yaml::generate(bool add_comments)
{
	string ret = "";

	yaml Y = *this;
	vector<yaml_data> D = Y.data; // prevent modifications

	vector<string> same; // same parts of path
	for (int i = 0; i < D.size(); ++i)
	{
		if (add_comments) ret += D[i].comment; // add comments before

		auto key_value = D[i].split_key_value();
		string xkey = key_value[0];
		vector<string> vkey = explode("/", xkey);

		// reconvert base64 encoded part
		for (int n = 0; n < vkey.size(); ++n)
		{
			if (substr(vkey[n], 0, 7) == "base64=")
			{
				vkey[n] = substr(vkey[n], 7);
				vkey[n] = cypher::base64_decode(vkey[n]);
			}
		}

		string spaces = ""; // spaces to add on each lines

		// get the minimum size of vkey.size and same.size
		int max_size = vkey.size();
		if (same.size() < max_size) max_size = same.size();
		same.resize(max_size); // can't have a size > max_size

		// get common path of vkey and same
		for (int n = 0; n < max_size; n++)
		{
			if (same[n] != vkey[n])
			{
				same.resize(n);
				break;
			}
		}

		// add spaces, based uppon same.size() and the presence of "-" for lists parts
		for (int n = 0; n < same.size(); ++n)
		{
			if (vkey[n][0] == '-') spaces += "    ";
			else spaces += "  ";
		}

		// add what's after <same>, until vkey.size - 1 to keep the key away
		for (int n = same.size(); n < vkey.size() - 1; ++n)
		{
			if (vkey[n][0] == '-')
			{
				ret += spaces + "- " + substr(vkey[n], 1) + ":\n";
				spaces += "    ";
			}
			else
			{
				ret += spaces + vkey[n] + ":\n";
				spaces += "  ";
			}
		}

		// <same> is this line for the next iteration
		same = vkey;

		// add key + [value]
		if (vkey.back()[0] != '-')
		{
			// regular value
			ret += spaces + vkey.back() + ": " + key_value[1] + "\n";
		}
		else
		{
			// it's a list
			ret += spaces + "- " + substr(vkey.back(), 1);
			if (key_value[1] != "") ret += ": " + key_value[1];
			ret += "\n";
		}
	}
	return ret;
}

yaml yaml::match(string reg)
{
	yaml ret;

	for (int i = 0; i < data.size(); ++i)
	{
		if (!regexp::match(data[i].path, reg)) continue; // no match
		ret = ret + data[i];
	}
	return ret;
}

yaml yaml::match(string reg, string format)
{
	yaml ret;

	vector<string> res; // store matches reformatted

	for (int i = 0; i < data.size(); ++i)
	{
		if (!regexp::match(data[i].path, reg)) continue; // no match
		string result = regexp::search(data[i].path, reg, format);
		res.push_back(result);
	}

	// remove duplicate
	res = mana::unique(res);

	for (int n = 0; n < res.size(); ++n)
	{
		for (int i = 0; i < data.size(); ++i)
		{
			if (!regexp::match(data[i].path, res[n])) continue; // no match
			ret += data[i];
		}
	}

	// remove duplicate
	ret.data = mana::unique(ret.data);

	return ret;
}

yaml yaml::match_replace(string reg, string format)
{
	yaml ret;

	for (int i = 0; i < data.size(); ++i)
	{
		if (!regexp::match(data[i].path, reg))
		{
			// no match, add it without any change
			// push_back to keep the structure intact
			ret.data.push_back(data[i]);
			continue;
		}
		string result = regexp::search(data[i].path, reg, format);
		ret.data.push_back(yaml_data(result));
	}

	return ret;
}

vector<string> yaml::search(string reg, string format, bool duplicate)
{
	vector<string> ret;

	for (int i = 0; i < data.size(); ++i)
	{
		if (!regexp::match(data[i].path, reg)) continue; // no match
		string result = regexp::search(data[i].path, reg, format);
		ret.push_back(result);
	}

	if (!duplicate) ret = unique(ret);
	return ret;
}

string yaml::search_first(string reg, string format)
{
	for (int i = 0; i < data.size(); ++i)
	{
		if (!regexp::match(data[i].path, reg)) continue; // no match
		// return first match
		return regexp::search(data[i].path, reg, format);
	}
	return "";
}

string yaml::value(string path)
{
	return search_first(path + ":(.*)", "$1");
}

vector<string> yaml::values(string path)
{
	return search(path + "/-(.*):$", "$1");
}

yaml yaml::operator+(const yaml_data &A)
{
	yaml ret = *this;

	if (ret.data.size() == 0)
	{
		ret.data.push_back(A);
		return ret;
	}

	// get key part
	string key = A.split_key_value()[0];

	// find the right place to add it
	// search for the position with maximized common part
	int max_common = 0;
	vector<yaml_data>::iterator pos = ret.data.begin();

	for (vector<yaml_data>::iterator it = ret.data.begin(); it != ret.data.end(); ++it)
	{
		string ckey = (*it).split_key_value()[0];

		// don't create duplicate
		if (key == ckey)
		{
			// replace old value with A, keeps comments
			(*it).path = A.path;
			return ret;
		}

		int cmp = common(key, ckey);
		bool new_max = (cmp > max_common); // more in common
		bool new_alpha_greater = ((cmp == max_common) && A > *pos); // same common, but updated because A > *pos alphanum wise

		if (new_max || new_alpha_greater)
		{
			pos = it;
			max_common = cmp;
		}
	}

	if (A > (*pos)) pos++; // add after, not before
	ret.data.insert(pos, A);

	return ret;
}

yaml yaml::operator+=(const yaml_data &A)
{
	*this = *this + A;
	return *this;
}

yaml yaml::operator+(const yaml &A)
{
	yaml ret = *this;
	for (int i = 0; i < A.data.size(); ++i)
		ret += A.data[i];
	return ret;
}

yaml yaml::operator+=(const yaml &A)
{
	for (int i = 0; i < A.data.size(); ++i)
		*this += A.data[i];
	return *this;
}

bool yaml::operator==(const yaml &A)
{
	// check data size
	if (this->data.size() != A.data.size()) return false;

	for (int i = 0; i < A.data.size(); ++i)
	{
		if (this->data[i] != A.data[i]) return false;
	}

	return true;
}

} // namespace
