#ifndef MANA_WT_H
#define MANA_WT_H

#include <Wt/WLocalDateTime.h>
#include <Wt/WTime.h>
#include <Wt/WDate.h>

using namespace Wt;
namespace mana
{

	namespace wt
	{
		WTime current_time();
		WDate current_date();
		WDateTime current_datetime();
		int week_number(const WDate &d);
	};

}; // namespace
#endif // MANA_WT_H
