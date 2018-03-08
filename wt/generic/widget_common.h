#ifndef widget_common_H
#define widget_common_H

#include <Wt/WWidget.h>

#include "mana.h"

using namespace mana;
using namespace Wt;

class widget_common
{
	public:
		widget_common();
		static void set_size(WWidget *widget, int w, int h);
		~widget_common();
};

#endif // widget_common_H
