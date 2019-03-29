#ifndef widget_button_del_H
#define widget_button_del_H

#include "mana.h"
#include "widget_div.h"
#include <Wt/WText.h>
#include <Wt/WCssDecorationStyle.h>

namespace mana
{

using namespace Wt;

class widget_button_del : public widget_div
{
	public:
		bool is_deletable = false;
		WText *button = NULL;
		WText *del = NULL;

		widget_button_del();
		widget_button_del(string text, bool is_deletable = true);

		void init(string text);
		void set_deletable(bool is_deletable);
		void resize(const WLength& width, const WLength& height);
		void setStyleClass(const WT_USTRING& styleClass);
		~widget_button_del();
};

} // namespace

#endif // widget_button_del_H
