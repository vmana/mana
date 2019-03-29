#include "widget_div.h"

namespace mana
{

/****    label_group    ****/

label_group::label_group()
{
}

void label_group::add_label(WText *l)
{
	labels.push_back(l);
}

void label_group::set_width(WLength w)
{
	for (auto &l : labels)
		l->setWidth(w);
}

/****    widget_div    ****/

widget_div::widget_div()
{
	layout = this->setLayout(make_unique<void_layout>());
}

void widget_div::set_awareness(bool aware)
{
	setLayoutSizeAware(aware);
}

void widget_div::clear_layout()
{
	int vertical_spacing = layout->verticalSpacing();
	int horizontal_spacing = layout->horizontalSpacing();
	WContainerWidget::clear();
	layout = this->setLayout(make_unique<void_layout>());
	layout->setHorizontalSpacing(horizontal_spacing);
	layout->setVerticalSpacing(vertical_spacing);
}

widget_div::~widget_div()
{
}

} // namespace
