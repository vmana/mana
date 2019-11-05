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
		barcode = this->text().toUTF8();
		this->setPlaceholderText(barcode);
		this->setText("");
		barcode_change_event.emit();
	}
}

void widget_barcode::set_barcode(string barcode)
{
	this->barcode = barcode;
	this->setPlaceholderText(barcode);
	this->setText("");
	/* barcode_change_event.emit(); */
}

} // namespace
