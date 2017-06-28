/*  This file is part of WP4 (http://github.com/oscarsaleta/WP4)
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  WP4 is free software: you can redistribute it and/or modify
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
/*  This file is part of P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  P4 is free software: you can redistribute it and/or modify
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

#include "WSphere.h"

#include "custom.h"
#include "file_tab.h"
#include "math_p4.h"
//#include "math_findpoint.h"
//#include "math_limitcycles.h"
#include "MyLogger.h"
#include "math_separatrice.h"
#include "plot_points.h"
#include "plot_tools.h"

#include <Wt/WEvent>
#include <Wt/WPainterPath>
#include <cmath>

using namespace Wt;

/*
    Coordinates on the sphere:

    - in a local study: the coordinates are cartesian
    - in a normal window: the coordinates are mapped to a square of width/height
   2.2
      Inside this square, the circle at infinity is displayed as a circle with
   radius 1.

    - in a zoom window: the coordinates are a part of the coordinates from a
   normal window.
*/

// parameters _x1,... are irrelevant if isZoom is false

WSphere::WSphere(WContainerWidget *parent, ScriptHandler *s, int width,
                 int height, std::string basename, double projection,
                 WVFStudy *study)
    : width_(width), height_(height), basename_(basename),
      projection_(projection)
{
    parentWnd = parent;
    scriptHandler_ = s;

    if (study == nullptr) {
        study_ = new WVFStudy(projection);
        studyCopied_ = false;
    } else {
        study_ = study;
        studyCopied_ = true;
    }

    typeOfView_ = 0;
    plotPrepared_ = false;
    plotDone_ = false;

    ReverseYaxis = false;

    resize(width_, height_);

    next = nullptr;
    CircleAtInfinity = nullptr;
    PLCircle = nullptr;

    gcfEval_ = false;
    gcfTask_ = EVAL_GCF_NONE;
    gcfError_ = false;
    gcfDashes_ = GCF_DASHES;
    gcfNPoints_ = GCF_POINTS;
    gcfPrec_ = GCF_PRECIS;

    gcfEval_ = false;
    gcfTask_ = EVAL_GCF_NONE;
    gcfError_ = false;
    gcfDashes_ = GCF_DASHES;
    gcfNPoints_ = GCF_POINTS;
    gcfPrec_ = GCF_PRECIS;

    mouseMoved().connect(this, &WSphere::mouseMovementEvent);
    clicked().connect(this, &WSphere::mouseClickEvent);
}

WSphere::WSphere(WContainerWidget *parent, ScriptHandler *s, int width,
                 int height, std::string basename, int type, double minx,
                 double maxx, double miny, double maxy, WVFStudy *study)
    : width_(width), height_(height), basename_(basename), typeOfView_(type),
      viewMinX_(minx), viewMaxX_(maxx), viewMinY_(miny), viewMaxY_(maxy)
{
    parentWnd = parent;
    scriptHandler_ = s;

    if (study == nullptr) {
        study_ = new WVFStudy();
        studyCopied_ = false;
    } else {
        study_ = study;
        studyCopied_ = true;
    }

    plotPrepared_ = false;
    plotDone_ = false;

    ReverseYaxis = false;

    resize(width_, height_);

    next = nullptr;
    CircleAtInfinity = nullptr;
    PLCircle = nullptr;

    gcfEval_ = false;
    gcfTask_ = EVAL_GCF_NONE;
    gcfError_ = false;
    gcfDashes_ = GCF_DASHES;
    gcfNPoints_ = GCF_POINTS;
    gcfPrec_ = GCF_PRECIS;

    mouseMoved().connect(this, &WSphere::mouseMovementEvent);
    clicked().connect(this, &WSphere::mouseClickEvent);
}

WSphere::~WSphere()
{
    g_globalLogger.debug("[WSphere] Deleting circle at infinity...");
    struct P4POLYLINES *t;
    while (CircleAtInfinity != nullptr) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        if (t != nullptr) {
            delete t;
            t = nullptr;
        }
    }
    g_globalLogger.debug("[WSphere] Deleting PL circle...");
    while (PLCircle != nullptr) {
        t = PLCircle;
        PLCircle = t->next;
        if (t != nullptr) {
            delete t;
            t = nullptr;
        }
    }
    g_globalLogger.debug("[WSphere] Deleting WVFStudy...");
    if (study_ != nullptr) {
        delete study_;
        study_ = nullptr;
    }

    g_globalLogger.debug("[WSphere] Deleted correctly");
}

bool WSphere::setupPlot(void)
{
    if (plotPrepared_) {
        firstTimePlot_ = false;
        return true;
    } else
        firstTimePlot_ = true;

    if (!studyCopied_ && !study_->readTables(basename_)) {
        return false;
    } else {
        switch (typeOfView_) {
        case 0:
            study_->typeofview_ = TYPEOFVIEW_SPHERE;
            study_->config_projection_ = projection_;
            break;
        case 1:
            study_->typeofview_ = TYPEOFVIEW_PLANE;
            study_->xmin_ = viewMinX_;
            study_->xmax_ = viewMaxX_;
            study_->ymin_ = viewMinY_;
            study_->ymax_ = viewMaxY_;
            break;
        case 2:
            study_->typeofview_ = TYPEOFVIEW_U1;
            study_->xmin_ = viewMinX_;
            study_->xmax_ = viewMaxX_;
            study_->ymin_ = viewMinY_;
            study_->ymax_ = viewMaxY_;
            break;
        case 3:
            study_->typeofview_ = TYPEOFVIEW_V1;
            study_->xmin_ = viewMinX_;
            study_->xmax_ = viewMaxX_;
            study_->ymin_ = viewMinY_;
            study_->ymax_ = viewMaxY_;
            break;
        case 4:
            study_->typeofview_ = TYPEOFVIEW_U2;
            study_->xmin_ = viewMinX_;
            study_->xmax_ = viewMaxX_;
            study_->ymin_ = viewMinY_;
            study_->ymax_ = viewMaxY_;
            break;
        case 5:
            study_->typeofview_ = TYPEOFVIEW_V2;
            study_->xmin_ = viewMinX_;
            study_->xmax_ = viewMaxX_;
            study_->ymin_ = viewMinY_;
            study_->ymax_ = viewMaxY_;
            break;
        }

        paintedXMin=0;
        paintedXMax=width_;
        paintedYMin=0;
        paintedYMax=height_;

        g_globalLogger.debug(
            "[WSphere] Setting up WVFStudy coordinate transformations...");
        study_->setupCoordinateTransformations();
        g_globalLogger.debug("[WSPhere] Transformations set up successfully");
    }

    struct P4POLYLINES *t;
    while (CircleAtInfinity != nullptr) {
        t = CircleAtInfinity;
        CircleAtInfinity = t->next;
        delete t;
        t = nullptr;
    }
    while (PLCircle != nullptr) {
        t = PLCircle;
        PLCircle = t->next;
        delete t;
        t = nullptr;
    }

    switch (study_->typeofview_) {
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V1:
    case TYPEOFVIEW_V2:
        x0 = study_->xmin_;
        y0 = study_->ymin_;
        x1 = study_->xmax_;
        y1 = study_->ymax_;
        break;
    case TYPEOFVIEW_SPHERE:
        x0 = -1.1;
        y0 = -1.1;
        x1 = 1.1;
        y1 = 1.1;
        break;
    }

    dx = x1 - x0;
    dy = y1 - y0;

    switch (study_->typeofview_) {
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        chartString_ = "";
        break;
    case TYPEOFVIEW_U1:
        setChartString(study_->p_, study_->q_, true, false);
        break;
    case TYPEOFVIEW_U2:
        setChartString(study_->p_, study_->q_, false, false);
        break;
    case TYPEOFVIEW_V1:
        setChartString(study_->p_, study_->q_, true, true);
        break;
    case TYPEOFVIEW_V2:
        setChartString(study_->p_, study_->q_, false, true);
        break;
    }

    if (study_->typeofview_ == TYPEOFVIEW_SPHERE) {
        CircleAtInfinity =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (study_->plweights_)
            PLCircle = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
                                      coWinH(RADIUS), coWinV(RADIUS));
    }

    return true;
}

void WSphere::paintEvent(WPaintDevice *p)
{
    if (!(plotPrepared_ = setupPlot())) {
        errorSignal_.emit("Error while reading Maple results, evaluate the "
                          "vector field first. If you did, probably the "
                          "execution ran out of time.");
        return;
    }

    WPainter paint(p);
    staticPainter = &paint;
    if (!plotDone_) {
        paint.fillRect(0., 0., width_, height_,
                       WBrush(QXFIGCOLOR(CBACKGROUND)));
        if (study_->typeofview_ != TYPEOFVIEW_PLANE) {
            if (study_->typeofview_ == TYPEOFVIEW_SPHERE) {
                if (study_->plweights_) {
                    plotPoincareLyapunovSphere();
                } else {
                    plotPoincareSphere();
                }
            } else
                plotLineAtInfinity();
        }
        if (gcfEval_) {
            int result =
                evalGcfStart(gcfFname_, gcfDashes_, gcfNPoints_, gcfPrec_);
            if (!result) {
                g_globalLogger.error("[WSphere] cannot compute Gcf");
            } else {
                // this calls evalGcfContinue at least once
                int i = 0;
                do {
                    result = evalGcfContinue(gcfFname_, GCF_POINTS, GCF_PRECIS);
                    if (gcfError_) {
                        g_globalLogger.error("[WSphere] error while computing "
                                             "evalGcfContinue at step: " +
                                             std::to_string(i));
                        break;
                    }
                    i++;
                } while (!result);
                // finish evaluation
                result = evalGcfFinish();
                if (!result) {
                    g_globalLogger.error(
                        "[WSphere] error while computing evalGcfFinish");
                } else {
                    g_globalLogger.debug("[WSphere] computed Gcf");
                }
            }
            plotGcf();
        }
        // drawLimitCycles(this);
        plotSeparatrices();
        if (firstTimePlot_) {
            for (int cnt = 0; cnt < 10; cnt++) {
                plot_all_sep(this);
            }
        }
        plotPoints();
        drawOrbits();
        plotCurves();
        plotIsoclines();
        plotDone_ = true;
    } else { // only draw orbits
        drawOrbits();
        plotCurves();
        plotIsoclines();
    }
}

void WSphere::setChartString(int p, int q, bool isu1v1chart, bool negchart)
{
    WString buf;
    if (isu1v1chart) {
        // { x = +/- 1/z2^p, y = z1/z2^q }
        if (p != 1 && q != 1)
            buf = WString("{x={1}/z2^{2}, y=z1/z2^{3}}")
                      .arg((int)(negchart ? -1 : 1))
                      .arg(p)
                      .arg(q);
        else if (p == 1 && q != 1)
            buf = WString("{x={1}/z2, y=z1/z2^{2}}")
                      .arg((int)(negchart ? -1 : 1))
                      .arg(q);
        else if (p != 1 && q == 1)
            buf = WString("{x={1}/z2^{2}, y=z1/z2}")
                      .arg((int)(negchart ? -1 : 1))
                      .arg(p);
        else
            buf = WString("{x={1}/z2, y=z1/z2}").arg((int)(negchart ? -1 : 1));
    } else {
        // { x = z1/z2^p, y = +/- 1/z2^q }
        if (p != 1 && q != 1)
            buf = WString("{x=1/z2^{1}, y={2}/z2^{3}}")
                      .arg(p)
                      .arg((int)(negchart ? -1 : 1))
                      .arg(q);
        else if (p == 1 && q != 1)
            buf = WString("{x=1/z2, y={1}/z2^{2}}")
                      .arg((int)(negchart ? -1 : 1))
                      .arg(q);
        else if (p != 1 && q == 1)
            buf = WString("{x=1/z2^{1}, y={2}/z2}")
                      .arg(p)
                      .arg((int)(negchart ? -1 : 1));
        else
            buf = WString("{x=1/z2, y={1}/z2}").arg((int)(negchart ? -1 : 1));
    }
    chartString_ = buf;
}

void WSphere::mouseMovementEvent(WMouseEvent e)
{
    int x, y;
    double wx, wy;
    double ucoord[2];
    WString buf;

    x = e.widget().x;
    y = e.widget().y;

    wx = coWorldX(x);
    wy = coWorldY(y);

    double pcoord[3];
    if ((study_->*(study_->is_valid_viewcoord))(wx, wy, pcoord)) {
        switch (study_->typeofview_) {
        case TYPEOFVIEW_PLANE:
            if (study_->typeofstudy_ == TYPEOFSTUDY_ONE) {
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
            (study_->*(study_->sphere_to_R2))(pcoord[0], pcoord[1], pcoord[2],
                                              ucoord);
            if (study_->p_ == 1 && study_->q_ == 1) {
                buf = WString("The Poincare sphere (x,y) = ({1},{2})")
                          .arg(std::to_string(ucoord[0]))
                          .arg(std::to_string(ucoord[1]));
            } else {
                buf = WString(
                          "The P-L sphere of type ({1},{2})  (x,y) = ({3},{4})")
                          .arg(study_->p_)
                          .arg(study_->q_)
                          .arg(std::to_string(ucoord[0]))
                          .arg(std::to_string(ucoord[1]));
            }
            break;
        case TYPEOFVIEW_U1:
            (study_->*(study_->sphere_to_U1))(pcoord[0], pcoord[1], pcoord[2],
                                              ucoord);
            buf = WString("The {1} chart (z2,z1) = ({2},{3})")
                      .arg((ucoord[1] >= 0 ? "U1" : "V1'"))
                      .arg(std::to_string(ucoord[1]))
                      .arg(std::to_string(ucoord[0]));
            buf += " " + chartString_;
            break;
        case TYPEOFVIEW_V1:
            (study_->*(study_->sphere_to_V1))(pcoord[0], pcoord[1], pcoord[2],
                                              ucoord);
            if (!study_->plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            buf = WString("The {1} chart (z2,z1) = ({2},{3})")
                      .arg((ucoord[1] >= 0) ? "V1" : "U1'")
                      .arg(std::to_string(ucoord[1]))
                      .arg(std::to_string(ucoord[0]));
            buf += " " + chartString_;
            break;
        case TYPEOFVIEW_U2:
            (study_->*(study_->sphere_to_U2))(pcoord[0], pcoord[1], pcoord[2],
                                              ucoord);
            buf = WString("The {1} chart (z1,z2) = ({2},{3})")
                      .arg((ucoord[1] >= 0) ? "U2" : "V2'")
                      .arg(std::to_string(ucoord[0]))
                      .arg(std::to_string(ucoord[1]));
            buf += " " + chartString_;
            break;
        case TYPEOFVIEW_V2:
            (study_->*(study_->sphere_to_V2))(pcoord[0], pcoord[1], pcoord[2],
                                              ucoord);
            if (!study_->plweights_) {
                ucoord[0] = -ucoord[0];
                ucoord[1] = -ucoord[1];
            }
            buf = WString("The {1} chart (z1,z2) = ({2},{3})")
                      .arg((ucoord[1] >= 0) ? "V2" : "U2'")
                      .arg(std::to_string(ucoord[0]))
                      .arg(std::to_string(ucoord[1]));
            buf += " " + chartString_;
            break;
        }
    } else {
        switch (study_->typeofview_) {
        case TYPEOFVIEW_PLANE:
            if (study_->typeofstudy_ == TYPEOFSTUDY_ONE)
                buf = "Local study";
            else
                buf = "Planar view";
            break;
        case TYPEOFVIEW_SPHERE:
            if (study_->p_ == 1 && study_->q_ == 1)
                buf = "The Poincare sphere";
            else {
                buf = WString("The P-L sphere of type ({1},{2})")
                          .arg(study_->p_)
                          .arg(study_->q_);
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
            break;
            ;
        }
    }

    hoverSignal_.emit(buf);
}

void WSphere::mouseClickEvent(WMouseEvent e)
{
    double wx = coWorldX(e.widget().x);
    double wy = coWorldY(e.widget().y);
    double pcoord[3];
    double ucoord[2];
    if ((study_->*(study_->is_valid_viewcoord))(wx, wy, pcoord)) {
        (study_->*(study_->sphere_to_R2))(pcoord[0], pcoord[1], pcoord[2],
                                          ucoord);
        clickedSignal_.emit(true, ucoord[0], ucoord[1]);
    } else {
        clickedSignal_.emit(false, 0, 0);
    }
}

double WSphere::coWorldX(int x)
{
    double wx;

    wx = (double)x;
    wx /= (width_ - 1);
    return (wx * dx + x0);
}

double WSphere::coWorldY(int y)
{
    double wy;

    wy = (double)(height_ - 1 - y);
    wy /= (height_ - 1);
    return (wy * dy + y0);
}

int WSphere::coWinX(double x)
{
    double wx;
    int iwx;

    wx = (x - x0) / dx;
    wx *= width_ - 1;

    iwx = (int)(wx + 0.5); // +0.5 to round upwards
    if (iwx >= width_)
        iwx = width_ - 1;

    return iwx;
}

int WSphere::coWinH(double deltax)
{
    double wx;

    wx = deltax / dx;
    wx *= width_ - 1;
    return (int)(wx + 0.5);
}

int WSphere::coWinV(double deltay)
{
    double wy;

    wy = deltay / dy;
    wy *= height_ - 1;
    return (int)(wy + 0.5);
}

int WSphere::coWinY(double y)
{
    double wy;
    int iwy;

    wy = (y - y0) / dy;
    wy *= height_ - 1;

    iwy = (int)(wy + 0.5); // +0.5 to round upwards
    if (iwy >= height_)
        iwy = height_ - 1;

    return (ReverseYaxis)
               ? iwy
               : height_ - 1 -
                     iwy; // on screen: vertical axis orientation is reversed
}

bool WSphere::getChartPos(int chart, double x1, double y1, double *pos)
{
    double pcoord[3];

    switch (chart) {
    case CHART_R2:
        (study_->*(study_->finite_to_viewcoord))(x1, y1, pos);
        break;
    case CHART_U1:
        (study_->*(study_->U1_to_sphere))(x1, 0, pcoord);
        (study_->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                 pcoord[2], pos);
        break;
    case CHART_U2:
        (study_->*(study_->U2_to_sphere))(x1, 0, pcoord);
        (study_->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                 pcoord[2], pos);
        break;
    case CHART_V1:
        (study_->*(study_->V1_to_sphere))(x1, 0, pcoord);
        (study_->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                 pcoord[2], pos);
        break;
    case CHART_V2:
        (study_->*(study_->V2_to_sphere))(x1, 0, pcoord);
        (study_->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                 pcoord[2], pos);
        break;
    }

    return true;
}

/*void WSphere::updatePointSelection( void )
{
     if( SelectingPointStep == 0 )
     {
         SelectingPointRadius = 0;
         update( SelectingX - SELECTINGPOINTSTEPS*4, SelectingY -
SELECTINGPOINTSTEPS*4,
                              SELECTINGPOINTSTEPS*4*2+1,
SELECTINGPOINTSTEPS*4*2+1 );
         return;
     }

     SelectingPointStep--;
     SelectingPointRadius = (SELECTINGPOINTSTEPS-SelectingPointStep)*4;
     update( SelectingX - SelectingPointRadius, SelectingY -
SelectingPointRadius,
             SelectingPointRadius + SelectingPointRadius + 1,
             SelectingPointRadius + SelectingPointRadius + 1 );
}*/

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------

void WSphere::plotPoint(struct saddle *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);
        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        win_plot_saddle(staticPainter, x, y);
    }
}

void WSphere::plotPoint(struct node *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        if (p->stable == -1)
            win_plot_stablenode(staticPainter, x, y);
        else
            win_plot_unstablenode(staticPainter, x, y);
    }
}

void WSphere::plotPoint(struct weak_focus *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        switch (p->type) {
        case FOCUSTYPE_STABLE:
            win_plot_stableweakfocus(staticPainter, x, y);
            break;
        case FOCUSTYPE_UNSTABLE:
            win_plot_unstableweakfocus(staticPainter, x, y);
            break;
        case FOCUSTYPE_CENTER:
            win_plot_center(staticPainter, x, y);
            break;
        default:
            win_plot_weakfocus(staticPainter, x, y);
            break;
        }
    }
}

void WSphere::plotPoint(struct strong_focus *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        if (p->stable == -1)
            win_plot_stablestrongfocus(staticPainter, x, y);
        else
            win_plot_unstablestrongfocus(staticPainter, x, y);
    }
}

void WSphere::plotPoint(struct degenerate *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        win_plot_degen(staticPainter, x, y);
    }
}

void WSphere::plotPoint(struct semi_elementary *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0 || pos[0] > x1 || pos[1] < y0 || pos[1] > y1)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin > x - SYMBOLWIDTH / 2)
            paintedXMin = x - SYMBOLWIDTH / 2;
        if (paintedXMax < x + SYMBOLWIDTH / 2)
            paintedXMax = x - SYMBOLWIDTH / 2;
        if (paintedYMin > y - SYMBOLHEIGHT / 2)
            paintedYMin = y - SYMBOLHEIGHT / 2;
        if (paintedYMax < y + SYMBOLHEIGHT / 2)
            paintedYMax = y - SYMBOLHEIGHT / 2;

        switch (p->type) {
        case 1:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 2:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 3:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 4:
            win_plot_sesaddlenode(staticPainter, x, y);
            break;
        case 5:
            win_plot_seunstablenode(staticPainter, x, y);
            break;
        case 6:
            win_plot_sesaddle(staticPainter, x, y);
            break;
        case 7:
            win_plot_sesaddle(staticPainter, x, y);
            break;
        case 8:
            win_plot_sestablenode(staticPainter, x, y);
            break;
        }
    }
}

void WSphere::plotPoints(void)
{
    struct saddle *sp;
    struct node *np;
    struct weak_focus *wfp;
    struct strong_focus *sfp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    for (sp = study_->first_saddle_point_; sp != nullptr; sp = sp->next_saddle)
        plotPoint(sp);
    for (np = study_->first_node_point_; np != nullptr; np = np->next_node)
        plotPoint(np);
    for (wfp = study_->first_wf_point_; wfp != nullptr; wfp = wfp->next_wf)
        plotPoint(wfp);
    for (sfp = study_->first_sf_point_; sfp != nullptr; sfp = sfp->next_sf)
        plotPoint(sfp);
    for (sep = study_->first_se_point_; sep != nullptr; sep = sep->next_se)
        plotPoint(sep);
    for (dp = study_->first_de_point_; dp != nullptr; dp = dp->next_de)
        plotPoint(dp);
}

void WSphere::plotPointSeparatrices(struct semi_elementary *p)
{
    struct sep *separatrice;

    for (separatrice = p->separatrices; separatrice != nullptr;
         separatrice = separatrice->next_sep)
        draw_sep(this, separatrice->first_sep_point);
}

void WSphere::plotPointSeparatrices(struct saddle *p)
{
    struct sep *separatrice;

    for (separatrice = p->separatrices; separatrice != nullptr;
         separatrice = separatrice->next_sep)
        draw_sep(this, separatrice->first_sep_point);
}

void WSphere::plotPointSeparatrices(struct degenerate *p)
{
    struct blow_up_points *blow_up;

    for (blow_up = p->blow_up; blow_up != nullptr;
         blow_up = blow_up->next_blow_up_point) {
        draw_sep(this, blow_up->first_sep_point);
    }
}

void WSphere::plotSeparatrices(void)
{
    struct saddle *sp;
    struct semi_elementary *sep;
    struct degenerate *dp;

    for (sp = study_->first_saddle_point_; sp != nullptr; sp = sp->next_saddle)
        plotPointSeparatrices(sp);
    for (sep = study_->first_se_point_; sep != nullptr; sep = sep->next_se)
        plotPointSeparatrices(sep);
    for (dp = study_->first_de_point_; dp != nullptr; dp = dp->next_de)
        plotPointSeparatrices(dp);
}

void WSphere::plotGcf(void) { draw_gcf(study_->gcf_points_, CSING, 1); }

void WSphere::plotCurves(void)
{
    g_globalLogger.debug("[WSphere] plotting curves");
    std::vector<curves>::const_iterator it;
    for (it = study_->curve_vector_.begin(); it != study_->curve_vector_.end();
         it++) {
        draw_curve(it->points, CCURV, 1);
    }
}

void WSphere::plotIsoclines(void)
{
    std::vector<isoclines>::const_iterator it;
    for (it = study_->isocline_vector_.begin();
         it != study_->isocline_vector_.end(); it++) {
        draw_isocline(it->points, it->color, 1);
    }
}

// -----------------------------------------------------------------------
//                          PLOT TOOLS
// -----------------------------------------------------------------------

P4POLYLINES *WSphere::produceEllipse(double cx, double cy, double a, double b,
                                     bool dotted, double resa, double resb)
{
    // this is an exact copy of the plotEllipse routine, except that output is
    // stored
    // in a list of points that is dynamically allocated.

    double theta, t1, t2, e, R, x, y, c, prevx, prevy;
    bool d;
    bool doton;
    int dotcount;
    P4POLYLINES *first;
    P4POLYLINES *last;

    prevx = prevy = 0;
    dotcount = 0;
    first = nullptr;
    last = nullptr;

    R = (resa < resb) ? resa : resb;
    if (R < 1.0)
        R = 1.0; // protection
    e = 2 * acos(1.0 - 0.5 / R);
    if (R * sin(e) > 1.0)
        e = asin(1.0 / R);

    theta = 0;

    d = false;
    doton = true;

    while (theta < TWOPI) {
        c = (x0 - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta >= t1 && theta < t2) {
                theta = t2 + e / 4;
                d = false;
                continue;
            }
        }
        c = (x1 - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta < t1) {
                theta = t1 + e / 4;
                d = false;
                continue;
            }
            if (theta >= t2) {
                theta = TWOPI + e / 4;
                d = false;
                break;
            }
        }
        c = (y0 - cy) / b;
        if (c > -1.0 && c < 1.0) {
            t1 = asin(c);
            t2 = PI - t1;
            if (t1 < 0) {
                t2 = t1 + TWOPI;
                t1 = PI - t1;
                if (theta >= t1 && theta < t2) {
                    theta = t2 + e / 4;
                    d = false;
                    continue;
                }
            } else {
                if (theta < t1) {
                    theta = t1 + e / 4;
                    d = false;
                    continue;
                }
                if (theta >= t2) {
                    theta = TWOPI + e / 4;
                    d = false;
                    break;
                }
            }
        }
        c = (y1 - cy) / b;
        if (c > -1.0 && c < 1.0) {
            t1 = asin(c);
            t2 = PI - t1;
            if (t1 < 0) {
                t2 = t1 + TWOPI;
                t1 = PI - t1;
                if (theta < t1) {
                    theta = t1 + e / 4;
                    d = false;
                    continue;
                }
                if (theta >= t2) {
                    theta = TWOPI;
                    d = false;
                    break;
                }
            } else {
                if (theta >= t1 && theta < t2) {
                    theta = t2 + e / 4;
                    d = false;
                    continue;
                }
            }
        }

        x = cx + a * cos(theta);
        y = cy + b * sin(theta);
        theta += e;

        // (x,y) is inside view

        if (!d) {
            if (doton) {
                d = true;
                prevx = x;
                prevy = y;
                dotcount = 0;
                doton = true;
            } else {
                if (++dotcount > 7 && dotted) {
                    d = false;
                    doton = (doton) ? false : true;
                    dotcount = 0;
                }
            }
        } else {
            if (doton) {
                if (first == nullptr) {
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
            if (++dotcount > 7 && dotted) {
                d = false;
                doton = (doton) ? false : true;
                dotcount = 0;
            }
        }
    }
    return first;
}

void WSphere::plotPoincareSphere(void)
{
    int color;
    WPainterPath path;
    P4POLYLINES *circlePoint = CircleAtInfinity;
    color = study_->singinf_ ? CSING : CLINEATINFINITY;
    staticPainter->setPen(QXFIGCOLOR(color));
    while (circlePoint != nullptr) {
        path.moveTo(coWinX(circlePoint->x1), coWinY(circlePoint->y1));
        path.lineTo(coWinX(circlePoint->x2), coWinY(circlePoint->y2));
        circlePoint = circlePoint->next;
    }
    path.closeSubPath();
    staticPainter->drawPath(path.crisp());
}

void WSphere::plotPoincareLyapunovSphere(void)
{
    int color;
    WPainterPath *path = new WPainterPath();
    P4POLYLINES *p = CircleAtInfinity;
    color = study_->singinf_ ? CSING : CLINEATINFINITY;
    staticPainter->setPen(QXFIGCOLOR(color));
    while (p != nullptr) {
        path->moveTo(coWinX(p->x1), coWinY(p->y1));
        path->lineTo(coWinX(p->x2), coWinY(p->y2));
        p = p->next;
    }
    path->closeSubPath();
    staticPainter->drawPath(path->crisp());

    p = PLCircle;
    color = CLINEATINFINITY;
    path = new WPainterPath();

    staticPainter->setPen(QXFIGCOLOR(color));
    while (p != nullptr) {
        path->moveTo(coWinX(p->x1), coWinY(p->y1));
        path->lineTo(coWinX(p->x2), coWinY(p->y2));
        p = p->next;
    }
    path->closeSubPath();
    staticPainter->drawPath(path->crisp());
    return;
}

void WSphere::plotLineAtInfinity(void)
{
    switch (study_->typeofview_) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0 < 0.0 && x1 > 0.0) {
            staticPainter->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter->drawLine(coWinX(0.0), 0, coWinX(0.0), height_ - 1);
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0 < 0.0 && y1 > 0.0) {
            staticPainter->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter->drawLine(0, coWinY(0.0), width_ - 1, coWinY(0.0));
        }

        break;
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_SPHERE:
        // should not appear
        break;
    }
}

void WSphere::drawLine(double _x1, double _y1, double _x2, double _y2,
                       int color)
{
    int wx1, wy1, wx2, wy2;

    if (staticPainter != nullptr) {
        if (_x1 >= x0 && _x1 <= x1 && _y1 >= y0 && _y1 <= y1) {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);

            if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
                // both points are visible in the window
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);

                if (paintedXMin > wx1)
                    paintedXMin = wx1;
                if (paintedXMax < wx1)
                    paintedXMax = wx1;
                if (paintedYMin > wy1)
                    paintedYMin = wy1;
                if (paintedYMax < wy1)
                    paintedYMax = wy1;
                if (paintedXMin > wx2)
                    paintedXMin = wx2;
                if (paintedXMax < wx2)
                    paintedXMax = wx2;
                if (paintedYMin > wy2)
                    paintedYMin = wy2;
                if (paintedYMax < wy2)
                    paintedYMax = wy2;

                staticPainter->setPen(QXFIGCOLOR(color));
                staticPainter->drawLine(wx1, wy1, wx2, wy2);
            } else {
                // only (_x2,_y2) is not visible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0,
                                           y1)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen(QXFIGCOLOR(color));

                    if (paintedXMin > wx1)
                        paintedXMin = wx1;
                    if (paintedXMax < wx1)
                        paintedXMax = wx1;
                    if (paintedYMin > wy1)
                        paintedYMin = wy1;
                    if (paintedYMax < wy1)
                        paintedYMax = wy1;
                    if (paintedXMin > wx2)
                        paintedXMin = wx2;
                    if (paintedXMax < wx2)
                        paintedXMax = wx2;
                    if (paintedYMin > wy2)
                        paintedYMin = wy2;
                    if (paintedYMax < wy2)
                        paintedYMax = wy2;

                    staticPainter->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        } else {
            if (_x2 >= x0 && _x2 <= x1 && _y2 >= y0 && _y2 <= y1) {
                // only (_x2,_y2) is visible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0,
                                           y1)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen(QXFIGCOLOR(color));

                    if (paintedXMin > wx1)
                        paintedXMin = wx1;
                    if (paintedXMax < wx1)
                        paintedXMax = wx1;
                    if (paintedYMin > wy1)
                        paintedYMin = wy1;
                    if (paintedYMax < wy1)
                        paintedYMax = wy1;
                    if (paintedXMin > wx2)
                        paintedXMin = wx2;
                    if (paintedXMax < wx2)
                        paintedXMax = wx2;
                    if (paintedYMin > wy2)
                        paintedYMin = wy2;
                    if (paintedYMax < wy2)
                        paintedYMax = wy2;

                    staticPainter->drawLine(wx1, wy1, wx2, wy2);
                }
            } else {
                // both end points are invisible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0, x1, y0,
                                           y1)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter->setPen(QXFIGCOLOR(color));

                    if (paintedXMin > wx1)
                        paintedXMin = wx1;
                    if (paintedXMax < wx1)
                        paintedXMax = wx1;
                    if (paintedYMin > wy1)
                        paintedYMin = wy1;
                    if (paintedYMax < wy1)
                        paintedYMax = wy1;
                    if (paintedXMin > wx2)
                        paintedXMin = wx2;
                    if (paintedXMax < wx2)
                        paintedXMax = wx2;
                    if (paintedYMin > wy2)
                        paintedYMin = wy2;
                    if (paintedYMax < wy2)
                        paintedYMax = wy2;

                    staticPainter->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        }
    }
}

void WSphere::drawPoint(double x, double y, int color)
{
    int _x, _y;
    if (staticPainter != nullptr) {
        if (x < x0 || x > x1 || y < y0 || y > y1)
            return;
        staticPainter->setPen(QXFIGCOLOR(color));
        _x = coWinX(x);
        _y = coWinY(y);

        if (paintedXMin > _x)
            paintedXMin = _x;
        if (paintedXMax < _x)
            paintedXMax = _x;
        if (paintedYMin > _y)
            paintedYMin = _y;
        if (paintedYMax < _y)
            paintedYMax = _y;

        staticPainter->drawPoint((double)_x, (double)_y);
    }
}