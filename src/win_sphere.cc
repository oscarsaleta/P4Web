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

#include "win_sphere.h"

#include "custom.h"
#include "file_tab.h"
#include "math_p4.h"
//#include "math_findpoint.h"
//#include "math_limitcycles.h"
#include "math_orbits.h"
#include "math_separatrice.h"
//#include "math_gcf.h"
#include "plot_points.h"
#include "plot_tools.h"


#include <cmath>
#include <Wt/WPainterPath>

using namespace Wt;


int WWinSphere::numSpheres = 0;
WWinSphere * * WWinSphere::SphereList = nullptr;


/*
    Coordinates on the sphere:

    - in a local study: the coordinates are cartesian
    - in a normal window: the coordinates are mapped to a square of width/height 2.2
      Inside this square, the circle at infinity is displayed as a circle with radius 1.

    - in a zoom window: the coordinates are a part of the coordinates from a normal window.
*/

// parameters _x1,... are irrelevant if isZoom is false

WWinSphere::WWinSphere( WContainerWidget * parent, int width, int height, std::string basename)
    : width_(width), height_(height), basename_(basename)
{
    study_ = new WVFStudy();

    ReverseYaxis = false;
    //PainterCache = nullptr;
    isPainterCacheDirty = true;
    //AnchorMap = nullptr;
    //refreshTimeout = nullptr;
    //SelectingTimer = nullptr;

//    setAttribute( Qt::WA_PaintOnScreen );

    SphereList = (WWinSphere * *)realloc( SphereList, sizeof(WWinSphere *) * (numSpheres+1) );
    SphereList[numSpheres++] = this;
    if( numSpheres > 1 )
    {
        SphereList[numSpheres-2]->next = this;
    }

    parentWnd = parent;
    //setMinimumSize(MINWIDTHPLOTWINDOW,MINHEIGHTPLOTWINDOW);         // THIS IS THE MINIMUM SIZE
    /*setWidth(parent->width());
    setHeight(parent->height());
    w = this->width().toPixels();
    h = this->height().toPixels();*/
    resize(width_,height_);
    w = width_;
    h = height_;
    idealh = w;
    SelectingPointStep =0;

    //horPixelsPerMM = ((double)w) / ((double)widthMM());
    //verPixelsPerMM = ((double)h) / ((double)heightMM());

    //setMouseTracking(true);
    //msgBar = bar;
    selectingZoom = false;
    selectingLCSection = false;
    //setFocusPolicy( Qt::ClickFocus );
    //setWindowFlags( windowFlags() );
    next = nullptr;

    iszoom = false;
    /*iszoom = isZoom;
    if( isZoom )
    {
        x0 = _x1;
        y0 = _y1;
        
        x1 = _x2;
        y1 = _y2;
    }*/

    CircleAtInfinity = nullptr;
    PLCircle = nullptr;
}



/*static WString makechartstring( int p, int q, bool isu1v1chart, bool negchart )
{
    WString buf;

    if( isu1v1chart )
    {
        // make { x = +/- 1/z2^p, y = z1/z2^q }

        if( p != 1 && q != 1 )
            buf.sprintf( "{x=%d/z2^%d,y=z1/z2^%d}", (int)(negchart ? -1 : 1), p, q );
        else if( p == 1 && q != 1 )
            buf.sprintf( "{x=%d/z2,y=z1/z2^%d}", (int)(negchart ? -1 : 1), q );
        else if( p != 1 && q == 1 )
            buf.sprintf( "{x=%d/z2^%d,y=z1/z2}", (int)(negchart ? -1 : 1), p );
        else
            buf.sprintf( "{x=%d/z2,y=z1/z2}", (int)(negchart ? -1 : 1) );
    }
    else
    {
        // make { x = 1/z2^p, y = +/- z1/z2^q }

        if( p != 1 && q != 1 )
            buf.sprintf( "{x=z1/z2^%d,y=%d/z2^%d}", p, (int)(negchart ? -1 : 1), q );
        else if( p == 1 && q != 1 )
            buf.sprintf( "{x=z1/z2,y=%d/z2^%d}", (int)(negchart ? -1 : 1), q );
        else if( p != 1 && q == 1 )
            buf.sprintf( "{x=z1/z2^%d,y=%d/z2}", p, (int)(negchart ? -1 : 1) );
        else
            buf.sprintf( "{x=z1/z2,y=%d/z2}", (int)(negchart ? -1 : 1) );
    }

    return (WString)buf;
}*/

void WWinSphere::SetupPlot( void )
{
    if (!study_->readTables(basename_)) {
        //parent()->printError("Error while reading results. Evaluate the vector field first");
        delete this;
        return;
    } else
        study_->setupCoordinateTransformations();

    struct P4POLYLINES * t;
    /*QPalette palette;

    spherebgcolor=CBACKGROUND;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor) );
    setPalette(palette);*/

    while( CircleAtInfinity != nullptr )
    {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t;//free( t );
        t = nullptr;
    }
    while( PLCircle != nullptr )
    {
        t = PLCircle;
        PLCircle = t->next;
        delete t;//free( t );
        t = nullptr;
    }

    if( !iszoom )
    {
        switch( study_->typeofview )
        {
        case TYPEOFVIEW_PLANE:
        case TYPEOFVIEW_U1:
        case TYPEOFVIEW_U2:
        case TYPEOFVIEW_V1:
        case TYPEOFVIEW_V2:
            x0 = study_->xmin;
            y0 = study_->ymin;
            x1 = study_->xmax;
            y1 = study_->ymax;
            break;
        case TYPEOFVIEW_SPHERE:
            x0 = -1.1;
            y0 = -1.1;
            x1 = 1.1;
            y1 = 1.1;
            break;
        }
    }

    dx = x1-x0;
    dy = y1-y0;

    double idealhd;
    
    idealhd = w;
    idealhd = (idealhd/dx)*dy;

    idealh = (int)(idealhd+.5);

    /*switch( study_->typeofview )
    {
    case TYPEOFVIEW_PLANE:  chartstring = "";   break;
    case TYPEOFVIEW_SPHERE: chartstring = "";   break;
    case TYPEOFVIEW_U1:     chartstring = makechartstring( study_->p, study_->q, true, false ); break;
    case TYPEOFVIEW_U2:     chartstring = makechartstring( study_->p, study_->q, false, false );break;
    case TYPEOFVIEW_V1:     chartstring = makechartstring( study_->p, study_->q, true, true );  break;
    case TYPEOFVIEW_V2:     chartstring = makechartstring( study_->p, study_->q, false, true ); break;
    }*/

    if( study_->typeofview == TYPEOFVIEW_SPHERE )
    {
        CircleAtInfinity = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
        if( study_->plweights )
            PLCircle = produceEllipse( 0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS), coWinV(RADIUS) );
    }
    
    isPainterCacheDirty = true;
}

WWinSphere::~WWinSphere()
{
    int i;

    struct P4POLYLINES * t;
    while( CircleAtInfinity != nullptr )
    {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t;//free( t );
        t = nullptr;
    }
    while( PLCircle != nullptr )
    {
        t = PLCircle;
        PLCircle = t->next;
        delete t;//free( t );
        t = nullptr;
    }

    for( i = 0; i < numSpheres; i++ )
    {
        if( SphereList[i] == this )
            break;
    }
    if( i == numSpheres )
        return;         // error: sphere not found?

    if( i > 0 )
        SphereList[i-1]->next = next;

    if( i < numSpheres-1 )
        memmove( SphereList+i, SphereList+i+1, sizeof(WWinSphere *) * (numSpheres-i-1) );

    numSpheres--;
    
    /*if( PainterCache != nullptr )
    {
        delete PainterCache;
        PainterCache = nullptr;
    }*/
    
    /*if( AnchorMap != nullptr )
    {
        delete AnchorMap;
        AnchorMap = nullptr;
    }*/
    
    /*if( refreshTimeout != nullptr )
    {
        delete refreshTimeout;
        refreshTimeout = nullptr;
    }*/
    /*if( SelectingTimer != nullptr )
    {
        delete SelectingTimer;
        SelectingTimer = nullptr;
    }*/
}

/*void WWinSphere::LoadAnchorMap( void )
{
     int x1,y1;
     int x2,y2;
     int aw,ah;         
     int s;
     if( selectingZoom )
     {
         x1 = zoomAnchor1.x();
         y1 = zoomAnchor1.y();
         x2 = zoomAnchor2.x();
         y2 = zoomAnchor2.y();
     }
     else if( selectingLCSection )
     {
         x1 = lcAnchor1.x();
         y1 = lcAnchor1.y();
         x2 = lcAnchor2.x();
         y2 = lcAnchor2.y();
     }
     else
         return;
     
     if( x1 > x2 ) { s=x1; x1=x2; x2=s; }
     if( y1 > y2 ) { s=y1; y1=y2; y2=s; }
     if( x1 < 0 ) x1 = 0;
     if( y1 < 0 ) y1 = 0;
     if( x2 >= width() ) x2 = width()-1;
     if( y2 >= height() ) y2 = height()-1;
     
     aw = x2-x1+1;
     ah = y2-y1+1;*/
     
         
/*     WString ms = msgBar->currentMessage();
     WString as;
     as.sprintf( " Load: (%d,%d,%d,%d)", x1,y1,aw,ah );
     msgBar->showMessage(as+ms);
*/     
/*     if( AnchorMap != nullptr )
     {
         if( AnchorMap->width() < aw || AnchorMap->height() < ah )
         {
             delete AnchorMap;
             AnchorMap = nullptr;
             AnchorMap = new QPixmap( aw,ah );
         }
     }
     else
     {
         AnchorMap = new QPixmap( aw,ah );
     }
     
     if( PainterCache == nullptr )
     {
         delete AnchorMap;
         AnchorMap = nullptr;
         return;
     }   

     QPainter paint( AnchorMap );
     if( selectingZoom )
     {
         // only copy rectangular edges, not inside
      //   paint.drawPixmap (    0,    0, aw,  1, *PainterCache, x1, y1, aw, 1 );
    //     paint.drawPixmap (    0, ah-1, aw,  1, *PainterCache, x1, y2, aw, 1 );
  //       paint.drawPixmap (    0,    0,  1, ah, *PainterCache, x1, y1, 1, ah );
//         paint.drawPixmap ( aw-1, 0,     1, ah, *PainterCache, x2, y1, 1, ah );

         paint.drawPixmap ( 0, 0, aw, ah, *PainterCache, x1, y1, aw, ah );

     }
     else
     {
         paint.drawPixmap ( 0, 0, aw, ah, *PainterCache, x1, y1, aw, ah );
     }
}*/

/*void WWinSphere::SaveAnchorMap( void )
{
     int x1,y1;
     int x2,y2;
     int aw,ah;         
     int s;

     if( AnchorMap == nullptr || PainterCache == nullptr || (!selectingZoom && !selectingLCSection) )
         return;

     if( selectingZoom )
     {
         x1 = zoomAnchor1.x();
         y1 = zoomAnchor1.y();
         x2 = zoomAnchor2.x();
         y2 = zoomAnchor2.y();
     }
     if( selectingLCSection )
     {
         x1 = lcAnchor1.x();
         y1 = lcAnchor1.y();
         x2 = lcAnchor2.x();
         y2 = lcAnchor2.y();
     }
     
     if( x1 > x2 ) { s=x1; x1=x2; x2=s; }
     if( y1 > y2 ) { s=y1; y1=y2; y2=s; }
     if( x1 < 0 ) x1 = 0;
     if( y1 < 0 ) y1 = 0;
     if( x2 >= width() ) x2 = width()-1;
     if( y2 >= height() ) y2 = height()-1;
     
     aw = x2-x1+1;
     ah = y2-y1+1;*/
/*         
     WString ms = msgBar->currentMessage();
     WString as;
     as.sprintf( " Save: (%d,%d,%d,%d)", x1,y1,aw,ah );
     msgBar->showMessage(as+ms);
*/
     /*QPainter paint( PainterCache );

     if( selectingZoom )
     {
         // only copy rectangular edges, not inside
//         paint.drawPixmap ( x1, y1, aw,  1, *AnchorMap,    0,    0, aw, 1 );
  //       paint.drawPixmap ( x1, y2, aw,  1, *AnchorMap,    0, ah-1, aw, 1 );
    //     paint.drawPixmap ( x1, y1,  1, ah, *AnchorMap,    0,    0, 1, ah );
      //   paint.drawPixmap ( x2, y1,  1, ah, *AnchorMap, aw-1,    0, 1, ah );
         paint.drawPixmap ( x1, y1, aw, ah, *AnchorMap, 0, 0, aw, ah );
     }
     else
     {
         //paint.drawPixmap ( x1, y1, aw, ah, *AnchorMap, 0, 0, aw, ah );
     }
     
     update( x1,y1,aw,ah );
}*/

/*void WWinSphere::adjustToNewSize(void)
{
    double idealhd;
    double reqratio;
    double ratio;
    WString buf;
    struct P4POLYLINES * t;

    w=width();
    h=height();

    idealhd = w;
    idealhd = (idealhd/dx)*dy;

    idealh = (int)(idealhd+.5);

    reqratio = ( ((double)w)/horPixelsPerMM ) / ( idealh/verPixelsPerMM );
    ratio = ( ((double)w)/horPixelsPerMM ) / ( ((double)h)/verPixelsPerMM );

    buf.sprintf( "Aspect Ratio = %f\n", (float)(ratio/reqratio) );
    //msgBar->showMessage(buf);

    while( CircleAtInfinity != nullptr )
    {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t;//free( t );
        t = nullptr;
    }
    while( PLCircle != nullptr )
    {
        t = PLCircle;
        PLCircle = t->next;
        delete t;//free( t );
        t = nullptr;
    }
    if( study_->typeofview == TYPEOFVIEW_SPHERE )
    {
        CircleAtInfinity = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
        if( study_->plweights )
            PLCircle = produceEllipse( 0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS), coWinV(RADIUS) );
    }

    if( PainterCache != nullptr )
    {
        delete PainterCache;
        PainterCache = nullptr;
        PainterCache = new QPixmap( size() );
        isPainterCacheDirty = false;

        QPainter paint( PainterCache );
        paint.fillRect(0,0,width(),height(), QColor( QXFIGCOLOR(CBACKGROUND) ) );
        
        if( study_->singinf )
            paint.setPen( QXFIGCOLOR(CSING) );
        else
            paint.setPen( QXFIGCOLOR(CLINEATINFINITY) );

        staticPainter = &paint;

        // Mental note: do not use prepareDrawing/FinishDrawing here,
        // since it is not good to do drawing for all spheres every time we
        // get a paint event from windows

        if( study_->typeofview != TYPEOFVIEW_PLANE )
        {
            if( study_->typeofview == TYPEOFVIEW_SPHERE )
            {
                if( study_->plweights )
                    plotPoincareLyapunovSphere();
                else
                    plotPoincareSphere();
            }
            else
                plotLineAtInfinity();
        }
        // during resizing : only plot essential information
//      plotSeparatrices();
//      plotGcf();
//      DrawOrbits(this);
//      DrawLimitCycles(this);
        plotPoints();

        QColor c  = QXFIGCOLOR(WHITE);
        c.setAlpha(128);
        paint.setPen(c);
        paint.drawText( 0, 0, width(), height(),
                           Qt::AlignHCenter | Qt::AlignVCenter, "Resizing ...  " );

        staticPainter = nullptr;*/

        /*if( refreshTimeout != nullptr )
            refreshTimeout->stop();
        else
        {
            refreshTimeout = new QTimer();
            connect( refreshTimeout, SIGNAL(timeout()), this, SLOT(refreshAfterResize()) );
        }
        refreshTimeout->start( 500 );*/
/*    }
}*/

/*void WWinSphere::refreshAfterResize( void )
{
     if( refreshTimeout != nullptr )
     {
         delete refreshTimeout;
         refreshTimeout = nullptr;
     }
    refresh();
}*/

void WWinSphere::paintEvent( WPaintDevice * p )
{
    //UNUSED(p);
    SetupPlot();
    WPainter paint(p);
    paint.fillRect(0.,0.,width_,height_, WBrush(QXFIGCOLOR(CBACKGROUND)));
    /*if (study_->singinf)
        paint.setPen(WPen(QXFIGCOLOR(CSING)));
    else
        paint.setPen(WPen(QXFIGCOLOR(CLINEATINFINITY)));*/
    staticPainter = &paint;
    if( study_->typeofview != TYPEOFVIEW_PLANE )
    {
        if( study_->typeofview == TYPEOFVIEW_SPHERE )
        {
            if( study_->plweights ) {
                plotPoincareLyapunovSphere(); //not used
            }
            else {
                plotPoincareSphere(); // only one used for now
            }
        }
        else
            plotLineAtInfinity(); //not used
    }
    //plotGcf();
    //drawOrbits(this);
    //drawLimitCycles(this);
    //plotSeparatrices();
    for (int cnt=0;cnt<10;cnt++)
        plot_all_sep(this);
    plotPoints();
    
}


/*void WWinSphere::MarkSelection( int x1, int y1, int x2, int y2, int selectiontype )
{
    int bx1, by1, bx2, by2;
     
    if( PainterCache == nullptr )
          return;
    
    bx1 = (x1<x2) ? x1 : x2;
    bx2 = (x1<x2) ? x2 : x1;
    by1 = (y1<y2) ? y1 : y2;
    by2 = (y1<y2) ? y2 : y1;
    
    if( bx1 < 0 ) bx1 = 0;
    if( by1 < 0 ) by1 = 0;
    if( bx2 >= width() ) bx2 = width()-1;
    if( by2 >= height() ) by2 = height()-1;

    QPainter p(PainterCache); 
    QColor c;

    switch(selectiontype )
    {
    case 0:
        c = QXFIGCOLOR(WHITE);
        c.setAlpha(32);
        p.setPen(QXFIGCOLOR(WHITE));
        p.setBrush(c);
        if( bx1 == bx2 || by1 == by2 )
           p.drawLine( bx1, by1, bx2, by2 );
        else
           p.drawRect( bx1, by1, bx2-bx1, by2-by1 );
        break;
    
    case 1:
        p.setPen( QXFIGCOLOR(WHITE) );
        p.drawLine( x1, y1, x2, y2 );
        break;
    }
    update( bx1, by1, bx2-bx1+1, by2-by1+1 );
}*/


double WWinSphere::coWorldX( int x )
{
    double wx;

    wx = (double)x;
    wx /= (w-1);
    return (wx*dx + x0);
}

double WWinSphere::coWorldY( int y )
{
    double wy;

    wy = (double)(h-1-y);
    wy /= (h-1);
    return (wy*dy + y0);
}

int WWinSphere::coWinX( double x )
{
    double wx;
    int iwx;

    wx = (x-x0)/dx;
    wx *= w-1;

    iwx = (int)(wx+0.5);        // +0.5 to round upwards
    if( iwx >= w )
        iwx = w-1;

    return iwx;
}

int WWinSphere::coWinH( double deltax )
{
    double wx;

    wx = deltax/dx;
    wx *= w-1;
    return (int)(wx+0.5);
}

int WWinSphere::coWinV( double deltay )
{
    double wy;

    wy = deltay/dy;
    wy *= h-1;
    return (int)(wy+0.5);
}

int WWinSphere::coWinY( double y )
{
    double wy;
    int iwy;

    wy = (y-y0)/dy;
    wy *= h-1;

    iwy = (int)(wy+0.5);        // +0.5 to round upwards
    if( iwy >= h )
        iwy = h-1;

    return ( ReverseYaxis ) ? iwy : h-1-iwy;        // on screen: vertical axis orientation is reversed
}



bool WWinSphere::getChartPos( int chart, double x0, double y0, double * pos )
{
    double pcoord[3];

    switch( chart )
    {
    case CHART_R2:
        (study_->*(study_->finite_to_viewcoord))( x0, y0, pos );
        break;
    case CHART_U1:
        (study_->*(study_->U1_to_sphere))( x0, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_U2:
        (study_->*(study_->U2_to_sphere))( x0, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_V1:
        (study_->*(study_->V1_to_sphere))( x0, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_V2:
        (study_->*(study_->V2_to_sphere))( x0, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    }

    return true;
}

/*void WWinSphere::updatePointSelection( void )
{
     if( SelectingPointStep == 0 )
     {
         SelectingPointRadius = 0;
         update( SelectingX - SELECTINGPOINTSTEPS*4, SelectingY - SELECTINGPOINTSTEPS*4,
                              SELECTINGPOINTSTEPS*4*2+1, SELECTINGPOINTSTEPS*4*2+1 );
         return;
     }
     
     SelectingPointStep--;
     SelectingPointRadius = (SELECTINGPOINTSTEPS-SelectingPointStep)*4;
     update( SelectingX - SelectingPointRadius, SelectingY - SelectingPointRadius,
             SelectingPointRadius + SelectingPointRadius + 1,
             SelectingPointRadius + SelectingPointRadius + 1 );
}*/



// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void WWinSphere::plotPoint( struct saddle * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );
        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        win_plot_saddle( staticPainter, x, y );
    }
}

void WWinSphere::plotPoint( struct node * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        if( p->stable == -1 )
            win_plot_stablenode( staticPainter, x, y );
        else
            win_plot_unstablenode( staticPainter, x, y );
    }
}

void WWinSphere::plotPoint( struct weak_focus * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        switch( p->type )
        {
        case FOCUSTYPE_STABLE:
            win_plot_stableweakfocus( staticPainter, x, y );
            break;
        case FOCUSTYPE_UNSTABLE:
            win_plot_unstableweakfocus( staticPainter, x, y );
            break;
        case FOCUSTYPE_CENTER:
            win_plot_center( staticPainter, x, y );
            break;
        default:
            win_plot_weakfocus( staticPainter, x, y );
            break;
        }
    }
}

void WWinSphere::plotPoint( struct strong_focus * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        if( p->stable == -1 )
            win_plot_stablestrongfocus( staticPainter, x, y );
        else
            win_plot_unstablestrongfocus( staticPainter, x, y );
    }
}

void WWinSphere::plotPoint( struct degenerate * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        win_plot_degen( staticPainter, x, y );
    }
}

void WWinSphere::plotPoint( struct semi_elementary * p )
{
    double pos[2];
    int x, y;

    if( p != nullptr )
    {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        switch(p->type)
        {
        case 1: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 2: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 3: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 4: win_plot_sesaddlenode( staticPainter, x, y ); break;
        case 5: win_plot_seunstablenode( staticPainter, x, y ); break;
        case 6: win_plot_sesaddle( staticPainter, x, y ); break;
        case 7: win_plot_sesaddle( staticPainter, x, y ); break;
        case 8: win_plot_sestablenode( staticPainter, x, y ); break;
        }
    }
}

void WWinSphere::plotPoints( void )
{
    struct saddle * sp;
    struct node * np;
    struct weak_focus * wfp;
    struct strong_focus * sfp;
    struct semi_elementary * sep;
    struct degenerate * dp;

    for( sp = study_->first_saddle_point; sp != nullptr; sp = sp->next_saddle )
        plotPoint( sp );
    for( np = study_->first_node_point; np != nullptr; np = np->next_node )
        plotPoint( np );
    for( wfp = study_->first_wf_point; wfp != nullptr; wfp = wfp->next_wf )
        plotPoint( wfp );
    for( sfp = study_->first_sf_point; sfp != nullptr; sfp = sfp->next_sf )
        plotPoint( sfp );
    for( sep = study_->first_se_point; sep != nullptr; sep = sep->next_se )
        plotPoint( sep );
    for( dp = study_->first_de_point; dp != nullptr; dp = dp->next_de )
        plotPoint( dp );
}

void WWinSphere::plotPointSeparatrices( struct semi_elementary * p )
{
    struct sep * separatrice;

    for( separatrice = p->separatrices; separatrice != nullptr; separatrice = separatrice->next_sep )
        draw_sep( this, separatrice->first_sep_point );
}

void WWinSphere::plotPointSeparatrices( struct saddle * p )
{
    struct sep * separatrice;

    for( separatrice = p->separatrices; separatrice != nullptr; separatrice = separatrice->next_sep )
        draw_sep( this, separatrice->first_sep_point );
}

void WWinSphere::plotPointSeparatrices( struct degenerate * p )
{
    struct blow_up_points *blow_up;

    for( blow_up = p->blow_up; blow_up != nullptr; blow_up = blow_up->next_blow_up_point )
    {
        draw_sep( this, blow_up->first_sep_point );
    }
}

void WWinSphere::plotSeparatrices( void )
{
    struct saddle * sp;
    struct semi_elementary * sep;
    struct degenerate * dp;

    for( sp = study_->first_saddle_point; sp != nullptr; sp = sp->next_saddle )
        plotPointSeparatrices( sp );
    for( sep = study_->first_se_point; sep != nullptr; sep = sep->next_se )
        plotPointSeparatrices( sep );
    for( dp = study_->first_de_point; dp != nullptr; dp = dp->next_de )
        plotPointSeparatrices( dp );
}

/*void WWinSphere::plotGcf(void )
{
    draw_gcf( this, study_->gcf_points, CSING, 1 );
}*/


// -----------------------------------------------------------------------
//                          PLOT TOOLS
// -----------------------------------------------------------------------

P4POLYLINES * WWinSphere::produceEllipse( double cx, double cy, double a, double b,
                                          bool dotted, double resa, double resb )
{
    // this is an exact copy of the plotEllipse routine, except that output is stored
    // in a list of points that is dynamically allocated.

    double theta, t1, t2, e, R, x, y, c, prevx, prevy;
    bool d;
    bool doton;
    int dotcount;
    P4POLYLINES * first;
    P4POLYLINES * last;

    prevx = prevy = 0;
    dotcount=0;
    first = nullptr;
    last = nullptr;

    R = (resa < resb) ? resa : resb;
    if( R < 1.0 ) R = 1.0; // protection
    e = 2*acos(1.0-0.5/R);
    if( R*sin(e) > 1.0 )
        e = asin(1.0/R);

    theta = 0;

    d = false;
    doton = true;

//  FILE * fp;
//  fp = fopen( "C:\\test.txt", "wt" );

    while( theta < TWOPI )
    {
//        fprintf( fp, "%f\n", (float)theta );
//        fflush(fp);
        c = (x0-cx)/a;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if( theta >= t1 && theta < t2 )
            {
//                fprintf( fp, "A EXCL [%f %f]\n", (float)t1, (float)t2 );
                theta = t2+e/4; d = false; continue;
            }
        }
        c = (x1-cx)/a;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if( theta < t1 )
            {
//                fprintf( fp, "B EXCL [-infinity %f]\n", (float)t1 );
                theta = t1+e/4; d = false; continue;
            }
            if( theta >= t2 )
            {
//                fprintf( fp, "C EXCL [%f, infinity]\n",  (float)t2 );
                theta = TWOPI+e/4; d = false; break;
            }
        }
        c = (y0-cy)/b;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = asin(c);
            t2 = PI-t1;
            if( t1 < 0 )
            {
                t2 = t1+TWOPI;
                t1 = PI-t1;
                if( theta >= t1 && theta < t2 )
                {
//                    fprintf( fp, "D EXCL [%f %f]\n", (float)t1, (float)t2 );
                    theta = t2+e/4; d = false; continue;
                }
            }
            else
            {
                if( theta < t1 )
                {
//                    fprintf( fp, "E EXCL [-infinity %f]\n", (float)t1 );
                    theta = t1+e/4; d = false; continue;
                }
                if( theta >= t2 )
                {
//                    fprintf( fp, "F EXCL [%f, infinity]\n",  (float)t2 );
                    theta = TWOPI+e/4; d = false; break;
                }
            }
        }
        c = (y1-cy)/b;
        if( c > -1.0 && c < 1.0 )
        {
            t1 = asin(c);
            t2 = PI-t1;
            if( t1 < 0 )
            {
                t2 = t1+TWOPI;
                t1 = PI-t1;
                if( theta < t1 )
                {
//                    fprintf( fp, "G EXCL [-infinity %f]\n", (float)t1 );
                    theta = t1+e/4; d = false; continue;
                }
                if( theta >= t2 )
                {
//                    fprintf( fp, "H EXCL [%f, infinity]\n",  (float)t2 );
                    theta = TWOPI; d = false; break;
                }
            }
            else
            {
                if( theta >= t1 && theta < t2 )
                {
//                    fprintf( fp, "I EXCL [%f %f]\n", (float)t1, (float)t2 );
                    theta = t2+e/4; d = false; continue;
                }
            }
        }

        x = cx + a*cos(theta);
        y = cy + b*sin(theta);
        theta += e;

        // (x,y) is inside view

        if( !d )
        {
            if( doton )
            {
                d = true;
                prevx = x;
                prevy = y;
                dotcount = 0;
                doton = true;
            }
            else
            {
                if( ++dotcount > 7 && dotted )
                {
                    d = false;
                    doton = (doton) ? false:true;
                    dotcount = 0;
                }
            }
        }
        else
        {
            if( doton )
            {
                if( first == nullptr )
                {
                    last = first = new P4POLYLINES;
                    last->next = nullptr;
                }
                else
                {
                    last->next = new P4POLYLINES;
                    last = last->next;
                    last->next = nullptr;
                }

                last->x1 = prevx;
                last->y1 = prevy;
                last->x2 = x;
                last->y2 = y;

                prevx = x;
                prevy = y;
            }
            if( ++dotcount > 7 && dotted )
            {
                d = false;
                doton = (doton) ? false:true;
                dotcount = 0;
            }
        }
    }
//  fclose(fp);
    return first;
}


void WWinSphere::plotPoincareSphere( void )
{
    int color;
    WPainterPath path;
    P4POLYLINES * circlePoint = CircleAtInfinity;
    color = study_->singinf ? CSING : CLINEATINFINITY;
    staticPainter->setPen(QXFIGCOLOR(color));
    while( circlePoint != nullptr )
    {
        path.moveTo(coWinX(circlePoint->x1),coWinY(circlePoint->y1));
        path.lineTo(coWinX(circlePoint->x2),coWinY(circlePoint->y2));
        circlePoint = circlePoint->next;
    }
    path.closeSubPath();
    staticPainter->drawPath(path.crisp());

}

void WWinSphere::plotPoincareLyapunovSphere( void )
{
    int color;
    P4POLYLINES * p;

    p = CircleAtInfinity;
    color = study_->singinf ? CSING : CLINEATINFINITY;

    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != nullptr )
    {
        staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }

    p = PLCircle;
    color = CLINEATINFINITY;

    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != nullptr )
    {
        staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }
    return;
}

void WWinSphere::plotLineAtInfinity( void )
{
    switch( study_->typeofview )
    {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if( x0 < 0.0 && x1 > 0.0 )
        {
            staticPainter->setPen( QXFIGCOLOR(CLINEATINFINITY) );
            staticPainter->drawLine( coWinX(0.0), 0, coWinX(0.0), h-1 );
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if( y0 < 0.0 && y1 > 0.0 )
        {
            staticPainter->setPen( QXFIGCOLOR(CLINEATINFINITY) );
            staticPainter->drawLine( 0, coWinY(0.0), w-1, coWinY(0.0) );
        }

        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}



void WWinSphere::drawLine( double _x1, double _y1, double _x2, double _y2, int color )
{
    int wx1, wy1, wx2, wy2;

    if( staticPainter != nullptr )
    {
        if( _x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1 )
        {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);

            if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 )
            {
                // both points are visible in the window

                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);

                if( paintedXMin > wx1 ) paintedXMin = wx1;
                if( paintedXMax < wx1 ) paintedXMax = wx1;
                if( paintedYMin > wy1 ) paintedYMin = wy1;
                if( paintedYMax < wy1 ) paintedYMax = wy1;
                if( paintedXMin > wx2 ) paintedXMin = wx2;
                if( paintedXMax < wx2 ) paintedXMax = wx2;
                if( paintedYMin > wy2 ) paintedYMin = wy2;
                if( paintedYMax < wy2 ) paintedYMax = wy2;

                staticPainter->setPen( QXFIGCOLOR(color) );
                staticPainter->drawLine( wx1, wy1, wx2, wy2 );
            }
            else
            {
                // only (_x2,_y2) is not visible

                if( lineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
                {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen( QXFIGCOLOR(color) );

                    if( paintedXMin > wx1 ) paintedXMin = wx1;
                    if( paintedXMax < wx1 ) paintedXMax = wx1;
                    if( paintedYMin > wy1 ) paintedYMin = wy1;
                    if( paintedYMax < wy1 ) paintedYMax = wy1;
                    if( paintedXMin > wx2 ) paintedXMin = wx2;
                    if( paintedXMax < wx2 ) paintedXMax = wx2;
                    if( paintedYMin > wy2 ) paintedYMin = wy2;
                    if( paintedYMax < wy2 ) paintedYMax = wy2;

                    staticPainter->drawLine( wx1, wy1, wx2, wy2 );
                }
            }
        }
        else
        {
            if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 )
            {
                // only (_x2,_y2) is visible

                if( lineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
                {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen( QXFIGCOLOR(color) );

                    if( paintedXMin > wx1 ) paintedXMin = wx1;
                    if( paintedXMax < wx1 ) paintedXMax = wx1;
                    if( paintedYMin > wy1 ) paintedYMin = wy1;
                    if( paintedYMax < wy1 ) paintedYMax = wy1;
                    if( paintedXMin > wx2 ) paintedXMin = wx2;
                    if( paintedXMax < wx2 ) paintedXMax = wx2;
                    if( paintedYMin > wy2 ) paintedYMin = wy2;
                    if( paintedYMax < wy2 ) paintedYMax = wy2;

                    staticPainter->drawLine( wx1, wy1, wx2, wy2 );
                }
            }
            else
            {
                // both end points are invisible

                if( lineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) )
                {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen( QXFIGCOLOR(color) );

                    if( paintedXMin > wx1 ) paintedXMin = wx1;
                    if( paintedXMax < wx1 ) paintedXMax = wx1;
                    if( paintedYMin > wy1 ) paintedYMin = wy1;
                    if( paintedYMax < wy1 ) paintedYMax = wy1;
                    if( paintedXMin > wx2 ) paintedXMin = wx2;
                    if( paintedXMax < wx2 ) paintedXMax = wx2;
                    if( paintedYMin > wy2 ) paintedYMin = wy2;
                    if( paintedYMax < wy2 ) paintedYMax = wy2;
                    
                    staticPainter->drawLine( wx1, wy1, wx2, wy2 );
                }
            }
        }
    }
}

void WWinSphere::drawPoint( double x, double y, int color )
{
    int _x, _y;
    if( staticPainter != nullptr )
    {
        if( x < x0 || x > x1 || y < y0 || y > y1 )
            return;
        staticPainter->setPen( QXFIGCOLOR(color) );
        _x=coWinX(x);
        _y=coWinY(y);

        if( paintedXMin > _x ) paintedXMin = _x;
        if( paintedXMax < _x ) paintedXMax = _x;
        if( paintedYMin > _y ) paintedYMin = _y;
        if( paintedYMax < _y ) paintedYMax = _y;

        staticPainter->drawPoint( _x, _y );
    }
}




void WWinSphere::refresh( void )
{
    isPainterCacheDirty = true;
    update();
}

void WWinSphere::CalculateHeightFromWidth( int * width, int * height, int maxheight = -1, double aspectratio = 1  )
{
    // given an optimal width in *width, this procedure calculates the
    // corresponding height in order to maintain the given aspectratio
    // If however the maximum height is violated, then we choose to
    // have the maximum height and calculate the corresponding width.

    // aspect ratio is 

    double w, h;
    
    w = (double)(*width);
    h = w * dy / dx;
    h *= aspectratio;

    if( (int)(h+0.5) <= maxheight || maxheight == -1 )
    {
        *height = (int)(h+0.5);
    }
    else
    {
        *height = maxheight;
        
        h = (double)maxheight;
        w = h * dx /dy;
        w /= aspectratio;

        *width = (int)(w+0.5);
    }
}


/*void WWinSphere::prepareDrawing()
{
    if( PainterCache == nullptr )
    {
        isPainterCacheDirty = true;
        PainterCache = new QPixmap( size() );
    }
    staticPainter = new QPainter(PainterCache);
    
    paintedXMin = width()-1;
    paintedYMin = height()-1;
    paintedXMax = 0;
    paintedYMax = 0;
    
    if( next != nullptr )
        next->prepareDrawing();
}*/

/*void WWinSphere::finishDrawing()
{
    if( next != nullptr )
        next->finishDrawing();

    if( staticPainter != nullptr )
    {
        staticPainter->end();
        delete staticPainter;
        staticPainter = nullptr;

        if( paintedXMin < 0 ) paintedXMin = 0;
        if( paintedXMax >= width() ) paintedXMax = width()-1;
        if( paintedYMin < 0 ) paintedYMin = 0;
        if( paintedYMax >= height() ) paintedYMax = height()-1;

        if( paintedYMax >= paintedYMin && paintedXMax >= paintedXMin )
            update( paintedXMin, paintedYMin, paintedXMax-paintedXMin+1, paintedYMax-paintedYMin+1);
    }
}*/

void WWinSphere::Signal_Evaluating( void )
{
/*
    QPalette palette;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor = CBACKGROUND) );
    setPalette(palette);
*/
}

void WWinSphere::Signal_Changed( void )
{
/*
    QPalette palette;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor = DARKGRAY) );
    setPalette(palette);
*/
}
