#include "widget_barcode.h"
namespace mana
{

widget_barcode::widget_barcode()
{
	this->keyPressed().connect(this, &widget_barcode::on_key_pressed);
}

void widget_barcode::on_key_pressed(WKeyEvent k)
{
	if (k.charCode() == 13)
	{
		need_erasure = true;
		return;
	}

	if (need_erasure)
	{
		this->setText(k.text());
		need_erasure = false;
	}
}

} // namespace
