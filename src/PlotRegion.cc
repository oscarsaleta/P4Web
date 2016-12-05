#include "PlotRegion.h"

#include <Wt/WBrush>
#include <Wt/WPainter>
#include <Wt/WPainterPath>


using namespace Wt;

PlotRegion::PlotRegion(WContainerWidget *parent, int width, int height) : width_(width), height_(height)
{
    resize(width_,height_);
    update();
}

PlotRegion::~PlotRegion()
{

}

void PlotRegion::setSize(int width, int height)
{
    width_ = width;
    height_ = height_;
    resize(width_,height_);
    update();
}

void PlotRegion::paintEvent(WPaintDevice *paintDevice)
{
    const WColor grs(111,111,111);
    WPainter painter(paintDevice);
    painter.fillRect(0,0,width_,height_,grs);
    Wt::WPainterPath path(Wt::WPointF(10, 10));
    path.addEllipse(2,2,width_-4,height_-4);
    //path.lineTo(30, 20);
    path.closeSubPath();
    WPen pen(Wt::white);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Wt::black);
    painter.drawPath(path);
}

/*void PlotRegion::paintCircle(int radius)
{
    WPainter painter();
    painter.drawEllipse(550/2,550/2,radius*0.5,radius*0.5);
}*/