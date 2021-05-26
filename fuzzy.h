#ifndef fuzzy_H
#define fuzzy_H

#include <algorithm>
#include <bits/stdc++.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

using namespace std;

namespace mana
{

class fuzzy_choice
{
	public:
		string text;
		size_t length;
		ssize_t match_start;	/* inclusive match start offset */
		ssize_t match_end;	/* exclusive match end offset */
		double score;
};

class fuzzy
{
	protected:

		string query;
		vector<fuzzy_choice> choices;
		void filter_choices();
		size_t min_match(const char *str, size_t offset, ssize_t *start, ssize_t *end);
		static int isu8cont(unsigned char c);
		static int isu8start(unsigned char c);
		static int xmbtowc(wchar_t *wc, const char *s);
		static size_t skipescseq(const char *str);
		static const char* strcasechr(const char *s1, const char *s2);

	public:

		fuzzy();
		fuzzy(vector<string> data);
		void set_data(vector<string> data);
		void add(string data);
		string search(string query);
		vector<fuzzy_choice> search_all(string query);
		vector<fuzzy_choice> search_all(string query, int maximum);
		void clear();
		int size();
		vector<fuzzy_choice> get_choices() const;
		void print();
};

} // namespace

#endif // fuzzy_H
