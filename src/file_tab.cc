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

#include "file_tab.h"

#include "MyLogger.h"
#include "custom.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "math_separatrice.h"

#include <string>

using namespace Wt;

/*
    This file contains the code to read the information from reduce/maple.
    The relevant structures for saddles etc are set up here.
*/

// -----------------------------------------------------------------------
//                              WVFStudy CONSTRUCTOR
// -----------------------------------------------------------------------

WVFStudy::WVFStudy(double projection) : config_projection_(projection)
{
    // initialize vector field structures:
    f_vec_field_[0] = nullptr;
    f_vec_field_[1] = nullptr;
    vec_field_U1_[0] = nullptr;
    vec_field_U1_[1] = nullptr;
    vec_field_U2_[0] = nullptr;
    vec_field_U2_[1] = nullptr;
    vec_field_V1_[0] = nullptr;
    vec_field_V1_[1] = nullptr;
    vec_field_V2_[0] = nullptr;
    vec_field_V2_[1] = nullptr;
    vec_field_C_[0] = nullptr;
    vec_field_C_[1] = nullptr;

    // initialize singular points structures:
    first_saddle_point_ = nullptr;
    first_se_point_ = nullptr;
    first_node_point_ = nullptr;
    first_sf_point_ = nullptr;
    first_wf_point_ = nullptr;
    first_de_point_ = nullptr;

    // initialize GCF:
    gcf_ = nullptr;
    gcf_U1_ = nullptr;
    gcf_U2_ = nullptr;
    gcf_V1_ = nullptr;
    gcf_V2_ = nullptr;
    gcf_C_ = nullptr;
    gcf_points_ = nullptr;
    last_gcf_point_ = nullptr;

    // initialize curves and isoclines
    last_curves_point_ = nullptr;
    last_isoclines_point_ = nullptr;

    // initialize limit cycles & orbits
    first_lim_cycle_ = nullptr;
    first_orbit_ = nullptr;
    current_orbit_ = nullptr;

    // initialize others
    xmin_ = -1.0;
    xmax_ = 1.0;
    ymin_ = -1.0;
    ymax_ = 1.0;
    p_ = 1;
    q_ = 1;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    singinf_ = false;
    dir_vec_field_ = 1;

    // initialize parameters
    // number of orbits in the limit cycle window
    config_lc_value_ = DEFAULT_LCORBITS;
    // number of points in the limit cycle window
    config_lc_numpoints_ = DEFAULT_LCPOINTS;
    // maximum step size
    config_hma_ = DEFAULT_HMA;
    // minimum step size
    config_hmi_ = DEFAULT_HMI;
    // step size
    config_step_ = DEFAULT_STEPSIZE;
    // current step size (during integration)
    config_currentstep_ = DEFAULT_STEPSIZE;
    // tolerance
    config_tolerance_ = DEFAULT_TOLERANCE;
    // number of points to integrate
    config_intpoints_ = DEFAULT_INTPOINTS;
    // line style (dashes or points)
    config_dashes_ = DEFAULT_LINESTYLE;
    config_kindvf_ = DEFAULT_INTCONFIG;
}

// -----------------------------------------------------------------------
//                              WVFStudy copy functions
// -----------------------------------------------------------------------

WVFStudy::WVFStudy(const WVFStudy &obj)
{
    f_vec_field_[0] = copy_term2(obj.f_vec_field_[0]);
    f_vec_field_[1] = copy_term2(obj.f_vec_field_[1]);
    vec_field_U1_[0] = copy_term2(obj.vec_field_U1_[0]);
    vec_field_U1_[1] = copy_term2(obj.vec_field_U1_[1]);
    vec_field_U2_[0] = copy_term2(obj.vec_field_U2_[0]);
    vec_field_U2_[1] = copy_term2(obj.vec_field_U2_[1]);
    vec_field_V1_[0] = copy_term2(obj.vec_field_V1_[0]);
    vec_field_V1_[1] = copy_term2(obj.vec_field_V1_[1]);
    vec_field_V2_[0] = copy_term2(obj.vec_field_V2_[0]);
    vec_field_V2_[1] = copy_term2(obj.vec_field_V2_[1]);
    vec_field_C_[0] = copy_term3(obj.vec_field_C_[0]);
    vec_field_C_[1] = copy_term3(obj.vec_field_C_[1]);

    first_saddle_point_ = copy_saddle(obj.first_saddle_point_);
    first_se_point_ = copy_semi_elementary(obj.first_se_point_);
    first_node_point_ = copy_node(obj.first_node_point_);
    first_sf_point_ = copy_strong_focus(obj.first_sf_point_);
    first_wf_point_ = copy_weak_focus(obj.first_wf_point_);
    first_de_point_ = copy_degenerate(obj.first_de_point_);

    gcf_ = copy_term2(obj.gcf_);
    gcf_U1_ = copy_term2(obj.gcf_U1_);
    gcf_U2_ = copy_term2(obj.gcf_U2_);
    gcf_V1_ = copy_term2(obj.gcf_V1_);
    gcf_V2_ = copy_term2(obj.gcf_V2_);
    gcf_C_ = copy_term3(obj.gcf_C_);
    gcf_points_ = copy_orbits_points(obj.gcf_points_);
    last_gcf_point_ = copy_orbits_points(obj.last_gcf_point_);

    last_curves_point_ = copy_orbits_points(obj.last_curves_point_);
    last_isoclines_point_ = copy_orbits_points(obj.last_isoclines_point_);

    first_lim_cycle_ = copy_orbits(obj.first_lim_cycle_);
    first_orbit_ = copy_orbits(obj.first_orbit_);
    current_orbit_ = copy_orbits(obj.current_orbit_);

    xmin_ = obj.xmin_;
    xmax_ = obj.xmax_;
    ymin_ = obj.ymin_;
    ymax_ = obj.ymax_;
    p_ = obj.p_;
    q_ = obj.q_;
    typeofstudy_ = obj.typeofstudy_;
    singinf_ = obj.singinf_;
    dir_vec_field_ = obj.dir_vec_field_;

    config_lc_value_ = obj.config_lc_value_;
    config_lc_numpoints_ = obj.config_lc_numpoints_;
    config_hma_ = obj.config_hma_;
    config_hmi_ = obj.config_hmi_;
    config_step_ = obj.config_step_;
    config_currentstep_ = obj.config_currentstep_;
    config_tolerance_ = obj.config_tolerance_;
    config_intpoints_ = obj.config_intpoints_;
    config_dashes_ = obj.config_dashes_;
    config_kindvf_ = obj.config_kindvf_;

    // TODO: copy isoclines and curves
    std::vector<curves>::const_iterator it1;
    for (it1 = obj.curve_vector_.begin(); it1 != obj.curve_vector_.end();
         it1++) {
        curves *curve = copy_curves((curves *)&(*it1));
        curve_vector_.push_back(*curve);
    }
    std::vector<isoclines>::const_iterator it2;
    for (it2 = obj.isocline_vector_.begin(); it2 != obj.isocline_vector_.end();
         it2++) {
        isoclines *isoc = copy_isoclines((isoclines *)&(*it2));
        isocline_vector_.push_back(*isoc);
    }
}

orbits *WVFStudy::copy_orbits(orbits *p)
{
    orbits *result = nullptr;
    orbits *o = p;
    orbits *last = nullptr;
    while (o != nullptr) {
        orbits *q = new orbits;
        q->pcoord[0] = o->pcoord[0];
        q->pcoord[1] = o->pcoord[1];
        q->pcoord[2] = o->pcoord[2];
        q->color = o->color;
        q->f_orbits = copy_orbits_points(o->f_orbits);
        q->current_f_orbits = copy_orbits_points(o->current_f_orbits);
        q->next_orbit = nullptr;
        if (last != nullptr)
            last->next_orbit = q;
        else
            result = q;
        last = q;
        o = o->next_orbit;
    }
    return result;
}

P4ORBIT WVFStudy::copy_orbits_points(P4ORBIT p)
{
    P4ORBIT result = nullptr;
    P4ORBIT o = p;
    P4ORBIT last = nullptr;
    while (o != nullptr) {
        P4ORBIT q = new orbits_points;
        q->color = o->color;
        q->pcoord[0] = o->pcoord[0];
        q->pcoord[1] = o->pcoord[1];
        q->pcoord[2] = o->pcoord[2];
        q->dashes = o->dashes;
        q->dir = o->dir;
        q->type = o->type;
        q->next_point = nullptr;
        if (last != nullptr)
            last->next_point = q;
        else
            result = q;
        last = q;
        o = o->next_point;
    }
    return result;
}

P4POLYNOM1 WVFStudy::copy_term1(P4POLYNOM1 p)
{
    P4POLYNOM1 result = nullptr;
    P4POLYNOM1 o = p;
    P4POLYNOM1 last = nullptr;
    while (o != nullptr) {
        P4POLYNOM1 q = new term1;
        q->exp = o->exp;
        q->coeff = o->coeff;
        q->next_term1 = nullptr;
        if (last != nullptr)
            last->next_term1 = q;
        else
            result = q;
        last = q;
        o = o->next_term1;
    }
    return result;
}

P4POLYNOM2 WVFStudy::copy_term2(P4POLYNOM2 p)
{
    P4POLYNOM2 result = nullptr;
    P4POLYNOM2 o = p;
    P4POLYNOM2 last = nullptr;
    while (o != nullptr) {
        P4POLYNOM2 q = new term2;
        q->exp_x = o->exp_x;
        q->exp_y = o->exp_y;
        q->coeff = o->coeff;
        q->next_term2 = nullptr;
        if (last != nullptr)
            last->next_term2 = q;
        else
            result = q;
        last = q;
        o = o->next_term2;
    }
    return result;
}

P4POLYNOM3 WVFStudy::copy_term3(P4POLYNOM3 p)
{
    P4POLYNOM3 result = nullptr;
    P4POLYNOM3 o = p;
    P4POLYNOM3 last = nullptr;
    while (o != nullptr) {
        P4POLYNOM3 q = new term3;
        q->exp_r = o->exp_r;
        q->exp_Co = o->exp_Co;
        q->exp_Si = o->exp_Si;
        q->coeff = o->coeff;
        q->next_term3 = nullptr;
        if (last != nullptr)
            last->next_term3 = q;
        else
            result = q;
        last = q;
        o = o->next_term3;
    }
    return result;
}

sep *WVFStudy::copy_sep(sep *p)
{
    sep *result = nullptr;
    sep *o = p;
    sep *last = nullptr;
    while (o != nullptr) {
        sep *q = new sep;
        q->first_sep_point = copy_orbits_points(o->first_sep_point);
        q->last_sep_point = copy_orbits_points(o->last_sep_point);
        q->type = o->type;
        q->direction = o->direction;
        q->d = o->d;
        q->notadummy = o->notadummy;
        q->separatrice = copy_term1(o->separatrice);
        q->next_sep = nullptr;
        if (last != nullptr)
            last->next_sep = q;
        else
            result = q;
        last = q;
        o = o->next_sep;
    }
    return result;
}

degenerate *WVFStudy::copy_degenerate(degenerate *p)
{
    degenerate *result = nullptr;
    degenerate *o = p;
    degenerate *last = nullptr;
    while (o != nullptr) {
        degenerate *q = new degenerate;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->chart = o->chart;
        q->epsilon = o->epsilon;
        q->notadummy = o->notadummy;
        q->blow_up = copy_blow_up_points(o->blow_up);
        q->next_de = nullptr;
        if (last != nullptr)
            last->next_de = q;
        else
            result = q;
        last = q;
        o = o->next_de;
    }
    return result;
}

blow_up_points *WVFStudy::copy_blow_up_points(blow_up_points *p)
{
    blow_up_points *result = nullptr;
    blow_up_points *o = p;
    blow_up_points *last = nullptr;
    while (o != nullptr) {
        blow_up_points *q = new blow_up_points;
        q->n = o->n;
        q->trans = copy_transformations(o->trans);
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->a11 = o->a11;
        q->a12 = o->a12;
        q->a21 = o->a21;
        q->a22 = o->a22;
        q->vector_field[0] = copy_term2(o->vector_field[0]);
        q->vector_field[1] = copy_term2(o->vector_field[1]);
        q->sep = copy_term1(o->sep);
        q->type = o->type;
        q->blow_up_vec_field = o->blow_up_vec_field;
        q->point[0] = o->point[0];
        q->point[1] = o->point[1];
        q->first_sep_point = copy_orbits_points(o->first_sep_point);
        q->last_sep_point = copy_orbits_points(o->last_sep_point);
        q->next_blow_up_point = nullptr;
        if (last != nullptr)
            last->next_blow_up_point = q;
        else
            result = q;
        last = q;
        o = o->next_blow_up_point;
    }
    return result;
}

transformations *WVFStudy::copy_transformations(transformations *p)
{
    transformations *result = nullptr;
    transformations *o = p;
    transformations *last = nullptr;
    while (o != nullptr) {
        transformations *q = new transformations;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->c1 = o->c1;
        q->c2 = o->c2;
        q->d1 = o->d1;
        q->d2 = o->d2;
        q->d3 = o->d3;
        q->d4 = o->d4;
        q->d = o->d;
        q->next_trans = nullptr;
        if (last != nullptr)
            last->next_trans = q;
        else
            result = q;
        last = q;
        o = o->next_trans;
    }
    return result;
}

weak_focus *WVFStudy::copy_weak_focus(weak_focus *p)
{
    weak_focus *result = nullptr;
    weak_focus *o = p;
    weak_focus *last = nullptr;
    while (o != nullptr) {
        weak_focus *q = new weak_focus;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->chart = o->chart;
        q->type = o->type;
        q->next_wf = nullptr;
        if (last != nullptr)
            last->next_wf = q;
        else
            result = q;
        last = q;
        o = o->next_wf;
    }
    return result;
}

strong_focus *WVFStudy::copy_strong_focus(strong_focus *p)
{
    strong_focus *result = nullptr;
    strong_focus *o = p;
    strong_focus *last = nullptr;
    while (o != nullptr) {
        strong_focus *q = new strong_focus;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->chart = o->chart;
        q->stable = o->stable;
        q->next_sf = nullptr;
        if (last != nullptr)
            last->next_sf = q;
        else
            result = q;
        last = q;
        o = o->next_sf;
    }
    return result;
}

node *WVFStudy::copy_node(node *p)
{
    node *result = nullptr;
    node *o = p;
    node *last = nullptr;
    while (o != nullptr) {
        node *q = new node;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->chart = o->chart;
        q->stable = o->stable;
        if (last != nullptr)
            last->next_node = q;
        else
            result = q;
        last = q;
        o = o->next_node;
    }
    return result;
}

semi_elementary *WVFStudy::copy_semi_elementary(semi_elementary *p)
{
    semi_elementary *result = nullptr;
    semi_elementary *o = p;
    semi_elementary *last = nullptr;
    while (o != nullptr) {
        semi_elementary *q = new semi_elementary;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->chart = o->chart;
        q->epsilon = o->epsilon;
        q->notadummy = o->notadummy;
        q->separatrices = copy_sep(o->separatrices);
        q->vector_field[0] = copy_term2(o->vector_field[0]);
        q->vector_field[1] = copy_term2(o->vector_field[1]);
        q->a11 = o->a11;
        q->a12 = o->a12;
        q->a21 = o->a21;
        q->a22 = o->a22;
        q->type = o->type;
        q->next_se = nullptr;
        if (last != nullptr)
            last->next_se = q;
        else
            result = q;
        last = q;
        o = o->next_se;
    }
    return result;
}

saddle *WVFStudy::copy_saddle(saddle *p)
{
    saddle *result = nullptr;
    saddle *o = p;
    saddle *last = nullptr;
    while (o != nullptr) {
        saddle *q = new saddle;
        q->x0 = o->x0;
        q->y0 = o->y0;
        q->chart = o->chart;
        q->epsilon = o->epsilon;
        q->notadummy = o->notadummy;
        q->separatrices = copy_sep(o->separatrices);
        q->vector_field[0] = copy_term2(o->vector_field[0]);
        q->vector_field[1] = copy_term2(o->vector_field[1]);
        q->a11 = o->a11;
        q->a12 = o->a12;
        q->a21 = o->a21;
        q->a22 = o->a22;
        q->next_saddle = nullptr;
        if (last != nullptr)
            last->next_saddle = q;
        else
            result = q;
        last = q;
        o = o->next_saddle;
    }
    return result;
}

curves *WVFStudy::copy_curves(curves *p)
{
    curves *result = nullptr;
    curves *o = p;
    if (o != nullptr) {
        curves *q = new curves;
        q->r2 = copy_term2(o->r2);
        q->u1 = copy_term2(o->u1);
        q->u2 = copy_term2(o->u2);
        q->v1 = copy_term2(o->v1);
        q->v2 = copy_term2(o->v2);
        q->c = copy_term3(o->c);
        q->points = copy_orbits_points(o->points);
        result = q;
    }
    return result;
}

isoclines *WVFStudy::copy_isoclines(isoclines *p)
{
    isoclines *result = nullptr;
    isoclines *o = p;
    if (o != nullptr) {
        isoclines *q = new isoclines;
        q->r2 = copy_term2(o->r2);
        q->u1 = copy_term2(o->u1);
        q->u2 = copy_term2(o->u2);
        q->v1 = copy_term2(o->v1);
        q->v2 = copy_term2(o->v2);
        q->c = copy_term3(o->c);
        q->points = copy_orbits_points(o->points);
        q->color = o->color;
        result = q;
    }
    return result;
}

// -----------------------------------------------------------------------
//                              WVFStudy DESTRUCTOR
// -----------------------------------------------------------------------

WVFStudy::~WVFStudy() { deleteVF(); }

// -----------------------------------------------------------------------
//                          WVFStudy::DeleteVF
// -----------------------------------------------------------------------

void WVFStudy::deleteVF()
{
    // Delete Vector Field
    delete_term2(f_vec_field_[0]);
    f_vec_field_[0] = nullptr;
    delete_term2(f_vec_field_[1]);
    f_vec_field_[1] = nullptr;
    delete_term2(vec_field_U1_[0]);
    vec_field_U1_[0] = nullptr;
    delete_term2(vec_field_U1_[1]);
    vec_field_U1_[1] = nullptr;
    delete_term2(vec_field_U2_[0]);
    vec_field_U2_[0] = nullptr;
    delete_term2(vec_field_U2_[1]);
    vec_field_U2_[1] = nullptr;
    delete_term2(vec_field_V1_[0]);
    vec_field_V1_[0] = nullptr;
    delete_term2(vec_field_V1_[1]);
    vec_field_V1_[1] = nullptr;
    delete_term2(vec_field_V2_[0]);
    vec_field_V2_[0] = nullptr;
    delete_term2(vec_field_V2_[1]);
    vec_field_V2_[1] = nullptr;
    delete_term3(vec_field_C_[0]);
    vec_field_C_[0] = nullptr;
    delete_term3(vec_field_C_[1]);
    vec_field_C_[1] = nullptr;

    // Delete singular points
    deleteSaddle(first_saddle_point_);
    first_saddle_point_ = nullptr;
    deleteSemiElementary(first_se_point_);
    first_se_point_ = nullptr;
    deleteNode(first_node_point_);
    first_node_point_ = nullptr;
    deleteDegenerate(first_de_point_);
    first_sf_point_ = nullptr;
    deleteStrongFocus(first_sf_point_);
    first_wf_point_ = nullptr;
    deleteWeakFocus(first_wf_point_);
    first_de_point_ = nullptr;

    // Delete GCF:
    delete_term2(gcf_);
    gcf_ = nullptr;
    delete_term2(gcf_U1_);
    gcf_U1_ = nullptr;
    delete_term2(gcf_U2_);
    gcf_U2_ = nullptr;
    delete_term2(gcf_V1_);
    gcf_V1_ = nullptr;
    delete_term2(gcf_V2_);
    gcf_V2_ = nullptr;
    delete_term3(gcf_C_);
    gcf_C_ = nullptr;
    deleteOrbitPoint(gcf_points_);
    gcf_points_ = nullptr;
    deleteOrbitPoint(last_gcf_point_);
    last_gcf_point_ = nullptr;

    // Delete curves:
    curve_vector_.clear();
    deleteOrbitPoint(last_curves_point_);
    last_curves_point_ = nullptr;

    // Delete isoclines
    isocline_vector_.clear();
    deleteOrbitPoint(last_isoclines_point_);
    last_isoclines_point_ = nullptr;

    // Delete all orbits
    deleteOrbit(first_orbit_);
    first_orbit_ = nullptr;
    current_orbit_ = nullptr;

    // Delete limit cycles
    deleteLimitCycle(first_lim_cycle_);
    first_lim_cycle_ = nullptr;
}

// -----------------------------------------------------------------------
//                          WVFStudy::DeleteSaddle
// -----------------------------------------------------------------------

void WVFStudy::deleteSaddle(saddle *p)
{
    saddle *q;

    while (p != nullptr) {
        q = p;
        p = p->next_saddle;

        delete_term2(q->vector_field[0]);
        delete_term2(q->vector_field[1]);
        if (q->notadummy)
            deleteSeparatrices(q->separatrices); // separatrices may be nullptr
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      WVFStudy::DeleteSemiElementary
// -----------------------------------------------------------------------

void WVFStudy::deleteSemiElementary(semi_elementary *p)
{
    semi_elementary *q;

    while (p != nullptr) {
        q = p;
        p = p->next_se;

        delete_term2(q->vector_field[0]);
        delete_term2(q->vector_field[1]);
        if (q->notadummy)
            deleteSeparatrices(q->separatrices); // separatrices may be nullptr
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      WVFStudy::DeleteDegenerate
// -----------------------------------------------------------------------

void WVFStudy::deleteDegenerate(degenerate *p)
{
    degenerate *q;

    while (p != nullptr) {
        q = p;
        p = p->next_de;

        if (q->notadummy)
            deleteBlowup(q->blow_up);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      WVFStudy::DeleteSeparatrices
// -----------------------------------------------------------------------

void WVFStudy::deleteSeparatrices(sep *p)
{
    sep *q;

    while (p != nullptr) {
        q = p;
        p = p->next_sep;

        deleteOrbitPoint(q->first_sep_point);
        if (q->notadummy)
            delete_term1(q->separatrice);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      WVFStudy::DeleteBlowup
// -----------------------------------------------------------------------

void WVFStudy::deleteBlowup(blow_up_points *b)
{
    blow_up_points *c;

    while (b != nullptr) {
        c = b;
        b = b->next_blow_up_point;
        deleteTransformations(c->trans);
        delete_term2(c->vector_field[0]);
        delete_term2(c->vector_field[1]);
        delete_term1(c->sep);
        deleteOrbitPoint(c->first_sep_point);
        delete c;
        c = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      WVFStudy::DeleteLimitCycle
// -----------------------------------------------------------------------

void WVFStudy::deleteLimitCycle(orbits *p)
{
    deleteOrbit(p); // limit cycle is implemented as orbit.
}

// -----------------------------------------------------------------------
//                      WVFStudy::DeleteOrbit
// -----------------------------------------------------------------------

void WVFStudy::deleteOrbit(orbits *p)
{
    orbits *q;

    while (p != nullptr) {
        q = p;
        p = p->next_orbit;

        deleteOrbitPoint(q->f_orbits);
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadTables
// -----------------------------------------------------------------------
//
// read filename_vec.tab
// read filename_inf.tab
// read filename_fin.tab
bool WVFStudy::readTables(std::string basename)
{
    FILE *fp;
    int j;
    int flag;

    deleteVF(); // initialize structures, delete previous vector field if any

    fp = fopen((basename + "_vec.tab").c_str(), "rt");
    if (fp == nullptr) {
        g_globalLogger.error("WVFStudy :: Cannot open file " + basename +
                             "_vec.tab.");
        deleteVF();
        return false;
    }

    if (fscanf(fp, "%d %d %d ", &typeofstudy_, &p_, &q_) != 3) {
        g_globalLogger.error("WVFStudy :: Cannot read typeofstudy_ in " +
                             basename + "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (fscanf(fp, "%lf %lf %lf %lf", &xmin_, &xmax_, &ymin_, &ymax_) !=
            4) {
            g_globalLogger.error("WVFStudy :: Cannot read min-max coords in " +
                                 basename + "_vec.tab.");
            deleteVF();
            fclose(fp);
            return false;
        }
        p_ = q_ = 1;
        typeofview_ = TYPEOFVIEW_PLANE;
    } else
        typeofview_ = TYPEOFVIEW_SPHERE;

    plweights_ = ((p_ == 1 && q_ == 1) ? false : true);

    double_p_ = (double)p_;
    double_q_ = (double)q_;
    double_p_plus_q_ = (double)(p_ + q_);
    double_p_minus_1_ = (double)(p_ - 1);
    double_q_minus_1_ = (double)(q_ - 1);
    double_q_minus_p_ = (double)(q_ - p_);

    if (!readGCF(fp)) {
        g_globalLogger.error("WVFStudy :: Cannot read gcf " + basename +
                             "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, f_vec_field_)) {
        g_globalLogger.error("WVFStudy :: Cannot read vector field in " +
                             basename + "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_U1_)) {
        g_globalLogger.error(
            "WVFStudy :: Cannot read vector field in U1-chart in " + basename +
            "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_V1_)) {
        g_globalLogger.error(
            "WVFStudy :: Cannot read vector field in V1-chart in " + basename +
            "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_U2_)) {
        g_globalLogger.error(
            "WVFStudy :: Cannot read vector field in U2-chart in " + basename +
            "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (!readVectorField(fp, vec_field_V2_)) {
        g_globalLogger.error(
            "WVFStudy :: Cannot read vector field in V2-chart in " + basename +
            "_vec.tab.");
        deleteVF();
        fclose(fp);
        return false;
    }

    if (plweights_) {
        if (!readVectorFieldCylinder(fp, vec_field_C_)) {
            g_globalLogger.error(
                "WVFStudy :: Cannot read vector field in Cylinder-chart in " +
                basename + "_vec.tab.");
            deleteVF();
            fclose(fp);
            return false;
        }
        singinf_ = 0;
    } else {
        if (fscanf(fp, "%d %d", &flag, &dir_vec_field_) != 2) {
            g_globalLogger.error("WVFStudy :: Cannot read sing-at-infinity "
                                 "flag and directions flag in " +
                                 basename + "_vec.tab.");
            deleteVF();
            fclose(fp);
            return false;
        }
        singinf_ = ((flag == 0) ? false : true);
    }

    fclose(fp);

    if (typeofstudy_ != TYPEOFSTUDY_INF) {
        fp = fopen((basename + "_fin.tab").c_str(), "rt");
        if (fp != nullptr) {
            if (!readPoints(fp)) {
                g_globalLogger.error(
                    "WVFStudy :: Problem reading singularity info from " +
                    basename + "_fin.tab: " + lasterror_.toUTF8() + ".");
                deleteVF();
                fclose(fp);
                return false;
            }
            fclose(fp);
        } else {
            g_globalLogger.error("WVFStudy :: Cannot open " + basename +
                                 "_fin.tab.");
            deleteVF();
            return false;
        }
    }

    if (typeofstudy_ != TYPEOFSTUDY_ONE && typeofstudy_ != TYPEOFSTUDY_FIN) {
        fp = fopen((basename + "_inf.tab").c_str(), "rt");
        if (fp != nullptr) {
            if (p_ == 1 && q_ == 1) {
                for (j = 1; j <= 2; j++) {
                    if (!readPoints(fp)) {
                        g_globalLogger.error(
                            "WVFStudy :: Cannot read singular points in " +
                            basename + "_inf.tab (" + std::to_string(j) +
                            "): " + lasterror_.toUTF8() + ".");
                        deleteVF();
                        fclose(fp);
                        return false;
                    }
                }
            } else {
                for (j = 1; j <= 4; j++) {
                    if (!readPoints(fp)) {
                        g_globalLogger.error(
                            "WVFStudy :: Cannot read singular points in " +
                            basename + "_inf.tab (" + std::to_string(j) +
                            "): " + lasterror_.toUTF8() + ".");
                        deleteVF();
                        fclose(fp);
                        return false;
                    }
                }
            }
            fclose(fp);
        } else {
            g_globalLogger.error("WVFStudy :: Cannot open " + basename +
                                 "_inf.tab.");
            deleteVF();
            return false;
        }
    }

    g_globalLogger.debug("WFStudy :: Files " + basename +
                         "_{vec,fin,inf}.tab read correctly.");
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadGCF
// -----------------------------------------------------------------------

bool WVFStudy::readGCF(FILE *fp)
{
    int N, degree_gcf;

    if (fscanf(fp, "%d", &degree_gcf) != 1)
        return false;

    if (degree_gcf) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_ = new term2;
        gcf_->next_term2 = nullptr;

        if (!readTerm2(fp, gcf_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_U1_ = new term2;
        gcf_U1_->next_term2 = nullptr;

        if (!readTerm2(fp, gcf_U1_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_U2_ = new term2;
        gcf_U2_->next_term2 = nullptr;

        if (!readTerm2(fp, gcf_U2_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        gcf_V1_ = new term2;
        gcf_V1_->next_term2 = nullptr;
        if (!readTerm2(fp, gcf_V1_, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        gcf_V2_ = new term2;
        gcf_V2_->next_term2 = nullptr;
        if (!readTerm2(fp, gcf_V2_, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            gcf_C_ = new term3;
            gcf_C_->next_term3 = nullptr;
            if (!readTerm3(fp, gcf_C_, N))
                return false;
        }
    } else {
        gcf_ = nullptr;
        gcf_U1_ = nullptr;
        gcf_U2_ = nullptr;
        gcf_V1_ = nullptr;
        gcf_V2_ = nullptr;
        gcf_C_ = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadCurve
// -----------------------------------------------------------------------
bool WVFStudy::readCurve(std::string basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;

    fp = fopen((basename + "_veccurve.tab").c_str(), "rt");
    if (fp == nullptr) {
        g_globalLogger.error("WFStudy :: Cannot open file " + basename +
                             "_veccurve.tab");
        return false;
    }

    curves new_curve;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.r2 = new term2;
        new_curve.r2->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.u1 = new term2;
        new_curve.u1->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.u2 = new term2;
        new_curve.u2->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.v1 = new term2;
        new_curve.v1->next_term2 = nullptr;
        if (!readTerm2(fp, new_curve.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        new_curve.v2 = new term2;
        new_curve.v2->next_term2 = nullptr;
        if (!readTerm2(fp, new_curve.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            new_curve.c = new term3;
            new_curve.c->next_term3 = nullptr;
            if (!readTerm3(fp, new_curve.c, N))
                return false;
        }
    } else {
        return false;
    }

    curve_vector_.push_back(new_curve);
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadIsoclines
// -----------------------------------------------------------------------
bool WVFStudy::readIsoclines(std::string basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;

    fp = fopen((basename + "_vecisoclines.tab").c_str(), "rt");
    if (fp == nullptr) {
        g_globalLogger.error("WVFSttudy :: Cannot open file " + basename +
                             "_vecisoclines.tab");
        return false;
    }

    isoclines new_isocline;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        // prepare a new isocline and link it to the list

        new_isocline.r2 = new term2;
        new_isocline.r2->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.u1 = new term2;
        new_isocline.u1->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.u2 = new term2;
        new_isocline.u2->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.v1 = new term2;
        new_isocline.v1->next_term2 = nullptr;
        if (!readTerm2(fp, new_isocline.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        new_isocline.v2 = new term2;
        new_isocline.v2->next_term2 = nullptr;
        if (!readTerm2(fp, new_isocline.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            new_isocline.c = new term3;
            new_isocline.c->next_term3 = nullptr;
            if (!readTerm3(fp, new_isocline.c, N))
                return false;
        } else {
            new_isocline.c = nullptr;
        }
    } else {
        return false;
    }

    isocline_vector_.push_back(new_isocline);
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadVectorField
// -----------------------------------------------------------------------

bool WVFStudy::readVectorField(FILE *fp, P4POLYNOM2 *vf)
{
    int M, N;

    vf[0] = new term2;
    vf[0]->next_term2 = nullptr;
    vf[1] = new term2;
    vf[1]->next_term2 = nullptr;

    if (fscanf(fp, "%d", &M) != 1)
        return false;
    if (!readTerm2(fp, vf[0], M))
        return false;
    if (fscanf(fp, "%d", &N) != 1)
        return false;
    if (!readTerm2(fp, vf[1], N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadVectorFieldCylinder
// -----------------------------------------------------------------------

bool WVFStudy::readVectorFieldCylinder(FILE *fp, P4POLYNOM3 *vf)
{
    int N;

    vf[0] = new term3;
    vf[0]->next_term3 = nullptr;
    vf[1] = new term3;
    vf[1]->next_term3 = nullptr;

    if (fscanf(fp, "%d", &N) != 1)
        return false;
    if (!readTerm3(fp, vf[0], N))
        return false;
    if (fscanf(fp, "%d", &N) != 1)
        return false;
    if (!readTerm3(fp, vf[1], N))
        return false;

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadPoints
// -----------------------------------------------------------------------

bool WVFStudy::readPoints(FILE *fp)
{
    int N, i, typ;

    if (fscanf(fp, "%d ", &N) != 1) {
        lasterror_ = "#sing not readable";
        return false;
    }

    for (i = 1; i <= N; i++) {
        if (fscanf(fp, "%d ", &typ) != 1) {
            lasterror_ =
                WString("sing #") + std::to_string(i) + " type not readable";
            return false;
        }
        switch (typ) {
        case SADDLE:
            if (!readSaddlePoint(fp)) {
                lasterror_ = WString("sing #") + std::to_string(i) +
                             " = saddle : " + lasterror_;
                return false;
            }
            break;
        case SEMI_HYPERBOLIC:
            if (!readSemiElementaryPoint(fp)) {
                lasterror_ = WString("sing #") + std::to_string(i) +
                             " = semi-el : " + lasterror_;
                return false;
            }
            break;
        case NODE:
            if (!readNodePoint(fp)) {
                lasterror_ = WString("sing #") + std::to_string(i) +
                             " = node : " + lasterror_;
                return false;
            }
            break;
        case STRONG_FOCUS:
            if (!readStrongFocusPoint(fp)) {
                lasterror_ = WString("sing #") + std::to_string(i) +
                             " = strongfocus : " + lasterror_;
                return false;
            }
            break;
        case WEAK_FOCUS:
            if (!readWeakFocusPoint(fp)) {
                lasterror_ = WString("sing #") + std::to_string(i) +
                             " = weakfocus : " + lasterror_;
                return false;
            }
            break;
        case NON_ELEMENTARY:
            if (!readDegeneratePoint(fp)) {
                lasterror_ = WString("sing #") + std::to_string(i) +
                             " = degen : " + lasterror_;
                return false;
            }
            break;
        default:
            lasterror_ = WString("sing #") + std::to_string(i) +
                         " type not exist (" + std::to_string(typ) + ")";
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadTerm1
// -----------------------------------------------------------------------

bool WVFStudy::readTerm1(FILE *fp, P4POLYNOM1 p, int N)
{
    int i;
    P4POLYNOM1 _p;

    _p = p;

    p->next_term1 = nullptr;
    if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2)
        return false;

    for (i = 2; i <= N; i++) {
        p->next_term1 = new term1;
        p = p->next_term1;
        p->next_term1 = nullptr;
        if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2) {
            delete_term1(_p->next_term1);
            _p->next_term1 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadTerm2
// -----------------------------------------------------------------------

bool WVFStudy::readTerm2(FILE *fp, P4POLYNOM2 p, int N)
{
    int i;
    P4POLYNOM2 _p;

    _p = p;
    p->next_term2 = nullptr;
    if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) != 3)
        return false;

    for (i = 2; i <= N; i++) {
        p->next_term2 = new term2;
        p = p->next_term2;
        p->next_term2 = nullptr;
        if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) !=
            3) {
            delete_term2(_p->next_term2);
            _p->next_term2 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadTerm3
// -----------------------------------------------------------------------

bool WVFStudy::readTerm3(FILE *fp, P4POLYNOM3 p, int N)
{
    int i;
    P4POLYNOM3 _p;
    _p = p;
    p->next_term3 = nullptr;
    if (fscanf(fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si),
               &(p->coeff)) != 4)
        return false;

    for (i = 2; i <= N; i++) {
        p->next_term3 = new term3;
        p = p->next_term3;
        p->next_term3 = nullptr;
        if (fscanf(fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si),
                   &(p->coeff)) != 4) {
            delete_term3(_p->next_term3);
            _p->next_term3 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadSaddlePoint
// -----------------------------------------------------------------------

bool WVFStudy::readSaddlePoint(FILE *fp)
{
    int N;
    sep *sep1;
    sep *sep2;

    // make room in structure

    saddle *last;
    saddle *point;

    last = nullptr;
    point = first_saddle_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_saddle;
    }

    point = new saddle;
    if (last == nullptr)
        first_saddle_point_ = point;
    else
        last->next_saddle = point;

    point->next_saddle = nullptr;

    // fill structure

    if (fscanf(fp, "%lf %lf", &(point->x0), &(point->y0)) != 2) {
        return false;
    }
    if (fscanf(fp, "%lf %lf %lf %lf", &(point->a11), &(point->a12),
               &(point->a21), &(point->a22)) != 4) {
        return false;
    }

    readVectorField(fp, point->vector_field);
    if (fscanf(fp, "%d ", &(point->chart)) != 1) {
        return false;
    }
    point->separatrices = new sep;
    sep1 = point->separatrices;
    if (fscanf(fp, "%d ", &(sep1->type)) != 1) {
        return false;
    }
    if (fscanf(fp, "%d ", &N) != 1) {
        return false;
    }
    sep1->notadummy = true;
    sep1->separatrice = new term1;
    readTerm1(fp, sep1->separatrice, N);
    sep1->direction = 1;
    sep1->d = 0;
    sep1->first_sep_point = nullptr;
    sep1->last_sep_point = nullptr;
    sep1->next_sep = nullptr;

    if (point->chart == CHART_R2 || singinf_) {
        // point is finite hence we have 4 separatrices or we have a line of
        // singularities
        // at infinity and hence we have also 4 separatrices after removing the
        // line

        sep2 = new sep;
        sep1->next_sep = sep2;

        sep2->type = sep1->type;
        sep2->direction = -1;
        sep2->d = 0;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
        sep2->first_sep_point = nullptr;
        sep2->last_sep_point = nullptr;

        sep1 = sep2->next_sep = new sep;

        if (fscanf(fp, "%d", &(sep1->type)) != 1) {
            return false;
        }
        if (fscanf(fp, "%d ", &N) != 1) {
            return false;
        }

        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->direction = 1;
        sep1->d = 1;
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;

        sep2 = new sep;
        sep1->next_sep = sep2;

        sep2->type = sep1->type;
        sep2->direction = -1;
        sep2->d = 1;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
        sep2->first_sep_point = nullptr;
        sep2->last_sep_point = nullptr;
        sep2->next_sep = nullptr;
    }

    if (fscanf(fp, "%lf ", &(point->epsilon)) != 1) {
        return false;
    }
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new saddle;
        last->next_saddle = point;
        point->next_saddle = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = ((last->chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point->notadummy = false;
        point->separatrices = last->separatrices;
        point->epsilon = last->epsilon;
        point->a11 = last->a11;
        point->a12 = last->a12;
        point->a21 = last->a21;
        point->a22 = last->a22;
        point->vector_field[0] = nullptr;
        point->vector_field[1] = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadSemiElementaryPoint
// -----------------------------------------------------------------------

bool WVFStudy::readSemiElementaryPoint(FILE *fp)
{
    // make room in structure

    semi_elementary *last;
    semi_elementary *point;
    int N;
    int s;
    sep *sep1;
    double y[2];
    bool ok;

    last = nullptr;
    point = first_se_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_se;
    }

    point = new semi_elementary;
    if (last == nullptr)
        first_se_point_ = point;
    else
        last->next_se = point;

    point->next_se = nullptr;

    if (fscanf(fp, "%lf %lf ", &(point->x0), &(point->y0)) != 2)
        return false;
    if (fscanf(fp, "%lf %lf %lf %lf ", &(point->a11), &(point->a12),
               &(point->a21), &(point->a22)) != 4)
        return false;
    readVectorField(fp, point->vector_field);
    if (fscanf(fp, "%d %d %d", &(point->type), &s, &(point->chart)) != 3)
        return false;

    switch (point->type) {
    case 1: // saddle-node
        if (s && point->chart != CHART_R2 && !singinf_) {
            // hyperbolic sep = line at infinity, not reported
            // center sep in the wrong direction
            // so no separatrices
            point->separatrices = nullptr;
        } else {
            // in the finite region (always s=1), the first sep is the center
            // sep, then the hyperbolic sep
            // at infinity, we  have s=0, so the center sep is the line at
            // infinity, not reported so only sep is
            // then the hyperbolic sep.

            point->separatrices = new sep;
            sep1 = point->separatrices;

            if (s)
                sep1->type = OT_CENT_STABLE;
            else
                sep1->type = OT_UNSTABLE;

            sep1->d = 0;
            if (((p_ == 1) && (q_ == 1) &&
                 ((point->chart == CHART_V1) || (point->chart == CHART_V2))) ||
                (point->chart == CHART_R2 || singinf_)) {
                sep1->direction = -1;
            } else {
                sep1->direction = 1;
            }

            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = nullptr;
            if (point->chart == CHART_R2 || singinf_) {
                // read second (hyperbolic) separatrix

                sep1->next_sep = new sep;
                sep1 = sep1->next_sep;
                sep1->type = OT_UNSTABLE;
                sep1->d = 1;
                sep1->direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1->notadummy = true;
                sep1->separatrice = new term1;
                readTerm1(fp, sep1->separatrice, N);
                sep1->first_sep_point = nullptr;
                sep1->last_sep_point = nullptr;

                // it is two-sided, so make a copy in other direction

                sep1->next_sep = new sep;
                sep1->next_sep->type = OT_UNSTABLE;
                sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1;
                sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
                sep1->next_sep->first_sep_point = nullptr;
                sep1->next_sep->last_sep_point = nullptr;
                sep1->next_sep->next_sep = nullptr;
            }
        }
        break;

    case 2: // saddle-node
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENUNSTABLE;
        else
            sep1->type = STYPE_STABLE;

        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;

        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1 = sep1->next_sep;
            sep1->type = STYPE_STABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_STABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 3: // saddle-node
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENSTABLE;
        else
            sep1->type = STYPE_UNSTABLE;
        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;

        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1 = sep1->next_sep;
            sep1->type = STYPE_UNSTABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_UNSTABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 4: // saddle-node
        if (s && (point->chart != CHART_R2) && !singinf_) {
            point->separatrices = nullptr;
        } else {
            point->separatrices = new sep;
            sep1 = point->separatrices;
            if (s)
                sep1->type = STYPE_CENUNSTABLE;
            else
                sep1->type = STYPE_STABLE;

            sep1->d = 0;
            if (((p_ == 1) && (q_ == 1) &&
                 ((point->chart == CHART_V1) || (point->chart == CHART_V2))) ||
                (point->chart == CHART_R2 || singinf_))
                sep1->direction = -1;
            else
                sep1->direction = 1;

            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = nullptr;
            if (point->chart == CHART_R2 || singinf_) {
                sep1->next_sep = new sep;
                sep1 = sep1->next_sep;
                sep1->type = STYPE_STABLE;
                sep1->d = 1;
                sep1->direction = 1;
                if (fscanf(fp, "%d ", &N) != 1)
                    return false;
                sep1->notadummy = true;
                sep1->separatrice = new term1;
                readTerm1(fp, sep1->separatrice, N);
                sep1->first_sep_point = nullptr;
                sep1->last_sep_point = nullptr;
                sep1->next_sep = new sep;
                sep1->next_sep->type = STYPE_STABLE;
                sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1;
                sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
                sep1->next_sep->first_sep_point = nullptr;
                sep1->next_sep->last_sep_point = nullptr;
                sep1->next_sep->next_sep = nullptr;
            }
        }
        break;

    case 6: // saddle
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENUNSTABLE;
        else
            sep1->type = STYPE_STABLE;
        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_CENUNSTABLE;
            sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = new sep;
            sep1 = sep1->next_sep->next_sep;
            sep1->type = STYPE_STABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_STABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    case 7: // saddle
        point->separatrices = new sep;
        sep1 = point->separatrices;
        if (s)
            sep1->type = STYPE_CENSTABLE;
        else
            sep1->type = STYPE_UNSTABLE;
        sep1->d = 0;
        if ((p_ == 1) && (q_ == 1) &&
            ((point->chart == CHART_V1) || (point->chart == CHART_V2)))
            sep1->direction = -1;
        else
            sep1->direction = 1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->first_sep_point = nullptr;
        sep1->last_sep_point = nullptr;
        sep1->next_sep = nullptr;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_CENSTABLE;
            sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = new sep;
            sep1 = sep1->next_sep->next_sep;
            sep1->type = STYPE_UNSTABLE;
            sep1->d = 1;
            sep1->direction = 1;
            if (fscanf(fp, "%d ", &N) != 1)
                return false;
            sep1->notadummy = true;
            sep1->separatrice = new term1;
            readTerm1(fp, sep1->separatrice, N);
            sep1->first_sep_point = nullptr;
            sep1->last_sep_point = nullptr;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_UNSTABLE;
            sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1;
            sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->first_sep_point = nullptr;
            sep1->next_sep->last_sep_point = nullptr;
            sep1->next_sep->next_sep = nullptr;
        }
        break;

    default:
        point->separatrices = nullptr;

        // change type of node if we have a gcf ?
        y[0] = point->x0;
        y[1] = point->y0;
        ok = false;
        switch (point->chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                ok = true;
            break;
        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                ok = true;
            break;
        case CHART_V1:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                ok = true;
            break;
        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                ok = true;
            break;
        case CHART_V2:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                ok = true;
            break;
        }
        if (ok) {
            if (point->type == 5)
                point->type = 8;
            else
                point->type = 5;
        }
    }

    if (fscanf(fp, "%lf ", &(point->epsilon)) != 1)
        return false;
    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && (point->chart != CHART_R2)) {
        point->next_se = new semi_elementary;
        point->next_se->x0 = point->x0;
        point->next_se->y0 = 0.0;
        if (dir_vec_field_ == 1)
            point->next_se->type = point->type;
        else
            switch (point->type) {
            case 1:
                point->next_se->type = 3;
                break;
            case 2:
                point->next_se->type = 4;
                break;
            case 3:
                point->next_se->type = 1;
                break;
            case 4:
                point->next_se->type = 2;
                break;
            case 6:
                point->next_se->type = 7;
                break;
            case 7:
                point->next_se->type = 6;
                break;
            default:
                point->next_se->type = point->type;
            }

        point->next_se->chart =
            (point->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->next_se->notadummy = false;
        point->next_se->separatrices = point->separatrices;
        point->next_se->epsilon = point->epsilon;
        point->next_se->a11 = point->a11;
        point->next_se->a12 = point->a12;
        point->next_se->a21 = point->a21;
        point->next_se->a22 = point->a22;
        point->next_se->vector_field[0] = nullptr;
        point->next_se->vector_field[1] = nullptr;
        point->next_se->next_se = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadStrongFocusPoint
// -----------------------------------------------------------------------

bool WVFStudy::readStrongFocusPoint(FILE *fp)
{
    double y[2];

    // make room in structure

    strong_focus *last;
    strong_focus *point;

    last = nullptr;
    point = first_sf_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_sf;
    }

    point = new strong_focus;
    if (last == nullptr)
        first_sf_point_ = point;
    else
        last->next_sf = point;

    point->next_sf = nullptr;

    // fill structure

    if (fscanf(fp, "%d %lf %lf ", &(point->stable), &(point->x0),
               &(point->y0)) != 3)
        return false;
    if (fscanf(fp, "%d ", &(point->chart)) != 1)
        return false;

    // change type of node if we have a gcf ?

    y[0] = point->x0;
    y[1] = point->y0;

    switch (point->chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V1:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];

        if (eval_term2(gcf_V1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V2:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];

        if (eval_term2(gcf_V2_, y) < 0)
            point->stable *= -1;
        break;
    }

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new strong_focus;
        last->next_sf = point;
        point->next_sf = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = ((point->chart == CHART_U1) ? CHART_V1 : CHART_V2);
        point->stable = last->stable * ((dir_vec_field_ == -1) ? -1 : 1);
    }

    return true;
}

bool WVFStudy::readWeakFocusPoint(FILE *fp)
{
    // make room in structure

    weak_focus *last;
    weak_focus *point;
    double y[2];

    last = nullptr;
    point = first_wf_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_wf;
    }

    point = new weak_focus;
    if (last == nullptr)
        first_wf_point_ = point;
    else
        last->next_wf = point;

    point->next_wf = nullptr;

    if (fscanf(fp, "%lf %lf ", &(point->x0), &(point->y0)) != 2)
        return false;
    if (fscanf(fp, "%d %d ", &(point->type), &(point->chart)) != 2)
        return false;

    ///* change type of node if we have a gcf ?

    if ((point->type == STYPE_STABLE) || (point->type == STYPE_UNSTABLE)) {
        y[0] = point->x0;
        y[1] = point->y0;

        switch (point->chart) {
        case CHART_R2:
            if (eval_term2(gcf_, y) < 0)
                point->type *= -1;
            break;

        case CHART_U1:
            if (eval_term2(gcf_U1_, y) < 0)
                point->type *= -1;
            break;

        case CHART_V1:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V1_, y) < 0)
                point->type *= -1;
            break;

        case CHART_U2:
            if (eval_term2(gcf_U2_, y) < 0)
                point->type *= -1;
            break;

        case CHART_V2:
            if ((p_ == 1) && (q_ == 1))
                y[0] = -y[0];
            if (eval_term2(gcf_V2_, y) < 0)
                point->type *= -1;
            break;
        }
    }

    if (singinf_ && (point->chart != CHART_R2)) {
        point->next_wf = new weak_focus;
        point->next_wf->x0 = point->x0;
        point->next_wf->y0 = 0.0;

        if (dir_vec_field_ == 1)
            point->next_wf->type = point->type;
        else
            switch (point->type) {
            case STYPE_STABLE:
                point->next_wf->type = STYPE_UNSTABLE;
                break;
            case STYPE_UNSTABLE:
                point->next_wf->type = STYPE_STABLE;
                break;
            default:
                point->next_wf->type = point->type;
                break;
            }

        point->next_wf->chart =
            (point->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point = point->next_wf;
        point->next_wf = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                  WVFStudy::ReadDegeneratePoint
// -----------------------------------------------------------------------

bool WVFStudy::readDegeneratePoint(FILE *fp)
{
    int n;

    // make room in structure

    degenerate *last;
    degenerate *point;

    last = nullptr;
    point = first_de_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_de;
    }

    point = new degenerate;
    if (last == nullptr)
        first_de_point_ = point;
    else
        last->next_de = point;

    point->next_de = nullptr;

    // load structure

    if (fscanf(fp, "%lf %lf %lf %d ", &(point->x0), &(point->y0),
               &(point->epsilon), &n) != 4)
        return false;
    point->blow_up = nullptr;
    if (n) {
        point->blow_up = new blow_up_points;
        readBlowupPoints(fp, point->blow_up, n);
        point->blow_up->blow_up_vec_field = true;
    }

    if (fscanf(fp, "%d ", &(point->chart)) != 1)
        return false;

    point->notadummy = true;

    // line at infinity a line of singularities in poincare disc

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new degenerate;
        last->next_de = point;
        point->next_de = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (last->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->epsilon = last->epsilon;
        point->notadummy = false;
        point->blow_up = last->blow_up;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadNodePoint
// -----------------------------------------------------------------------

bool WVFStudy::readNodePoint(FILE *fp)
{
    double y[2];

    // make room in structure

    node *last;
    node *point;

    last = nullptr;
    point = first_node_point_;
    while (point != nullptr) {
        last = point;
        point = point->next_node;
    }

    point = new node;
    if (last == nullptr)
        first_node_point_ = point;
    else
        last->next_node = point;

    point->next_node = nullptr;

    // load point structure

    if (fscanf(fp, "%lf %lf %d ", &(point->x0), &(point->y0),
               &(point->stable)) != 3)
        return false;
    if (fscanf(fp, "%d ", &(point->chart)) != 1)
        return false;

    // change type of node if we have a gcf ?

    y[0] = point->x0;
    y[1] = point->y0;

    switch (point->chart) {
    case CHART_R2:
        if (eval_term2(gcf_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U1:
        if (eval_term2(gcf_U1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V1:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];

        if (eval_term2(gcf_V1_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_U2:
        if (eval_term2(gcf_U2_, y) < 0)
            point->stable *= -1;
        break;

    case CHART_V2:
        if (p_ == 1 && q_ == 1)
            y[0] = -y[0];
        if (eval_term2(gcf_V2_, y) < 0)
            point->stable *= -1;
        break;
    }

    // if line at infinity a line of singularities in poincare disc:
    // duplicate singularity by using a symmetry

    if (singinf_ != 0 && point->chart != CHART_R2) {
        last = point;
        point = new node;
        last->next_node = point;
        point->next_node = nullptr;

        point->x0 = last->x0;
        point->y0 = 0.0;
        point->chart = (last->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->stable = last->stable * (dir_vec_field_ == -1) ? -1 : 1;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadTransformations
// -----------------------------------------------------------------------

bool WVFStudy::readTransformations(FILE *fp, transformations *trans, int n)
{

    int i;

    if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(trans->x0), &(trans->y0),
               &(trans->c1), &(trans->c2), &(trans->d1), &(trans->d2),
               &(trans->d3), &(trans->d4), &(trans->d)) != 9)
        return false;

    trans->next_trans = nullptr;

    for (i = 2; i <= n; i++) {
        trans->next_trans = new transformations;
        trans = trans->next_trans;

        if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d", &(trans->x0),
                   &(trans->y0), &(trans->c1), &(trans->c2), &(trans->d1),
                   &(trans->d2), &(trans->d3), &(trans->d4), &(trans->d)) != 9)
            return false;

        trans->next_trans = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------
//                      WVFStudy::ReadBlowupPoints
// -----------------------------------------------------------------------

bool WVFStudy::readBlowupPoints(FILE *fp, blow_up_points *b, int n)
{
    int i, N, typ;

    for (i = 1; i <= n; i++) {
        if (fscanf(fp, "%d ", &(b->n)) != 1)
            return false;

        b->trans = new transformations;

        readTransformations(fp, b->trans, b->n);
        if (fscanf(fp, "%lf %lf ", &(b->x0), &(b->y0)) != 2)
            return false;
        if (fscanf(fp, "%lf %lf %lf %lf ", &(b->a11), &(b->a12), &(b->a21),
                   &(b->a22)) != 4)
            return false;
        readVectorField(fp, b->vector_field);
        b->sep = new term1;
        if (fscanf(fp, "%d ", &N) != 1)
            return false;
        readTerm1(fp, b->sep, N);

        if (fscanf(fp, "%d ", &typ) != 1)
            return false;
        switch (typ) {
        case 1:
            b->type = STYPE_UNSTABLE;
            break;
        case 2:
            b->type = STYPE_STABLE;
            break;
        case 3:
            b->type = STYPE_UNSTABLE;
            break;
        case 4:
            b->type = STYPE_STABLE;
            break;
        case 5:
            b->type = STYPE_CENUNSTABLE;
            break;
        case 6:
            b->type = STYPE_CENSTABLE;
            break;
        case 7:
            b->type = STYPE_CENUNSTABLE;
            break;
        case 8:
            b->type = STYPE_CENSTABLE;
            break;
        case 9:
            b->type = STYPE_UNSTABLE;
            break;
        case 10:
            b->type = STYPE_UNSTABLE;
            break;
        case 11:
            b->type = STYPE_STABLE;
            break;
        case 12:
            b->type = STYPE_STABLE;
            break;
        case 13:
            b->type = STYPE_UNSTABLE;
            break;
        case 14:
            b->type = STYPE_UNSTABLE;
            break;
        case 15:
            b->type = STYPE_STABLE;
            break;
        case 16:
            b->type = STYPE_STABLE;
            break;
        }
        b->first_sep_point = nullptr;
        b->last_sep_point = nullptr;
        b->next_blow_up_point = nullptr;

        if (i < n) {
            b->next_blow_up_point = new blow_up_points;
            b = b->next_blow_up_point;
        }
    }
    return true;
}

void WVFStudy::setupCoordinateTransformations(void)
{
    if (!plweights_) {
        U1_to_sphere = &WVFStudy::U1_to_psphere;
        U2_to_sphere = &WVFStudy::U2_to_psphere;
        V1_to_sphere = &WVFStudy::VV1_to_psphere;
        V2_to_sphere = &WVFStudy::VV2_to_psphere;
        sphere_to_R2 = &WVFStudy::psphere_to_R2;
        R2_to_sphere = &WVFStudy::R2_to_psphere;
        sphere_to_U1 = &WVFStudy::psphere_to_U1;
        sphere_to_U2 = &WVFStudy::psphere_to_U2;
        sphere_to_V1 = &WVFStudy::psphere_to_VV1;
        sphere_to_V2 = &WVFStudy::psphere_to_VV2;

        integrate_sphere_sep = &WVFStudy::integrate_poincare_sep;
        integrate_sphere_orbit = &WVFStudy::integrate_poincare_orbit;
        eval_lc = &WVFStudy::eval_lc_poincare;
        less2 = &WVFStudy::less_poincare;
        change_dir = &WVFStudy::change_dir_poincare;

        switch (typeofview_) {
        case TYPEOFVIEW_SPHERE:
            viewcoord_to_sphere = &WVFStudy::ucircle_psphere;
            sphere_to_viewcoord = &WVFStudy::psphere_ucircle;
            finite_to_viewcoord = &WVFStudy::finite_ucircle;
            is_valid_viewcoord = &WVFStudy::isvalid_psphereviewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::default_sphere_to_viewcoordpair;
            break;

        case TYPEOFVIEW_PLANE:
            viewcoord_to_sphere = &WVFStudy::R2_to_psphere;
            sphere_to_viewcoord = &WVFStudy::psphere_to_R2;
            finite_to_viewcoord = &WVFStudy::identitytrf_R2;
            is_valid_viewcoord = &WVFStudy::isvalid_R2viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::default_sphere_to_viewcoordpair;
            break;

        case TYPEOFVIEW_U1:
            viewcoord_to_sphere = &WVFStudy::xyrevU1_to_psphere;
            sphere_to_viewcoord = &WVFStudy::psphere_to_xyrevU1;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_U1viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::psphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_U2:
            viewcoord_to_sphere = &WVFStudy::U2_to_psphere;
            sphere_to_viewcoord = &WVFStudy::psphere_to_U2;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_U2viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::psphere_to_viewcoordpair_discontinuousy;
            break;

        case TYPEOFVIEW_V1:
            viewcoord_to_sphere = &WVFStudy::xyrevV1_to_psphere;
            sphere_to_viewcoord = &WVFStudy::psphere_to_xyrevV1;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_V1viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::psphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_V2:
            viewcoord_to_sphere = &WVFStudy::V2_to_psphere;
            sphere_to_viewcoord = &WVFStudy::psphere_to_V2;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_V2viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::psphere_to_viewcoordpair_discontinuousy;
            break;
        }
    } else {
        U1_to_sphere = &WVFStudy::U1_to_plsphere;
        U2_to_sphere = &WVFStudy::U2_to_plsphere;
        V1_to_sphere = &WVFStudy::V1_to_plsphere;
        V2_to_sphere = &WVFStudy::V2_to_plsphere;
        sphere_to_R2 = &WVFStudy::plsphere_to_R2;
        R2_to_sphere = &WVFStudy::R2_to_plsphere;
        sphere_to_U1 = &WVFStudy::plsphere_to_U1;
        sphere_to_U2 = &WVFStudy::plsphere_to_U2;
        sphere_to_V1 = &WVFStudy::plsphere_to_V1;
        sphere_to_V2 = &WVFStudy::plsphere_to_V2;

        integrate_sphere_sep = &WVFStudy::integrate_lyapunov_sep;
        integrate_sphere_orbit = &WVFStudy::integrate_lyapunov_orbit;
        eval_lc = &WVFStudy::eval_lc_lyapunov;
        less2 = &WVFStudy::less_lyapunov;
        change_dir = &WVFStudy::change_dir_lyapunov;

        switch (typeofview_) {
        case TYPEOFVIEW_SPHERE:
            viewcoord_to_sphere = &WVFStudy::annulus_plsphere;
            sphere_to_viewcoord = &WVFStudy::plsphere_annulus;
            finite_to_viewcoord = &WVFStudy::finite_annulus;
            is_valid_viewcoord = &WVFStudy::isvalid_plsphereviewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::default_sphere_to_viewcoordpair;
            break;
        case TYPEOFVIEW_PLANE:
            viewcoord_to_sphere = &WVFStudy::R2_to_plsphere;
            sphere_to_viewcoord = &WVFStudy::plsphere_to_R2;
            finite_to_viewcoord = &WVFStudy::identitytrf_R2;
            is_valid_viewcoord = &WVFStudy::isvalid_R2viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::default_sphere_to_viewcoordpair;
            break;
        case TYPEOFVIEW_U1:
            viewcoord_to_sphere = &WVFStudy::xyrevU1_to_plsphere;
            sphere_to_viewcoord = &WVFStudy::plsphere_to_xyrevU1;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_U1viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::plsphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_U2:
            viewcoord_to_sphere = &WVFStudy::U2_to_plsphere;
            sphere_to_viewcoord = &WVFStudy::plsphere_to_U2;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_U2viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::plsphere_to_viewcoordpair_discontinuousy;
            break;

        case TYPEOFVIEW_V1:
            viewcoord_to_sphere = &WVFStudy::xyrevV1_to_plsphere;
            sphere_to_viewcoord = &WVFStudy::plsphere_to_xyrevV1;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_V1viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::plsphere_to_viewcoordpair_discontinuousx;
            break;

        case TYPEOFVIEW_V2:
            viewcoord_to_sphere = &WVFStudy::V2_to_plsphere;
            sphere_to_viewcoord = &WVFStudy::plsphere_to_V2;
            finite_to_viewcoord = &WVFStudy::default_finite_to_viewcoord;
            is_valid_viewcoord = &WVFStudy::isvalid_V2viewcoord;
            sphere_to_viewcoordpair =
                &WVFStudy::plsphere_to_viewcoordpair_discontinuousy;
            break;
        }
    }
}

#include "file_tab.h"

#include "custom.h"
#include "math_p4.h"
#include "math_polynom.h"

#include <cmath>

// -----------------------------------------------------------------------
//                      FUNCTION PROTOTYPES
// -----------------------------------------------------------------------
/*

    There are, logically, 4 categories of FINITE coordinate systems:

    (I)     Real coordinates
    (II)    View coordinates
    (III)   Poincare coordinates
    (IV)    Poincare-Lyapunov coordinates

    Category (I) are coordinates used by Maple/reduce, and also the points
    given in dialog boxes by the user, are always (I)-coordinates.
    Category (II) are coordinates that are obtained by interacting with
    the plot-window or zoom-window.  When the user clicks, the click-coordinates
    are in Category (II). When a local study is wanted, these coordinates are
    exactly the same as (I).  Otherwhise, Category-(II) coordinates are
   coordinates
    inside a rectangular box x=-1.1 .. 1.1, y=-1.1 .. 1.1, wherein the circle
    at infinity is a circle with radius 1.

*/

// -----------------------------------------------------------------------
//                  IMPLEMENTATION OF THE POINCARE CHARTS
// -----------------------------------------------------------------------

void WVFStudy::R2_to_psphere(double x, double y, double *pcoord)
{
    double s;

    s = sqrt(1.0 + x * x + y * y);

    pcoord[0] = x / s;
    pcoord[1] = y / s;
    pcoord[2] = 1.0 / s;
}

void WVFStudy::psphere_to_R2(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = X / Z;
    rcoord[1] = Y / Z;
}

void WVFStudy::psphere_ucircle(double X, double Y, double Z, double *ucoord)
{
    double k;

    if (config_projection_ == 0) {
        ucoord[0] = X;
        ucoord[1] = Y;
    } else {
        k = -Z / (Z - config_projection_);
        ucoord[0] = (1.0 + k) * X;
        ucoord[1] = (1.0 + k) * Y;
    }
}

void WVFStudy::ucircle_psphere(double u, double v, double *pcoord)
{
    double k, projection;

    projection = config_projection_;

    if (projection == 0) {
        pcoord[0] = u;
        pcoord[1] = v;
        pcoord[2] = sqrt(1.0 - u * u - v * v);
    } else {
        k = (sqrt(-projection * projection * (u * u + v * v) +
                  projection * projection + u * u + v * v) -
             u * u - v * v) /
            (projection * projection + u * u + v * v);

        pcoord[0] = (1.0 + k) * u;
        pcoord[1] = (1.0 + k) * v;
        pcoord[2] = -k * projection;
    }
}

void WVFStudy::finite_ucircle(double x, double y, double *ucoord)
{
    double pcoord[3];

    R2_to_psphere(x, y, pcoord);
    psphere_ucircle(pcoord[0], pcoord[1], pcoord[2], ucoord);
}

void WVFStudy::psphere_to_U1(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = Y / X;
    rcoord[1] = Z / X;
}

void WVFStudy::psphere_to_U2(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = X / Y;
    rcoord[1] = Z / Y;
}

void WVFStudy::psphere_to_V1(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = -Y / X;
    rcoord[1] = -Z / X;
}

void WVFStudy::psphere_to_V2(double X, double Y, double Z, double *rcoord)
{
    rcoord[0] = -X / Y;
    rcoord[1] = -Z / Y;
}

void WVFStudy::U1_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[0] = 1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[1] = z1 * pcoord[0];
    pcoord[2] = z2 * pcoord[0];
}

void WVFStudy::V1_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[0] = -1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[1] = -z1 * pcoord[0];
    pcoord[2] = -z2 * pcoord[0];
}

void WVFStudy::U2_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[1] = 1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[0] = z1 * pcoord[1];
    pcoord[2] = z2 * pcoord[1];
}

void WVFStudy::V2_to_psphere(double z1, double z2, double *pcoord)
{
    pcoord[1] = -1 / (sqrt(1 + z1 * z1 + z2 * z2));
    pcoord[0] = -z1 * pcoord[1];
    pcoord[2] = -z2 * pcoord[1];
}

void WVFStudy::VV1_to_psphere(double z1, double z2, double *pcoord)
{
    /* (z1,z2) is a point on the U1 chart */

    U1_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void WVFStudy::psphere_to_VV1(double X, double Y, double Z, double *rcoord)
{
    psphere_to_U1(-X, -Y, -Z, rcoord);
}

void WVFStudy::VV2_to_psphere(double z1, double z2, double *pcoord)
{

    /* (z1,z2) is a point on the U2 chart */

    U2_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void WVFStudy::psphere_to_VV2(double X, double Y, double Z, double *rcoord)
{
    psphere_to_U2(-X, -Y, -Z, rcoord);
}

void WVFStudy::UU1_to_psphere(double z1, double z2, double *pcoord)
{
    /* (z1,z2) is a point on the V1 chart */
    V1_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void WVFStudy::psphere_to_UU1(double X, double Y, double Z, double *rcoord)
{
    psphere_to_V1(-X, -Y, -Z, rcoord);
}

void WVFStudy::UU2_to_psphere(double z1, double z2, double *pcoord)
{
    /* (z1,z2) is a point on the V2 chart */
    V2_to_psphere(z1, z2, pcoord);
    pcoord[0] = -pcoord[0];
    pcoord[1] = -pcoord[1];
    pcoord[2] = -pcoord[2];
}

void WVFStudy::psphere_to_UU2(double X, double Y, double Z, double *rcoord)
{
    psphere_to_V2(-X, -Y, -Z, rcoord);
}

bool WVFStudy::isvalid_psphereviewcoord(double u, double v, double *pcoord)
{
    if (u * u + v * v > 1.0)
        return false;

    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool WVFStudy::isvalid_U1viewcoord(double u, double v, double *pcoord)
{
    // the u coordinate must be positive, except when p is odd.

    if (plweights_) {
        if ((p_ % 2) == 0 && u < 0)
            return false;
    }

    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool WVFStudy::isvalid_U2viewcoord(double u, double v, double *pcoord)
{
    if (plweights_) {
        if ((q_ % 2) == 0 && v < 0)
            return false;
    }

    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool WVFStudy::isvalid_V1viewcoord(double u, double v, double *pcoord)
{
    if (plweights_) {
        if ((p_ % 2) == 0 && u < 0)
            return false;
    }

    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool WVFStudy::isvalid_V2viewcoord(double u, double v, double *pcoord)
{
    if (plweights_) {
        if ((q_ % 2) == 0 && v < 0)
            return false;
    }

    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

bool WVFStudy::isvalid_R2viewcoord(double u, double v, double *pcoord)
{
    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

void WVFStudy::psphere_to_xyrevU1(double X, double Y, double Z, double *rcoord)
{
    double _rcoord[2];
    psphere_to_U1(X, Y, Z, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void WVFStudy::psphere_to_xyrevV1(double X, double Y, double Z, double *rcoord)
{
    double _rcoord[2];
    psphere_to_V1(X, Y, Z, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void WVFStudy::xyrevU1_to_psphere(double z1, double z2, double *pcoord)
{
    U1_to_psphere(z2, z1, pcoord);
}

void WVFStudy::xyrevV1_to_psphere(double z1, double z2, double *pcoord)
{
    V1_to_psphere(z2, z1, pcoord);
}

// -----------------------------------------------------------------------
//              IMPLEMENTATION OF THE POINCARE-LYPANOV CHARTS
// -----------------------------------------------------------------------

/* function for transformation U1,V1 chart to the cylinder
   on chart x=(+-)1/s^p, y=u/s^q
   on cylinder x=cos(theta)/r^p, y=sin(theta)/r^q
   (u,s) -> (r,theta)
   find positive root of  y^p+u^2*y^q-1
   r=sqrt(y)*s and theta=atan(u*sqrt(y)^(q-p)) if s<>0 !!
   if s=0 and u<>0 then solve (+-1)^q*u^p*cos(theta)^q-sin(theta)^p
                        and r=0
   if u=0 then r=s and theta=0 or Pi
*/

bool WVFStudy::isvalid_plsphereviewcoord(double u, double v, double *pcoord)
{
    if (u * u + v * v > 1.0)
        return false;
    (this->*viewcoord_to_sphere)(u, v, pcoord);
    return true;
}

// void cylinder_to_U1( double r, double theta, double * c);
// void cylinder_to_U2( double r, double theta, double * c );
// void cylinder_to_V1( double r, double theta, double * c);
// void cylinder_to_V2( double r, double theta, double * c);
// void U1_to_cylinder( double u, double s, double * c);
// void U2_to_cylinder( double u, double s, double * c);
void V1_to_cylinder(double u, double s, double *c);
void V2_to_cylinder(double u, double s, double *c);

static double U = 0.0;

double WVFStudy::func_U1(double x)
{
    return pow(x, double_p_) + U * U * pow(x, double_q_) - 1.0;
}

double WVFStudy::dfunc_U1(double x)
{
    return double_p_ * pow(x, double_p_minus_1_) +
           U * U * double_q_ * pow(x, double_q_minus_1_);
}

double WVFStudy::func_U1_s0(double theta)
{
    /* find theta if s=0 and u<>0 */
    return U * pow(cos(theta), double_q_) - pow(sin(theta), double_p_);
}

double WVFStudy::dfunc_U1_s0(double theta)
{
    return (-double_q_ * U * pow(cos(theta), double_q_minus_1_) * sin(theta) -
            double_p_ * cos(theta) * pow(sin(theta), double_p_minus_1_));
}

void WVFStudy::U1_to_cylinder(double u, double s, double *c)
{
    /* input (u,s) output c=(r,theta)
        x=1/s^p, y=u/s^q
    */
    double x[2], y;
    if (u == 0) {
        c[0] = s;
        c[1] = 0;
    } else if (s == 0) {
        c[0] = 0;
        U = pow(u, double_p_);
        if (u > 0) {
            x[0] = 0;
            x[1] = PI / 2.0;
        } else {
            x[0] = -PI / 2.0;
            x[1] = 0;
        }
        c[1] = find_root(&WVFStudy::func_U1_s0, &WVFStudy::dfunc_U1_s0, x);
    } else {
        x[0] = 0;
        x[1] = 1;
        U = u;
        y = find_root(&WVFStudy::func_U1, &WVFStudy::dfunc_U1, x);
        c[0] = sqrt(y) * s;
        c[1] = atan(u * pow(sqrt(y), double_q_minus_p_));
    }
}

/*
void WVFStudy::cylinder_to_U1( double r, double theta, double * c)
{
    c[1]=r/pow(cos(theta),__one_over_p);
    c[0]=sin(theta)*pow(c[1]/r,double_q_);
}
*/

void WVFStudy::V1_to_cylinder(double u, double s, double *c)
{
    /* input (u,s) output c=(r,theta)
        x=-1/s^p, y=u/s^q
    */
    double x[2], y;
    if (u == 0) {
        c[0] = s;
        c[1] = PI;
    } else if (s == 0) {
        c[0] = 0;
        U = pow(u, double_p_) * __minus_one_to_q;
        if (u > 0) {
            x[0] = PI / 2;
            x[1] = PI;
        } else {
            x[0] = -PI;
            x[1] = -PI / 2;
        }
        c[1] = find_root(&WVFStudy::func_U1_s0, &WVFStudy::dfunc_U1_s0, x);
    } else {
        x[0] = 0;
        x[1] = 1;
        U = u;
        y = find_root(&WVFStudy::func_U1, &WVFStudy::dfunc_U1, x);
        c[0] = sqrt(y) * s;
        c[1] = atan(-u * pow(sqrt(y), double_q_minus_p_));
        if (c[1] > 0)
            c[1] -= PI;
        else
            c[1] += PI;
    }
}

/*
void WVFStudy::cylinder_to_V1( double r, double theta, double * c)
{
    c[1]=r/pow(-cos(theta),__one_over_p);
    c[0]=sin(theta)*pow(c[1]/r,double_q_);
}
*/

/* function for transformation U2,V2 chart to the cylinder
   on chart x=u/s^p, y=(+-)1/s^q
   on cylinder x=cos(theta)/r^p, y=sin(theta)/r^q
   (u,s) -> (r,theta)
   find positive root of  u^2*y^p+y^q-1
   r=sqrt(y)*s and theta=atan(sqrt(y)^(q-p)/u) if s<>0
   if s=0 then solve u^q*sin(theta)^p-cos(theta)^q
*/

double WVFStudy::func_U2(double x)
{
    return (U * U * pow(x, double_p_) + pow(x, double_q_) - 1.0);
}

double WVFStudy::dfunc_U2(double x)
{
    return (double_p_ * U * U * pow(x, double_p_minus_1_) +
            double_q_ * pow(x, double_q_minus_1_));
}

double WVFStudy::func_U2_s0(double theta)
{
    return (U * pow(sin(theta), double_p_) - pow(cos(theta), double_q_));
}

double WVFStudy::dfunc_U2_s0(double theta)
{
    return (double_p_ * U * cos(theta) * pow(sin(theta), double_p_minus_1_) +
            double_q_ * sin(theta) * pow(cos(theta), double_q_minus_1_)); ////
}

void WVFStudy::U2_to_cylinder(double u, double s, double *c)
/* input (u,s) output c=(r,theta)
    x=u/s^p, y=1/s^q
*/
{
    double x[2], y;
    if (u == 0) {
        c[0] = s;
        c[1] = PI / 2;
    } else if (s == 0) {
        c[0] = 0;
        U = pow(u, double_q_);
        if (u > 0) {
            x[0] = 0;
            x[1] = PI / 2.0;
        } else {
            x[0] = PI / 2.0;
            x[1] = PI;
        }
        c[1] = find_root(&WVFStudy::func_U2_s0, &WVFStudy::dfunc_U2_s0, x);
    } else {
        x[0] = 0;
        x[1] = 1;
        U = u;
        y = find_root(&WVFStudy::func_U2, &WVFStudy::dfunc_U2, x);
        c[0] = sqrt(y) * s;
        c[1] = atan(pow(sqrt(y), double_q_minus_p_) / u);
        if (c[1] < 0)
            c[1] += PI;
    }
}

/*
void WVFStudy::cylinder_to_U2( double r, double theta, double * c )
{
    c[1] = r/pow(sin(theta),__one_over_q);
    c[0] = cos(theta)*pow(c[1]/r,double_p_);
}
*/

void WVFStudy::V2_to_cylinder(double u, double s, double *c)
/* input (u,s) output c=(r,theta)
    x=u/s^p, y=-1/s^q
*/
{
    double x[2], y;

    if (u == 0) {
        c[0] = s;
        c[1] = -PI / 2;
    } else {
        if (s == 0) {
            c[0] = 0;
            U = pow(u, double_q_) * __minus_one_to_p;
            if (u > 0) {
                x[0] = -PI / 2;
                x[1] = 0;
            } else {
                x[0] = -PI;
                x[1] = -PI / 2;
            }
            c[1] = find_root(&WVFStudy::func_U2_s0, &WVFStudy::dfunc_U2_s0, x);
        } else {
            x[0] = 0;
            x[1] = 1;
            U = u;
            y = find_root(&WVFStudy::func_U2, &WVFStudy::dfunc_U2, x);
            c[0] = sqrt(y) * s;
            c[1] = atan(-pow(sqrt(y), double_q_minus_p_) / u);
            if (c[1] > 0)
                c[1] -= PI;
        }
    }
}

/*
void WVFStudy::cylinder_to_V2( double r, double theta, double * c)
{
    c[1] = r / pow( -sin(theta), __one_over_q );
    c[0] = cos(theta) * pow( c[1]/r, double_p_ );
}
*/

// -----------------------------------------------------------------------
//                          ANNULUS_PLSPHERE
// -----------------------------------------------------------------------
//
// Change of coordinates from plot-coordinates to Poincare-Lyapunov coordinates
//
// In the plot window, coordinates are mapped into a rectangular box x: -1..1,
// y: -1..1.
// Inside this box, the circle at infinity is a circle with radius 1.0.
//
// The change of coordinates to PL-coordinates is given by this routine:

void WVFStudy::annulus_plsphere(double x, double y, double *p)
{
    if (x * x + y * y < RADIUS2) {
        p[0] = 0; // mark it as a finite point
        p[1] = x / RADIUS;
        p[2] = y / RADIUS;
    } else {
        p[0] = 1; // mark it as a point near infinity
        p[1] = (1.0 - x * x - y * y) / (1.0 - RADIUS2);
        p[2] = atan2(y, x);
    }
}

// -----------------------------------------------------------------------
//                          PLSPHERE_ANNULUS
// -----------------------------------------------------------------------
//
// Inverse of annulus_plsphere.
//
// projection cylinder on annulus
//  first we project the cylinder the cone x^2+y^2=1-(1-RADIUS2)*r

void WVFStudy::plsphere_annulus(double x, double y, double z, double *u)
{
    double zz;

    if (x == 0) {
        u[0] = y * RADIUS;
        u[1] = z * RADIUS;
    } else {
        zz = sqrt(1.0 - (1.0 - RADIUS2) * y);
        u[0] = zz * cos(z);
        u[1] = zz * sin(z);
    }
}

// -----------------------------------------------------------------------
//                          R2_TO_PLSPHERE
// -----------------------------------------------------------------------
//
// Change of real coordinates to Poincare-Lyapunov sphere.
//
// PL-sphere coordinates correspond to R^2-coordinates inside a ball
// with radius 1 around the origin.
//
// Therefore, the first plsphere coordinate is either 0.0 or 1.0, indicating
// which chart the point belongs to:
//
//
//      (x,y)  --> (0,x,y)              if (x,y) in ball with radius 1
//      (x,y)  --> (1,u,v)              outside ball
//
//
//  We calculate (u,v) through the inverse of the prescription (x,y) =
//  (cos(v)/u^p, sin(v)/u^q):
//
//      (cos(v),sin(v)) = ( x*u^p, y*u^q )
//
//  Hence, u is a root of "x^2 u^(2p) + y^2 u^(2q) - 1 = 0".
//  We calculate a root using numeric analysis.
//
//  Once we have calculated u, we determine v using atan2.

static double A = 0.0;
static double B = 0.0;

double WVFStudy::func(double z)
{
    return pow(z, double_p_) * A + pow(z, double_q_) * B - 1.0;
}

double WVFStudy::dfunc(double z)
{
    return double_p_ * pow(z, double_p_minus_1_) * A +
           double_q_ * pow(z, double_q_minus_1_) * B;
}

void WVFStudy::R2_to_plsphere(double x, double y, double *pcoord)
{
    double z[2];

    if ((x * x + y * y) <= 1.0) {
        pcoord[0] = 0.0;
        pcoord[1] = x;
        pcoord[2] = y;
    } else {
        pcoord[0] = 1.0;
        A = x * x;
        B = y * y;
        z[0] = 0.0;
        z[1] = 1.0;

        pcoord[1] = find_root(&WVFStudy::func, &WVFStudy::dfunc, z);
        pcoord[1] = sqrt(pcoord[1]);
        pcoord[2] =
            atan2(pow(pcoord[1], double_q_) * y, pow(pcoord[1], double_p_) * x);
    }
}

// -----------------------------------------------------------------------
//                          plsphere_to_R2
// -----------------------------------------------------------------------
//
// Change of coordinates from Poincare-Lyapunov sphere to real coordinates
//
// PL-sphere coordinates correspond to R^2-coordinates inside a ball
// with radius 1 around the origin.
//
// Therefore, the first plsphere coordinate is either 0.0 or 1.0, indicating
// which chart the point belongs to:
//
//
//      (0,u,v)  --> (x,y) = (u,v)
//      (1,u,v)  --> (x,y) = (cos(v)/u^p, sin(v)/u^q)
//

void WVFStudy::plsphere_to_R2(double ch, double u, double v, double *c)
{
    if (ch) {
        c[0] = cos(v) / pow(u, double_p_);
        c[1] = sin(v) / pow(u, double_q_);
    } else {
        c[0] = u;
        c[1] = v;
    }
}

// -----------------------------------------------------------------------
//                          FINITE_ANNULUS
// -----------------------------------------------------------------------
//
// Change "Real" coordinates to "Plot" coordinates.
//
// It is simply by combining Symbolic-->PL, PL-->Plot

void WVFStudy::finite_annulus(double x, double y, double *u)
{
    double pcoord[3];

    R2_to_plsphere(x, y, pcoord);
    plsphere_annulus(pcoord[0], pcoord[1], pcoord[2], u);
}

void WVFStudy::cylinder_to_plsphere(double r, double theta, double *pcoord)
{
    if (r < 1.0) {
        pcoord[0] = 1;
        pcoord[1] = r;
        pcoord[2] = theta;
    } else {
        pcoord[0] = 0;
        pcoord[1] = cos(theta) / pow(r, double_p_);
        pcoord[2] = sin(theta) / pow(r, double_q_);
    }
}

void WVFStudy::U1_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    U1_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void WVFStudy::xyrevU1_to_plsphere(double z1, double z2, double *pcoord)
{
    U1_to_plsphere(z2, z1, pcoord);
}

void WVFStudy::V1_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    V1_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void WVFStudy::xyrevV1_to_plsphere(double z1, double z2, double *pcoord)
{
    V1_to_plsphere(z2, z1, pcoord);
}

void WVFStudy::U2_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    U2_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void WVFStudy::V2_to_plsphere(double x0, double y0, double *pcoord)
{
    double c[2];
    V2_to_cylinder(x0, y0, c);
    cylinder_to_plsphere(c[0], c[1], pcoord);
}

void WVFStudy::identitytrf_R2(double x, double y, double *ucoord)
{
    ucoord[0] = x;
    ucoord[1] = y;
}

double WVFStudy::floatinfinity(void) { return pow(0, -1); }

void WVFStudy::plsphere_to_U1(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = cos(y);

        if (a < 0) {
            if ((p_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_p); // - (-cos(y))^(-1/p)
        } else
            a = pow(a, -__one_over_p); // cos(y)^(-1/p)

        rcoord[0] = sin(y) * pow(a, double_q_); // sin(y) * cos(y)^(-q/p)
        rcoord[1] = x * a;                      // x * cos(y)^(-1/p)
    } else {
        if (x < 0) {
            if ((p_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-x, -__one_over_p);
        } else
            a = pow(x, -__one_over_p); // x^(-1/p)

        rcoord[0] = y * pow(a, double_q_);
        rcoord[1] = a;
    }
}

void WVFStudy::plsphere_to_xyrevU1(double ch, double x, double y,
                                   double *rcoord)
{
    double _rcoord[2];
    plsphere_to_U1(ch, x, y, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}

void WVFStudy::plsphere_to_U2(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = sin(y);

        if (a < 0) {
            if ((q_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_q); // - (-sin(y))^(-1/q)
        } else
            a = pow(a, -__one_over_q); // sin(y)^(-1/q)

        rcoord[0] = cos(y) * pow(a, double_p_); // cos(y) * sin(y)^(-p/q)
        rcoord[1] = x * a;                      // x * sin(y)^(-1/q)
    } else {
        if (y < 0) {
            if ((q_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-y, -__one_over_q);
        } else
            a = pow(y, -__one_over_q); // x^(-1/p)

        rcoord[0] = x * pow(a, double_p_);
        rcoord[1] = a;
    }
}
void WVFStudy::plsphere_to_V1(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = -cos(y);

        if (a < 0) {
            if ((p_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_p); // - (-cos(y))^(-1/p)
        } else
            a = pow(a, -__one_over_p); // cos(y)^(-1/p)

        rcoord[0] = sin(y) * pow(a, double_q_); // sin(y) * cos(y)^(-q/p)
        rcoord[1] = x * a;                      // x * cos(y)^(-1/p)
    } else {
        if (x > 0) {
            if ((p_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(x, -__one_over_p);
        } else
            a = pow(-x, -__one_over_p); // x^(-1/p)

        rcoord[0] = y * pow(a, double_q_);
        rcoord[1] = a;
    }
}
void WVFStudy::plsphere_to_xyrevV1(double ch, double x, double y,
                                   double *rcoord)
{
    double _rcoord[2];
    plsphere_to_V1(ch, x, y, _rcoord);
    rcoord[0] = _rcoord[1];
    rcoord[1] = _rcoord[0];
}
void WVFStudy::plsphere_to_V2(double ch, double x, double y, double *rcoord)
{
    double a;

    if (ch == 1) {
        a = -sin(y);

        if (a < 0) {
            if ((q_ % 2) == 0) {
                // p is even: so we have a problem
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(-a, -__one_over_q); // - (-sin(y))^(-1/q)
        } else
            a = pow(a, -__one_over_q); // sin(y)^(-1/q)

        rcoord[0] = cos(y) * pow(a, double_p_); // cos(y) * sin(y)^(-p/q)
        rcoord[1] = x * a;                      // x * sin(y)^(-1/q)
    } else {
        if (y > 0) {
            if ((q_ % 2) == 0) {
                rcoord[0] = floatinfinity();
                rcoord[1] = floatinfinity();
                return;
            } else
                a = -pow(y, -__one_over_q);
        } else
            a = pow(-y, -__one_over_q); // x^(-1/p)

        rcoord[0] = x * pow(a, double_p_);
        rcoord[1] = a;
    }
}

// -----------------------------------------------------------------------
//                      EVALUATION OF VECTOR FIELDS
// -----------------------------------------------------------------------

void WVFStudy::eval_r_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (config_kindvf_ == INTCONFIG_ORIGINAL && gcf_ != nullptr)
        s = eval_term2(gcf_, y);

    f[0] = s * eval_term2(f_vec_field_[0], y);
    f[1] = s * eval_term2(f_vec_field_[1], y);
}

void WVFStudy::eval_U1_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (config_kindvf_ == INTCONFIG_ORIGINAL && gcf_U1_ != nullptr)
        s = eval_term2(gcf_U1_, y);

    if (config_kindvf_ == INTCONFIG_ORIGINAL && singinf_)
        s *= y[1];

    f[0] = s * eval_term2(vec_field_U1_[0], y);
    f[1] = s * eval_term2(vec_field_U1_[1], y);
}

void WVFStudy::eval_U2_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (config_kindvf_ == INTCONFIG_ORIGINAL && gcf_U2_ != nullptr)
        s = eval_term2(gcf_U2_, y);

    if (config_kindvf_ == INTCONFIG_ORIGINAL && singinf_)
        s *= y[1];

    f[0] = s * eval_term2(vec_field_U2_[0], y);
    f[1] = s * eval_term2(vec_field_U2_[1], y);
}

void WVFStudy::eval_V1_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (config_kindvf_ == INTCONFIG_ORIGINAL && gcf_V1_ != nullptr)
        s = eval_term2(gcf_V1_, y);

    if (config_kindvf_ == INTCONFIG_ORIGINAL && singinf_)
        s *= y[1];

    f[0] = s * eval_term2(vec_field_V1_[0], y);
    f[1] = s * eval_term2(vec_field_V1_[1], y);
}

void WVFStudy::eval_V2_vec_field(double *y, double *f)
{
    double s = 1.0;

    if (config_kindvf_ == INTCONFIG_ORIGINAL && gcf_V2_ != nullptr)
        s = eval_term2(gcf_V2_, y);

    if (config_kindvf_ == INTCONFIG_ORIGINAL && singinf_)
        s *= y[1];

    f[0] = s * eval_term2(vec_field_V2_[0], y);
    f[1] = s * eval_term2(vec_field_V2_[1], y);
}

void WVFStudy::eval_vec_field_cyl(double *y, double *f)
{
    double s = 1.0;

    if (config_kindvf_ == INTCONFIG_ORIGINAL && gcf_C_ != nullptr)
        s = eval_term3(gcf_C_, y);

    f[0] = s * eval_term3(vec_field_C_[0], y);
    f[1] = s * eval_term3(vec_field_C_[1], y);
}

void WVFStudy::default_finite_to_viewcoord(double x, double y, double *ucoord)
{
    double pcoord[3];

    (this->*R2_to_sphere)(x, y, pcoord);
    (this->*sphere_to_viewcoord)(pcoord[0], pcoord[1], pcoord[2], ucoord);
}

bool WVFStudy::default_sphere_to_viewcoordpair(double *p, double *q, double *u1,
                                               double *u2, double *u3,
                                               double *u4)
{
    /*
        when plotting a line, one checks plot coordinates for both endpoints.
        Normally, this is just it.

        However, when the plot canvas has a discontinuity (for example when
       plotting
        two charts at infinity at the same time), this routine should be
       bypassed,
        so that intermediate points ensure that the line plotting is done
       correctly.
    */
    (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);
    UNUSED(u3); // u3&u4 are not used in this case
    UNUSED(u4);
    return true; // returns true: line piece does not undergo a discontinuity in
                 // coordinates
}

bool WVFStudy::psphere_to_viewcoordpair_discontinuousx(double *p, double *q,
                                                       double *u1, double *u2,
                                                       double *u3, double *u4)
{
    /*
        when plotting a line, one checks plot coordinates for both endpoints.
        Normally, this is just it.

        However, when the plot canvas has a discontinuity (for example when
       plotting
        two charts at infinity at the same time), this routine should be
       bypassed,
        so that intermediate points ensure that the line plotting is done
       correctly.
    */

    if (p[0] * q[0] < 0) {
        double t, y, z;
        t = q[0] / (q[0] - p[0]);
        y = t * p[1] + (1 - t) * q[1];
        z = t * p[2] + (1 - t) * q[2];

        (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        if (p[0] < 0) {
            (this->*sphere_to_viewcoord)(-1e-8, y, z, u2);
            (this->*sphere_to_viewcoord)(1e-8, y, z, u3);
        } else {
            (this->*sphere_to_viewcoord)(1e-8, y, z, u2);
            (this->*sphere_to_viewcoord)(-1e-8, y, z, u3);
        }
        (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u4);
        return false;
    }

    (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);

    return true; // returns true: line piece does not undergo a discontinuity in
                 // coordinates
}

bool WVFStudy::psphere_to_viewcoordpair_discontinuousy(double *p, double *q,
                                                       double *u1, double *u2,
                                                       double *u3, double *u4)
{
    /*
        when plotting a line, one checks plot coordinates for both endpoints.
        Normally, this is just it.

        However, when the plot canvas has a discontinuity (for example when
       plotting
        two charts at infinity at the same time), this routine should be
       bypassed,
        so that intermediate points ensure that the line plotting is done
       correctly.
    */

    if (p[1] * q[1] < 0) {
        double t, x, z;
        t = q[1] / (q[1] - p[1]);
        x = t * p[0] + (1 - t) * q[0];
        z = t * p[2] + (1 - t) * q[2];

        (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        if (p[1] < 0) {
            (this->*sphere_to_viewcoord)(x, -1e-8, z, u2);
            (this->*sphere_to_viewcoord)(x, 1e-8, z, u3);
        } else {
            (this->*sphere_to_viewcoord)(x, 1e-8, z, u2);
            (this->*sphere_to_viewcoord)(x, -1e-8, z, u3);
        }
        (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u4);
        return false;
    }

    (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);

    return true; // returns true: line piece does not undergo a discontinuity in
                 // coordinates
}

bool WVFStudy::plsphere_to_viewcoordpair_discontinuousx(double *p, double *q,
                                                        double *u1, double *u2,
                                                        double *u3, double *u4)
{
    double c[2], qx, qy, px, py;
    double pp[3], qq[3];
    double ap[2], aq[2];
    double t, y;

    if (p[0] == 0 || q[0] == 0) {
        plsphere_to_R2(p[0], p[1], p[2], c);
        px = c[0];
        py = c[1];
        plsphere_to_R2(q[0], q[1], q[2], c);
        qx = c[0];
        qy = c[1];

        if (qx * px < 0) {
            // the line y=0 has been crossed
            t = px / (px - qx);
            y = t * qy + (1 - t) * py; // (x,0) is an intermediate point.

            (this->*sphere_to_viewcoord)(0, px, py, u1);
            if (px < 0) {
                (this->*sphere_to_viewcoord)(0, -1e-8, y, u2);
                (this->*sphere_to_viewcoord)(0, 1e-8, y, u3);
            } else {
                (this->*sphere_to_viewcoord)(0, 1e-8, y, u2);
                (this->*sphere_to_viewcoord)(0, -1e-8, y, u3);
            }
            (this->*sphere_to_viewcoord)(0, qx, qy, u4);
            return false;
        }
        (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);
        return true;
    }

    // both p and q lie in the annulus.

    if (cos(p[2]) * cos(q[2]) < 0) {
        plsphere_annulus(p[0], p[1], p[2], ap);
        plsphere_annulus(p[0], p[1], p[2], aq);

        t = ap[0] / (ap[0] - aq[0]);
        y = t * aq[1] + (1 - t) * ap[1];

        if (cos(p[2]) < 0) {
            annulus_plsphere(-1e-8, y, pp);
            annulus_plsphere(1e-8, y, qq);
        } else {
            annulus_plsphere(1e-8, y, pp);
            annulus_plsphere(-1e-8, y, qq);
        }
        (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        (this->*sphere_to_viewcoord)(pp[0], pp[1], pp[2], u2);
        (this->*sphere_to_viewcoord)(qq[0], qq[1], qq[2], u3);
        (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u4);

        return false;
    }
    (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);
    return true;
}

bool WVFStudy::plsphere_to_viewcoordpair_discontinuousy(double *p, double *q,
                                                        double *u1, double *u2,
                                                        double *u3, double *u4)
{
    double c[2], qx, qy, px, py;
    double pp[3], qq[3];
    double ap[2], aq[2];
    double t, x;

    if (p[0] == 0 || q[0] == 0) {
        plsphere_to_R2(p[0], p[1], p[2], c);
        px = c[0];
        py = c[1];
        plsphere_to_R2(q[0], q[1], q[2], c);
        qx = c[0];
        qy = c[1];

        if (qy * py < 0) {
            // the line y=0 has been crossed
            t = py / (py - qy);
            x = t * qx + (1 - t) * px; // (x,0) is an intermediate point.

            (this->*sphere_to_viewcoord)(0, px, py, u1);
            if (py < 0) {
                (this->*sphere_to_viewcoord)(0, x, -1e-8, u2);
                (this->*sphere_to_viewcoord)(0, x, 1e-8, u3);
            } else {
                (this->*sphere_to_viewcoord)(0, x, 1e-8, u2);
                (this->*sphere_to_viewcoord)(0, x, -1e-8, u3);
            }
            (this->*sphere_to_viewcoord)(0, qx, qy, u4);
            return false;
        }
        (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);
        return true;
    }

    // both p and q lie in the annulus.

    if (sin(p[2]) * sin(q[2]) < 0) {
        plsphere_annulus(p[0], p[1], p[2], ap);
        plsphere_annulus(p[0], p[1], p[2], aq);

        t = ap[1] / (ap[1] - aq[1]);
        x = t * aq[0] + (1 - t) * ap[0];

        if (sin(p[2]) < 0) {
            annulus_plsphere(x, -1e-8, pp);
            annulus_plsphere(x, 1e-8, qq);
        } else {
            annulus_plsphere(x, 1e-8, pp);
            annulus_plsphere(x, -1e-8, qq);
        }
        (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
        (this->*sphere_to_viewcoord)(pp[0], pp[1], pp[2], u2);
        (this->*sphere_to_viewcoord)(qq[0], qq[1], qq[2], u3);
        (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u4);

        return false;
    }
    (this->*sphere_to_viewcoord)(p[0], p[1], p[2], u1);
    (this->*sphere_to_viewcoord)(q[0], q[1], q[2], u2);
    return true;
}

bool WVFStudy::less_poincare(double *p1, double *p2)
{
    if ((p1[0] * p2[2]) < (p2[0] * p1[2]))
        return 1;
    if (((p1[0] * p2[2]) == (p2[0] * p1[2])) &&
        ((p1[1] * p2[2]) < (p2[1] * p1[2])))
        return true;
    return false;
}

double WVFStudy::eval_lc_poincare(double *pp, double a, double b, double c)
{
    return (a * pp[0] + b * pp[1] + c * pp[2]);
}

double WVFStudy::eval_lc_lyapunov(double *pp, double a, double b, double c)
{
    if (pp[0])
        return (a * pow(pp[1], double_q_) * cos(pp[2]) +
                b * pow(pp[1], double_p_) * sin(pp[2]) +
                c * pow(pp[1], double_p_plus_q_));
    else
        return (a * pp[1] + b * pp[2] + c);
}

bool WVFStudy::less_lyapunov(double *p1, double *p2)
{
    double u[2], v[2];

    plsphere_annulus(p1[0], p1[1], p1[2], u);
    plsphere_annulus(p2[0], p2[1], p2[2], v);
    if (u[0] < v[0])
        return 1;
    if ((u[0] == v[0]) && (u[1] < v[1]))
        return 1;
    return 0;
}

void WVFStudy::set_current_step(double curstep)
{
    config_currentstep_ = curstep;
}
