#ifndef WIDGET_DATAVIEW_H
#define WIDGET_DATAVIEW_H

#include "wt/generic/widget_div.h"
#include <Wt/WStackedWidget.h>
#include <Wt/WText.h>
#include <Wt/WImage.h>
#include <Wt/WLineEdit.h>

class data_table;
class filter_dataview;

class widget_dataview : public widget_div
{
	protected:
		widget_div *header;
		WContainerWidget *div_data;
		data_table *data = NULL;
		int panel_height = 0; // update on resize
		bool add_allowed = true;
		bool edit_allowed = true;

		void on_data_selection_change(int index);
		void hide_corner();
		void show_corner();

	public:

		WText *title;
		// panel contains widgets on edit or add
		WContainerWidget *panel;
		widget_div *p_panel = NULL;

		WStackedWidget *button_corner;
		WImage *img_corner_empty, *img_corner_add, *img_corner_cancel, *img_corner_del;

		WStackedWidget *footer;
		widget_div *footer_empty, *footer_edit, *footer_add, *footer_cancel_valid;
		// direct access buttons
		WText *button_valid, *button_cancel;

		widget_dataview();
		widget_dataview(bool connect_signals);
		// return a pointer to inserted data_table
		data_table* set_data(unique_ptr<data_table> dt);
		widget_div* set_raw_data(unique_ptr<widget_div> dt);

		void allow_vertical_scroll(bool allow);

		virtual void on_corner_add_click();
		virtual void on_corner_cancel_click();
		virtual void on_corner_del_click();

		virtual void on_footer_edit_click();
		virtual void on_footer_add_click();
		virtual void on_footer_cancel_click();
		virtual void on_footer_valid_click();

		void allow_add(bool allowed);
		void allow_edit(bool allowed);
		void allow_add_edit(bool allowed);
		bool is_edit_allowed();
		bool is_add_allowed();

		// signals
		/* Signal<> selection_change_event; */

		// ignore width, compute with sum of data->sizes
		void resize(const WLength& width, const WLength& height);
		~widget_dataview();
};

class data_table : public widget_div
{
	protected:
		widget_div *div_titles;
		widget_div *div_lines;

	public:

		vector<widget_div*> lines;
		vector<filter_dataview*> titles;
		vector<int> sizes;
		WContainerWidget *filler; // bottom space filler
		int selected_index = -1;

		data_table();

		virtual void generate_titles() = 0;
		virtual void generate_lines() = 0;
		virtual unique_ptr<widget_div> generate_add_view() = 0;
		virtual unique_ptr<widget_div> generate_edit_view() = 0;

		virtual void on_add_event() = 0;
		virtual void on_edit_event() = 0;
		virtual void on_del_event() = 0;

		virtual void on_line_click(int index);
		void select_none(); // remove selection if any
		void apply_titles_style();
		void apply_lines_style();

		// signals
		Signal<int> selection_change_event;
		Signal<> hide_corner_event;
		Signal<> show_corner_event;

		~data_table();
};

class filter_dataview : public WStackedWidget
{
	protected:
		bool is_allowed_filter = true;

	public:
		WText *title;
		WLineEdit *edit_filter;
		WImage *status;
		widget_div *div_status_title; // status + title

		void allow_filter(bool allowed);
		void update_filter();
		void on_filter_click();
		void on_enter_pressed();

		void resize(const WLength& width, const WLength& height);
		void setWidth(const WLength& width);

		// signals
		Signal<> on_filter_change;

		filter_dataview();
		~filter_dataview();
};



#endif // WIDGET_DATAVIEW_H