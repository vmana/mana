#include "widget_panel.h"
namespace mana
{

widget_panel::widget_panel()
{
	header = this->addNew<WContainerWidget>();
	title = header->addNew<WText>();
	body = this->addNew<WContainerWidget>();

	body->setOverflow(Overflow::Hidden);
	body->setStyleClass("widget-panel-body-expand");
	header->setStyleClass("widget-panel-header widget-panel-header-expand");
	header->setPadding(8, Side::Left);
	header->decorationStyle().setCursor(Cursor::PointingHand);
}

void widget_panel::collapse()
{
	if (collapsed) return;
	collapsed = !collapsed;
	body->resize(0, 0);
	header->setStyleClass("widget-panel-header");
	body->setStyleClass("widget-panel-body");
}

void widget_panel::expand()
{
	if (!collapsed) return;
	collapsed = !collapsed;
	body->resize(WLength::Auto, WLength::Auto);
	header->setStyleClass("widget-panel-header widget-panel-header-expand");
	body->setStyleClass("widget-panel-body-expand");
}

void widget_panel::reverse()
{
	if (collapsed) expand();
	else collapse();
}

widget_panel::~widget_panel()
{
}

} // namespace
