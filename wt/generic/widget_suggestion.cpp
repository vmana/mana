#include "widget_suggestion.h"

widget_suggestion::widget_suggestion()
{
	suggestion = make_unique<WSuggestionPopup>(js_suggestion_matcher, js_suggestion_replacer);
	suggestion->forEdit(this);
}

widget_suggestion::~widget_suggestion()
{
}

