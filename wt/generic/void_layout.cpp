#include "void_layout.h"
namespace mana
{

void_layout::void_layout()
{
	this->setHorizontalSpacing(0);
	this->setVerticalSpacing(0);
	this->setContentsMargins(0, 0, 0, 0);
}

void void_layout::update(WLayoutItem *item)
{
	WGridLayout::update(item);
}

void_layout::~void_layout()
{
}

} // namespace

