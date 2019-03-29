#include "widget_save_icon.h"
namespace mana
{

widget_save_icon::widget_save_icon() : duration(2000)
{
	// timer to reset css after save()
	timer = addChild(make_unique<WTimer>());
	timer->setSingleShot(true);
	timer->setInterval(duration);
	timer->timeout().connect(this, &widget_save_icon::reset);

	setStyleClass("widget-save-icon-hidden");
	setMinimumSize(62, 62);
	resize(62, 62);
}

void widget_save_icon::save()
{
	if (is_mobile) setStyleClass("wmobile-save-icon");
	else setStyleClass("widget-save-icon");

	// reset state after 'duration' ms
	timer->start();
}

void widget_save_icon::cancel()
{
	// immediate cancel, don't wait for duration and cancel fired signal
	reset();
	timer->stop();
}

void widget_save_icon::reset()
{
	if (is_mobile) setStyleClass("wmobile-save-icon-hidden");
	else setStyleClass("widget-save-icon-hidden");
}

widget_save_icon::~widget_save_icon()
{
}

} // namespace
