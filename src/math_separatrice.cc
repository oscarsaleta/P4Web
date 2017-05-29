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

#include "math_separatrice.h"

#include "custom.h"
#include "file_tab.h"
#include "math_intblowup.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "plot_tools.h"

#include <cmath>

/*void (*change_epsilon)( WSphere *, double ) = nullptr;
void (*start_plot_sep)( WSphere * ) = nullptr;
void (*cont_plot_sep)( WSphere * ) = nullptr;
void (*plot_next_sep)( WSphere * ) = nullptr;
void (*select_next_sep)( WSphere * ) = nullptr;*/

int findSepColor2(P4POLYNOM2 f, int type, double y[2])
{
    int color;

    if (eval_term2(f, y) >= 0) {
        switch (type) {
        case OT_STABLE:
            color = CSTABLE;
            break;
        case OT_UNSTABLE:
            color = CUNSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_STABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_UNSTABLE;
            break;
        default:
            color = 0;
            break; // to avoid compiler warnings
        }
    } else {
        switch (type) {
        case OT_STABLE:
            color = CUNSTABLE;
            break;
        case OT_UNSTABLE:
            color = CSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_UNSTABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_STABLE;
            break;
        default:
            color = 0;
            break; // to avoid compiler warnings
        }
    }
    return (color);
}

int findSepColor3(P4POLYNOM3 f, int type, double y[2])
{
    int color;

    if (eval_term3(f, y) >= 0) {
        switch (type) {
        case OT_STABLE:
            color = CSTABLE;
            break;
        case OT_UNSTABLE:
            color = CUNSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_STABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_UNSTABLE;
            break;
        default:
            color = 0;
            break;
        }
    } else {
        switch (type) {
        case OT_STABLE:
            color = CUNSTABLE;
            break;
        case OT_UNSTABLE:
            color = CSTABLE;
            break;
        case OT_CENT_STABLE:
            color = CCENT_UNSTABLE;
            break;
        case OT_CENT_UNSTABLE:
            color = CCENT_STABLE;
            break;
        default:
            color = 0;
            break;
        }
    }
    return (color);
}

/*integrate poincare sphere case p=q=1 */
void WVFStudy::integrate_poincare_sep(double p0, double p1, double p2,
                                      double *pcoord, double *hhi, int *type,
                                      int *color, int *dashes, int *dir,
                                      double h_min, double h_max)
{
    double y[2], theta;

    *dashes = true;
    *dir = 1;
    if (pcoord[2] > ZCOORD) {
        psphere_to_R2(p0, p1, p2, y);
        rk78(&WVFStudy::eval_r_vec_field, y, hhi, h_min, h_max,
             config_tolerance_);
        R2_to_psphere(y[0], y[1], pcoord);
        *color = findSepColor2(gcf_, *type, y);
    } else {
        theta = atan2(fabs(p1), fabs(p0));
        if ((theta < PI_DIV4) && (theta > -PI_DIV4)) {
            if (p0 > 0) {
                psphere_to_U1(p0, p1, p2, y);
                rk78(&WVFStudy::eval_U1_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_) {
                    U1_to_psphere(y[0], y[1], pcoord);
                    *color = findSepColor2(gcf_U1_, *type, y);
                } else {
                    VV1_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                        *type = change_type(*type);
                    }
                    psphere_to_V1(pcoord[0], pcoord[1], pcoord[2], y);
                    *color = findSepColor2(gcf_V1_, *type, y);
                    *dashes = false;
                }
            } else {
                psphere_to_V1(p0, p1, p2, y);
                rk78(&WVFStudy::eval_V1_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_) {
                    V1_to_psphere(y[0], y[1], pcoord);
                    *color = findSepColor2(gcf_V1_, *type, y);
                } else {
                    UU1_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                        *type = change_type(*type);
                    }
                    psphere_to_U1(pcoord[0], pcoord[1], pcoord[2], y);
                    *color = findSepColor2(gcf_U1_, *type, y);
                    *dashes = false;
                }
            }
        } else {
            if (p1 > 0) {
                psphere_to_U2(p0, p1, p2, y);
                rk78(&WVFStudy::eval_U2_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_) {
                    U2_to_psphere(y[0], y[1], pcoord);
                    *color = findSepColor2(gcf_U2_, *type, y);
                } else {
                    VV2_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                        *type = change_type(*type);
                    }
                    psphere_to_V2(pcoord[0], pcoord[1], pcoord[2], y);
                    *color = findSepColor2(gcf_V2_, *type, y);
                    *dashes = false;
                }
            } else {
                psphere_to_V2(p0, p1, p2, y);
                rk78(&WVFStudy::eval_V2_vec_field, y, hhi, h_min, h_max,
                     config_tolerance_);
                if (y[1] >= 0 || !singinf_) {
                    V2_to_psphere(y[0], y[1], pcoord);
                    *color = findSepColor2(gcf_V2_, *type, y);
                } else {
                    UU2_to_psphere(y[0], y[1], pcoord);
                    if (dir_vec_field_ == 1) {
                        *dir = -1;
                        *hhi = -(*hhi);
                        *type = change_type(*type);
                    }
                    psphere_to_U2(pcoord[0], pcoord[1], pcoord[2], y);
                    *color = findSepColor2(gcf_U2_, *type, y);
                    *dashes = false;
                }
            }
        }
    }
}

/* integrate on the Poincare-Lyapunov sphere */
void WVFStudy::integrate_lyapunov_sep(double p0, double p1, double p2,
                                      double *pcoord, double *hhi, int *type,
                                      int *color, int *dashes, int *dir,
                                      double h_min, double h_max)
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
        *color = findSepColor2(gcf_, *type, y);
    } else {
        y[0] = p1;
        y[1] = p2;
        rk78(&WVFStudy::eval_vec_field_cyl, y, hhi, h_min, h_max,
             config_tolerance_);
        if (y[1] >= TWOPI)
            y[1] -= TWOPI;
        cylinder_to_plsphere(y[0], y[1], pcoord);
        *color = findSepColor3(gcf_C_, *type, y);
    }
}

static orbits_points *integrate_sep(WSphere *spherewnd, double pcoord[3],
                                    double step, int dir, int type,
                                    int points_to_int, orbits_points **orbit)
{
    int i, d, h;
    int color, dashes;
    double hhi;
    double pcoord2[3], h_min, h_max;
    orbits_points *first_orbit_ = nullptr, *last_orbit = nullptr;

    /* if we intergrate a separatrice and use the original vector field
    then it is possible that we have to change the direction of the
    separatrice, because the separatrice is evaluate for the reduced
    vector field
    */
    if (spherewnd->study_->config_kindvf_ == INTCONFIG_ORIGINAL &&
        ((spherewnd->study_)->*(spherewnd->study_->change_dir))(pcoord))
        hhi = -spherewnd->study_->config_step_ * dir;
    else
        hhi = (double)dir * step;

    h_min = spherewnd->study_->config_hmi_;
    h_max = spherewnd->study_->config_hma_;
    copy_x_into_y(pcoord, pcoord2);
    for (i = 1; i <= points_to_int; ++i) {
        ((spherewnd->study_)->*(spherewnd->study_->integrate_sphere_sep))(
            pcoord[0], pcoord[1], pcoord[2], pcoord, &hhi, &type, &color,
            &dashes, &d, h_min, h_max);

        if ((i % UPDATEFREQ_STEPSIZE) == 0)
            spherewnd->study_->set_current_step(fabs(hhi));
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
        last_orbit->dashes = dashes * spherewnd->study_->config_dashes_;
        last_orbit->dir = d * h;
        last_orbit->type = type;
        last_orbit->next_point = nullptr;
        if (dashes * spherewnd->study_->config_dashes_)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }
    spherewnd->study_->set_current_step(fabs(hhi));
    *orbit = last_orbit;
    return (first_orbit_);
}

int change_type(int type)
{
    int t;

    switch (type) {
    case OT_STABLE:
        t = OT_UNSTABLE;
        break;
    case OT_UNSTABLE:
        t = OT_STABLE;
        break;
    case OT_CENT_STABLE:
        t = OT_CENT_UNSTABLE;
        break;
    case OT_CENT_UNSTABLE:
        t = OT_CENT_STABLE;
        break;
    case OT_ORBIT:
        t = OT_ORBIT;
        break;
    default:
        t = type;
        break;
    }
    return t;
}

/* find a t with a<=t^2+sep(t)^2<=b, a=(epsilon-epsilon/100)^2
   and b=(epsilon+epsilon/100)^2 */

static double find_step(term1 *sep, double epsilon, int dir)
{
    double t, t1, t2, r0, a, b;

    t = epsilon * dir;
    a = pow(epsilon - epsilon / 100, 2.0);
    b = pow(epsilon + epsilon / 100, 2.0);
    r0 = pow(t, 2.0) + pow(eval_term1(sep, t), 2.0);
    if ((r0 > a) && (r0 < b))
        return (t);
    if (r0 <= a) {
        do {
            t *= 2.;
            r0 = pow(t, 2.0) + pow(eval_term1(sep, t), 2.0);
        } while (r0 <= a);
        if (r0 < b)
            t2 = t;
        else {
            t1 = t / 2;
            t2 = (t + t1) / 2;
            for (;;) {
                r0 = pow(t2, 2.0) + pow(eval_term1(sep, t2), 2.0);
                if ((r0 > a) && (r0 < b))
                    break;
                if (r0 <= a)
                    t1 = t2;
                else
                    t = t2;
                t2 = (t + t1) / 2;
            }
        }
    } else {
        do {
            t /= 2.;
            r0 = pow(t, 2.0) + pow(eval_term1(sep, t), 2.0);
        } while (r0 >= b);
        if (r0 > a)
            t2 = t;
        else {
            t1 = 2. * t;
            t2 = (t + t1) / 2;
            for (;;) {
                r0 = pow(t2, 2.0) + pow(eval_term1(sep, t2), 2.0);
                if ((r0 > a) && (r0 < b))
                    break;
                if (r0 <= a)
                    t = t2;
                else
                    t1 = t2;
                t2 = (t + t1) / 2;
            }
        }
    }
    return (t2);
}

orbits_points *plot_separatrice(WSphere *spherewnd, double x0, double y0,
                                double a11, double a12, double a21, double a22,
                                double epsilon, sep *sep1,
                                orbits_points **orbit, short int chart)
{
    double t = 0.0, h, y, pcoord[3], pcoord2[3], point[2];
    int i, dashes, ok = true;
    orbits_points *first_orbit_ = nullptr, *last_orbit = nullptr, *sep2;
    int color, dir, type;

    /* if we have a line of singularities at infinity then we have to change the
    chart if the chart is V1 or V2 */
    if (spherewnd->study_->singinf_) {
        if (chart == CHART_V1)
            chart = CHART_U1;
        else if (chart == CHART_V2)
            chart = CHART_U2;
    }

    /* h=(epsilon/100)*sep1->direction; */
    h = find_step(sep1->separatrice, epsilon, sep1->direction) / 100;

    first_orbit_ = new orbits_points;
    last_orbit = first_orbit_;
    point[0] = x0;
    point[1] = y0;

    switch (chart) {
    case CHART_R2:
        ((spherewnd->study_)->*(spherewnd->study_->R2_to_sphere))(x0, y0,
                                                                  pcoord);
        color = findSepColor2(spherewnd->study_->gcf_, sep1->type, point);
        break;
    case CHART_U1:
        ((spherewnd->study_)->*(spherewnd->study_->U1_to_sphere))(x0, y0,
                                                                  pcoord);
        color = findSepColor2(spherewnd->study_->gcf_U1_, sep1->type, point);
        break;
    case CHART_V1:
        ((spherewnd->study_)->*(spherewnd->study_->V1_to_sphere))(x0, y0,
                                                                  pcoord);
        if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1)) {
            spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1], pcoord[2],
                                             point);
        }
        color = findSepColor2(spherewnd->study_->gcf_V1_, sep1->type, point);
        break;
    case CHART_U2:
        ((spherewnd->study_)->*(spherewnd->study_->U2_to_sphere))(x0, y0,
                                                                  pcoord);
        color = findSepColor2(spherewnd->study_->gcf_U2_, sep1->type, point);
        break;
    case CHART_V2:
        ((spherewnd->study_)->*(spherewnd->study_->V2_to_sphere))(x0, y0,
                                                                  pcoord);
        if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1)) {
            spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1], pcoord[2],
                                             point);
        }
        color = findSepColor2(spherewnd->study_->gcf_U2_, sep1->type, point);
        break;
    default:
        color = 0;
        break;
    }

    copy_x_into_y(pcoord, last_orbit->pcoord);
    type = sep1->type;

    switch (sep1->type) {
    case OT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    case OT_CENT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_CENT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    default:
        dir = 0;
        break;
    }

    last_orbit->color = color;
    last_orbit->dashes = 0;
    copy_x_into_y(pcoord, pcoord2);
    for (i = 0; i <= 99; i++) {
        t = t + h;
        y = eval_term1(sep1->separatrice, t);
        if ((t * t + y * y) > epsilon * epsilon)
            break;
        if (sep1->d) {
            point[0] = x0 + a11 * y + a12 * t;
            point[1] = y0 + a21 * y + a22 * t;
        } else {
            point[0] = x0 + a11 * t + a12 * y;
            point[1] = y0 + a21 * t + a22 * y;
        }
        last_orbit->next_point = new orbits_points;
        last_orbit = last_orbit->next_point;
        dashes = true;

        switch (chart) {
        case CHART_R2:
            ((spherewnd->study_)->*(spherewnd->study_->R2_to_sphere))(
                point[0], point[1], pcoord);
            color = findSepColor2(spherewnd->study_->gcf_, sep1->type, point);
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
                type = sep1->type;
                color = findSepColor2(spherewnd->study_->gcf_U1_, type, point);
            } else {
                spherewnd->study_->VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (spherewnd->study_->dir_vec_field_ == 1)
                        dir *= -1;
                }
                spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
                if (spherewnd->study_->dir_vec_field_ == 1)
                    type = change_type(sep1->type);
                else
                    type = sep1->type;
                color = findSepColor2(spherewnd->study_->gcf_V1_, type, point);
            }
            break;
        case CHART_V1:
            ((spherewnd->study_)->*(spherewnd->study_->V1_to_sphere))(
                point[0], point[1], pcoord);
            if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1))
                spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
            color = findSepColor2(spherewnd->study_->gcf_V1_, sep1->type, point);
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
                type = sep1->type;
                color = findSepColor2(spherewnd->study_->gcf_U2_, type, point);
            } else {
                spherewnd->study_->VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                    if (spherewnd->study_->dir_vec_field_ == 1)
                        dir *= -1;
                }
                spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
                if (spherewnd->study_->dir_vec_field_ == 1)
                    type = change_type(sep1->type);
                else
                    type = sep1->type;
                color = findSepColor2(spherewnd->study_->gcf_V2_, type, point);
            }
            break;
        case CHART_V2:
            ((spherewnd->study_)->*(spherewnd->study_->V2_to_sphere))(
                point[0], point[1], pcoord);
            if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1))
                spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
            color = findSepColor2(spherewnd->study_->gcf_V2_, sep1->type, point);
            break;
        }

        copy_x_into_y(pcoord, last_orbit->pcoord);
        last_orbit->color = color;
        last_orbit->dashes = dashes * spherewnd->study_->config_dashes_;
        last_orbit->dir = dir;
        last_orbit->type = type;
        if (last_orbit->dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }

    last_orbit->next_point = integrate_sep(
        spherewnd, pcoord, spherewnd->study_->config_step_, last_orbit->dir,
        type, spherewnd->study_->config_intpoints_, &sep2);
    *orbit = sep2;
    return (first_orbit_);
}

static double power(double a, double b)
{
    if (b == 0)
        return (1.);
    else
        return (pow(a, b));
}

void make_transformations(transformations *trans, double x0, double y0,
                          double *point)
{
    double x, y;

    x = x0;
    y = y0;
    while (trans != nullptr) {
        point[0] = trans->x0 +
                   (double)trans->c1 * power(x, (double)trans->d1) *
                       power(y, (double)trans->d2);
        point[1] = trans->y0 +
                   (double)trans->c2 * power(x, (double)trans->d3) *
                       power(y, (double)trans->d4);
        x = point[0];
        y = point[1];
        trans = trans->next_trans;
    }
}

static orbits_points *plot_sep_blow_up(WSphere *spherewnd, double x0, double y0,
                                       int chart, double epsilon,
                                       blow_up_points *de_sep,
                                       orbits_points **orbit)
{
    double h, t = 0, y, pcoord[3], pcoord2[3], point[2];
    int i, color, dir, dashes, type, ok = true;
    orbits_points *first_orbit_ = nullptr, *last_orbit = nullptr, *sep;

    /* if we have a line of singularities at infinity then we have to change the
    chart if the chart is V1 or V2 */
    if (spherewnd->study_->singinf_) {
        if (chart == CHART_V1)
            chart = CHART_U1;
        else if (chart == CHART_V2)
            chart = CHART_U2;
    }

    h = epsilon / 100;

    type = de_sep->type;

    switch (de_sep->type) {
    case OT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    case OT_CENT_STABLE:
        dir = OT_STABLE;
        break;
    case OT_CENT_UNSTABLE:
        dir = OT_UNSTABLE;
        break;
    default:
        dir = 0;
        break;
    }
    first_orbit_ = new orbits_points;
    last_orbit = first_orbit_;
    point[0] = x0;
    point[1] = y0;
    switch (chart) {
    case CHART_R2:
        ((spherewnd->study_)->*(spherewnd->study_->R2_to_sphere))(x0, y0,
                                                                  pcoord);
        color = findSepColor2(spherewnd->study_->gcf_, de_sep->type, point);
        break;
    case CHART_U1:
        ((spherewnd->study_)->*(spherewnd->study_->U1_to_sphere))(x0, y0,
                                                                  pcoord);
        color = findSepColor2(spherewnd->study_->gcf_U1_, de_sep->type, point);
        break;
    case CHART_V1:
        ((spherewnd->study_)->*(spherewnd->study_->V1_to_sphere))(x0, y0,
                                                                  pcoord);
        if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1))
            spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1], pcoord[2],
                                             point);
        color = findSepColor2(spherewnd->study_->gcf_V1_, de_sep->type, point);
        break;
    case CHART_U2:
        ((spherewnd->study_)->*(spherewnd->study_->U2_to_sphere))(x0, y0,
                                                                  pcoord);
        color = findSepColor2(spherewnd->study_->gcf_U2_, de_sep->type, point);
        break;
    case CHART_V2:
        ((spherewnd->study_)->*(spherewnd->study_->V2_to_sphere))(x0, y0,
                                                                  pcoord);
        if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1))
            spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1], pcoord[2],
                                             point);
        color = findSepColor2(spherewnd->study_->gcf_V2_, de_sep->type, point);
        break;
    default:
        color = 0;
        break;
    }
    copy_x_into_y(pcoord, last_orbit->pcoord);
    last_orbit->color = color;
    last_orbit->dashes = 0;
    copy_x_into_y(pcoord, pcoord2);
    for (i = 0; i <= 99; i++) {
        last_orbit->next_point = new orbits_points;
        last_orbit = last_orbit->next_point;
        dashes = true;
        t = t + h;
        y = eval_term1(de_sep->sep, t);
        make_transformations(
            de_sep->trans, de_sep->x0 + de_sep->a11 * t + de_sep->a12 * y,
            de_sep->y0 + de_sep->a21 * t + de_sep->a22 * y, point);
        switch (chart) {
        case CHART_R2:
            ((spherewnd->study_)->*(spherewnd->study_->R2_to_sphere))(
                point[0], point[1], pcoord);
            color = findSepColor2(spherewnd->study_->gcf_, de_sep->type, point);
            break;
        case CHART_U1:
            if (point[1] >= 0 || !spherewnd->study_->singinf_) {
                ((spherewnd->study_)->*(spherewnd->study_->U1_to_sphere))(
                    point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep->type;
                color = findSepColor2(spherewnd->study_->gcf_U1_, type, point);
            } else {
                spherewnd->study_->VV1_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
                if (spherewnd->study_->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                color = findSepColor2(spherewnd->study_->gcf_V1_, type, point);
            }
            break;
        case CHART_V1:
            ((spherewnd->study_)->*(spherewnd->study_->V1_to_sphere))(
                point[0], point[1], pcoord);
            if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1))
                spherewnd->study_->psphere_to_V1(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
            color =
                findSepColor2(spherewnd->study_->gcf_V1_, de_sep->type, point);
            break;
        case CHART_U2:
            if (point[1] >= 0 || !spherewnd->study_->singinf_) {
                ((spherewnd->study_)->*(spherewnd->study_->U2_to_sphere))(
                    point[0], point[1], pcoord);
                if (!ok) {
                    dashes = false;
                    ok = true;
                }
                type = de_sep->type;
                color = findSepColor2(spherewnd->study_->gcf_U2_, type, point);
            } else {
                spherewnd->study_->VV2_to_psphere(point[0], point[1], pcoord);
                if (ok) {
                    dashes = false;
                    ok = false;
                }
                spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
                if (spherewnd->study_->dir_vec_field_ == 1)
                    type = change_type(de_sep->type);
                else
                    type = de_sep->type;
                color = findSepColor2(spherewnd->study_->gcf_V2_, type, point);
            }
            break;
        case CHART_V2:
            ((spherewnd->study_)->*(spherewnd->study_->V2_to_sphere))(
                point[0], point[1], pcoord);
            if ((spherewnd->study_->p_ == 1) && (spherewnd->study_->q_ == 1))
                spherewnd->study_->psphere_to_V2(pcoord[0], pcoord[1],
                                                 pcoord[2], point);
            color =
                findSepColor2(spherewnd->study_->gcf_V2_, de_sep->type, point);
            break;
        }
        copy_x_into_y(pcoord, last_orbit->pcoord);
        last_orbit->color = color;
        last_orbit->dashes = dashes * spherewnd->study_->config_dashes_;
        last_orbit->dir = dir;
        last_orbit->type = type;
        if (last_orbit->dashes)
            (*plot_l)(spherewnd, pcoord, pcoord2, color);
        else
            (*plot_p)(spherewnd, pcoord, color);
        copy_x_into_y(pcoord, pcoord2);
    }
    de_sep->point[0] = t;
    de_sep->point[1] = y;
    de_sep->blow_up_vec_field = true;
    last_orbit->next_point = integrate_blow_up(
        spherewnd, pcoord2, de_sep, spherewnd->study_->config_step_, dir,
        last_orbit->type, &sep, chart);
    *orbit = sep;
    return (first_orbit_);
}

/*
void start_plot_saddle_sep( WSphere * spherewnd )
{
    double p[3];
    orbits_points *points;

    draw_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point);
    if (spherewnd->study_->selected_sep_->last_sep_point) {
        copy_x_into_y(spherewnd->study_->selected_sep_->last_sep_point->pcoord,
p);
        spherewnd->study_->selected_sep_->last_sep_point->next_point =
            integrate_sep(spherewnd, p,
                spherewnd->study_->config_currentstep_,
                spherewnd->study_->selected_sep_->last_sep_point->dir,
                spherewnd->study_->selected_sep_->last_sep_point->type,
                spherewnd->study_->config_intpoints_, &points);
        spherewnd->study_->selected_sep_->last_sep_point = points;
    } else {
        spherewnd->study_->selected_sep_->first_sep_point =
            plot_separatrice(spherewnd,
                spherewnd->study_->selected_saddle_point_->x0,
                spherewnd->study_->selected_saddle_point_->y0,
                spherewnd->study_->selected_saddle_point_->a11,
                spherewnd->study_->selected_saddle_point_->a12,
                spherewnd->study_->selected_saddle_point_->a21,
                spherewnd->study_->selected_saddle_point_->a22,
                spherewnd->study_->selected_saddle_point_->epsilon,
                spherewnd->study_->selected_sep_, &points,
                spherewnd->study_->selected_saddle_point_->chart);
        spherewnd->study_->selected_sep_->last_sep_point = points;
    }
}

void cont_plot_saddle_sep( WSphere * spherewnd )
{
    double p[3];
    orbits_points *points;

    copy_x_into_y(spherewnd->study_->selected_sep_->last_sep_point->pcoord,p);
    spherewnd->study_->selected_sep_->last_sep_point->next_point =
        integrate_sep(spherewnd, p,
            spherewnd->study_->config_currentstep_,
            spherewnd->study_->selected_sep_->last_sep_point->dir,
            spherewnd->study_->selected_sep_->last_sep_point->type,
            spherewnd->study_->config_intpoints_, &points);
    spherewnd->study_->selected_sep_->last_sep_point = points;
}

void plot_next_saddle_sep( WSphere * spherewnd )
{
    draw_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point);
    if (
!(spherewnd->study_->selected_sep_=spherewnd->study_->selected_sep_->next_sep) )
        spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_saddle_point_->separatrices;
    start_plot_saddle_sep( spherewnd );
}

void select_next_saddle_sep( WSphere * spherewnd )
{
    draw_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point);
    if ( !(spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_sep_->next_sep) )
        spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_saddle_point_->separatrices;
    draw_selected_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point,CW_SEP);
}*/

static void plot_all_saddle_sep(WSphere *spherewnd, saddle *point)
{
    sep *sep1;
    orbits_points *points;
    double p[3];

    while (point != nullptr) {
        if (point->notadummy) {
            sep1 = point->separatrices;
            while (sep1) {
                if (sep1->last_sep_point) {
                    copy_x_into_y(sep1->last_sep_point->pcoord, p);
                    sep1->last_sep_point->next_point = integrate_sep(
                        spherewnd, p, spherewnd->study_->config_currentstep_,
                        sep1->last_sep_point->dir, sep1->last_sep_point->type,
                        spherewnd->study_->config_intpoints_, &points);
                    sep1->last_sep_point = points;
                } else {
                    sep1->first_sep_point = plot_separatrice(
                        spherewnd, point->x0, point->y0, point->a11, point->a12,
                        point->a21, point->a22, point->epsilon, sep1, &points,
                        point->chart);
                    sep1->last_sep_point = points;
                }
                sep1 = sep1->next_sep;
            }
        }
        point = point->next_saddle;
    }
}
/*
void start_plot_se_sep( WSphere * spherewnd )
{
    orbits_points *points;
    double p[3];

    draw_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point);
    if (spherewnd->study_->selected_sep_->last_sep_point) {
        copy_x_into_y(spherewnd->study_->selected_sep_->last_sep_point->pcoord,p);
        spherewnd->study_->selected_sep_->last_sep_point->next_point =
            integrate_sep(spherewnd, p, spherewnd->study_->config_currentstep_,
spherewnd->study_->selected_sep_->last_sep_point->dir,
                    spherewnd->study_->selected_sep_->last_sep_point->type,
spherewnd->study_->config_intpoints_, &points);
        spherewnd->study_->selected_sep_->last_sep_point = points;
    } else {
        spherewnd->study_->selected_sep_->first_sep_point =
            plot_separatrice(spherewnd,spherewnd->study_->selected_se_point_->x0,spherewnd->study_->selected_se_point_->y0,
                    spherewnd->study_->selected_se_point_->a11,
spherewnd->study_->selected_se_point_->a12,
                    spherewnd->study_->selected_se_point_->a21,
spherewnd->study_->selected_se_point_->a22,
                    spherewnd->study_->selected_se_point_->epsilon,
spherewnd->study_->selected_sep_, &points,
                    spherewnd->study_->selected_se_point_->chart);
        spherewnd->study_->selected_sep_->last_sep_point = points;
    }
}

void cont_plot_se_sep( WSphere * spherewnd )
{
    double p[3];
    orbits_points *points;

    copy_x_into_y(spherewnd->study_->selected_sep_->last_sep_point->pcoord,p);
    spherewnd->study_->selected_sep_->last_sep_point->next_point =
        integrate_sep(spherewnd,p,spherewnd->study_->config_currentstep_,
spherewnd->study_->selected_sep_->last_sep_point->dir,
                spherewnd->study_->selected_sep_->last_sep_point->type,
spherewnd->study_->config_intpoints_, &points);
    spherewnd->study_->selected_sep_->last_sep_point = points;
}

void plot_next_se_sep( WSphere * spherewnd )
{
    draw_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point);
    if ( !(spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_sep_->next_sep) )
        spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_se_point_->separatrices;
    start_plot_se_sep( spherewnd );
}


void select_next_se_sep( WSphere * spherewnd )
{
    draw_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point);
    if ( !(spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_sep_->next_sep) )
        spherewnd->study_->selected_sep_ =
spherewnd->study_->selected_se_point_->separatrices;
    draw_selected_sep(spherewnd,spherewnd->study_->selected_sep_->first_sep_point,CW_SEP);
}
*/
static void plot_all_se_sep(WSphere *spherewnd, semi_elementary *point)
{
    sep *sep1;
    orbits_points *points;
    double p[3];

    while (point != nullptr) {
        if (point->notadummy) {
            sep1 = point->separatrices;
            while (sep1 != nullptr) {
                if (sep1->last_sep_point) {
                    copy_x_into_y(sep1->last_sep_point->pcoord, p);
                    sep1->last_sep_point->next_point = integrate_sep(
                        spherewnd, p, spherewnd->study_->config_currentstep_,
                        sep1->last_sep_point->dir, sep1->last_sep_point->type,
                        spherewnd->study_->config_intpoints_, &points);
                    sep1->last_sep_point = points;
                } else {
                    sep1->first_sep_point = plot_separatrice(
                        spherewnd, point->x0, point->y0, point->a11, point->a12,
                        point->a21, point->a22, point->epsilon, sep1, &points,
                        point->chart);
                    sep1->last_sep_point = points;
                }
                sep1 = sep1->next_sep;
            }
        }
        point = point->next_se;
    }
}
/*
void start_plot_de_sep( WSphere * spherewnd )
{
  orbits_points *points;
  double p[3];

    draw_sep(spherewnd,spherewnd->study_->selected_de_sep_->first_sep_point);
    if ( spherewnd->study_->selected_de_sep_->last_sep_point ) {
        if ( spherewnd->study_->selected_de_sep_->blow_up_vec_field ) {
            copy_x_into_y(
spherewnd->study_->selected_de_sep_->last_sep_point->pcoord, p );
            spherewnd->study_->selected_de_sep_->last_sep_point->next_point =
                integrate_blow_up( spherewnd, p,
                    spherewnd->study_->selected_de_sep_,
                    spherewnd->study_->config_currentstep_,
                    spherewnd->study_->selected_de_sep_->last_sep_point->dir,
                    spherewnd->study_->selected_de_sep_->last_sep_point->type,
                    &points,
                    spherewnd->study_->selected_de_point_->chart );

            spherewnd->study_->selected_de_sep_->last_sep_point = points;
        } else {
            copy_x_into_y(
spherewnd->study_->selected_de_sep_->last_sep_point->pcoord, p );
            spherewnd->study_->selected_de_sep_->last_sep_point->next_point =
                integrate_sep( spherewnd, p,
                    spherewnd->study_->config_currentstep_,
                    spherewnd->study_->selected_de_sep_->last_sep_point->dir,
                    spherewnd->study_->selected_de_sep_->last_sep_point->type,
                    spherewnd->study_->config_intpoints_,
                    &points );

            spherewnd->study_->selected_de_sep_->last_sep_point = points;
        }
    } else {
        spherewnd->study_->selected_de_sep_->first_sep_point =
            plot_sep_blow_up( spherewnd,
                spherewnd->study_->selected_de_point_->x0,
                spherewnd->study_->selected_de_point_->y0,
                spherewnd->study_->selected_de_point_->chart,
                spherewnd->study_->selected_de_point_->epsilon,
                spherewnd->study_->selected_de_sep_,
                &points );

            spherewnd->study_->selected_de_sep_->last_sep_point = points;
    }
}

void cont_plot_de_sep( WSphere * spherewnd )
{
    double p[3];
    orbits_points * points;

    copy_x_into_y( spherewnd->study_->selected_de_sep_->last_sep_point->pcoord, p
);
    if ( spherewnd->study_->selected_de_sep_->blow_up_vec_field ) {
        spherewnd->study_->selected_de_sep_->last_sep_point->next_point =
integrate_blow_up(
                    spherewnd,
                    //spherewnd->study_->selected_de_point_->x0,
                    //spherewnd->study_->selected_de_point_->y0,
                    p,
                    spherewnd->study_->selected_de_sep_,
                    spherewnd->study_->config_currentstep_,
                    spherewnd->study_->selected_de_sep_->last_sep_point->dir,
                    spherewnd->study_->selected_de_sep_->last_sep_point->type,
                    &points,
                    spherewnd->study_->selected_de_point_->chart );
    } else {
        spherewnd->study_->selected_de_sep_->last_sep_point->next_point =
integrate_sep(
                    spherewnd,
                    p,
                    spherewnd->study_->config_currentstep_,
                    spherewnd->study_->selected_de_sep_->last_sep_point->dir,
                    spherewnd->study_->selected_de_sep_->last_sep_point->type,
                    spherewnd->study_->config_intpoints_,
                    &points);
    }

    spherewnd->study_->selected_de_sep_->last_sep_point = points;
}

void plot_next_de_sep( WSphere * spherewnd )
{
    draw_sep(spherewnd,spherewnd->study_->selected_de_sep_->first_sep_point);
    if ( !(spherewnd->study_->selected_de_sep_ =
spherewnd->study_->selected_de_sep_->next_blow_up_point) )
        spherewnd->study_->selected_de_sep_ =
spherewnd->study_->selected_de_point_->blow_up;
    start_plot_de_sep( spherewnd );
}


void select_next_de_sep( WSphere * spherewnd )
{
    draw_sep(spherewnd,spherewnd->study_->selected_de_sep_->first_sep_point);
    if ( !(spherewnd->study_->selected_de_sep_ =
spherewnd->study_->selected_de_sep_->next_blow_up_point) )
        spherewnd->study_->selected_de_sep_ =
spherewnd->study_->selected_de_point_->blow_up;
    draw_selected_sep(spherewnd,spherewnd->study_->selected_de_sep_->first_sep_point,CW_SEP);
}
*/
static void plot_all_de_sep(WSphere *spherewnd, struct degenerate *point)
{
    blow_up_points *de_sep;
    orbits_points *sep;
    double p[3];

    while (point != nullptr) {
        if (point->notadummy) {
            de_sep = point->blow_up;
            while (de_sep != nullptr) {
                if (de_sep->last_sep_point) {
                    copy_x_into_y(de_sep->last_sep_point->pcoord, p);
                    if (de_sep->blow_up_vec_field) {
                        de_sep->last_sep_point->next_point = integrate_blow_up(
                            spherewnd, p, de_sep,
                            spherewnd->study_->config_currentstep_,
                            de_sep->last_sep_point->dir,
                            de_sep->last_sep_point->type, &sep, point->chart);
                    } else {
                        de_sep->last_sep_point->next_point = integrate_sep(
                            spherewnd, p, spherewnd->study_->config_currentstep_,
                            de_sep->last_sep_point->dir,
                            de_sep->last_sep_point->type,
                            spherewnd->study_->config_intpoints_, &sep);
                    }
                    de_sep->last_sep_point = sep;
                } else {
                    de_sep->first_sep_point = plot_sep_blow_up(
                        spherewnd, point->x0, point->y0, point->chart,
                        point->epsilon, de_sep, &sep);
                    de_sep->last_sep_point = sep;
                }
                de_sep = de_sep->next_blow_up_point;
            }
        }
        point = point->next_de;
    }
}

void plot_all_sep(WSphere *spherewnd)
{
    plot_all_saddle_sep(spherewnd, spherewnd->study_->first_saddle_point_);
    plot_all_se_sep(spherewnd, spherewnd->study_->first_se_point_);
    plot_all_de_sep(spherewnd, spherewnd->study_->first_de_point_);
}

void draw_sep(WSphere *spherewnd, orbits_points *sep)
{
    double pcoord[3];

    if (sep) {
        do {
            if (sep->dashes)
                (*plot_l)(spherewnd, sep->pcoord, pcoord, sep->color);
            else
                (*plot_p)(spherewnd, sep->pcoord, sep->color);
            copy_x_into_y(sep->pcoord, pcoord);
        } while ((sep = sep->next_point) != nullptr);
    }
}

void draw_selected_sep(WSphere *spherewnd, orbits_points *sep, int color)
{
    double pcoord[3];

    if (sep) {
        do {
            if (sep->dashes)
                (*plot_l)(spherewnd, sep->pcoord, pcoord, color);
            else
                (*plot_p)(spherewnd, sep->pcoord, color);
            copy_x_into_y(sep->pcoord, pcoord);
        } while ((sep = sep->next_point) != nullptr);
    }
}

void change_epsilon_saddle(WSphere *spherewnd, double epsilon)
{
    sep *separatrice;

    spherewnd->study_->selected_saddle_point_->epsilon = epsilon;
    separatrice = spherewnd->study_->selected_saddle_point_->separatrices;
    while (separatrice != nullptr) {
        draw_selected_sep(spherewnd, separatrice->first_sep_point, CBACKGROUND);
        spherewnd->study_->deleteOrbitPoint(separatrice->first_sep_point);
        separatrice->last_sep_point = nullptr;
        separatrice->first_sep_point = nullptr;
        separatrice = separatrice->next_sep;
    }
}

void change_epsilon_se(WSphere *spherewnd, double epsilon)
{
    sep *separatrice;

    spherewnd->study_->selected_se_point_->epsilon = epsilon;
    separatrice = spherewnd->study_->selected_se_point_->separatrices;
    while (separatrice != nullptr) {
        draw_selected_sep(spherewnd, separatrice->first_sep_point, CBACKGROUND);
        spherewnd->study_->deleteOrbitPoint(separatrice->first_sep_point);
        separatrice->last_sep_point = nullptr;
        separatrice->first_sep_point = nullptr;
        separatrice = separatrice->next_sep;
    }
}

void change_epsilon_de(WSphere *spherewnd, double epsilon)
{
    blow_up_points *separatrice;

    spherewnd->study_->selected_de_point_->epsilon = epsilon;
    separatrice = spherewnd->study_->selected_de_point_->blow_up;
    while (separatrice != nullptr) {
        draw_selected_sep(spherewnd, separatrice->first_sep_point, CBACKGROUND);
        spherewnd->study_->deleteOrbitPoint(separatrice->first_sep_point);
        separatrice->last_sep_point = nullptr;
        separatrice->first_sep_point = nullptr;
        separatrice = separatrice->next_blow_up_point;
    }
}
