#ifndef widget_suggestion_H
#define widget_suggestion_H

#include <Wt/WSuggestionPopup.h>
#include <Wt/WLineEdit.h>

#include "mana.h"

namespace mana
{

using namespace Wt;

const string js_suggestion_matcher =
	"function (edit) {" \
	"return function(suggestion) {" \
	"if (suggestion == null) return edit.value;" \
	"if (edit.value == '') return { match:false, suggestion:''};" \
	"var reg = new RegExp('.*' + edit.value +'.*', 'i');" \
	"var ret = suggestion.match(reg);" \
	"return { match : ret, suggestion : suggestion };" \
	"}}";

const string js_suggestion_replacer =
	"function (edit, suggestion_text, suggestion_value) {" \
	"edit.value = suggestion_value;" \
	"}";


class widget_suggestion : public WLineEdit
{
	public:
		unique_ptr<WSuggestionPopup> suggestion;
		widget_suggestion();
		~widget_suggestion();
};

} // namespace

#endif // widget_suggestion_H
