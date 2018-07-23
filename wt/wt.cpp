#include "wt.h"

wt::wt()
{
}

WTime wt::current_time()
{
	return WLocalDateTime::currentServerDateTime().time();
}

WDate wt::current_date()
{
	return WLocalDateTime::currentServerDateTime().date();
}

WDateTime wt::current_datetime()
{
	return WDateTime(current_date(), current_time());
}

wt::~wt()
{
}
