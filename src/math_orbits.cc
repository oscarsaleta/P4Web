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
#include "file_tab.h"

#include "custom.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "plot_tools.h"

#include <cmath>

// -----------------------------------------------------------------------
//                      INTEGRATEORBIT
// -----------------------------------------------------------------------
//
// dir = -1: backwards, dir=0: continue, dir=+1: forwards

void WSphere::integrateOrbit(int dir)
{
    orbits_points *sep;
    double pcoord[3], ucoord[2];

    if (dir == 0) {
        // continue orbit button has been pressed

        dir = study_->current_orbit_->current_f_orbits->dir;

        copy_x_into_y(study_->current_orbit_->current_f_orbits->pcoord, pcoord);
        study_->current_orbit_->current_f_orbits->next_point =
            integrate_orbit(pcoord, study_->config_currentstep_, dir, CORBIT,
                            study_->config_intpoints_, &sep);

        study_->current_orbit_->current_f_orbits = sep;
        return;
    }

    copy_x_into_y(study_->current_orbit_->pcoord, pcoord);
    ((study_)->*(study_->sphere_to_R2))(pcoord[0], pcoord[1], pcoord[2],
                                        ucoord);
    if (study_->config_kindvf_ == INTCONFIG_ORIGINAL)
        if (eval_term2(study_->gcf_, ucoord) < 0)
            dir = -dir;

    if (study_->current_orbit_->f_orbits == nullptr) {
        study_->current_orbit_->f_orbits =
            integrate_orbit(pcoord, study_->config_step_, dir, CORBIT,
                            study_->config_intpoints_, &sep);
    } else {
        study_->current_orbit_->current_f_orbits->next_point = new orbits_points;
        study_->current_orbit_->current_f_orbits =
            study_->current_orbit_->current_f_orbits->next_point;
        copy_x_into_y(pcoord, study_->current_orbit_->current_f_orbits->pcoord);
        study_->current_orbit_->current_f_orbits->dashes = 0;
        study_->current_orbit_->current_f_orbits->color = CORBIT;
        study_->current_orbit_->current_f_orbits->dir = dir;
        study_->current_orbit_->current_f_orbits->next_point =
            integrate_orbit(pcoord, study_->config_step_, dir, CORBIT,
                            study_->config_intpoints_, &sep);
    }
    study_->current_orbit_->current_f_orbits = sep;
}

//// -----------------------------------------------------------------------
////                      STARTORBIT
//// -----------------------------------------------------------------------
bool WSphere::startOrbit(double x, double y, bool R)
{
    double pcoord[3];
    double ucoord[2];

    if (R)
        (study_->*(study_->R2_to_sphere))(x, y, pcoord);
    else
        (study_->*(study_->viewcoord_to_sphere))(x, y, pcoord);

    if (study_->first_orbit_ == nullptr) {
        study_->first_orbit_ = new orbits;
        study_->current_orbit_ = study_->first_orbit_;
    } else {
        if (pcoord[0] == study_->current_orbit_->pcoord[0] &&
            pcoord[1] == study_->current_orbit_->pcoord[1] &&
            pcoord[2] == study_->current_orbit_->pcoord[2]) {
            return false;
        }
        study_->current_orbit_->next_orbit = new orbits;
        study_->current_orbit_ = study_->current_orbit_->next_orbit;
    }

    copy_x_into_y(pcoord, study_->current_orbit_->pcoord);
    study_->current_orbit_->color = CORBIT;
    study_->current_orbit_->f_orbits = nullptr;
    study_->current_orbit_->next_orbit = nullptr;

    (study_->*(study_->sphere_to_viewcoord))(pcoord[0], pcoord[1], pcoord[2],
                                             ucoord);

    return true;
}

//// -----------------------------------------------------------------------
////                      DRAWORBIT
//// -----------------------------------------------------------------------

void WSphere::drawOrbit(double *pcoord, orbits_points *points, int color)
{
    double pcoord1[3];

    copy_x_into_y(pcoord, pcoord1);
    (*plot_p)(this, pcoord, color);

    while (points != nullptr) {
        if (points->dashes) {
            (*plot_l)(this, pcoord1, points->pcoord, color);
        } else {
            (*plot_p)(this, points->pcoord, color);
        }

        copy_x_into_y(points->pcoord, pcoord1);

        points = points->next_point;
    }
}

//// -----------------------------------------------------------------------
////                      DRAWORBITS
//// -----------------------------------------------------------------------
// called from paintEvent()
void WSphere::drawOrbits()
{
    orbits *orbit;

    for (orbit = study_->first_orbit_; orbit != nullptr;
         orbit = orbit->next_orbit) {
        drawOrbit(orbit->pcoord, orbit->f_orbits, orbit->color);
    }
}

//// -----------------------------------------------------------------------
////                      DELETELASTORBIT
//// -----------------------------------------------------------------------

void WSphere::deleteLastOrbit()
{
    orbits *orbit1, *orbit2;

    if (study_->current_orbit_ == nullptr)
        return;

    orbit2 = study_->current_orbit_;
    if (study_->first_orbit_ == study_->current_orbit_) {
        study_->first_orbit_ = nullptr;
        study_->current_orbit_ = nullptr;
    } else {
        orbit1 = study_->first_orbit_;
        do {
            study_->current_orbit_ = orbit1;
            orbit1 = orbit1->next_orbit;
        } while (orbit1 != orbit2);

        study_->current_orbit_->next_orbit = nullptr;
    }
    study_->deleteOrbitPoint(orbit2->f_orbits);
    delete orbit2;
    orbit2 = nullptr;
}

/*integrate poincare sphere case p=q=1 */
void WVFStudy::integrate_poincare_orbit(double p0, double p1, double p2,
                                        double *pcoord, double *hhi,
                                        int *dashes, int *dir, double h_min,
                                        double h_max)
{
    double y[2], theta;

    *dashes = true;
    *dir = 1;
    if (pcoord[2] > ZCOORD) {
        psphere_to_R2(p0, p1, p2, y);
        rk78(&WVFStudy::eval_r_vec_field, y, hhi, h_min, h_max,
             config_tolerance_);
        R2_to_psphere(y[0], y[1], pcoord);
    } else {
        theta = atan2(fabs(p1), fabs(p0));
        if ((theta < PI_DIV4) && (theta > -PI_DIV4)) {
            if (p0 > 0) {
                psphere_to_U1(p0, p1, p2, y);
                rk78(&WVFStudy::eval_U1_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_)
                    U1_to_psphere(y[0], y[1], pcoord);
                else {
                    VV1_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            } else {
                psphere_to_V1(p0, p1, p2, y);
                rk78(&WVFStudy::eval_V1_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_)
                    V1_to_psphere(y[0], y[1], pcoord);
                else {
                    UU1_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            }
        } else {
            if (p1 > 0) {
                psphere_to_U2(p0, p1, p2, y);
                rk78(&WVFStudy::eval_U2_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_)
                    U2_to_psphere(y[0], y[1], pcoord);
                else {
                    VV2_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            } else {
                psphere_to_V2(p0, p1, p2, y);
                rk78(&WVFStudy::eval_V2_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_)
                    V2_to_psphere(y[0], y[1], pcoord);
                else {
                    UU2_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                    }
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], y);
                    *dashes = false;
                }
            }
        }
    }
}

/* integrate on the Poincare-Lyapunov sphere */
void WVFStudy::integrate_lyapunov_orbit(double p0, double p1, double p2,
                                        double *pcoord, double *hhi,
                                        int *dashes, int *dir, double h_min,
                                        double h_max)
{
    double y[2];

    *dashes = true;
    *dir = 1;
    if (p0 == 0) {
        y[0] = p1;
        y[1] = p2;
        rk78(&WVFStudy::eval_r_vec_field, y, hhi, h_min, h_max,
             config_tolerance_);
        R2_to_plsphere(y[0], y[1], pcoord);
    } else {
        y[0] = p1;
        y[1] = p2;
        rk78(&WVFStudy::eval_vec_field_cyl, y, hhi, h_min, h_max,
             config_tolerance_);
        if (y[1] >= TWOPI)
            y[1] -= TWOPI;
        cylinder_to_plsphere(y[0], y[1], pcoord);
    }
}

orbits_points *WSphere::integrate_orbit(double pcoord[3], double step, int dir,
                                        int color, int points_to_int,
                                        orbits_points **orbit)
{
    int i, d, h;
    int dashes;
    double hhi;
    double pcoord2[3], h_min, h_max;
    orbits_points *first_orbit_ = nullptr, *last_orbit = nullptr;

    hhi = (double)dir * step;
    h_min = study_->config_hmi_;
    h_max = study_->config_hma_;
    copy_x_into_y(pcoord, pcoord2);
    for (i = 1; i <= points_to_int; ++i) {
        ((study_)->*(study_->integrate_sphere_orbit))(
            pcoord[0], pcoord[1], pcoord[2], pcoord, &hhi, &dashes, &d, h_min,
            h_max);

        if ((i % UPDATEFREQ_STEPSIZE) == 0)
            study_->set_current_step(fabs(hhi));

        if (last_orbit == nullptr) {
            first_orbit_ = new orbits_points;
            last_orbit = first_orbit_;
            h = dir;
        } else {
            last_orbit->next_point = new orbits_points;
            h = last_orbit->dir;
            last_orbit = last_orbit->next_point;
        }

        copy_x_into_y(pcoord, last_orbit->pcoord);
        last_orbit->color = color;
        last_orbit->dashes = dashes * study_->config_dashes_;
        last_orbit->dir = d * h;
        last_orbit->next_point = nullptr;
        /*if (dashes * study_->config_dashes_)
            (*plot_l)(this,pcoord,pcoord2,color);
        else
            (*plot_p)(this,pcoord,color);*/
        // update();
        copy_x_into_y(pcoord, pcoord2);
    }
    study_->set_current_step(fabs(hhi));
    *orbit = last_orbit;

    return (first_orbit_);
}
