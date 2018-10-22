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

WSphere::WSphere(ScriptHandler *s, int width, int height, std::string basename,
                 double projection, std::unique_ptr<WVFStudy> study)
    : scriptHandler_(s), width_(width), height_(height), basename_(basename),
      projection_(projection)
{
    if (!study) {
        study_ = std::make_unique<WVFStudy>(projection);
        studyCopied_ = false;
    } else {
        study_ = std::move(study);
        studyCopied_ = true;
    }

    typeOfView_ = 0;
    plotPrepared_ = false;
    plotDone_ = false;

    reverseYaxis_ = false;

    resize(width_, height_);

    // next = nullptr;
    // circleAtInfinity_ = nullptr;
    // pLCircle_ = nullptr;

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

WSphere::WSphere(ScriptHandler *s, int width, int height, std::string basename,
                 int type, double minx, double maxx, double miny, double maxy,
                 std::unique_ptr<WVFStudy> study)
    : scriptHandler_(s), width_(width), height_(height), basename_(basename),
      typeOfView_(type), viewMinX_(minx), viewMaxX_(maxx), viewMinY_(miny),
      viewMaxY_(maxy)
{

    if (study == nullptr) {
        study_ = std::make_unique<WVFStudy>();
        studyCopied_ = false;
    } else {
        study_ = std::move(study);
        studyCopied_ = true;
    }

    plotPrepared_ = false;
    plotDone_ = false;

    reverseYaxis_ = false;

    resize(width_, height_);

    // next = nullptr;
    // circleAtInfinity_ = nullptr;
    // pLCircle_ = nullptr;

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
    // g_globalLogger.debug("[WSphere] Deleting circle at infinity...");
    // struct P4POLYLINES *t;
    // while (circleAtInfinity_ != nullptr) {
    //     t = circleAtInfinity_;
    //     circleAtInfinity_ = t->next;
    //     if (t != nullptr) {
    //         delete t;
    //         t = nullptr;
    //     }
    // }
    // g_globalLogger.debug("[WSphere] Deleting PL circle...");
    // while (pLCircle_ != nullptr) {
    //     t = pLCircle_;
    //     pLCircle_ = t->next;
    //     if (t != nullptr) {
    //         delete t;
    //         t = nullptr;
    //     }
    // }
    // g_globalLogger.debug("[WSphere] Deleting WVFStudy...");
    // if (study_ != nullptr) {
    //     delete study_;
    //     study_ = nullptr;
    // }

    g_globalLogger.debug("[WSphere] Deleted correctly");
}

bool WSphere::setupPlot()
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

        paintedXMin_ = 0;
        paintedXMax_ = width_;
        paintedYMin_ = 0;
        paintedYMax_ = height_;

        g_globalLogger.debug(
            "[WSphere] Setting up WVFStudy coordinate transformations...");
        study_->setupCoordinateTransformations();
        g_globalLogger.debug("[WSPhere] Transformations set up successfully");
    }

    std::vector<P4POLYLINES>().swap(circleAtInfinity_);
    std::vector<P4POLYLINES>().swap(pLCircle_);
    // struct P4POLYLINES *t;
    // while (circleAtInfinity_ != nullptr) {
    //     t = circleAtInfinity_;
    //     circleAtInfinity_ = t->next;
    //     delete t;
    //     t = nullptr;
    // }
    // while (pLCircle_ != nullptr) {
    //     t = pLCircle_;
    //     pLCircle_ = t->next;
    //     delete t;
    //     t = nullptr;
    // }

    switch (study_->typeofview_) {
    case TYPEOFVIEW_PLANE:
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V1:
    case TYPEOFVIEW_V2:
        x0_ = study_->xmin_;
        y0_ = study_->ymin_;
        x1_ = study_->xmax_;
        y1_ = study_->ymax_;
        break;
    case TYPEOFVIEW_SPHERE:
        x0_ = -1.1;
        y0_ = -1.1;
        x1_ = 1.1;
        y1_ = 1.1;
        break;
    }

    dx_ = x1_ - x0_;
    dy_ = y1_ - y0_;

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
        circleAtInfinity_ =
            produceEllipse(0.0, 0.0, 1.0, 1.0, false, coWinH(1.0), coWinV(1.0));
        if (study_->plweights_)
            pLCircle_ = produceEllipse(0.0, 0.0, RADIUS, RADIUS, true,
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
    staticPainter_ = &paint;
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
    if ((study_.get()->*(study_->is_valid_viewcoord))(wx, wy, pcoord)) {
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
            (study_.get()->*(study_->sphere_to_R2))(pcoord[0], pcoord[1],
                                                    pcoord[2], ucoord);
            if (study_->p_ == 1 && study_->q_ == 1) {
                buf = WString("The Poincare sphere (x,y) = ({1},{2})")
                          .arg(std::to_string(ucoord[0]))
                          .arg(std::to_string(ucoord[1]));
            } else {
                buf = WString("The P-L sphere of type ({1},{2})  (x,y) = "
                              "({3},{4})")
                          .arg(study_->p_)
                          .arg(study_->q_)
                          .arg(std::to_string(ucoord[0]))
                          .arg(std::to_string(ucoord[1]));
            }
            break;
        case TYPEOFVIEW_U1:
            (study_.get()->*(study_->sphere_to_U1))(pcoord[0], pcoord[1],
                                                    pcoord[2], ucoord);
            buf = WString("The {1} chart (z2,z1) = ({2},{3})")
                      .arg((ucoord[1] >= 0 ? "U1" : "V1'"))
                      .arg(std::to_string(ucoord[1]))
                      .arg(std::to_string(ucoord[0]));
            buf += " " + chartString_;
            break;
        case TYPEOFVIEW_V1:
            (study_.get()->*(study_->sphere_to_V1))(pcoord[0], pcoord[1],
                                                    pcoord[2], ucoord);
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
            (study_.get()->*(study_->sphere_to_U2))(pcoord[0], pcoord[1],
                                                    pcoord[2], ucoord);
            buf = WString("The {1} chart (z1,z2) = ({2},{3})")
                      .arg((ucoord[1] >= 0) ? "U2" : "V2'")
                      .arg(std::to_string(ucoord[0]))
                      .arg(std::to_string(ucoord[1]));
            buf += " " + chartString_;
            break;
        case TYPEOFVIEW_V2:
            (study_.get()->*(study_->sphere_to_V2))(pcoord[0], pcoord[1],
                                                    pcoord[2], ucoord);
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
    if ((study_.get()->*(study_->is_valid_viewcoord))(wx, wy, pcoord)) {
        (study_.get()->*(study_->sphere_to_R2))(pcoord[0], pcoord[1], pcoord[2],
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
    return (wx * dx_ + x0_);
}

double WSphere::coWorldY(int y)
{
    double wy;

    wy = (double)(height_ - 1 - y);
    wy /= (height_ - 1);
    return (wy * dy_ + y0_);
}

int WSphere::coWinX(double x)
{
    double wx;
    int iwx;

    wx = (x - x0_) / dx_;
    wx *= width_ - 1;

    iwx = (int)(wx + 0.5); // +0.5 to round upwards
    if (iwx >= width_)
        iwx = width_ - 1;

    return iwx;
}

int WSphere::coWinH(double deltax)
{
    double wx;

    wx = deltax / dx_;
    wx *= width_ - 1;
    return (int)(wx + 0.5);
}

int WSphere::coWinV(double deltay)
{
    double wy;

    wy = deltay / dy_;
    wy *= height_ - 1;
    return (int)(wy + 0.5);
}

int WSphere::coWinY(double y)
{
    double wy;
    int iwy;

    wy = (y - y0_) / dy_;
    wy *= height_ - 1;

    iwy = (int)(wy + 0.5); // +0.5 to round upwards
    if (iwy >= height_)
        iwy = height_ - 1;

    return (reverseYaxis_)
               ? iwy
               : height_ - 1 -
                     iwy; // on screen: vertical axis orientation is reversed
}

bool WSphere::getChartPos(int chart, double x1_, double y1_, double *pos)
{
    double pcoord[3];

    switch (chart) {
    case CHART_R2:
        (study_.get()->*(study_->finite_to_viewcoord))(x1_, y1_, pos);
        break;
    case CHART_U1:
        (study_.get()->*(study_->U1_to_sphere))(x1_, 0, pcoord);
        (study_.get()->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                       pcoord[2], pos);
        break;
    case CHART_U2:
        (study_.get()->*(study_->U2_to_sphere))(x1_, 0, pcoord);
        (study_.get()->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                       pcoord[2], pos);
        break;
    case CHART_V1:
        (study_.get()->*(study_->V1_to_sphere))(x1_, 0, pcoord);
        (study_.get()->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
                                                       pcoord[2], pos);
        break;
    case CHART_V2:
        (study_.get()->*(study_->V2_to_sphere))(x1_, 0, pcoord);
        (study_.get()->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1],
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
        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin_ > x - SYMBOLWIDTH / 2)
            paintedXMin_ = x - SYMBOLWIDTH / 2;
        if (paintedXMax_ < x + SYMBOLWIDTH / 2)
            paintedXMax_ = x - SYMBOLWIDTH / 2;
        if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
            paintedYMin_ = y - SYMBOLHEIGHT / 2;
        if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
            paintedYMax_ = y - SYMBOLHEIGHT / 2;

        win_plot_saddle(staticPainter_, x, y);
    }
}

void WSphere::plotPoint(struct node *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin_ > x - SYMBOLWIDTH / 2)
            paintedXMin_ = x - SYMBOLWIDTH / 2;
        if (paintedXMax_ < x + SYMBOLWIDTH / 2)
            paintedXMax_ = x - SYMBOLWIDTH / 2;
        if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
            paintedYMin_ = y - SYMBOLHEIGHT / 2;
        if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
            paintedYMax_ = y - SYMBOLHEIGHT / 2;

        if (p->stable == -1)
            win_plot_stablenode(staticPainter_, x, y);
        else
            win_plot_unstablenode(staticPainter_, x, y);
    }
}

void WSphere::plotPoint(struct weak_focus *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin_ > x - SYMBOLWIDTH / 2)
            paintedXMin_ = x - SYMBOLWIDTH / 2;
        if (paintedXMax_ < x + SYMBOLWIDTH / 2)
            paintedXMax_ = x - SYMBOLWIDTH / 2;
        if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
            paintedYMin_ = y - SYMBOLHEIGHT / 2;
        if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
            paintedYMax_ = y - SYMBOLHEIGHT / 2;

        switch (p->type) {
        case FOCUSTYPE_STABLE:
            win_plot_stableweakfocus(staticPainter_, x, y);
            break;
        case FOCUSTYPE_UNSTABLE:
            win_plot_unstableweakfocus(staticPainter_, x, y);
            break;
        case FOCUSTYPE_CENTER:
            win_plot_center(staticPainter_, x, y);
            break;
        default:
            win_plot_weakfocus(staticPainter_, x, y);
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

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin_ > x - SYMBOLWIDTH / 2)
            paintedXMin_ = x - SYMBOLWIDTH / 2;
        if (paintedXMax_ < x + SYMBOLWIDTH / 2)
            paintedXMax_ = x - SYMBOLWIDTH / 2;
        if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
            paintedYMin_ = y - SYMBOLHEIGHT / 2;
        if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
            paintedYMax_ = y - SYMBOLHEIGHT / 2;

        if (p->stable == -1)
            win_plot_stablestrongfocus(staticPainter_, x, y);
        else
            win_plot_unstablestrongfocus(staticPainter_, x, y);
    }
}

void WSphere::plotPoint(struct degenerate *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin_ > x - SYMBOLWIDTH / 2)
            paintedXMin_ = x - SYMBOLWIDTH / 2;
        if (paintedXMax_ < x + SYMBOLWIDTH / 2)
            paintedXMax_ = x - SYMBOLWIDTH / 2;
        if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
            paintedYMin_ = y - SYMBOLHEIGHT / 2;
        if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
            paintedYMax_ = y - SYMBOLHEIGHT / 2;

        win_plot_degen(staticPainter_, x, y);
    }
}

void WSphere::plotPoint(struct semi_elementary *p)
{
    double pos[2];
    int x, y;

    if (p != nullptr) {
        getChartPos(p->chart, p->x0, p->y0, pos);

        if (pos[0] < x0_ || pos[0] > x1_ || pos[1] < y0_ || pos[1] > y1_)
            return;

        x = coWinX(pos[0]);
        y = coWinY(pos[1]);

        if (paintedXMin_ > x - SYMBOLWIDTH / 2)
            paintedXMin_ = x - SYMBOLWIDTH / 2;
        if (paintedXMax_ < x + SYMBOLWIDTH / 2)
            paintedXMax_ = x - SYMBOLWIDTH / 2;
        if (paintedYMin_ > y - SYMBOLHEIGHT / 2)
            paintedYMin_ = y - SYMBOLHEIGHT / 2;
        if (paintedYMax_ < y + SYMBOLHEIGHT / 2)
            paintedYMax_ = y - SYMBOLHEIGHT / 2;

        switch (p->type) {
        case 1:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 2:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 3:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 4:
            win_plot_sesaddlenode(staticPainter_, x, y);
            break;
        case 5:
            win_plot_seunstablenode(staticPainter_, x, y);
            break;
        case 6:
            win_plot_sesaddle(staticPainter_, x, y);
            break;
        case 7:
            win_plot_sesaddle(staticPainter_, x, y);
            break;
        case 8:
            win_plot_sestablenode(staticPainter_, x, y);
            break;
        }
    }
}

void WSphere::plotPoints()
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

void WSphere::plotSeparatrices()
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

void WSphere::plotGcf() { draw_gcf(study_->gcf_points_, CSING, 1); }

void WSphere::plotCurves()
{
    g_globalLogger.debug("[WSphere] plotting curves");
    std::vector<curves>::const_iterator it;
    for (it = study_->curve_vector_.begin(); it != study_->curve_vector_.end();
         it++) {
        draw_curve(it->points, CCURV, 1);
    }
}

void WSphere::plotIsoclines()
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

std::vector<P4POLYLINES> WSphere::produceEllipse(double cx, double cy, double a,
                                                 double b, bool dotted,
                                                 double resa, double resb)
{
    // this is an exact copy of the plotEllipse routine, except that output is
    // stored in a list of points that is dynamically allocated.

    double theta{0};
    double t1, t2;
    double e;
    double R{(resa < resb) ? resa : resb};
    double x, y, c;
    double prevx{0.0}, prevy{0.0};
    bool d{false}, doton{true};
    int dotcount{0};
    std::vector<P4POLYLINES> result;

    if (R < 1.0)
        R = 1.0; // protection
    e = 2 * acos(1.0 - 0.5 / R);
    if (R * sin(e) > 1.0)
        e = asin(1.0 / R);

    while (theta < TWOPI) {
        c = (x0_ - cx) / a;
        if (c > -1.0 && c < 1.0) {
            t1 = acos(c);
            t2 = TWOPI - t1;
            if (theta >= t1 && theta < t2) {
                theta = t2 + e / 4;
                d = false;
                continue;
            }
        }
        c = (x1_ - cx) / a;
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
        c = (y0_ - cy) / b;
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
        c = (y1_ - cy) / b;
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
                result.emplace_back(prevx, prevy, x, y);
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
    return result;
}

void WSphere::plotPoincareSphere()
{
    int color;
    WPainterPath path;
    color = study_->singinf_ ? CSING : CLINEATINFINITY;
    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto pt : circleAtInfinity_) {
        path.moveTo(coWinX(pt.x1), coWinY(pt.y1));
        path.lineTo(coWinX(pt.x2), coWinY(pt.y2));
    }
    path.closeSubPath();
    staticPainter_->drawPath(path.crisp());
}

void WSphere::plotPoincareLyapunovSphere()
{
    int color;
    auto path = std::make_unique<WPainterPath>();
    color = study_->singinf_ ? CSING : CLINEATINFINITY;
    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto p : circleAtInfinity_) {
        path->moveTo(coWinX(p.x1), coWinY(p.y1));
        path->lineTo(coWinX(p.x2), coWinY(p.y2));
    }
    path->closeSubPath();
    staticPainter_->drawPath(path->crisp());

    color = CLINEATINFINITY;
    path.reset(new WPainterPath());

    staticPainter_->setPen(QXFIGCOLOR(color));
    for (auto p : pLCircle_) {
        path->moveTo(coWinX(p.x1), coWinY(p.y1));
        path->lineTo(coWinX(p.x2), coWinY(p.y2));
    }
    path->closeSubPath();
    staticPainter_->drawPath(path->crisp());
    return;
}

void WSphere::plotLineAtInfinity()
{
    switch (study_->typeofview_) {
    case TYPEOFVIEW_U1:
    case TYPEOFVIEW_V1:
        if (x0_ < 0.0 && x1_ > 0.0) {
            staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter_->drawLine(coWinX(0.0), 0, coWinX(0.0), height_ - 1);
        }
        break;
    case TYPEOFVIEW_U2:
    case TYPEOFVIEW_V2:
        if (y0_ < 0.0 && y1_ > 0.0) {
            staticPainter_->setPen(QXFIGCOLOR(CLINEATINFINITY));
            staticPainter_->drawLine(0, coWinY(0.0), width_ - 1, coWinY(0.0));
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

    if (staticPainter_ != nullptr) {
        if (_x1 >= x0_ && _x1 <= x1_ && _y1 >= y0_ && _y1 <= y1_) {
            wx1 = coWinX(_x1);
            wy1 = coWinY(_y1);

            if (_x2 >= x0_ && _x2 <= x1_ && _y2 >= y0_ && _y2 <= y1_) {
                // both points are visible in the window
                wx2 = coWinX(_x2);
                wy2 = coWinY(_y2);

                if (paintedXMin_ > wx1)
                    paintedXMin_ = wx1;
                if (paintedXMax_ < wx1)
                    paintedXMax_ = wx1;
                if (paintedYMin_ > wy1)
                    paintedYMin_ = wy1;
                if (paintedYMax_ < wy1)
                    paintedYMax_ = wy1;
                if (paintedXMin_ > wx2)
                    paintedXMin_ = wx2;
                if (paintedXMax_ < wx2)
                    paintedXMax_ = wx2;
                if (paintedYMin_ > wy2)
                    paintedYMin_ = wy2;
                if (paintedYMax_ < wy2)
                    paintedYMax_ = wy2;

                staticPainter_->setPen(QXFIGCOLOR(color));
                staticPainter_->drawLine(wx1, wy1, wx2, wy2);
            } else {
                // only (_x2,_y2) is not visible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter_->setPen(QXFIGCOLOR(color));

                    if (paintedXMin_ > wx1)
                        paintedXMin_ = wx1;
                    if (paintedXMax_ < wx1)
                        paintedXMax_ = wx1;
                    if (paintedYMin_ > wy1)
                        paintedYMin_ = wy1;
                    if (paintedYMax_ < wy1)
                        paintedYMax_ = wy1;
                    if (paintedXMin_ > wx2)
                        paintedXMin_ = wx2;
                    if (paintedXMax_ < wx2)
                        paintedXMax_ = wx2;
                    if (paintedYMin_ > wy2)
                        paintedYMin_ = wy2;
                    if (paintedYMax_ < wy2)
                        paintedYMax_ = wy2;

                    staticPainter_->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        } else {
            if (_x2 >= x0_ && _x2 <= x1_ && _y2 >= y0_ && _y2 <= y1_) {
                // only (_x2,_y2) is visible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter_->setPen(QXFIGCOLOR(color));

                    if (paintedXMin_ > wx1)
                        paintedXMin_ = wx1;
                    if (paintedXMax_ < wx1)
                        paintedXMax_ = wx1;
                    if (paintedYMin_ > wy1)
                        paintedYMin_ = wy1;
                    if (paintedYMax_ < wy1)
                        paintedYMax_ = wy1;
                    if (paintedXMin_ > wx2)
                        paintedXMin_ = wx2;
                    if (paintedXMax_ < wx2)
                        paintedXMax_ = wx2;
                    if (paintedYMin_ > wy2)
                        paintedYMin_ = wy2;
                    if (paintedYMax_ < wy2)
                        paintedYMax_ = wy2;

                    staticPainter_->drawLine(wx1, wy1, wx2, wy2);
                }
            } else {
                // both end points are invisible
                if (lineRectangleIntersect(_x1, _y1, _x2, _y2, x0_, x1_, y0_,
                                           y1_)) {
                    wx1 = coWinX(_x1);
                    wy1 = coWinY(_y1);
                    wx2 = coWinX(_x2);
                    wy2 = coWinY(_y2);
                    staticPainter_->setPen(QXFIGCOLOR(color));

                    if (paintedXMin_ > wx1)
                        paintedXMin_ = wx1;
                    if (paintedXMax_ < wx1)
                        paintedXMax_ = wx1;
                    if (paintedYMin_ > wy1)
                        paintedYMin_ = wy1;
                    if (paintedYMax_ < wy1)
                        paintedYMax_ = wy1;
                    if (paintedXMin_ > wx2)
                        paintedXMin_ = wx2;
                    if (paintedXMax_ < wx2)
                        paintedXMax_ = wx2;
                    if (paintedYMin_ > wy2)
                        paintedYMin_ = wy2;
                    if (paintedYMax_ < wy2)
                        paintedYMax_ = wy2;

                    staticPainter_->drawLine(wx1, wy1, wx2, wy2);
                }
            }
        }
    }
}

void WSphere::drawPoint(double x, double y, int color)
{
    int _x, _y;
    if (staticPainter_ != nullptr) {
        if (x < x0_ || x > x1_ || y < y0_ || y > y1_)
            return;
        staticPainter_->setPen(QXFIGCOLOR(color));
        _x = coWinX(x);
        _y = coWinY(y);

        if (paintedXMin_ > _x)
            paintedXMin_ = _x;
        if (paintedXMax_ < _x)
            paintedXMax_ = _x;
        if (paintedYMin_ > _y)
            paintedYMin_ = _y;
        if (paintedYMax_ < _y)
            paintedYMax_ = _y;

        staticPainter_->drawPoint((double)_x, (double)_y);
    }
}