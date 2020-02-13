#ifndef widget_stack_H
#define widget_stack_H

#include "mana.h"
#include <Wt/WStackedWidget.h>

namespace mana
{

using namespace Wt;

class widget_stack : public WStackedWidget
{
	protected:
	//	Wt::Signal<bool> signal_on_visible_change;

	public:
		// signals
		//Wt::Signal<bool>& on_visible_change();

		widget_stack();
		~widget_stack();
};

} // namespace

#endif // widget_stack_H
