#ifndef widget_panel_H
#define widget_panel_H

#include <Wt/WWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WCssDecorationStyle.h>

#include "mana.h"

namespace mana
{

using namespace Wt;

class widget_panel : public WContainerWidget
{
	public:

		bool collapsed = false;
		WText *title;
		WContainerWidget *header;
		WContainerWidget *body;

		widget_panel();
		void collapse();
		void expand();
		void reverse();
		~widget_panel();
};

} // namespace

#endif // widget_panel_H
