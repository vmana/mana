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
		yaml(string &S); // generate from yaml string

		bool load(string filename);
		bool load_string(string &S);
		bool load_vector(vector<string> &lines);
		bool save(string filename, bool add_comments = true);
		void print(string *S = NULL); // if NULL, cout, else print to string
		void sort();
		void add_top_comment(string S);
		void remove_comments();
		string generate(bool add_comments = true);

		yaml search(string path); // search by regex
		// search path by regex into format using $1 $2... which is then searched again
		// exemple ("(.*)/ip:.*" , "$1.*") get the parents which have the key "ip"
		yaml search(string path, string format);
		yaml replace(string path, string format);
		vector<string> value(string path, string format, bool duplicate = false);
		string single_value(string path, string format);

		yaml operator+(const yaml_data &A);
		yaml operator+=(const yaml_data &A);
		yaml operator+(const yaml &A);
		yaml operator+=(const yaml &A);
		bool operator==(const yaml &A);
};

} // namespace

#endif // YAML_H
