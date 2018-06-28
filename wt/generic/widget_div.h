#ifndef widget_div_H
#define widget_div_H

#include <Wt/WWidget.h>
#include <Wt/WContainerWidget.h>

#include "void_layout.h"
#include "mana.h"

typedef AlignmentFlag Align;

using namespace mana;
using namespace Wt;

class widget_div : public WContainerWidget
{
	public:
		void_layout *layout = NULL;

		widget_div();
		void set_awareness(bool aware);
		void clear_layout();
		void update(WLayoutItem *item = nullptr);

		template <typename W, typename ...Args>
		W* addLayout(Args&& ...args)
		{
			if (!layout) return NULL;
			std::unique_ptr<W> w{new W()};
			W *result = w.get();
			layout->addWidget(std::move(w), (args)...);
			return result;
		}

		~widget_div();
};

#endif // widget_div_H
