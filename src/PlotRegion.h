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

#ifndef PLOTREGION_H
#define PLOTREGION_H

#include <Wt/WPaintedWidget>

#include <Wt/WPaintDevice>

class PlotRegion : public Wt::WPaintedWidget
{
public:
    PlotRegion(Wt::WContainerWidget *parent = 0, int width = 200, int height = 200);
    ~PlotRegion();
    void setSize(int width, int height);
    
protected:
    void paintEvent(Wt::WPaintDevice *);

private:
    int width_;
    int height_;

    void paintBackground(Wt::WPaintDevice *);
    void paintSingularPoints(Wt::WPaintDevice *);
    void paintSeparatrices(Wt::WPaintDevice *);
};

#endif // PLOTREGION_H