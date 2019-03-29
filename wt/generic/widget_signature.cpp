#include "widget_signature.h"
#include <fstream>
#include <Wt/WCssDecorationStyle.h>

namespace mana
{

widget_signature::widget_signature(int width, int height) :
	WPaintedWidget(),
	png_image("png", width, height)
{
	resize(width, height);
	setColor(WColor(0,0,0));

	// white background
	decorationStyle().setBackgroundColor(WColor(255,255,255));

	mouseDragged().connect(this, &widget_signature::mouseDrag);
	mouseWentDown().connect(this, &widget_signature::mouseDown);
	touchStarted().connect(this, &widget_signature::touchStart);
	touchMoved().connect(this, &widget_signature::touchMove);
	touchMoved().preventDefaultAction();

	WPainter painter_background(&png_image);
	painter_background.fillRect(WRectF(0, 0, width, height), WBrush(WColor(255,255,255)));
}

void widget_signature::clear()
{
	update();
}

void widget_signature::save(string filename)
{
	// save signature to png
	std::ofstream outfile(filename, std::ios::out | std::ios::binary);
	png_image.write(outfile);
}

void widget_signature::setColor(const WColor& c)
{
	color_ = c;
}

void widget_signature::paintEvent(WPaintDevice *paintDevice)
{
	WPainter painter(paintDevice);
	// internal painter for png export
	WPainter painter_internal(&png_image);

	painter.setRenderHint(RenderHint::Antialiasing);
	painter_internal.setRenderHint(RenderHint::Antialiasing);

	WPen pen;
	pen.setWidth(3);
	pen.setColor(color_);
	pen.setCapStyle(PenCapStyle::Flat);
	pen.setJoinStyle(PenJoinStyle::Miter);
	painter.setPen(pen);
	painter.drawPath(painter_path);

	// do the same thing on our internal painter
	painter_internal.setPen(pen);
	painter_internal.drawPath(painter_path);

	painter_path = WPainterPath(painter_path.currentPosition());
}


void widget_signature::mouseDown(const WMouseEvent& e)
{
	Coordinates c = e.widget();
	painter_path = WPainterPath(WPointF(c.x, c.y));
}

void widget_signature::mouseDrag(const WMouseEvent& e)
{
	Coordinates c = e.widget();
	painter_path.lineTo(c.x, c.y);
	update(PaintFlag::Update);
}

void widget_signature::touchStart(const WTouchEvent& e)
{
	Coordinates c = e.touches()[0].widget();
	painter_path = WPainterPath(WPointF(c.x, c.y));
}

void widget_signature::touchMove(const WTouchEvent& e)
{
	Coordinates c = e.touches()[0].widget();
	painter_path.lineTo(c.x, c.y);
	update(PaintFlag::Update);
}

} // namespace
