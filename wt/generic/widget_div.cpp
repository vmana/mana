#include "widget_div.h"

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

