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
//#include "math_orbits.h"
#include "math_separatrice.h"
#include "MyLogger.h"
//#include "math_gcf.h"
#include "plot_points.h"
#include "plot_tools.h"


#include <cmath>
#include <Wt/WPainterPath>
#include <Wt/WEvent>

using namespace Wt;


//int WWinSphere::numSpheres = 0;
//WWinSphere * * WWinSphere::SphereList = nullptr;


/*
    Coordinates on the sphere:

    - in a local study: the coordinates are cartesian
    - in a normal window: the coordinates are mapped to a square of width/height 2.2
      Inside this square, the circle at infinity is displayed as a circle with radius 1.

    - in a zoom window: the coordinates are a part of the coordinates from a normal window.
*/

// parameters _x1,... are irrelevant if isZoom is false

WWinSphere::WWinSphere( WContainerWidget * parent, int width, int height, std::string basename, double projection) :
    width_(width),
    height_(height),
    basename_(basename),
    parentWnd(parent),
    typeOfView_(0),
    projection_(projection),
    plotPrepared_(false),
    plotDone_(false)
{
    study_ = new WVFStudy(projection);

    ReverseYaxis = false;

    /*SphereList = (WWinSphere * *)realloc( SphereList, sizeof(WWinSphere *) * (numSpheres+1) );
    SphereList[numSpheres++] = this;
    if( numSpheres > 1 ) {
        SphereList[numSpheres-2]->next = this;
    }*/

    resize(width_,height_);
    
    next = nullptr;
    CircleAtInfinity = nullptr;
    PLCircle = nullptr;

    mouseMoved().connect(this,&WWinSphere::mouseMovementEvent);
    clicked().connect(this,&WWinSphere::mouseClickEvent);

}

WWinSphere::WWinSphere( WContainerWidget * parent, int width, int height, std::string basename, int type, double minx, double maxx, double miny, double maxy) :
    width_(width),
    height_(height),
    basename_(basename),
    parentWnd(parent),
    typeOfView_(type),
    viewMinX_(minx),
    viewMaxX_(maxx),
    viewMinY_(miny),
    viewMaxY_(maxy),
    plotPrepared_(false),
    plotDone_(false)
{
    study_ = new WVFStudy();

    ReverseYaxis = false;

    /*SphereList = (WWinSphere * *)realloc( SphereList, sizeof(WWinSphere *) * (numSpheres+1) );
    SphereList[numSpheres++] = this;
    if( numSpheres > 1 ) {
        SphereList[numSpheres-2]->next = this;
    }*/

    resize(width_,height_);
    
    next = nullptr;
    CircleAtInfinity = nullptr;
    PLCircle = nullptr;

    mouseMoved().connect(this,&WWinSphere::mouseMovementEvent);
    clicked().connect(this,&WWinSphere::mouseClickEvent);

}


WWinSphere::~WWinSphere()
{
    int i;

    struct P4POLYLINES * t;
    while( CircleAtInfinity != nullptr ) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t;//free( t );
        t = nullptr;
    }
    while( PLCircle != nullptr ) {
        t = PLCircle;
        PLCircle = t->next;
        delete t;//free( t );
        t = nullptr;
    }

    /*for( i = 0; i < numSpheres; i++ ) {
        if( SphereList[i] == this )
            break;
    }
    if( i == numSpheres )
        return;         // error: sphere not found?

    if( i > 0 )
        SphereList[i-1]->next = next;

    if( i < numSpheres-1 )
        memmove( SphereList+i, SphereList+i+1, sizeof(WWinSphere *) * (numSpheres-i-1) );

    numSpheres--;*/
}


bool WWinSphere::setupPlot( void )
{
    if (plotPrepared_)
        return true;

    if (!study_->readTables(basename_)) {
        //parent()->printError("Error while reading results. Evaluate the vector field first");
        //delete this;
        return false;
    } else {
        switch(typeOfView_) {
        case 0:
            study_->typeofview = TYPEOFVIEW_SPHERE;
            study_->config_projection = projection_;
            break;
        case 1:
            study_->typeofview = TYPEOFVIEW_PLANE;
            study_->xmin = viewMinX_;
            study_->xmax = viewMaxX_;
            study_->ymin = viewMinY_;
            study_->ymax = viewMaxY_;
            break;
        case 2:
            study_->typeofview = TYPEOFVIEW_U1;
            study_->xmin = viewMinX_;
            study_->xmax = viewMaxX_;
            study_->ymin = viewMinY_;
            study_->ymax = viewMaxY_;
            break;
        case 3:
            study_->typeofview = TYPEOFVIEW_V1;
            study_->xmin = viewMinX_;
            study_->xmax = viewMaxX_;
            study_->ymin = viewMinY_;
            study_->ymax = viewMaxY_;
            break;
        case 4:
            study_->typeofview = TYPEOFVIEW_U2;
            study_->xmin = viewMinX_;
            study_->xmax = viewMaxX_;
            study_->ymin = viewMinY_;
            study_->ymax = viewMaxY_;
            break;
        case 5:
            study_->typeofview = TYPEOFVIEW_V2;
            study_->xmin = viewMinX_;
            study_->xmax = viewMaxX_;
            study_->ymin = viewMinY_;
            study_->ymax = viewMaxY_;
            break;
        }
        study_->setupCoordinateTransformations();
    }

    struct P4POLYLINES * t;
    /*QPalette palette;

    spherebgcolor=CBACKGROUND;
    palette.setColor(backgroundRole(), QXFIGCOLOR(spherebgcolor) );
    setPalette(palette);*/

    while( CircleAtInfinity != nullptr ) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t;//free( t );
        t = nullptr;
    }
    while( PLCircle != nullptr ) {
        t = PLCircle;
        PLCircle = t->next;
        delete t;//free( t );
        t = nullptr;
    }

    switch( study_->typeofview ) {
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

    dx = x1-x0;
    dy = y1-y0;


    switch(study_->typeofview) {
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        chartString_ = "";
        break;
    case TYPEOFVIEW_U1:
        setChartString(study_->p,study_->q,true,false);
        break;
    case TYPEOFVIEW_U2:
        setChartString(study_->p,study_->q,false,false);
        break;
    case TYPEOFVIEW_V1:
        setChartString(study_->p,study_->q,true,true);
        break;
    case TYPEOFVIEW_V2:
        setChartString(study_->p,study_->q,false,true);
        break;
    }
    
    if( study_->typeofview == TYPEOFVIEW_SPHERE ) {
        CircleAtInfinity = produceEllipse( 0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0) );
        if( study_->plweights )
            PLCircle = produceEllipse( 0.0, 0.0, RADIUS, RADIUS, true, coWinH(RADIUS), coWinV(RADIUS) );
    }
    return true;
}


void WWinSphere::paintEvent( WPaintDevice * p )
{
    if (!(plotPrepared_=setupPlot())) {
        errorSignal_.emit("Error while reading Maple results, evaluate the vector field first. If you did, probably the execution ran out of time.");
        return;
    }
    WPainter paint(p);
    staticPainter = &paint;
    if (!plotDone_) {
        paint.fillRect(0.,0.,width_,height_, WBrush(QXFIGCOLOR(CBACKGROUND)));
        if( study_->typeofview != TYPEOFVIEW_PLANE ) {
            if( study_->typeofview == TYPEOFVIEW_SPHERE ) {
                if( study_->plweights ) {
                    plotPoincareLyapunovSphere(); //not used
                } else {
                    plotPoincareSphere(); // only one used for now
                }
            } else
                plotLineAtInfinity(); //not used
        }
        //plotGcf();
        //drawLimitCycles(this);
        plotSeparatrices();
        for (int cnt=0;cnt<10;cnt++)
            plot_all_sep(this);
        plotPoints();
    } else {
        drawOrbits();
    }
    plotDone_ = true;
}

void WWinSphere::setChartString( int p, int q, bool isu1v1chart, bool negchart )
{
    WString buf;
    if (isu1v1chart) {
        // { x = +/- 1/z2^p, y = z1/z2^q }
        if (p!=1 && q!=1)
            buf = WString("[x={1}/z2^{2}, y=z1/z2^{3}]").arg((int)(negchart ? -1 : 1)).arg(p).arg(q);
        else if (p==1 && q!=1)
            buf = WString("[x={1}/z2, y=z1/z2^{2}]").arg((int)(negchart ? -1 : 1)).arg(q);
        else if (p!=1 && q==1)
            buf = WString("[x={1}/z2^{2}, y=z1/z2]").arg((int)(negchart ? -1 : 1)).arg(p);
        else
            buf = WString("[x={1}/z2, y=z1/z2]").arg((int)(negchart ? -1 : 1));
    } else {
        // { x = z1/z2^p, y = +/- 1/z2^q }
        if (p!=1 && q!=1)
            buf = WString("[x=1/z2^{1}, y={2}/z2^{3}]").arg(p).arg((int)(negchart ? -1 : 1)).arg(q);
        else if (p==1 && q!=1)
            buf = WString("[x=1/z2, y={1}/z2^{2}]").arg((int)(negchart ? -1 : 1)).arg(q);
        else if (p!=1 && q==1)
            buf = WString("[x=1/z2^{1}, y={2}/z2]").arg(p).arg((int)(negchart ? -1 : 1));
        else
            buf = WString("[x=1/z2, y={1}/z2]").arg((int)(negchart ? -1 : 1));
    }
    chartString_ = buf;
}

void WWinSphere::mouseMovementEvent( WMouseEvent e )
{
    int x,y;
    double wx,wy;
    double ucoord[2];
    WString buf;

    x = e.widget().x;
    y = e.widget().y;

    wx = coWorldX(x);
    wy = coWorldY(y);

    double pcoord[3];
    if ((study_->*(study_->is_valid_viewcoord))(wx,wy,pcoord)) {
        switch (study_->typeofview) {
        case TYPEOFVIEW_PLANE:
            if (study_->typeofstudy == TYPEOFSTUDY_ONE) {
                buf = WString("Local study  (x,y) = ({1},{2})")
                    .arg(std::to_string(wx))
                    .arg(std::to_string(wy));
            } else {
                buf = WString("Planar view  (x,y) = ({1},{2})")
                    .arg(std::to_string(wx))
                    .arg(std::to_string(wy));
            }
            break;
        case TYPEOFVIEW_SPHERE:
            (study_->*(study_->sphere_to_R2))(pcoord[0],pcoord[1],pcoord[2],ucoord);
            if (study_->p == 1 && study_->q == 1) {
                buf = WString("The Poincare sphere (x,y) = ({1},{2})")
                    .arg(std::to_string(ucoord[0]))
                    .arg(std::to_string(ucoord[1]));
            } else {
                buf = WString("The P-L sphere of type ({1},{2})  (x,y) = ({3},{4})")
                    .arg(study_->p)
                    .arg(study_->q)
                    .arg(std::to_string(ucoord[0]))
                    .arg(std::to_string(ucoord[1]));
            }
            break;
        case TYPEOFVIEW_U1:
            (study_->*(study_->sphere_to_U1))(pcoord[0],pcoord[1],pcoord[2],ucoord);
            buf = WString("The {1} chart (z2,z1) = ({2},{3})")
                .arg((ucoord[1]>=0 ? "U1" : "V1'"))
                .arg(std::to_string(ucoord[1]))
                .arg(std::to_string(ucoord[0]));
            buf += " "+chartString_;
            break;
        case TYPEOFVIEW_V1:
            (study_->*(study_->sphere_to_V1))(pcoord[0],pcoord[1],pcoord[2],ucoord);
            if (!study_->plweights) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            buf = WString("The {1} chart (z2,z1) = ({2},{3})")
                .arg((ucoord[1]>=0) ? "V1" : "U1'")
                .arg(std::to_string(ucoord[1]))
                .arg(std::to_string(ucoord[0]));
            buf += " "+chartString_;
            break;
        case TYPEOFVIEW_U2:
            (study_->*(study_->sphere_to_U2))(pcoord[0],pcoord[1],pcoord[2],ucoord);
            buf = WString("The {1} chart (z1,z2) = ({2},{3})")
                .arg((ucoord[1]>=0) ? "U2" : "V2'")
                .arg(std::to_string(ucoord[0]))
                .arg(std::to_string(ucoord[1]));
            buf += " "+chartString_;
            break;
        case TYPEOFVIEW_V2:
            (study_->*(study_->sphere_to_V2))(pcoord[0],pcoord[1],pcoord[2],ucoord);
            if (!study_->plweights) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            buf = WString("The {1} chart (z1,z2) = ({2},{3})")
                .arg((ucoord[1]>=0) ? "V2" : "U2'")
                .arg(std::to_string(ucoord[0]))
                .arg(std::to_string(ucoord[1]));
            buf += " "+chartString_;
            break;
        }
    } else {
        switch (study_->typeofview) {
        case TYPEOFVIEW_PLANE:
            if (study_->typeofstudy == TYPEOFSTUDY_ONE)
                buf = "Local study";
            else
                buf = "Planar view";
            break;
        case TYPEOFVIEW_SPHERE:
            if (study_->p == 1 && study_->q ==1)
                buf = "The Poincare sphere";
            else {
                buf = WString("The P-L sphere of type ({1},{2})")
                    .arg(study_->p)
                    .arg(study_->q);
            }
            break;
        case TYPEOFVIEW_U1:
            buf = "The U1 chart";
            break;
        case TYPEOFVIEW_V1:
            buf = "The V1 chart";
            break;
        case TYPEOFVIEW_U2:
            buf = "The U2 chart";
            break;
        case TYPEOFVIEW_V2:
            buf = "The V2 chart";
            break;;
        }
    }
    
    hoverSignal_.emit(buf);
}

void WWinSphere::mouseClickEvent( WMouseEvent e )
{
    double wx = coWorldX(e.widget().x);
    double wy = coWorldY(e.widget().y);
    double pcoord[3];
    double ucoord[2];
    if ((study_->*(study_->is_valid_viewcoord))(wx,wy,pcoord)) {
        (study_->*(study_->sphere_to_R2))(pcoord[0],pcoord[1],pcoord[2],ucoord);
        clickedSignal_.emit(true,ucoord[0],ucoord[1]);
    } else {
        clickedSignal_.emit(false,0,0);
    }
}



double WWinSphere::coWorldX( int x )
{
    double wx;

    wx = (double)x;
    wx /= (width_-1);
    return (wx*dx + x0);
}

double WWinSphere::coWorldY( int y )
{
    double wy;

    wy = (double)(height_-1-y);
    wy /= (height_-1);
    return (wy*dy + y0);
}

int WWinSphere::coWinX( double x )
{
    double wx;
    int iwx;

    wx = (x-x0)/dx;
    wx *= width_-1;

    iwx = (int)(wx+0.5);        // +0.5 to round upwards
    if( iwx >= width_ )
        iwx = width_-1;

    return iwx;
}

int WWinSphere::coWinH( double deltax )
{
    double wx;

    wx = deltax/dx;
    wx *= width_-1;
    return (int)(wx+0.5);
}

int WWinSphere::coWinV( double deltay )
{
    double wy;

    wy = deltay/dy;
    wy *= height_-1;
    return (int)(wy+0.5);
}

int WWinSphere::coWinY( double y )
{
    double wy;
    int iwy;

    wy = (y-y0)/dy;
    wy *= height_-1;

    iwy = (int)(wy+0.5);        // +0.5 to round upwards
    if( iwy >= height_ )
        iwy = height_-1;

    return ( ReverseYaxis ) ? iwy : height_-1-iwy;        // on screen: vertical axis orientation is reversed
}



bool WWinSphere::getChartPos( int chart, double x1, double y1, double * pos )
{
    double pcoord[3];

    switch( chart ) {
    case CHART_R2:
        (study_->*(study_->finite_to_viewcoord))( x1, y1, pos );
        break;
    case CHART_U1:
        (study_->*(study_->U1_to_sphere))( x1, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_U2:
        (study_->*(study_->U2_to_sphere))( x1, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_V1:
        (study_->*(study_->V1_to_sphere))( x1, 0, pcoord );
        (study_->*(study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], pos );
        break;
    case CHART_V2:
        (study_->*(study_->V2_to_sphere))( x1, 0, pcoord );
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

    if( p != nullptr ) {
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

    if( p != nullptr ) {
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

    if( p != nullptr ) {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        switch( p->type ) {
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

    if( p != nullptr ) {
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

    if( p != nullptr ) {
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

    if( p != nullptr ) {
        getChartPos( p->chart, p->x0, p->y0, pos );

        if( pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1 )
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if( paintedXMin > x - SYMBOLWIDTH/2 ) paintedXMin = x - SYMBOLWIDTH/2;
        if( paintedXMax < x + SYMBOLWIDTH/2 ) paintedXMax = x - SYMBOLWIDTH/2;
        if( paintedYMin > y - SYMBOLHEIGHT/2 ) paintedYMin = y - SYMBOLHEIGHT/2;
        if( paintedYMax < y + SYMBOLHEIGHT/2 ) paintedYMax = y - SYMBOLHEIGHT/2;

        switch(p->type) {
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

    for( blow_up = p->blow_up; blow_up != nullptr; blow_up = blow_up->next_blow_up_point ) {
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

    while( theta < TWOPI ) {
//        fprintf( fp, "%f\n", (float)theta );
//        fflush(fp);
        c = (x0-cx)/a;
        if( c > -1.0 && c < 1.0 ) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if( theta >= t1 && theta < t2 ) {
//                fprintf( fp, "A EXCL [%f %f]\n", (float)t1, (float)t2 );
                theta = t2+e/4; d = false; continue;
            }
        }
        c = (x1-cx)/a;
        if( c > -1.0 && c < 1.0 ) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if( theta < t1 ) {
//                fprintf( fp, "B EXCL [-infinity %f]\n", (float)t1 );
                theta = t1+e/4; d = false; continue;
            }
            if( theta >= t2 ) {
//                fprintf( fp, "C EXCL [%f, infinity]\n",  (float)t2 );
                theta = TWOPI+e/4; d = false; break;
            }
        }
        c = (y0-cy)/b;
        if( c > -1.0 && c < 1.0 ) {
            t1 = asin(c);
            t2 = PI-t1;
            if( t1 < 0 ) {
                t2 = t1+TWOPI;
                t1 = PI-t1;
                if( theta >= t1 && theta < t2 ) {
//                    fprintf( fp, "D EXCL [%f %f]\n", (float)t1, (float)t2 );
                    theta = t2+e/4; d = false; continue;
                }
            } else {
                if( theta < t1 ) {
//                    fprintf( fp, "E EXCL [-infinity %f]\n", (float)t1 );
                    theta = t1+e/4; d = false; continue;
                }
                if( theta >= t2 ) {
//                    fprintf( fp, "F EXCL [%f, infinity]\n",  (float)t2 );
                    theta = TWOPI+e/4; d = false; break;
                }
            }
        }
        c = (y1-cy)/b;
        if( c > -1.0 && c < 1.0 ) {
            t1 = asin(c);
            t2 = PI-t1;
            if( t1 < 0 ) {
                t2 = t1+TWOPI;
                t1 = PI-t1;
                if( theta < t1 ) {
//                    fprintf( fp, "G EXCL [-infinity %f]\n", (float)t1 );
                    theta = t1+e/4; d = false; continue;
                }
                if( theta >= t2 ) {
//                    fprintf( fp, "H EXCL [%f, infinity]\n",  (float)t2 );
                    theta = TWOPI; d = false; break;
                }
            } else {
                if( theta >= t1 && theta < t2 ) {
//                    fprintf( fp, "I EXCL [%f %f]\n", (float)t1, (float)t2 );
                    theta = t2+e/4; d = false; continue;
                }
            }
        }

        x = cx + a*cos(theta);
        y = cy + b*sin(theta);
        theta += e;

        // (x,y) is inside view

        if( !d ) {
            if( doton ) {
                d = true;
                prevx = x;
                prevy = y;
                dotcount = 0;
                doton = true;
            } else {
                if( ++dotcount > 7 && dotted ) {
                    d = false;
                    doton = (doton) ? false:true;
                    dotcount = 0;
                }
            }
        }
        else
        {
            if( doton ) {
                if( first == nullptr ) {
                    last = first = new P4POLYLINES;
                    last->next = nullptr;
                } else {
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
            if( ++dotcount > 7 && dotted ) {
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
    while( circlePoint != nullptr ) {
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
    WPainterPath *path = new WPainterPath();
    P4POLYLINES * p = CircleAtInfinity;
    color = study_->singinf ? CSING : CLINEATINFINITY;
    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != nullptr ) {
        path->moveTo(coWinX(p->x1),coWinY(p->y1));
        path->lineTo(coWinX(p->x2),coWinY(p->y2));
        //staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }
    path->closeSubPath();
    staticPainter->drawPath(path->crisp());

    p = PLCircle;
    color = CLINEATINFINITY;
    path = new WPainterPath();

    staticPainter->setPen( QXFIGCOLOR(color) );
    while( p != nullptr ) {
        path->moveTo(coWinX(p->x1),coWinY(p->y1));
        path->lineTo(coWinX(p->x2),coWinY(p->y2));
        //staticPainter->drawLine( coWinX(p->x1), coWinY(p->y1), coWinX(p->x2), coWinY(p->y2) );
        p = p->next;
    }
    path->closeSubPath();
    staticPainter->drawPath(path->crisp());
    return;
}

void WWinSphere::plotLineAtInfinity( void )
{
    switch( study_->typeofview ) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if( x0 < 0.0 && x1 > 0.0 ) {
            staticPainter->setPen( QXFIGCOLOR(CLINEATINFINITY) );
            staticPainter->drawLine( coWinX(0.0), 0, coWinX(0.0), height_-1 );
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if( y0 < 0.0 && y1 > 0.0 ) {
            staticPainter->setPen( QXFIGCOLOR(CLINEATINFINITY) );
            staticPainter->drawLine( 0, coWinY(0.0), width_-1, coWinY(0.0) );
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

    if( staticPainter != nullptr ) {
        if( _x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1 ) {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);

            if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 ) {
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
            } else {
                // only (_x2,_y2) is not visible

                if( lineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) ) {
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
            if( _x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1 ) {
                // only (_x2,_y2) is visible

                if( lineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) ) {
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
            } else {
                // both end points are invisible

                if( lineRectangleIntersect( _x1, _y1, _x2, _y2, x0, x1, y0, y1 ) ) {
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
    if( staticPainter != nullptr ) {
        if( x < x0 || x > x1 || y < y0 || y > y1 )
            return;
        staticPainter->setPen( QXFIGCOLOR(color) );
        _x=coWinX(x);
        _y=coWinY(y);

        if( paintedXMin > _x )
            paintedXMin = _x;
        if( paintedXMax < _x )
            paintedXMax = _x;
        if( paintedYMin > _y )
            paintedYMin = _y;
        if( paintedYMax < _y )
            paintedYMax = _y;

        staticPainter->drawPoint( (double)_x, (double)_y );
    }
}

