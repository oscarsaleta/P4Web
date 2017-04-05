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

#include "MyLogger.h"
#include "ScriptHandler.h"
#include "custom.h"
#include "math_p4.h"
#include "plot_tools.h"

#include <cmath>

// static global variables
static int GcfDashes = 1;

// function definitions
void WVFStudy::rplane_plsphere0(double x, double y, double *pcoord)
{
    R2_to_plsphere(x * cos(y), x * sin(y), pcoord);
}

bool WSphere::evalGcfStart(std::string fname, int dashes, int points,
                           int precis)
{
    if (study_->gcf_points != nullptr) {
        /* here we just paint in black over the previous gcf,
           we can simply reset the plot or something */
        // sp->prepareDrawing();
        // draw_gcf( sp, study_->gcf_points, CBACKGROUND, GcfDashes );
        // sp->finishDrawing();
        study_->deleteOrbitPoint(study_->gcf_points);
        study_->gcf_points = nullptr;
    }

    if (study_->plweights)
        gcfTask_ = EVAL_GCF_LYP_R2;
    else
        gcfTask_ = EVAL_GCF_R2;

    gcfError_ = false;
    // GcfSphere = sp;
    GcfDashes = dashes;
    if (runTask(fname, gcfTask_, points, precis) < 0)
        return false;
    return true;
}

// returns true when finished.  Then run EvalGCfFinish to see if error occurred
// or not
bool WSphere::evalGcfContinue(std::string fname, int points, int prec)
{
    if (gcfTask_ == EVAL_GCF_NONE)
        return true;

    if (!readTaskResults(fname, gcfTask_)) {
        gcfError_ = true;
        globalLogger__.error("WSphere :: error at gcf readTaskResults");
        return true;
    }
    gcfTask_++;
    if (gcfTask_ == EVAL_GCF_FINISHPOINCARE ||
        gcfTask_ == EVAL_GCF_FINISHLYAPUNOV) {
        return true;
    }

    if (runTask(fname, gcfTask_, points, prec) < 0) {
        gcfError_ = true;
        globalLogger__.error("WSphere :: error at gcf runTask");
        return true;
    }

    return false; // still busy
}

bool WSphere::evalGcfFinish(void) // return false in case an error occured
{
    if (gcfTask_ != EVAL_GCF_NONE) {

        gcfTask_ = EVAL_GCF_NONE;

        if (gcfError_) {
            gcfError_ = false;
            return false;
        }
    }
    return true;
}

int WSphere::runTask(std::string fname, int task, int points, int prec)
{
    bool value;
    // std::string fname = randomFileName(TMP_DIR,"_gcf.tab");

    globalLogger__.debug("WSphere :: will run GCF task=" +
                         std::to_string(task) + " using file=" + fname);

    switch (task) {
    case EVAL_GCF_R2:
        value = prepareGcf(fname, study_->gcf, -1, 1, prec, points);
        break;
    case EVAL_GCF_U1:
        value = prepareGcf(fname, study_->gcf_U1, 0, 1, prec, points);
        break;
    case EVAL_GCF_V1:
        value = prepareGcf(fname, study_->gcf_U1, -1, 0, prec, points);
        break;
    case EVAL_GCF_U2:
        value = prepareGcf(fname, study_->gcf_U2, 0, 1, prec, points);
        break;
    case EVAL_GCF_V2:
        value = prepareGcf(fname, study_->gcf_U2, -1, 0, prec, points);
        break;
    case EVAL_GCF_LYP_R2:
        value = prepareGcf_LyapunovR2(fname, study_->gcf, prec, points);
        break;
    case EVAL_GCF_CYL1:
        value = prepareGcf_LyapunovCyl(fname, study_->gcf_C, -PI_DIV4, PI_DIV4,
                                       prec, points);
        break;
    case EVAL_GCF_CYL2:
        value = prepareGcf_LyapunovCyl(fname, study_->gcf_C, PI_DIV4,
                                       PI - PI_DIV4, prec, points);
        break;
    case EVAL_GCF_CYL3:
        value = prepareGcf_LyapunovCyl(fname, study_->gcf_C, PI - PI_DIV4,
                                       PI + PI_DIV4, prec, points);
        break;
    case EVAL_GCF_CYL4:
        value = prepareGcf_LyapunovCyl(fname, study_->gcf_C, -PI + PI_DIV4,
                                       -PI_DIV4, prec, points);
        break;
    default:
        value = false;
        break;
    }

    if (value)
        return evaluateMapleScript(fname,60).si_status;
    else
        return -1;
}

bool WSphere::readTaskResults(std::string fname,
                              int task) // , int points, int prec, int memory )
{
    globalLogger__.debug("WSphere :: called readTaskResults with fname=" +
                         fname + " and task=" + std::to_string(task));
    bool value;

    switch (task) {
    case EVAL_GCF_R2:
        value = read_gcf(fname, &WVFStudy::R2_to_psphere);
        break;
    case EVAL_GCF_U1:
        value = read_gcf(fname, &WVFStudy::U1_to_psphere);
        break;
    case EVAL_GCF_V1:
        value = read_gcf(fname, &WVFStudy::VV1_to_psphere);
        break;
    case EVAL_GCF_U2:
        value = read_gcf(fname, &WVFStudy::U2_to_psphere);
        break;
    case EVAL_GCF_V2:
        value = read_gcf(fname, &WVFStudy::VV2_to_psphere);
        break;
    case EVAL_GCF_LYP_R2:
        value = read_gcf(fname, &WVFStudy::rplane_plsphere0);
        break;
    case EVAL_GCF_CYL1:
        value = read_gcf(fname, &WVFStudy::cylinder_to_plsphere);
        break;
    case EVAL_GCF_CYL2:
        value = read_gcf(fname, &WVFStudy::cylinder_to_plsphere);
        break;
    case EVAL_GCF_CYL3:
        value = read_gcf(fname, &WVFStudy::cylinder_to_plsphere);
        break;
    case EVAL_GCF_CYL4:
        value = read_gcf(fname, &WVFStudy::cylinder_to_plsphere);
        break;
    default:
        value = false;
    }

    return value;
}

void WSphere::draw_gcf(orbits_points *sep, int color, int dashes)
{
    double pcoord[3];

    while (sep != nullptr) {
        if (sep->dashes && dashes)
            (*plot_l)(this, pcoord, sep->pcoord, color);
        else
            (*plot_p)(this, sep->pcoord, color);
        copy_x_into_y(sep->pcoord, pcoord);

        sep = sep->next_point;
    }
}

void WVFStudy::insert_gcf_point(double x0, double y0, double z0, int dashes)
{
    if (gcf_points != nullptr) {
        last_gcf_point->next_point = new orbits_points;
        last_gcf_point = last_gcf_point->next_point;
    } else {
        last_gcf_point = new orbits_points;
        gcf_points = last_gcf_point;
    }

    last_gcf_point->pcoord[0] = x0;
    last_gcf_point->pcoord[1] = y0;
    last_gcf_point->pcoord[2] = z0;

    last_gcf_point->dashes = dashes;
    last_gcf_point->color = CSING;
    last_gcf_point->next_point = nullptr;
}

bool WSphere::read_gcf(std::string fname,
                       void (WVFStudy::*chart)(double, double, double *))
{

    int t;
    int k;
    FILE *fp;
    double x, y;
    double pcoord[3];
    int d, c;

    fp = fopen(std::string(fname + "_gcf.tab").c_str(), "r");
    if (fp == nullptr) {
        globalLogger__.debug("WSphere :: cannot open file " +
                             std::string(fname + "_gcf.tab") + " for reading");
        return false;
    }

    k = 0;
    while (1) {
        d = 0;
        while (fscanf(fp, "%lf %lf", &x, &y) == 2) {
            k++;
            (study_->*chart)(x, y, pcoord);
            study_->insert_gcf_point(pcoord[0], pcoord[1], pcoord[2], d);
            // d=1;
            d = GcfDashes;
        }
        for (c = getc(fp); isspace(c);)
            c = getc(fp);
        if (c != ',')
            break;
    }

    fclose(fp);
    return true;
}
