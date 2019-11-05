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
		bool need_erasure = false;
		void on_key_pressed(WKeyEvent k);

	public:
		widget_barcode();
};

} // namespace

#endif // widget_barcode_H
