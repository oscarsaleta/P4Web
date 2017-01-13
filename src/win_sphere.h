/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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

#ifndef WIN_SPHERE_H
#define WIN_SPHERE_H

#include "custom.h"
#include "file_tab.h"

#include <Wt/WContainerWidget>
#include <Wt/WPainter>
#include <Wt/WPaintDevice>
//#include <QPixmap>
#include <Wt/WPointF>
//#include <QStatusBar>
#include <Wt/WString>
//#include <QTimer>
#include <Wt/WPaintedWidget>


#define SELECTINGPOINTSTEPS         5
#define SELECTINGPOINTSPEED         150


class WWinSphere : public Wt::WPaintedWidget
{
    static int numSpheres;
    static WWinSphere **SphereList;

public:
    WWinSphere( Wt::WContainerWidget *parent=0, int width=255, int height=255, std::string basename="" );
    ~WWinSphere();

    int width_;                 ///< width of the plotting area
    int height_;                ///< height of the plotting area
    WVFStudy *study_;            ///< WVFStudy class where results from Maple are stored
    std::string basename_;
    
    //void paintSphere();

    int coWinX( double x );     // coordinate changes: from world to windows coordinates
    int coWinY( double y );
    double coWorldX( int x );   // from windows to world coordinates
    double coWorldY( int y );
    int coWinV( double );
    int coWinH( double );

    double horPixelsPerMM;
    double verPixelsPerMM;

    double x0, y0;              // world-coordinates of upper-left corner
    double x1, y1;              // world-coordinates of upper-right corner
    double dx;                  // x1-x0
    double dy;                  // y1-y0

    //QPixmap * PainterCache;
    bool isPainterCacheDirty;
    int paintedXMin;            // to know the update rectangle after painting
    int paintedXMax;            // we keep to smallest rectangle enclosing
    int paintedYMin;            // all painted objects.
    int paintedYMax;

    Wt::WString chartstring;

protected:
    void paintEvent( Wt::WPaintDevice * );


private:
    Wt::WPainter * staticPainter;
    Wt::WContainerWidget * parentWnd;
    bool iszoom;
    bool ReverseYaxis;          // when calculating coordinates: this determines orientation
                                // of horizontal axis.  Normally false, only true when printing.
    
    P4POLYLINES * CircleAtInfinity;
    P4POLYLINES * PLCircle;
    //QTimer * refreshTimeout;

public:
    int spherebgcolor;
    WWinSphere * next;          // visible to PlotWnd
    int SelectingX, SelectingY, SelectingPointStep, SelectingPointRadius;
    //QTimer * SelectingTimer;

    bool getChartPos( int, double, double, double * );
    void adjustToNewSize( void );

    void Signal_Evaluating( void );
    void Signal_Changed( void );
    void plotPoint( struct saddle * );
    void plotPoint( struct node * );
    void plotPoint( struct semi_elementary * );
    void plotPoint( struct weak_focus * );
    void plotPoint( struct strong_focus * );
    void plotPoint( struct degenerate * );
    void plotPointSeparatrices( struct semi_elementary * p );
    void plotPointSeparatrices( struct saddle * p );
    void plotPointSeparatrices( struct degenerate * p );
    void plotPoints( void );
    void plotSeparatrices( void );
    void plotGcf( void );
    void plotPoincareSphere( void );
    void plotPoincareLyapunovSphere( void );
    void plotLineAtInfinity( void );
    void MarkSelection( int x1, int y1, int x2, int y2, int selectiontype );

    P4POLYLINES *produceEllipse( double cx, double cy, double a, double b, bool dotted, double resa, double resb );

    void prepareDrawing( void );
    void drawPoint( double x, double y, int color );
    void drawLine( double x1, double y1, double x2, double y2, int color );
    void finishDrawing( void );

    void SaveAnchorMap( void );
    void LoadAnchorMap( void );

public slots:
    void SetupPlot( void );
    void refresh( void );
    void CalculateHeightFromWidth( int * width, int * height, int maxheight, double aspectratio );
    //void refreshAfterResize( void );
    void updatePointSelection( void );

private:
    bool selectingZoom;
    bool selectingLCSection;
    Wt::WPointF zoomAnchor1;
    Wt::WPointF zoomAnchor2;
    Wt::WPointF lcAnchor1;
    Wt::WPointF lcAnchor2;
    //QPixmap * AnchorMap;

    //QStatusBar * msgBar;
    int PrintMethod;

public:
    int oldw;                       // used while printing
    int oldh;
    int w;                          // width of window
    int h;                          // height of window
    int idealh;                     // ideal height of window to get good aspect ratio
};


#endif /* WIN_SPHERE_H */
