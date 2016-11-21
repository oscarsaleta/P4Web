#include "PlotRegion.h"

#include <Wt/WBrush>
#include <Wt/WPainter>


using namespace Wt;

PlotRegion::PlotRegion(WContainerWidget *parent, int width, int height)
{
    setSize(width,height);
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