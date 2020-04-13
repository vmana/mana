#ifndef YAML_H
#define YAML_H

#include "common.h"
#include "cypher.h"

namespace mana
{

class yaml_data
{
	public:
		string path;
		string comment; // comments below this path

		yaml_data();
		yaml_data(string path); // a/b/-c/d:value
		void load(string path);
		vector<string> split_key_value() const;
		friend bool operator==(const yaml_data &A, const yaml_data &B);
		friend bool operator!=(const yaml_data &A, const yaml_data &B);
		friend bool operator<(const yaml_data &A, const yaml_data &B);
		friend bool operator>(const yaml_data &A, const yaml_data &B);
};

class yaml
{
	protected :
		int space_count(string &S); // count spaces on the left
		static string get_parent_path(yaml_data &D, int depth);

	public :
		vector<yaml_data> data;

		yaml();
		yaml(const yaml &Y);
		yaml(string S); // generate from yaml string

		bool load(string filename);
		bool load_string(string &S);
		bool load_vector(vector<string> &lines);
		bool save(string filename, bool add_comments = true);
		void print(string *S = NULL); // if NULL, cout, else print to string
		void sort();
		void add_top_comment(string S);
		void remove_comments();
		string generate(bool add_comments = true);

		// search by regex and return a yaml subtree
		// example: ".*/ip:.*" return all lines which contain the key "ip"
		yaml match(string reg);

		// search path by regex into format using $1 $2... which is then searched again
		// example: ("(.*)/ip:.*" , "$1.*") get the parents which have the key "ip"
		yaml match(string reg, string format);

		// replace matched values by something else, which can contain $1 $2 ...
		// example: match_replace("(aaa/)(.*)", "$1bbb/$2"); insert a new level, aaa/xxx:y => aaa/bbb/xxx:y
		yaml match_replace(string reg, string format);

		// search and return found strings
		// duplicate try to remove any duplicates found in results
		// example: search(".*/ip:(.*)", "$1") return all values for the key "ip"
		// example: search("(.*)/aaa/.*", "$1") return all direct parent of the node "aaa"
		vector<string> search(string reg, string format, bool duplicate = false);

		// search and return the first found string
		// example: search(".*/ip:(.*)", "$1") return the first value for the key "ip"
		string search_first(string reg, string format);

		// search a value for a path.
		// return "" if not found
		string value(string path);

		// search a list of value for a path.
		// return empty list if not found
		vector<string> values(string path);

		yaml operator+(const yaml_data &A);
		yaml operator+=(const yaml_data &A);
		yaml operator+(const yaml &A);
		yaml operator+=(const yaml &A);
		bool operator==(const yaml &A);
};

} // namespace

#endif // YAML_H
