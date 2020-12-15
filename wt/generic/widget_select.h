#ifndef widget_select_H
#define widget_select_H

#include "mana.h"
#include <Wt/WComboBox.h>
#include <Wt/WStringListModel.h>

namespace mana
{

using namespace Wt;

template <class T>
class widget_select : public WComboBox
{
	public:
		shared_ptr<WStringListModel> model;
		widget_select()
		{
			model = make_shared<WStringListModel>();
			this->setModel(model);
		}

		widget_select(vector<tuple<string, T>> V) : widget_select()
		{
			for (auto &v : V)
			{
				add(v);
			}
		}

		void add(tuple<string, T> v)
		{
			auto &[display, value] = v;
			model->addString(display);
			model->setData(model->rowCount() - 1, 0, value, ItemDataRole::User);
		}

		T value()
		{
			T ret;
			int row = currentIndex();
			if (row == -1) return ret;
			try
			{
				return any_cast<T>(model->data(model->index(row, 0), Wt::ItemDataRole::User));
			} catch (const bad_any_cast &e) {}
			return ret;
		}
};

} // namespace

#endif // widget_select_H
