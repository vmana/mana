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

} // namespace wt
} // namespace mana
