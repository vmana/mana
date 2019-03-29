#ifndef widget_div_H
#define widget_div_H

#include <Wt/WWidget.h>
#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>

#include "void_layout.h"
#include "mana.h"

namespace mana
{

typedef AlignmentFlag Align;
using namespace Wt;

class label_group
{
	protected:
		vector<WText*> labels;
	public:
		label_group();
		void add_label(WText *l);
		void set_width(WLength w);
};

class widget_div : public WContainerWidget
{
	public:
		void_layout *layout = NULL;

		widget_div();
		void set_awareness(bool aware);
		void clear_layout();

		template <typename W, typename ...Args>
		W* addLayout(Args&& ...args)
		{
			if (!layout) return NULL;
			std::unique_ptr<W> w{new W()};
			W *result = w.get();
			layout->addWidget(move(w), (args)...);
			return result;
		}

		template <typename W, typename ...Args>
		W* addGroup(string label, Args&& ...args)
		{
			if (!layout) return NULL;

			std::unique_ptr<widget_div> u_div{new widget_div()};
			widget_div *new_div = u_div.get();
			auto new_text = new_div->addLayout<WText>(0, 0, Align::Middle);
			new_text->setText(label);
			W *result = new_div->addLayout<W>(0, 1);

			layout->addWidget(move(u_div), (args)...);
			return result;
		}

		template <typename W, typename ...Args>
		W* addGroup(label_group &group, string label, Args&& ...args)
		{
			if (!layout) return NULL;

			std::unique_ptr<widget_div> u_div{new widget_div()};
			widget_div *new_div = u_div.get();
			auto new_text = new_div->addLayout<WText>(0, 0, Align::Middle);
			new_text->setText(label);
			new_text->setStyleClass("label_group");
			group.add_label(new_text);
			W *result = new_div->addLayout<W>(0, 1);
			new_div->layout->setColumnStretch(1, 1);

			layout->addWidget(move(u_div), (args)...);
			return result;
		}

		~widget_div();
};

} // namespace

#endif // widget_div_H
