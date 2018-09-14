#include "widget_dataview.h"
#include <Wt/WAnimation.h>
#include <Wt/WWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WMessageBox.h>

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
	header->resize(WLength::Auto, 36);

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
	footer->setHeight(30); // needed for panel_height

	// footer empty
	footer_empty = footer->addNew<widget_div>();
	auto text_empty = footer_empty->addLayout<WText>(0, 0, Align::Middle|Align::Center);
	text_empty->setHeight(30);

	// footer edit
	footer_edit = footer->addNew<widget_div>();
	footer_edit->resize(WLength::Auto, 30);
	footer_edit->addStyleClass("dataview-footer-button");
	auto img_edit = footer_edit->addLayout<WImage>(0, 0, Align::Middle|Align::Center);
	auto text_edit = footer_edit->addLayout<WText>(0, 1, Align::Middle|Align::Center);
	footer_edit->layout->setColumnStretch(1, 1);
	// footer edit : widgets
	img_edit->setImageLink("img/dataview/edit.png");
	img_edit->setMargin(6, Side::Left);
	text_edit->setText("EDITER");
	text_edit->setPadding(30, Side::Right);

	// footer add
	footer_add = footer->addNew<widget_div>();
	footer_add->resize(WLength::Auto, 30);
	footer_add->addStyleClass("dataview-footer-button");
	auto img_add = footer_add->addLayout<WImage>(0, 0, Align::Middle|Align::Center);
	auto text_add = footer_add->addLayout<WText>(0, 1, Align::Middle|Align::Center);
	footer_add->layout->setColumnStretch(1, 1);
	// footer add : widgets
	img_add->setImageLink("img/dataview/valid.png");
	img_add->setMargin(6, Side::Left);
	text_add->setText("VALIDER");
	text_add->setPadding(30, Side::Right);

	// footer cancel valid
	footer_cancel_valid = footer->addNew<widget_div>();
	footer_cancel_valid->resize(WLength::Auto, 30);
	footer_cancel_valid->setStyleClass("dataview-footer-cancel-valid");
	button_cancel = footer_cancel_valid->addLayout<WText>(0, 0);
	button_valid = footer_cancel_valid->addLayout<WText>(0, 1);
	// footer cancel valid : widgets
	button_cancel->setText("ANNULER");
	button_valid->setText("VALIDER");
	button_cancel->addStyleClass("dataview-footer-button");
	button_valid->addStyleClass("dataview-footer-button");

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
	data = div_data->addWidget(move(dt));

	// recreate panel, pseudo hidden
	panel = div_data->addNew<WContainerWidget>();
	panel->setStyleClass("dataview-panel");
	panel->setPositionScheme(PositionScheme::Absolute);
	panel->setOffsets(0, Side::Left|Side::Top);
	panel->resize(0, 0);

	// binding
	data->selection_change_event.connect(this, &widget_dataview::on_data_selection_change);
	data->hide_corner_event.connect(this, &widget_dataview::hide_corner);
	data->show_corner_event.connect(this, &widget_dataview::show_corner);

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
	if (index == -1)
	{
		footer->setCurrentWidget(footer_empty);

		show_corner();

		// hide panel, used by external select_none call
		panel->resize("100%", 0);
	}
	else
	{
		if (is_edit_allowed()) footer->setCurrentWidget(footer_edit);
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

void widget_dataview::on_corner_add_click()
{
	button_corner->setCurrentWidget(img_corner_cancel);
	// show panel
	panel->resize("100%", "100%");
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
	panel->resize("100%", 0);

	if (!data) return; // prevents crash if undef
	if (data->selected_index == -1)
	{
		footer->setCurrentWidget(footer_empty);
	}
	else
	{
		if (is_edit_allowed()) footer->setCurrentWidget(footer_edit);
	}
}

void widget_dataview::on_corner_del_click()
{
	show_corner();

	// hide panel
	panel->resize("100%", 0);
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
	panel->resize("100%", "100%");
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
	panel->resize("100%", 0);
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
	panel->resize("100%", 0);

	if (!data) return; // prevents crash if undef
	if (data->selected_index == -1)
	{
		footer->setCurrentWidget(footer_empty);
	}
	else
	{
		if (is_edit_allowed()) footer->setCurrentWidget(footer_edit);
	}
}

void widget_dataview::on_footer_valid_click()
{
	show_corner();

	// hide panel
	panel->resize("100%", 0);
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
	return add_allowed;
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
	return edit_allowed;
}

void widget_dataview::confirm_delete(bool confim, string message)
{
	need_confirm_delete = confim;
	if (message != "") confirm_delete_message = message;
	else confirm_delete_message = "Voulez-vous vraiment supprimer cette ligne ?";
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
	int sum = 0;
	for (auto &n : data->sizes)
		sum += n;

	// add border x2
	sum += 2;

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

data_table::~data_table()
{
}

/****    filter_dataview    ****/

filter_dataview::filter_dataview()
{
	div_status_title = addNew<widget_div>();
	status = div_status_title->addLayout<WImage>(0, 0, Align::Middle|Align::Left);
	title = div_status_title->addLayout<WText>(0, 1);

	edit_filter = addNew<WLineEdit>();

	// style and data
	setStyleClass("filter-dataview");
	div_status_title->decorationStyle().setCursor(Cursor::PointingHand);
	div_status_title->layout->setColumnStretch(1, 1);
	status->setImageLink("img/dataview/filter.png");

	// set default to div_status_title
	setCurrentWidget(div_status_title);

	// signals binding
	div_status_title->clicked().connect(this, &filter_dataview::on_filter_click);
	edit_filter->enterPressed().connect(this, &filter_dataview::on_enter_pressed);
	edit_filter->escapePressed().connect(this, &filter_dataview::on_enter_pressed);
	/* edit_filter->mouseWentOut().connect(this, &filter_dataview::on_enter_pressed); */
;
}

void filter_dataview::allow_filter(bool allowed)
{
	is_allowed_filter = allowed;
	if (allowed)
	{
		div_status_title->decorationStyle().setCursor(Cursor::PointingHand);
		update_filter();
	}
	else
	{
		div_status_title->decorationStyle().setCursor(Cursor::Arrow);
		status->setImageLink("");
		status->resize(30, 30);
	}
}

void filter_dataview::update_filter()
{
	// update img
	if (edit_filter->text() == "") status->setImageLink("img/dataview/filter.png");
	else status->setImageLink("img/dataview/filter_green.png");
}

void filter_dataview::on_filter_click()
{
	if (!is_allowed_filter) return;
	setCurrentWidget(edit_filter);
	edit_filter->setFocus();
}

void filter_dataview::on_enter_pressed()
{
	setCurrentWidget(div_status_title);
	//
	// remove % and '
	string new_filter = edit_filter->text().toUTF8();
	new_filter = str_replace("%", "", new_filter);

	update_filter();
	on_filter_change.emit();
}

void filter_dataview::resize(const WLength& width, const WLength& height)
{
	div_status_title->resize(width, height);

	int edit_width = width.value() - 9; // 9 = 1 (border) + 1 (border) + 6 (padding) + 1 (border)
	if (edit_width < 20) edit_width = 20;
	edit_filter->setWidth(edit_width);

	WStackedWidget::resize(width, height);
}

void filter_dataview::setWidth(const WLength& width)
{
	resize(width, height());
}

filter_dataview::~filter_dataview()
{
}
