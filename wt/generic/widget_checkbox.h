#ifndef widget_checkbox_h
#define widget_checkbox_h

#include <Wt/WImage.h>
#include <Wt/WLink.h>
#include <Wt/WSignal.h>

namespace mana
{

using namespace Wt;
using namespace std;

class widget_checkbox : public WImage
{
	protected:
		bool internal_checked = false;
		string img_checked;
		string img_unchecked;
		void update_state();
		void on_click();
		bool change_allowed = false;

	public:
		Signal<> checked;
		Signal<> unchecked;
		Signal<bool> state_changed;
		widget_checkbox(string img_checked, string img_unchecked);
		bool is_checked();
		void set_checked(bool checked);
		void allow_change(bool allowed);
};

} // namespace

#endif // widget_checkbox_h
