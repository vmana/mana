#include "widget_template.h"
#include "mana.h"

namespace mana
{

widget_template::widget_template()
{
}

widget_template::widget_template(const WString &text)
{
	setTemplateText(text, TextFormat::UnsafeXHTML);
}

widget_template::widget_template(const widget_template &T)
{
	std::ignore = T;
}

void widget_template::set_text(const WString &text)
{
	clear();
	setTemplateText(text, TextFormat::UnsafeXHTML);
}

unique_ptr<widget_template> widget_template::load(string filename)
{
	string tpl = file::read_content(filename);
	auto ret = make_unique<widget_template>(tpl);
	return ret;
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

} // namespace

