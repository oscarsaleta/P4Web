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

#include "math_intblowup.h"

#include "math_p4.h"
#include "math_polynom.h"
#include "math_separatrice.h"
#include "plot_tools.h"

// static global variables
/* To get rid of this, we would have to change the interface of
    WVFStudy::rk78 and add a void pointer in which to pass any
    additional arguments if needed (and this means we would
    have to change the interface of any function passed to
    rk78 to match this change)
*/
static struct term2 *vec_field[2];

// function definitions
void WVFStudy::eval_blow_vec_field(double *y, double *f)
{
    f[0] = eval_term2(vec_field[0], y);
    f[1] = eval_term2(vec_field[1], y);
}

struct orbits_points *
integrate_blow_up(WSphere *spherewnd, // double x0, double y0,
                  double *pcoord2, struct blow_up_points *de_sep, double step,
                  int dir, int type, struct orbits_points **orbit, int chart)
{
    int i;
    double hhi, point[2];
    double pcoord[3];
    double y[2];
    int color, dashes, ok = true;
    struct orbits_points *first_orbit_ = nullptr, *last_orbit = nullptr;

    vec_field[0] = de_sep->vector_field[0];
    vec_field[1] = de_sep->vector_field[1];
    if (spherewnd->study_->plweights_ == false &&
        (chart == CHART_V1 || chart == CHART_V2))
        dir = spherewnd->study_->dir_vec_field_ * dir;

    hhi = (double)dir * step;
    y[0] = de_sep->point[0];
    y[1] = de_sep->point[1];
    for (i = 1; i <= spherewnd->study_->config_intpoints_; ++i) {
        spherewnd->study_->rk78(&WVFStudy::eval_blow_vec_field, y, &hhi,
                                spherewnd->study_->config_hmi_,
                                spherewnd->study_->config_hma_,
                                spherewnd->study_->config_tolerance_);
        make_transformations(
            de_sep->trans, de_sep->x0 + de_sep->a11 * y[0] + de_sep->a12 * y[1],
            de_sep->y0 + de_sep->a21 * y[0] + de_sep->a22 * y[1], point);
        dashes = true;
        switch (chart) {
        case CHART_R2:
            ((spherewnd->study_)->*(spherewnd->study_->R2_to_sphere))(
                point[0], point[1], pcoord);
            color = findSepColor2(spherewnd->study_->gcf_, type, point);
            break;

        case CHART_U1:
            if (point[1] >= 0 || !spherewnd->study_->singinf_) {
                ((spherewnd->study_)->*(spherewnd->study_->U1_to_sphere))(
                    point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (spherewnd->study_->dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep->type;
                color = findSepColor2(spherewnd->study_->gcf_U1_, type, point);
            } else {
                spherewnd->study_->VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (spherewnd->study_->dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (spherewnd->study_->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;

                spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
                color = findSepColor2(spherewnd->study_->gcf_V1_, type, point);
            }
            break;

        case CHART_V1:
            ((spherewnd->study_)->*(spherewnd->study_->V1_to_sphere))(
                point[0], point[1], pcoord);
            if (spherewnd->study_->plweights_ == false)
                spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
            color = findSepColor2(spherewnd->study_->gcf_V1_, type, point);
            break;

        case CHART_U2:
            if (point[1] >= 0 || !spherewnd->study_->singinf_) {
                ((spherewnd->study_)->*(spherewnd->study_->U2_to_sphere))(
                    point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                    if (spherewnd->study_->dir_vec_field_ == 1)
                        dir *= -1;
                }
                type = de_sep->type;
                color = findSepColor2(spherewnd->study_->gcf_U2_, type, point);
            } else {
                spherewnd->study_->VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (spherewnd->study_->dir_vec_field_ == 1)
                        dir *= -1;
                }
                if (spherewnd->study_->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
                color = findSepColor2(spherewnd->study_->gcf_V2_, type, point);
            }
            break;

        case CHART_V2:
            ((spherewnd->study_)->*(spherewnd->study_->V2_to_sphere))(
                point[0], point[1], pcoord);
            if (spherewnd->study_->plweights_ == false)
                spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
            color = findSepColor2(spherewnd->study_->gcf_V2_, type, point);
            break;

        default:
            color = 0;
            break;
        }

        if (last_orbit == nullptr) {
            first_orbit_ = new orbits_points;
            last_orbit = first_orbit_;
        } else {
            last_orbit->next_point = new orbits_points;
            last_orbit = last_orbit->next_point;
        }
        last_orbit->pcoord[0] = pcoord[0];
        last_orbit->pcoord[1] = pcoord[1];
        last_orbit->pcoord[2] = pcoord[2];
        last_orbit->color = color;
        last_orbit->dashes = dashes * spherewnd->study_->config_dashes_;
        last_orbit->dir = ((spherewnd->study_->plweights_ == false) &&
                           (chart == CHART_V1 || chart == CHART_V2))
                              ? spherewnd->study_->dir_vec_field_ * dir
                              : dir;
        last_orbit->type = type;
        last_orbit->next_point = nullptr;
        if (last_orbit->dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);

        if (y[0] * y[0] + y[1] * y[1] >= 1.0) {
            de_sep->blow_up_vec_field = false;
            break;
        }
        copy_x_into_y(pcoord, pcoord2);
    }
    de_sep->point[0] = y[0];
    de_sep->point[1] = y[1];
    *orbit = last_orbit;

    return (first_orbit_);
}
