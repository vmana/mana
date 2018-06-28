#ifndef void_layout_H
#define void_layout_H

#include <Wt/WGridLayout.h>

#include "mana.h"

using namespace mana;
using namespace Wt;

class void_layout : public WGridLayout
{
	public:
		void_layout();
		void update(WLayoutItem *item = nullptr);
		~void_layout();
};

#endif // void_layout_H
