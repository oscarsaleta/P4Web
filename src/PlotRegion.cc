#include "PlotRegion.h"

#include <Wt/WPainter>

using namespace Wt;

PlotRegion::PlotRegion(WContainerWidget *parent, int width, int height) : width_(width), height_(height)
{
    resize(width_,height_);
    update();
}

PlotRegion::~PlotRegion()
{

}

void PlotRegion::paintEvent(WPaintDevice *paintDevice)
{
    WPainter painter(paintDevice);
    painter.drawRect(0,0,width_,height_);
}