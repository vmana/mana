#include "widget_common.h"

namespace mana
{

widget_common::widget_common()
{
}

void widget_common::set_size(WWidget *widget, int w, int h)
{
	widget->setMinimumSize(w, h);
	widget->setMaximumSize(w, h);
	widget->resize(w, h);
}

widget_common::~widget_common()
{
}

} // namespace
