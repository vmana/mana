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

template<class T = int>
class fuzzy_choice
{
	public:
		string text;
		T value; // can be anything
		size_t length;
		ssize_t match_start;	/* inclusive match start offset */
		ssize_t match_end;	/* exclusive match end offset */
		double score;
};

template<class T = int>
class fuzzy
{
	public:

		fuzzy() {}

		fuzzy(vector<string> data)
		{
			set_data(data);
		}

		fuzzy(vector<tuple<string, T>> data)
		{
			set_data(data);
		}

		void set_data_string(vector<string> data)
		{
			choices.clear();
			for (auto &d : data)
			{
				if (d.length() == 0) continue;

				fuzzy_choice<> c;
				c.text = d;
				c.value = 0;
				c.length = d.length();
				c.match_start = -1;
				c.match_end = -1;
				c.score = 0;
				choices.push_back(c);
			}
		}

		void set_data(vector<tuple<string, T>> data)
		{
			choices.clear();
			for (auto &d : data)
			{
				auto &[text, value] = d;
				if (text.length() == 0) continue;

				fuzzy_choice<T> c;
				c.text = text;
				c.value = value;
				c.length = text.length();
				c.match_start = -1;
				c.match_end = -1;
				c.score = 0;
				choices.push_back(c);
			}
		}

		void add(string data)
		{
			add({data, 0});
		}

		void add(tuple<string, T> data)
		{
			auto &[text, value] = data;
			if (text.length() == 0) return;

			fuzzy_choice<T> c;
			c.text = text;
			c.value = value;
			c.length = text.length();
			c.match_start = -1;
			c.match_end = -1;
			c.score = 0;
			choices.push_back(c);
		}

		string search(string query)
		{
			this->query = query;
			filter_choices();
			if (choices.size() > 0) return choices[0].text;
			return "";
		}

		vector<fuzzy_choice<T>> search_all(string query)
		{
			this->query = query;

			vector<fuzzy_choice<T>> ret;
			filter_choices();

			// fill ret based on choices
			for (auto &c : choices)
			{
				ret.push_back(c);
			}

			return ret;
		}

		vector<fuzzy_choice<T>> search_all(string query, int maximum)
		{
			this->query = query;
			bool empty_query = (query == "");

			vector<fuzzy_choice<T>> ret;
			filter_choices();

			// fill ret based on choices
			for (int i = 0; i < maximum && i < choices.size(); i++)
			{
				auto &c = choices[i];
				if (c.score == 0 && !empty_query) break;
				ret.push_back(c);
			}

			return ret;
		}

		void clear()
		{
			choices.clear();
		}

		int size()
		{
			return choices.size();
		}

		vector<fuzzy_choice<T>> get_choices() const
		{
			return choices;
		}

		void print()
		{
			for (auto &c : choices)
			{
				if (c.score == 0) break;
				cout << c.score << " : " << c.match_start << "-" << c.match_end << " : " << c.text << endl;
			}
		}

	protected:

		string query;
		vector<fuzzy_choice<T>> choices;

		void filter_choices()
		{
			for (int i = 0; i < choices.size(); i++)
			{
				auto &c = choices[i];
				if (min_match(c.text.c_str(), 0, &c.match_start, &c.match_end) == INT_MAX)
				{
					c.match_start = c.match_end = -1;
					c.score = 0;
				}
				else
				{
					size_t match_length = c.match_end - c.match_start;
					c.score = (double)query.length()/match_length/c.length;
				}
			}

			std::sort(choices.begin(), choices.end(), [=](const fuzzy_choice<T> &c1, const fuzzy_choice<T> &c2)
			{
				if (c1.score < c2.score) return false;
				if (c1.score > c2.score) return true;

				/* The two choices have an equal score. */
				/* Sort based on the address of string since it reflects the initial input order. */
				/* The comparison is inverted since the choice with the lowest address must come first. */

				if (c1.text < c2.text) return true;
				if (c1.text > c2.text) return false;

				return true;
			});
		}

		size_t min_match(const char *str, size_t offset, ssize_t *start, ssize_t *end)
		{
			const char *e, *q, *s;
			size_t length;

			q = query.c_str();
			if (*q == '\0' || (s = e = strcasechr(&str[offset], q)) == NULL)
				return INT_MAX;

			for (;;)
			{
				for (e++, q++; isu8cont(*q); e++, q++)
					continue;
				if (*q == '\0')
					break;
				if ((e = strcasechr(e, q)) == NULL)
					return INT_MAX;
			}

			length = e - s;
			/* LEQ is used to obtain the shortest left-most match. */
			if (length == query.length() || length <= min_match(str, s - str + 1, start, end))
			{
				*start = s - str;
				*end = e - str;
			}

			return *end - *start;
		}

		static int isu8cont(unsigned char c)
		{
			return MB_CUR_MAX > 1 && (c & (0x80 | 0x40)) == 0x80;
		}

		static int isu8start(unsigned char c)
		{
			return MB_CUR_MAX > 1 && (c & (0x80 | 0x40)) == (0x80 | 0x40);
		}

		static int xmbtowc(wchar_t *wc, const char *s)
		{
			int n;

			n = mbtowc(wc, s, MB_CUR_MAX);
			if (n == -1)
			{
				/* Assign in order to suppress ignored return value warning. */
				n = mbtowc(NULL, NULL, MB_CUR_MAX);
				return 0;
			}
			return n;
		}

		/* Returns the length of a CSI or OSC escape sequence located at the beginning of str. */
		static size_t skipescseq(const char *str)
		{
			size_t i;
			int csi = 0;
			int osc = 0;

			if (str[0] == '\033' && str[1] == '[')
				csi = 1;
			else if (str[0] == '\033' && str[1] == ']')
				osc = 1;
			else
				return 0;

			for (i = 2; str[i] != '\0'; i++)
			{
				if ((csi && str[i] >= '@' && str[i] <= '~') || (osc && str[i] == '\a'))
					break;
			}

			return i + 1;
		}

		/* Returns a pointer to first occurrence of the first character in s2 in s1 with */
		/* respect to Unicode characters disregarding case. */
		static const char* strcasechr(const char *s1, const char *s2)
		{
			wchar_t wc1, wc2;
			size_t i;
			int nbytes;

			if (xmbtowc(&wc2, s2) == 0) return NULL;

			for (i = 0; s1[i] != '\0';)
			{
				if ((nbytes = skipescseq(s1 + i)) > 0)
					/* A match inside an escape sequence is ignored. */;
				else if ((nbytes = xmbtowc(&wc1, s1 + i)) == 0)
					nbytes = 1;
				else if (wcsncasecmp(&wc1, &wc2, 1) == 0)
					return s1 + i;
				i += nbytes;
			}

			return NULL;
		}
};

} // namespace

#endif // fuzzy_H
