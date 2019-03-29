#ifndef widget_save_icon_H
#define widget_save_icon_H

#include <Wt/WContainerWidget.h>
#include <Wt/WTimer.h>

#include "widget_div.h"
#include "mana.h"

namespace mana
{

using namespace Wt;

class widget_save_icon : public widget_div
{
	protected:
		void reset();
		WTimer *timer;

	public:

		chrono::milliseconds duration;
		bool is_mobile = false;

		widget_save_icon();
		void save();
		void cancel(); // immediate cancel, don't wait for duration and cancel fired signal
		~widget_save_icon();
};

} // namespace

#endif // widget_save_icon_H
