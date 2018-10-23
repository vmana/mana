#include "widget_template.h"
#include "mana.h"

using namespace mana;

widget_template::widget_template()
{
}

widget_template::widget_template(const WString &text) :
	WTemplate(text)
{
}

widget_template::widget_template(const widget_template &T)
{
	std::ignore = T;
}

/* widget_template widget_template::operator+(const widget_template &T) */
/* { */
/* 	widget_template ret = *this; */
/* 	/1* for (int i = 0; i < A.data.size(); ++i) *1/ */
/* 	/1* 	ret += A.data[i]; *1/ */
/* 	return ret; */
/* } */

widget_template widget_template::operator+=(std::unique_ptr<widget_template> &T)
{
	string new_id = mana::randstring(20);
	this->setTemplateText(this->templateText() + "${" + new_id + "}");
	this->bindWidget(new_id, move(T));
	return *this;
}

widget_template::~widget_template()
{
}
