#include "widget_checkbox.h"
#include "mana.h"

namespace mana
{

widget_checkbox::widget_checkbox(string img_checked, string img_unchecked)
{
	this->img_checked = img_checked;
	this->img_unchecked = img_unchecked;
	setImageLink(img_unchecked);
	this->clicked().connect(this, &widget_checkbox::on_click);
}

void widget_checkbox::update_state()
{
	if (internal_checked)
	{
		setImageLink(img_checked);
		checked.emit();
	}
	else
	{
		setImageLink(img_unchecked);
		unchecked.emit();
	}
}
bool widget_checkbox::is_checked()
{
	return internal_checked;
}

void widget_checkbox::set_checked(bool checked)
{
	if (internal_checked == checked) return;
	internal_checked = checked;
	update_state();
	state_changed.emit(checked);
}

void widget_checkbox::on_click()
{
	internal_checked = !internal_checked;
	update_state();
	state_changed.emit(internal_checked);
}

} // namespace

