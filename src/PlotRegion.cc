#include "PlotRegion.h"

#include <Wt/WBrush>
#include <Wt/WPainter>


using namespace Wt;

PlotRegion::PlotRegion(WContainerWidget *parent, int width, int height) : width_(width), height_(height), WPaintedWidget(parent)
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
    const WColor ngr(0,0,0);
    WPainter painter(paintDevice);
    painter.fillRect(0,0,width_,height_,ngr);
}

void PlotRegion::paintCircle(int radius)
{
    WPainter painter();
    painter.drawEllipse(550/2,550/2,radius*0.5,radius*0.5);
}