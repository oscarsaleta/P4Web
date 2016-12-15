/*  P4 (Polynomial Planar Phase Portraits) WEB VERSION SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4Web
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PlotRegion.h"

#include "P4src/file_tab.h"

#include <Wt/WBrush>
#include <Wt/WPainter>
#include <Wt/WPainterPath>
#include <Wt/WPointF>


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
    paintBackground(paintDevice);
    paintSingularPoints(paintDevice);
    paintSeparatrices(paintDevice);
}

void PlotRegion::paintBackground(WPaintDevice *paintDevice)
{
    const WColor grs(111,111,111);
    WPainter painter(paintDevice);
    painter.fillRect(0,0,width_,height_,grs);
    WPainterPath path(WPointF(10, 10));
    path.addEllipse(2,2,width_-4,height_-4);
    path.closeSubPath();
    WPen pen(white);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Wt::black);
    painter.drawPath(path);
}

void PlotRegion::paintSingularPoints(WPaintDevice *paintDevice)
{
    return;
}

void PlotRegion::paintSeparatrices(WPaintDevice *paintDevice)
{
    return;
}
