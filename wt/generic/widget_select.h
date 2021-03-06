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

		void add(string display, T value)
		{
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

		void set_by_value(string value)
		{
			auto values = model->stringList();
			for (int i = 0; i < values.size(); i++)
			{
				if (values[i].toUTF8() == value)
				{
					WComboBox::setCurrentIndex(i);
					break;
				}
			}
		}

		void setCurrentIndex(int i)
		{
			WComboBox::setCurrentIndex(i);
		}

		void setCurrentValue(T v)
		{
			for (int i = 0; i < model->rowCount(); i++)
			{
				try
				{
					auto vi = any_cast<T>(model->data(model->index(i, 0), Wt::ItemDataRole::User));
					if (v == vi)
					{
						this->setCurrentIndex(i);
						break;
					}
				} catch (const bad_any_cast &e) {}
			}
		}
};

} // namespace

#endif // widget_select_H
