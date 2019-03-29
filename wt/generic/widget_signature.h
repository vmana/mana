#ifndef widget_signature_H
#define widget_signature_H

#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainterPath.h>
#include <Wt/WRasterImage.h>

#include "mana.h"

namespace mana
{

using namespace Wt;

class widget_signature : public WPaintedWidget
{
	public:
		WRasterImage png_image;

		widget_signature(int width, int height);
		void clear();
		void setColor(const WColor& c);
		void save(string filename);

	protected:
		virtual void paintEvent(WPaintDevice *paintDevice);

	private:
		WPainterPath painter_path;
		WColor color_;

		void mouseDown(const WMouseEvent& e);
		void mouseDrag(const WMouseEvent& e);
		void touchStart(const WTouchEvent& e);
		void touchMove(const WTouchEvent& e);
};

} // namespace

#endif // widget_signature_H

