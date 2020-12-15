#include "widget_select.h"
namespace mana
{

widget_select::widget_select()
{
	model = make_shared<WStringListModel>();
}

} // namespace
