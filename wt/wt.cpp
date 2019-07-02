#include "wt.h"
namespace mana
{
namespace wt
{

WTime current_time()
{
	return WLocalDateTime::currentServerDateTime().time();
}

WDate current_date()
{
	return WLocalDateTime::currentServerDateTime().date();
}

WDateTime current_datetime()
{
	return WDateTime(current_date(), current_time());
}

int week_number(const WDate &d)
{
	constexpr int days_per_week = 7;
	int day_of_year = WDate(d.year(), 1, 1).daysTo(d);

	const int wday = d.dayOfWeek() - 1;
	const int delta = wday ? wday - 1 : days_per_week - 1;
	return (day_of_year + days_per_week - delta) / days_per_week;
}

} // namespace wt
} // namespace mana
