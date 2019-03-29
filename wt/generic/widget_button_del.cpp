#include "widget_button_del.h"

namespace mana
{

widget_button_del::widget_button_del()
{
	init("");
}

widget_button_del::widget_button_del(string text, bool is_deletable)
{
	this->is_deletable = is_deletable;
	init(text);
}

void widget_button_del::init(string text)
{
	this->setStyleClass("widget-button-del");

	button = this->layout->addWidget(make_unique<WText>(text), 0, 0);
	button->decorationStyle().setCursor(Cursor::PointingHand);

	del = this->layout->addWidget(make_unique<WText>(), 0, 1);
	del->resize(30, WLength::Auto);
	del->setStyleClass("widget-button-del-D");

	if (!is_deletable) del->hide();

	del->decorationStyle().setCursor(Cursor::PointingHand);
}

void widget_button_del::set_deletable(bool is_deletable)
{
	this->is_deletable = is_deletable;
	resize(this->width(), this->height());
}

void widget_button_del::resize(const WLength& width, const WLength& height)
{
	int new_width = width.value();
	if (new_width < 30) new_width = 30;

	widget_div::resize(new_width, height);
	if (is_deletable)
	{
		button->resize(new_width - 30, height);
		del->show();
	}
	else
	{
		button->resize(new_width, height);
		del->hide();
	}
}

void widget_button_del::setStyleClass(const WT_USTRING& styleClass)
{
	auto new_style = styleClass + " widget-button-del";
	widget_div::setStyleClass(new_style);
}

widget_button_del::~widget_button_del()
{
}

} // namespace
