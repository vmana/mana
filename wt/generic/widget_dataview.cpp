#include "widget_dataview.h"
#include <Wt/WAnimation.h>
#include <Wt/WWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WMessageBox.h>
#include <Wt/WTemplate.h>

namespace mana
{

/****    widget_dataview    ****/

widget_dataview::widget_dataview(bool connect_signals) : widget_div()
{
	setStyleClass("dataview");
	int row = 0, col = 0;
	header = addLayout<widget_div>(row++, col, Align::Top);
	div_data = addLayout<WContainerWidget>(row, col);
	layout->setRowStretch(row, 1);
	row++;
	footer = addLayout<WStackedWidget>(row++, col);

	title = header->addLayout<WText>(0, 0, Align::Middle|Align::Center);
	button_corner = header->addLayout<WStackedWidget>(0, 1, Align::Middle|Align::Center);

	// header
	header->layout->setColumnStretch(0, 1);
	header->setStyleClass("dataview-header");
	header->resize(WLength::Auto, header_height);

	title->setPadding(36, Side::Left);

	// panel, pseudo hidden
	panel = div_data->addNew<WContainerWidget>();
	panel->setStyleClass("dataview-panel");
	panel->setPositionScheme(PositionScheme::Absolute);
	panel->setOffsets(0, Side::Left|Side::Top);
	panel->resize(0, 0);

	/****     stack corner    ****/

	button_corner->setMargin(6, Side::Right);

	// corner empty
	img_corner_empty = button_corner->addNew<WImage>();
	img_corner_empty->setImageLink("img/dataview/empty.png");
	// corner add
	img_corner_add = button_corner->addNew<WImage>();
	img_corner_add->setImageLink("img/dataview/plus.png");
	img_corner_add->setToolTip("Ajouter un nouvel élément");
	img_corner_add->decorationStyle().setCursor(Cursor::PointingHand);
	// corner cancel
	img_corner_cancel = button_corner->addNew<WImage>();
	img_corner_cancel->setImageLink("img/dataview/minus.png");
	img_corner_cancel->setToolTip("Annuler l'ajout");
	img_corner_cancel->decorationStyle().setCursor(Cursor::PointingHand);
	// corner del
	img_corner_del = button_corner->addNew<WImage>();
	img_corner_del->setImageLink("img/dataview/trash.png");
	img_corner_del->setToolTip("Supprimer cet élément");
	img_corner_del->decorationStyle().setCursor(Cursor::PointingHand);

	// data_table
	div_data->setStyleClass("data-table");
	div_data->setOverflow(Overflow::Auto, Orientation::Vertical);
	div_data->setOverflow(Overflow::Hidden, Orientation::Horizontal);

	/****     stack footer    ****/

	footer->setStyleClass("dataview-footer");
	footer->setHeight(footer_height); // needed for panel_height

	// footer empty
	footer_empty = footer->addNew<widget_div>();
	auto text_empty = footer_empty->addLayout<WText>(0, 0, Align::Middle|Align::Center);
	text_empty->setHeight(footer_height);

	// footer edit
	footer_edit = footer->addNew<widget_div>();
	footer_edit->resize(WLength::Auto, footer_height);
	footer_edit->addStyleClass("dataview-footer-button");
	auto text_edit = footer_edit->addLayout<WText>(0, 0, Align::Middle|Align::Center);
	text_edit->setText("EDITER");

	// footer add
	footer_add = footer->addNew<widget_div>();
	footer_add->resize(WLength::Auto, footer_height);
	footer_add->addStyleClass("dataview-footer-button");
	auto img_add = footer_add->addLayout<WImage>(0, 0, Align::Middle|Align::Center);
	auto text_add = footer_add->addLayout<WText>(0, 1, Align::Middle|Align::Center);
	footer_add->layout->setColumnStretch(1, 1);
	// footer add : widgets
	img_add->setImageLink("img/dataview/valid.png");
	img_add->setMargin(6, Side::Left);
	text_add->setText("VALIDER");

	// footer cancel valid
	footer_cancel_valid = footer->addNew<widget_div>();
	footer_cancel_valid->resize(WLength::Auto, footer_height);
	footer_cancel_valid->setStyleClass("dataview-footer-cancel-valid");
	button_cancel = footer_cancel_valid->addLayout<WText>(0, 0, Align::Middle|Align::Center);
	button_valid = footer_cancel_valid->addLayout<WText>(0, 1, Align::Middle|Align::Center);
	// footer cancel valid : widgets
	button_cancel->setText("ANNULER");
	button_valid->setText("VALIDER");
	button_cancel->addStyleClass("dataview-footer-button");
	button_valid->addStyleClass("dataview-footer-button");

	// footer custom
	footer_custom = footer->addNew<widget_div>();
	footer_custom->resize(WLength::Auto, footer_height);
	footer_custom->addStyleClass("dataview-footer-button");

	// default stack
	button_corner->setCurrentWidget(img_corner_add);
	footer->setCurrentWidget(footer_empty);

	// signals binding
	if (connect_signals)
	{
		img_corner_add->clicked().connect(this, &widget_dataview::on_corner_add_click);
		img_corner_del->clicked().connect(this, &widget_dataview::on_corner_del_click);
		img_corner_cancel->clicked().connect(this, &widget_dataview::on_corner_cancel_click);
		footer_add->clicked().connect(this, &widget_dataview::on_footer_add_click);
		footer_edit->clicked().connect(this, &widget_dataview::on_footer_edit_click);
		button_valid->clicked().connect(this, &widget_dataview::on_footer_valid_click);
		button_cancel->clicked().connect(this, &widget_dataview::on_footer_cancel_click);
	}
}

widget_dataview::widget_dataview() : widget_dataview(true)
{
}

data_table* widget_dataview::set_data(unique_ptr<data_table> dt)
{
	div_data->clear();

	// recreate panel, pseudo hidden
	panel = div_data->addNew<WContainerWidget>();
	panel->setStyleClass("dataview-panel");
	panel->setPositionScheme(PositionScheme::Absolute);
	panel->setOffsets(0, Side::Left|Side::Top);
	panel->resize(0, 0);

	data = div_data->addWidget(move(dt));

	// binding
	data->selection_change_event.connect(this, &widget_dataview::on_data_selection_change);
	data->hide_corner_event.connect(this, &widget_dataview::hide_corner);
	data->show_corner_event.connect(this, &widget_dataview::show_corner);
	data->line_allow_edit.connect(this, &widget_dataview::line_allow_edit);
	data->line_allow_add.connect(this, &widget_dataview::line_allow_add);

	return data;
}

widget_div* widget_dataview::set_raw_data(unique_ptr<widget_div> dt)
{
	div_data->clear();
	data = NULL;
	auto raw_data = div_data->addWidget(move(dt));
	return raw_data;
}

void widget_dataview::allow_vertical_scroll(bool allow)
{
	if (allow) div_data->setOverflow(Overflow::Auto, Orientation::Vertical);
	else div_data->setOverflow(Overflow::Hidden, Orientation::Vertical);
}

void widget_dataview::on_data_selection_change(int index)
{
	if (footer->currentWidget() == footer_custom) return;

	if (index == -1)
	{
		footer->setCurrentWidget(footer_empty);

		show_corner();

		// hide panel, used by external select_none call
		hide_panel();
	}
	else
	{
		if (is_edit_allowed()) footer->setCurrentWidget(footer_edit);
		else footer->setCurrentWidget(footer_empty);
	}
}

void widget_dataview::hide_corner()
{
	button_corner->setCurrentWidget(img_corner_empty);
}

void widget_dataview::show_corner()
{
	if (is_add_allowed()) button_corner->setCurrentWidget(img_corner_add);
	else button_corner->setCurrentWidget(img_corner_empty);
}

void widget_dataview::hide_panel()
{
	if (data)
	{
		data->show();
	}
	div_data->setOverflow(Overflow::Auto, Orientation::Vertical);
	panel->resize("100%", 0);
}

void widget_dataview::show_panel()
{
	if (data)
	{
		// check if scrolled, if not, don't need to hide data
		// it prevents flickers
		if (div_data->scrollTop() > 0)
		{
			data->hide();
		}
	}
	div_data->setOverflow(Overflow::Hidden, Orientation::Vertical);
	panel->resize("100%", "100%");
}

void widget_dataview::switch_panel()
{
	if (panel->height() == 0)
	{
		show_panel();
	}
	else
	{
		hide_panel();
	}
}


void widget_dataview::on_corner_add_click()
{
	button_corner->setCurrentWidget(img_corner_cancel);
	// show panel
	show_panel();
	footer->setCurrentWidget(footer_add);

	// generate widgets inside panel
	if (data)
	{
		auto gen_data = data->generate_add_view();
		panel->clear();
		p_panel = panel->addWidget(move(gen_data));
		p_panel->resize("100%", panel_height);
	}
}

void widget_dataview::on_corner_cancel_click()
{
	show_corner();

	// hide panel
	hide_panel();

	if (!data) return; // prevents crash if undef
	if (data->selected_index == -1)
	{
		footer->setCurrentWidget(footer_empty);
	}
	else
	{
		if (is_edit_allowed()) footer->setCurrentWidget(footer_edit);
		else footer->setCurrentWidget(footer_empty);
	}
}

void widget_dataview::on_corner_del_click()
{
	// if not allowed, no click action & return
	if (!is_delete_allowed()) return;

	show_corner();

	// hide panel
	hide_panel();
	footer->setCurrentWidget(footer_empty);

	if (!data) return; // prevents crash if undef
	if (need_confirm_delete)
	{
		auto confirm = addChild(make_unique<WMessageBox>());
		confirm->setWindowTitle("Confirmation"); confirm->setIcon(Icon::Warning);
		confirm->setText(confirm_delete_message);

		confirm->addButton(StandardButton::Yes);
		confirm->addButton(StandardButton::No);
		confirm->setDefaultButton(StandardButton::No);

		confirm->setModal(true);
		confirm->buttonClicked().connect([=]()
		{
			auto res = confirm->buttonResult();
			removeChild(confirm);
			if (res == StandardButton::Yes)
			{
				data->on_del_event(); // call real implementation
				// reload
				data->generate_lines();
			}
		});
		confirm->show();
	}
	else
	{
		// no confirmation
		data->on_del_event(); // call real implementation
		// reload
		data->generate_lines();
	}
}

void widget_dataview::on_footer_edit_click()
{
	button_corner->setCurrentWidget(img_corner_del);
	// show panel
	show_panel();
	footer->setCurrentWidget(footer_cancel_valid);

	// generate widgets inside panel
	if (data)
	{
		auto gen_data = data->generate_edit_view();
		panel->clear();
		p_panel = panel->addWidget(move(gen_data));
		p_panel->resize("100%", panel_height);
	}
}

void widget_dataview::on_footer_add_click()
{
	show_corner();

	// hide panel
	hide_panel();
	footer->setCurrentWidget(footer_empty);

	if (!data) return; // prevents crash if undef
	data->on_add_event(); // call real implementation
	// reload
	data->generate_lines();
}

void widget_dataview::on_footer_cancel_click()
{
	show_corner();

	// hide panel
	hide_panel();

	if (!data) return; // prevents crash if undef
	if (data->selected_index == -1)
	{
		footer->setCurrentWidget(footer_empty);
	}
	else
	{
		if (is_edit_allowed()) footer->setCurrentWidget(footer_edit);
		else footer->setCurrentWidget(footer_empty);
	}
}

void widget_dataview::on_footer_valid_click()
{
	show_corner();

	// hide panel
	hide_panel();
	footer->setCurrentWidget(footer_empty);

	if (!data) return; // prevents crash if undef
	data->on_edit_event(); // call real implementation
	// reload
	data->generate_lines();
}

void widget_dataview::allow_add_edit(bool allowed)
{
	allow_add(allowed);
	allow_edit(allowed);
}

void widget_dataview::allow_add(bool allowed)
{
	add_allowed = allowed;
	show_corner();
}

bool widget_dataview::is_add_allowed()
{
	return (add_allowed && line_add_allowed);
}

void widget_dataview::allow_edit(bool allowed)
{
	edit_allowed = allowed;
	if (!allowed)
	{
		footer->setCurrentWidget(footer_empty);
	}
}

bool widget_dataview::is_edit_allowed()
{
	return (edit_allowed && line_edit_allowed);
}

void widget_dataview::allow_delete(bool allowed)
{
	delete_allowed = allowed;
	if (!allowed)
	{
		img_corner_del->setImageLink("img/dataview/empty.png");
		img_corner_del->setToolTip("");
		img_corner_del->decorationStyle().setCursor(Cursor::Arrow);
	}
	else
	{
		img_corner_del->setImageLink("img/dataview/trash.png");
		img_corner_del->setToolTip("Supprimer cet élément");
		img_corner_del->decorationStyle().setCursor(Cursor::Arrow);
	}
}

bool widget_dataview::is_delete_allowed()
{
	return (delete_allowed && line_delete_allowed);
}

void widget_dataview::line_allow_add(bool allowed)
{
	line_add_allowed = allowed;
}

void widget_dataview::line_allow_edit(bool allowed)
{
	line_edit_allowed = allowed;
}

void widget_dataview::line_allow_delete(bool allowed)
{
	line_delete_allowed = allowed;
}

void widget_dataview::confirm_delete(bool confim, string message)
{
	need_confirm_delete = confim;
	if (message != "") confirm_delete_message = message;
	else confirm_delete_message = "Voulez-vous vraiment supprimer cette ligne ?";
}

WText* widget_dataview::add_footer_custom(string text)
{
	int count = footer_custom->layout->columnCount();
	auto ret = footer_custom->addLayout<WText>(0, count, Align::Middle|Align::Center);
	ret->addStyleClass("dataview-footer-button");
	ret->setText(text);
	return ret;
}

void widget_dataview::resize(const WLength& width, const WLength& height)
{
	if (!data)
	{
		widget_div::resize(width, height);
		return; // not initialized
	}
	// compute width, ignore param
	std::ignore = width;

	// sum + add border x2
	int sum = data->computed_width() + 2;

	// update panel height
	panel_height = height.value() - header->height().value() - footer->height().value();
	if (panel_height < 0) panel_height = 0;

	// call parent resize
	widget_div::resize(sum, height);
}

widget_dataview::~widget_dataview()
{
}

/****    data_table    ****/

data_table::data_table()
{
	div_titles = addLayout<widget_div>(0, 0);
	div_lines = addLayout<widget_div>(1, 0);
}

unique_ptr<widget_div> data_table::generate_add_view()
{
	auto ret = make_unique<widget_div>();
	return ret;
}

unique_ptr<widget_div> data_table::generate_edit_view()
{
	auto ret = make_unique<widget_div>();
	return ret;
}

void data_table::on_add_event()
{
}

void data_table::on_edit_event()
{
}

void data_table::on_del_event()
{
}

void data_table::on_line_click(int index)
{
	// if selected_index != -1, remove css from previous
	bool previous_selected = false;
	if (selected_index != -1)
	{
		previous_selected = lines[selected_index]->hasStyleClass("data-line-selected");
		lines[selected_index]->removeStyleClass("data-line-selected");
	}

	// special case, to remove selection
	if (index == selected_index && previous_selected)
	{
		selected_index = -1;
		// emit signal
		selection_change_event.emit(selected_index);
		return;
	}

	selected_index = index;
	lines[index]->addStyleClass("data-line-selected");

	// emit signal
	selection_change_event.emit(selected_index);
}

void data_table::select_none()
{
	if (selected_index != -1)
	{
		lines[selected_index]->removeStyleClass("data-line-selected");
	}
	selected_index = -1;

	// emit signal
	selection_change_event.emit(selected_index);
}

void data_table::apply_titles_style()
{
	int i = 0;
	for (auto &t : titles)
	{
		t->resize(sizes[i], WLength::Auto);
		t->on_filter_change.connect(this, &data_table::generate_lines);
		i++;
	}
}

void data_table::apply_lines_style()
{
	int i = 0;
	for (auto &l : lines)
	{
		l->addStyleClass("data-line data-line-" + convert::int_string(i % 2));

		// add paddings to each line's element, and border (if not last)
		for (int pos = 0; pos < sizes.size(); ++pos)
		{
			auto current = l->layout->itemAt(pos)->widget();
			current->setWidth(sizes[pos]);
			current->setMinimumSize(sizes[pos], WLength::Auto);
			current->addStyleClass("data-line-content");
			if (pos != sizes.size() - 1)
			{
				current->addStyleClass("data-line-content-border");
			}
		}

		// signal binding for line click
		l->clicked().connect(bind(&data_table::on_line_click, this, i));
		i++;
	}
}

void data_table::select_first_line()
{
	if (lines.size() > 0) on_line_click(0);
}

int data_table::computed_width()
{
	int sum = 0;
	for (auto &n : sizes)
		sum += n;
	return sum;
}

data_table::~data_table()
{
}

/****    filter_dataview    ****/

filter_dataview::filter_dataview()
{
	div_status_title = addNew<widget_div>();
	status = div_status_title->addLayout<WImage>(0, 0, Align::Middle|Align::Left);
	title = div_status_title->addLayout<WText>(0, 1);

	// style and data
	setStyleClass("filter-dataview");
	div_status_title->decorationStyle().setCursor(Cursor::PointingHand);
	div_status_title->layout->setColumnStretch(1, 1);
	status->setImageLink("img/dataview/filter.png");

	// set default to div_status_title
	setCurrentWidget(div_status_title);

	// signals binding
	div_status_title->clicked().connect(this, &filter_dataview::on_filter_click);
}

filter_dataview_edit::filter_dataview_edit():filter_dataview()
{
	edit_filter = addNew<WLineEdit>();
	edit_filter->enterPressed().connect(this, &filter_dataview_edit::on_enter_pressed);
	edit_filter->escapePressed().connect(this, &filter_dataview_edit::on_enter_pressed);
}

filter_dataview_date::filter_dataview_date():filter_dataview()
{
	auto edit_picker = addNew<WLineEdit>();
	auto img_picker = make_unique<WImage>("img/dataview/calendar.png");
	img_picker->resize(25, 25);
	date_filter = addNew<WDatePicker>(move(img_picker), edit_picker);
	date_filter->setFormat("dd/MM/yyyy");
	date_filter->changed().connect(this, &filter_dataview_date::on_changed);
	edit_picker->enterPressed().connect(this, &filter_dataview_date::on_changed);
	edit_picker->escapePressed().connect(this, &filter_dataview_date::on_changed);
}

filter_dataview_combo::filter_dataview_combo():filter_dataview()
{
	combo_filter = addNew<WComboBox>();
	combo_filter->changed().connect(this, &filter_dataview_combo::on_changed);
	combo_filter->enterPressed().connect(this, &filter_dataview_combo::on_changed);
	combo_filter->escapePressed().connect(this, &filter_dataview_combo::on_changed);
}


string filter_dataview_edit::filter()
{
	string filter = edit_filter->text().toUTF8();
	return filter;
}

string filter_dataview_date::filter()
{
	string filter = date_filter->lineEdit()->text().toUTF8();
	return filter;
}

string filter_dataview_combo::filter()
{
	string filter = combo_filter->currentText().toUTF8();
	return filter;
}

void filter_dataview_edit::setValue(string txt)
{
	edit_filter->setText(txt);
}

void filter_dataview_date::setValue(string txt)
{
	date_filter->lineEdit()->setText(txt);
}

void filter_dataview_combo::setValue(string value)
{

	combo_filter->setValueText(value);
}

void filter_dataview_combo::create_combo(vector<string> v)
{
	combo_filter->addItem("");
	for (int i = 0; i < v.size(); ++i)
	{
		combo_filter->addItem(v[i]);
	}
}

void filter_dataview::allow_filter(bool allowed)
{
	is_allowed_filter = allowed;
	if (allowed)
	{
		div_status_title->decorationStyle().setCursor(Cursor::PointingHand);
		update_filter();
		status->resize(30, 30);
	}
	else
	{
		div_status_title->decorationStyle().setCursor(Cursor::Arrow);
		status->setImageLink("");
		status->resize(5, 30);
	}
}

void filter_dataview_edit::update_filter()
{
	// update im
	if (edit_filter->text() == "") status->setImageLink("img/dataview/filter.png");
	else status->setImageLink("img/dataview/filter_green.png");
	}

void filter_dataview_date::update_filter()
{
	// update img
	if (date_filter->lineEdit()->text() == "") status->setImageLink("img/dataview/filter.png");
	else status->setImageLink("img/dataview/filter_green.png");
}

void filter_dataview_combo::update_filter()
{
	// update img
	if (combo_filter->currentIndex() == 0) status->setImageLink("img/dataview/filter.png");
	else status->setImageLink("img/dataview/filter_green.png");
}

void filter_dataview_edit::on_filter_click()
{
	if (!is_allowed_filter) return;
	setCurrentWidget(edit_filter);
	edit_filter->setFocus();
}

void filter_dataview_date::on_filter_click()
{
	if (!is_allowed_filter) return;
	setCurrentWidget(date_filter);
	date_filter->setFocus();
}

void filter_dataview_combo::on_filter_click()
{
	if (!is_allowed_filter) return;
	setCurrentWidget(combo_filter);
	combo_filter->setFocus();
}

void filter_dataview_edit::on_enter_pressed()
{
	setCurrentWidget(div_status_title);
	//
	// remove % and '
	string new_filter = edit_filter->text().toUTF8();
	new_filter = str_replace("%", "", new_filter);

	update_filter();
	on_filter_change.emit();
}

void filter_dataview_date::on_changed()
{
	setCurrentWidget(div_status_title);
	//
	// remove % and '
	string new_filter = date_filter->lineEdit()->text().toUTF8();
	new_filter = str_replace("%", "", new_filter);

	update_filter();
	on_filter_change.emit();
}

void filter_dataview_combo::on_changed()
{
	setCurrentWidget(div_status_title);
	//
	// remove % and '
	string new_filter = combo_filter->currentText().toUTF8();
	new_filter = str_replace("%", "", new_filter);

	update_filter();
	on_filter_change.emit();
}

void filter_dataview_edit::resize(const WLength& width, const WLength& height)
{
	div_status_title->resize(width, height);

	int edit_width = width.value() - 9; // 9 = 1 (border) + 1 (border) + 6 (padding) + 1 (border)
	if (edit_width < 20) edit_width = 20;
	edit_filter->setWidth(edit_width);

	WStackedWidget::resize(width, height);
}


void filter_dataview_date::resize(const WLength& width, const WLength& height)
{
	div_status_title->resize(width, height);

	int date_width = width.value() - 9; // 9 = 1 (border) + 1 (border) + 6 (padding) + 1 (border)
	if (date_width < 20) date_width = 20;
	date_filter->setWidth(date_width);

	WStackedWidget::resize(width, height);
}

void filter_dataview_combo::resize(const WLength& width, const WLength& height)
{
	div_status_title->resize(width, height);

	int combo_width = width.value() - 9; // 9 = 1 (border) + 1 (border) + 6 (padding) + 1 (border)
	if (combo_width < 20) combo_width = 20;
	combo_filter->setWidth(combo_width);

	WStackedWidget::resize(width, height);
}


void filter_dataview::setWidth(const WLength& width)
{
	resize(width, height());
}

filter_dataview::~filter_dataview()
{
}

filter_dataview_edit::~filter_dataview_edit()
{
}

filter_dataview_date::~filter_dataview_date()
{
}


filter_dataview_combo::~filter_dataview_combo()
{
}

} // namespace
