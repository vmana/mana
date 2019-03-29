#ifndef widget_template_h
#define widget_template_h

#include <Wt/WTemplate.h>

namespace mana
{

using namespace Wt;

class widget_template : public WTemplate
{
	public:
		widget_template();
		widget_template(const WString &text);
		widget_template(const widget_template &T);
		/* widget_template operator+(const widget_template &T); */
		widget_template operator+=(std::unique_ptr<widget_template> &T);
		~widget_template();
};

} // namespace

#endif // widget_template_h
