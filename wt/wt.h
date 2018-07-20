#ifndef MANA_WT_H
#define MANA_WT_H

#include <Wt/WLocalDateTime.h>
#include <Wt/WTime.h>
#include <Wt/WDate.h>

using namespace Wt;

class wt
{
	public:
		wt();
		static WTime current_time();
		static WDate current_date();
		static WDateTime current_datetime();
		~wt();
};
#endif // MANA_WT_H
