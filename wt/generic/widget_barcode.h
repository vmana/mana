#ifndef widget_barcode_H
#define widget_barcode_H

#include <Wt/WLineEdit.h>

#include "mana.h"

namespace mana
{

using namespace Wt;

class widget_barcode : public WLineEdit
{
	protected:
		void on_key_pressed(WKeyEvent k);

	public:
		string barcode = "";
		widget_barcode();
		void set_barcode(string barcode);

		Signal<> barcode_change_event;
};

} // namespace

#endif // widget_barcode_H
