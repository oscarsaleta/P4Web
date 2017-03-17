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

#ifndef FILE_TAB_H
#define FILE_TAB_H

/*!
 * @brief This file implements the class WVFStudy and defines
 * singularities/separatrices structs
 * @file file_tab.h
 *
 * This file contains the code to read the information from maple.
 * The relevant structures for saddles etc are set up here.
 */

#include "custom.h"

#include <Wt/WString>
#include <Wt/WTextArea>

// -----------------------------------------------------------------------
//                      General polynomial expressions
// -----------------------------------------------------------------------

/**
 * Linked list of univariate terms a*x^i
 */
struct term1 {
    int exp;                  ///< exponent (i) */
    double coeff;             ///< coefficient (a) */
    struct term1 *next_term1; ///< next term1 (linked list) */

    /**
     * Constructor method
     */
    term1() : next_term1(nullptr){};
};

/**
 * Typedef for a pointer to a struct term1
 */
typedef struct term1 *P4POLYNOM1;

/**
 * Linked list of terms a*x^i*y^j
 */
struct term2 {
    int exp_x;                ///< x exponent (i)
    int exp_y;                ///< y exponent (j)
    double coeff;             ///< coefficient (a)
    struct term2 *next_term2; ///< next term2 (linked list)

    /**
     * Constructor method
     */
    term2() : next_term2(nullptr){};
};

/**
 * Typedef for a pointer to a struct term2
 */
typedef struct term2 *P4POLYNOM2;

/**
 * Linked list of terms a*r^i*cos(theta1)^j*sin(theta2)^k
 */
struct term3 {
    int exp_r;                ///< r exponent (i) */
    int exp_Co;               ///< cos exponent (theta1) */
    int exp_Si;               ///< sin exponent (theta2) */
    double coeff;             ///< coefficient (a) */
    struct term3 *next_term3; ///< next term3 (linked list) */

    /**
     * Constructor method
     */
    term3() : next_term3(nullptr){};
};

/**
 * Typedef for a pointer to a struct term3
 */
typedef struct term3 *P4POLYNOM3;

// -----------------------------------------------------------------------
//                              Orbits
// -----------------------------------------------------------------------

/**
 * Linked list of orbit points
 *
 * This struct holds information about coordinates and also about style
 * for plotting purposes.
 */
struct orbits_points {
    int color; ///< color of seperatrice */

    double pcoord[3]; /**< point on the poincare sphere -> p=(X,Y,Z)
                       or
                       on the poincare-lyapunov sphere
                       -> p=(0,x,y) or p=(1,r,theta) */
    int dashes;       ///< indicates if the line is dashed or continuous
    int dir;  ///< if we have a line of sing at infinity and have to change */
    int type; /**< the direction if we integrate the orbit of separatrice
               and sometimes the type */

    struct orbits_points
        *next_point; ///< pointer to next orbits_points (linked list) */

    /**
     * Constructor method
     */
    orbits_points() : next_point(nullptr){};
};

/**
 * Typedef for a pointer to a struct orbits_points
 */
typedef struct orbits_points *P4ORBIT;

/**
 * Linked list of orbits
 *
 * Every orbit contains two pointers to a linked list of orbit points
 * (#orbits_points)
 */
struct orbits {
    double pcoord[3];          ///< startpoint
    int color;                 ///< color of the orbit
    P4ORBIT f_orbits;          /**< linked list of orbit points (which makes
                                    the full orbit) */
    P4ORBIT current_f_orbits;  ///< current point
    struct orbits *next_orbit; ///< pointer to next orbits (linked lists)

    /**
     * Constructor method
     */
    orbits() : next_orbit(nullptr){};
};

// -----------------------------------------------------------------------
//                      Blow up structure
// -----------------------------------------------------------------------

/**
 * Linked list of transformations for blow up
 *
 * A point (x,y) is translated to F(x,y)=(c1*x^d1*y^d2,c2*x^d3*y^d4)/x^d
 */
struct transformations {
    double x0; ///< translation point
    double y0; ///< translation point
    int c1;    ///< coefficients for F(x,y)
    int c2;    ///< coefficients for F(x,y)
    int d1;    ///< coefficients for F(x,y)
    int d2;    ///< coefficients for F(x,y)
    int d3;    ///< coefficients for F(x,y)
    int d4;    ///< coefficients for F(x,y)
    int d;     ///< X/x^d
    struct transformations
        *next_trans; /**< pointer to next transformations (linked
                        list) */

    /**
     * Constructor method
     */
    transformations() : next_trans(nullptr){};
};

/**
 * Linked list of blow up points
 */
struct blow_up_points {
    int n;                         ///< number of #transformations
    struct transformations *trans; ///< linked list of #transformations
    double x0;  ///< last point that is not degenerate (x0,y0)
    double y0;  ///< last point that is not degenerate (x0,y0)
    double a11; ///< transformation matrix (2x2)
    double a12; ///< transformation matrix (2x2)
    double a21; ///< transformation matrix (2x2)
    double a22; ///< transformation matrix (2x2)
    struct term2 *vector_field[2]; ///< vector field
    struct term1 *sep;             ///< sep (t,g(t))
    int type;                      ///< type of seperatrice (STYPE_STABLE, ...)
    int blow_up_vec_field; /**< if true then use the blow up vector field if
                            the modulus of the last point of the separatrices is
                            less
                            than 1 */
    double point[2];       ///< end point sep in blow up chart

    struct orbits_points *first_sep_point; ///< first separatrice point
    struct orbits_points *last_sep_point;  ///< last separatrice point
    struct blow_up_points
        *next_blow_up_point; ///< pointer to next blow up point (linked list)

    /**
     * Constructor method
     */
    blow_up_points()
        : trans(nullptr), sep(nullptr), first_sep_point(nullptr),
          last_sep_point(nullptr){};
};

/**
 * Linked list of separatrices
 */
struct sep {
    struct orbits_points *first_sep_point; ///< first separatrice point
    struct orbits_points *last_sep_point;  ///< last separatrice point
    int type;                              /**< possible values:
                                               STYPE_STABLE, UNSTABLE,
                                               CENSTABLE or CENUNSTABLE */
    int direction;  ///< gives the direction of the separatrice
    int d;          ///< flag for @c separatrice
    bool notadummy; /**< false if separatrice is a copy of a structure (obtained
                     through a symmetry) */
    struct term1 *separatrice; ///< if d=0 -> (t,f(t)), d=1 ->(f(t),t)
    struct sep *next_sep;      ///< pointer to next separatrice (linked list)

    /**
     * Constructor method
     */
    sep()
        : first_sep_point(nullptr), last_sep_point(nullptr),
          separatrice(nullptr), next_sep(nullptr){};
};

// -----------------------------------------------------------------------
//                          Singularities
// -----------------------------------------------------------------------

/**
 * General singularity struct (linked list)
 *
 * Part of the structure that is the same for all types of singularities
 */
struct genericsingularity {
    double x0;                       ///< x coordinate of singularity
    double y0;                       ///< y coordinate of singularity
    struct genericsingularity *next; ///< next singularity
    int chart;                       ///< chart where the singularity is located

    /**
     * Constructor method
     */
    genericsingularity() : next(nullptr){};
};

/**
 * Saddle singularity (linked list)
 */
struct saddle {
    double x0;                  ///< x coordinate of singularity
    double y0;                  ///< y coordinate of singularity
    struct saddle *next_saddle; ///< next saddle
    int chart;                  ///< chart where the singularity is located

    double epsilon; /**< radius of sphere up to which separatrices
                        are painted as the Taylor approximation
                        of the invariant manifold */
    int notadummy;  /**< flag that indicates that this singularity
                        does not come from a blowup of a degenerate */

    struct sep *separatrices;      ///< separatrices for this singularity
    struct term2 *vector_field[2]; ///< vector field {xdot,ydot}
    double a11;                    ///< transformation matrix
    double a12;                    ///< transformation matrix
    double a21;                    ///< transformation matrix
    double a22;                    ///< transformation matrix

    /**
     * Constructor method
     */
    saddle() : next_saddle(nullptr), separatrices(nullptr){};
};

/**
 * Semi-elementary singularity (linked list)
 */
struct semi_elementary {
    double x0;                       ///< x coordinate of singularity
    double y0;                       ///< y coordinate of singularity
    struct semi_elementary *next_se; ///< next semi-elementary
    int chart;                       ///< chart where the singularity is located

    double epsilon; /**< radius of sphere up to which separatrices
                        are painted as the Taylor approximation
                        of the invariant manifold */
    int notadummy;  /**< flag that indicates that this singularity
                        does not come from a blowup of a degenerate */

    struct sep *separatrices;      ///< center sep (t,f(t)), sep (g(t),t)
    struct term2 *vector_field[2]; ///< vector field
    double a11;                    ///< transformation matrix
    double a12;                    ///< transformation matrix
    double a21;                    ///< transformation matrix
    double a22;                    ///< transformation matrix

    int type; ///< type of semi-elementary point

    /**
     * Constructor method
     */
    semi_elementary() : separatrices(nullptr){};
};

/**
 * Degenerate singularity (linked list)
 */
struct degenerate {
    double x0;                  ///< x coordinate of singularity
    double y0;                  ///< y coordinate of singularity
    struct degenerate *next_de; ///< next degenerate
    int chart;                  ///< chart where the singularity is located

    double epsilon; /**< radius of sphere up to which separatrices
                        are computed as the Taylor approximation of
                        blow up separatrices */
    int notadummy;  /**< flag that indicates that this singularity
                        does not come from a blowup of a degenerate */

    struct blow_up_points
        *blow_up; ///< blow up points for the degenerate singularity

    /**
     * Constructor method
     */
    degenerate() : next_de(nullptr), blow_up(nullptr){};
};

/**
 * Node singularity (linked list)
 */
struct node {
    double x0;              ///< x coordinate of singularity
    double y0;              ///< y coordinate of singularity
    struct node *next_node; ///< next node
    int chart;              ///< chart where the singularity is located

    int stable; ///< flag that indicates if it is stable

    /**
     * Constructor method
     */
    node() : next_node(nullptr){};
};

/**
 * Strong focus singularity
 */
struct strong_focus {
    double x0;                    ///< x coordinate of singularity
    double y0;                    ///< y coordinate of singularity
    struct strong_focus *next_sf; ///< next strong focus
    int chart;                    ///< chart where the singularity is located

    int stable; ///< flag that indicates it is stable

    /**
     * Constructor method
     */
    strong_focus() : next_sf(nullptr){};
};

/**
 * Weak focus singularity
 */
struct weak_focus {
    double x0;                  ///< x coordinate of singularity
    double y0;                  ///< y coordinate of singularity
    struct weak_focus *next_wf; ///< next weak focus
    int chart;                  ///< chart where the singularity is located

    int type; ///< type of weak focus

    /**
     * Constructor method
     */
    weak_focus() : next_wf(nullptr){};
};

// -----------------------------------------------------------------------
//                          Some definitions
// -----------------------------------------------------------------------

#define TYPEOFSTUDY_ALL 0 ///< study all singularities
#define TYPEOFSTUDY_FIN 1 ///< study only finite singularities
#define TYPEOFSTUDY_INF 2 ///< study only infinite singularities
#define TYPEOFSTUDY_ONE 3 ///< study one particular singularity

#define CHART_R2 0 ///< R2 chart
#define CHART_U1 1 ///< U1 chart (Poincaré sphere)
#define CHART_U2 2 ///< U2 chart (Poincaré sphere)
#define CHART_V1 3 ///< V1 chart (Poincaré sphere)
#define CHART_V2 4 ///< V2 chart (Poincaré sphere)

#define SADDLE 1          ///< saddle identifier
#define NODE 2            ///< node identifier
#define WEAK_FOCUS 3      ///< weak focus identifier
#define STRONG_FOCUS 4    ///< strong focus identifier
#define SEMI_HYPERBOLIC 5 ///< semi hyperbolic identifier
#define NON_ELEMENTARY 6  ///< non-elementary identifier

#define STYPE_UNSTABLE (1)   ///< separatrice type unstable
#define STYPE_STABLE (-1)    ///< separatrice type stable
#define STYPE_CENUNSTABLE 2  ///< separatrice type center unstable
#define STYPE_CENSTABLE (-2) ///< separatrice type center table
#define STYPE_ORBIT 3        ///< separatrice type orbit

#define FOCUSTYPE_UNKNOWN 0    ///< weak focus type unknown
#define FOCUSTYPE_UNSTABLE (1) ///< weak focus type unstable
#define FOCUSTYPE_STABLE (-1)  ///< weak focus type stable
#define FOCUSTYPE_CENTER 4     ///< weak focus type center

#define SETYPE_SADDLENODE_UNSTABSEP                                            \
    1 ///< semi-elementary saddle node unstable separatrice 1
#define SETYPE_SADDLENODE_UNSTABSEP2                                           \
    2 ///< semi-elementary saddle node unstable separatrice 2

#define OT_STABLE STYPE_STABLE             ///< orbit type stable
#define OT_UNSTABLE STYPE_UNSTABLE         ///< orbit type unstable
#define OT_CENT_STABLE STYPE_CENSTABLE     ///< orbit type center stable
#define OT_CENT_UNSTABLE STYPE_CENUNSTABLE ///< orbit type center unstable
#define OT_ORBIT STYPE_ORBIT               ///< orbit type orbit

// -----------------------------------------------------------------------
//                          Results class
// -----------------------------------------------------------------------

/**
 * Class that parses Maple results and processes them
 *
 * @class WVFStudy
 *
 * This class has functions to read Maple results, interpret them, and
 * create all the structures needed to store the singularities (classified
 * by type, etc), orbits, compute the coordinates for singularities in
 * different charts...
 */
class WVFStudy
{
  public:
    // class constructor and destructor methods

    /**
     * Constructor method
     */
    WVFStudy(double projection = DEFAULT_PROJECTION);
    /**
     * Destructor method
     */
    ~WVFStudy();

    // general information

    int typeofstudy;        ///< type of study for these results
    TYPEOFVIEWS typeofview; /**< type of view of the plot. Can be
                                TYPEOFVIEW_PLANE or TYPEOFVIEW_SPHERE */
    int p;                  ///< Lyapunov weight
    int q;                  ///< Lyapunov weight
    bool plweights;         ///< true if p!=1 or q!=1; false if p=q=1

    double double_p;         ///< shortcut: (double)p
    double double_q;         ///< shortcut: (double)q
    double double_p_plus_q;  ///< shortcut: (double)(p+q)
    double double_p_minus_1; ///< shortcut: (double)(p-1)
    double double_q_minus_1; ///< shortcut: (double)(q-1)
    double double_q_minus_p; ///< shortcut: (double)(q-p)

    double xmin;       ///< in case of local study
    double xmax;       ///< in case of local study
    double ymin;       ///< in case of local study
    double ymax;       ///< in case of local study
    bool singinf;      ///< flag for looking for singularities at infinity
    int dir_vec_field; ///< direction of vector field

    Wt::WString lasterror; ///< last error emitted

    // vector field in various charts

    P4POLYNOM2 f_vec_field[2];  ///< vector field in R2
    P4POLYNOM2 vec_field_U1[2]; ///< vectori field in U1
    P4POLYNOM2 vec_field_U2[2]; ///< vectori field in U2
    P4POLYNOM2 vec_field_V1[2]; ///< vectori field in V1
    P4POLYNOM2 vec_field_V2[2]; ///< vectori field in V2
    P4POLYNOM3 vec_field_C[2];  ///< vectori field in C

    // singular points and their properties:

    saddle *first_saddle_point;      ///< linked list of saddles
    semi_elementary *first_se_point; ///< linked list of semi-elementaries
    node *first_node_point;          ///< linked list of nodes
    strong_focus *first_sf_point;    ///< linked list of strong foci
    weak_focus *first_wf_point;      ///< linked list of weak foci
    degenerate *first_de_point;      ///< linked list of degenerates

    // Greatest common factor if present:

    P4POLYNOM2 gcf;    ///< gcf in R
    P4POLYNOM2 gcf_U1; ///< gcf in chart U1
    P4POLYNOM2 gcf_U2; ///< gcf in chart U2
    P4POLYNOM2 gcf_V1; ///< gcf in chart V1
    P4POLYNOM2 gcf_V2; ///< gcf in chart V2
    P4POLYNOM3 gcf_C;  ///< gcf in C

    orbits_points *gcf_points;     ///< orbits points of the gcf
    orbits_points *last_gcf_point; ///< last gcf point

    // limit cycles

    orbits *first_lim_cycle; ///< linked list of limit cycles
    orbits *first_orbit;     ///< linked list of orbits

    // ------ Configuration

    int config_lc_value;       ///< orbits in limit cycle window
    double config_hma;         ///< maximum step size
    double config_hmi;         ///< minimum step size
    double config_step;        ///< step size
    double config_currentstep; ///< current step size (during integration)
    double config_tolerance;   ///< tolerance
    double config_projection;  ///< projection in the case of Poincaré sphere
    int config_intpoints;      ///< number of points to integrate
    int config_lc_numpoints;   ///< number of points in the limit cycle window
    bool config_dashes;        ///< line style (dashes or points)
    bool config_kindvf;        ///< true for original VF, false for reduced

    // run-time when plotting

    orbits *current_orbit;     ///< current orbit for plotting
    orbits *current_lim_cycle; ///< current limit cycle for plotting

    double selected_ucoord[2];          ///< selected coordinate (plotting)
    saddle *selected_saddle_point;      ///< selected saddle (plotting)
    semi_elementary *selected_se_point; ///< selected semi-elementary (plotting)
    degenerate *selected_de_point;      ///< selected degenerate (plotting)
    sep *selected_sep;                  ///< selected separatrice (plotting)
    blow_up_points
        *selected_de_sep; ///< selected blowup for degenerate (plotting)

    // coordinate transformation routines, set up when starting the plot

    /**
     * Current view coordinates to sphere coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current view coordinates). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*viewcoord_to_sphere)(double, double, double *);
    /**
     * Sphere coordinates to current view coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current view coordinates). The functions are implemented in
     * math_charts.cc.
     */
    bool (WVFStudy::*sphere_to_viewcoordpair)(double *, double *, double *,
                                              double *, double *, double *);

    /**
     * Finite coordinates to current view coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current view coordinates). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*finite_to_viewcoord)(double, double, double *);
    /**
     * Sphere coordinates to current view coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current view coordinates). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*sphere_to_viewcoord)(double, double, double, double *);
    /**
     * Check whether current view coordinates are valid
     *
     * This is a pointer to a function (which function will depend
     * on the current view coordinates). The functions are implemented in
     * math_charts.cc.
     */
    bool (WVFStudy::*is_valid_viewcoord)(double, double, double *);
    /**
     * Integrate separatrices in current sphere
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_separatrice.cc.
     */
    void (WVFStudy::*integrate_sphere_sep)(double, double, double, double *,
                                           double *, int *, int *, int *, int *,
                                           double, double);
    /**
     * U1 chart coordinates to sphere coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*U1_to_sphere)(double, double, double *);
    /**
     * U2 chart coordinates to sphere coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*U2_to_sphere)(double, double, double *);
    /**
     * V1 chart coordinates to sphere coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*V1_to_sphere)(double, double, double *);
    /**
     * V2 chart coordinates to sphere coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*V2_to_sphere)(double, double, double *);
    /**
     * Sphere coordinates to U1 chart coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*sphere_to_U1)(double, double, double, double *);
    /**
     * Sphere coordinates to U2 chart coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*sphere_to_U2)(double, double, double, double *);
    /**
     * Sphere coordinates to V1 chart coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*sphere_to_V1)(double, double, double, double *);
    /**
     * Sphere coordinates to V2 chart coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*sphere_to_V2)(double, double, double, double *);

    /**
     * Sphere coordinates to R2 coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*sphere_to_R2)(double, double, double, double *);
    /**
     * R2 coordinates to sphere coordinates
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_charts.cc.
     */
    void (WVFStudy::*R2_to_sphere)(double, double, double *);
    /**
     * Integrate an orbit in the current sphere
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_orbits.cc.
     */
    void (WVFStudy::*integrate_sphere_orbit)(double, double, double, double *,
                                             double *, int *, int *, double,
                                             double);
    /**
     * Evaluate a limit cycle in the current sphere
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_p4.cc.
     */
    double (WVFStudy::*eval_lc)(double *, double, double, double);
    /**
     * Less method for current sphere
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_p4.cc.
     */
    bool (WVFStudy::*less2)(double *, double *);
    /**
     * Change direction in current sphere
     *
     * This is a pointer to a function (which function will depend
     * on the current sphere used). The functions are implemented in
     * math_changedir.h.
     */
    int (WVFStudy::*change_dir)(double *);

    // initialization and destruction of structures

    /**
     * Function used for destructing the vector field
     *
     * This function is called by the destructor ~WVFStudy()
     */
    void deleteVF(void);

    /**
     * Delete the saddles linked list
     *
     * @param s linked list of saddles to delete
     *
     * This function is called by deleteVF()
     */
    void deleteSaddle(saddle *s);
    /**
     * Delete the semi-elementary linked list
     *
     * @param se linked list of semi-elementary singularities
     *  to delete
     *
     * This function is called by deleteVF()
     */
    void deleteSemiElementary(semi_elementary *se);
    /**
     * Delete the nodes linked list
     *
     * @param n linked list of nodes to delete
     *
     * This function is called by deleteVF()
     */
    void deleteNode(node *n);                 /**
                   * Delete the strong foci linked list
                   *
                   * @param sf linked list of strong foci to delete
                   *
                   * This function is called by deleteVF()
                   */
    void deleteStrongFocus(strong_focus *sf); /**
   * Delete the weak foci linked list
   *
   * @param wf linked list of weak foci to delete
   *
   * This function is called by deleteVF()
   */
    void deleteWeakFocus(weak_focus *wf);
    /**
     * Delete the degenerate singularities linked list
     *
     * @param d linked list of degenerate singularities
     * to delete
     *
     * This function is called by deleteVF()
     */
    void deleteDegenerate(degenerate *d);
    /**
     * Delete the separatrices linked list
     *
     * @param s linked list of separatrices to delete
     *
     * This function is called by deleteVF()
     */
    void deleteSeparatrices(sep *s);
    /**
     * Delete the transformations linked list
     *
     * @param t linked list of transformations to delete
     *
     * This function is called by deleteVF()
     */
    void deleteTransformations(transformations *t);
    /**
     * Delete the blowup linked list
     *
     * @param b linked list of blowup to delete
     *
     * This function is called by deleteVF()
     */
    void deleteBlowup(blow_up_points *b);

    /**
     * Delete the limit cycles (orbits) linked list
     *
     * @param o linked list of limit cycles (orbits)
     * to delete
     *
     * This function is called by deleteVF()
     */
    void deleteLimitCycle(orbits *o);
    /**
     * Delete the orbits points linked list
     *
     * @param p linked list of orbits points to delete
     *
     * This function is called by deleteVF()
     */
    void deleteOrbitPoint(P4ORBIT p);
    /**
     * Delete the orbits linked list
     *
     * @param o linked list of orbits to delete
     *
     * This function is called by deleteVF()
     */
    void deleteOrbit(orbits *o);

    // reading of the Maple/Reduce results

    /**
     * Main function for reading the Maple output file
     *
     * @param  basename name of the file where results are stored
     * @return          @c true if no error or @c false if error
     *
     * This function is everything we need from this class. Every
     * other function/struct/variable is used by this one to store
     * the singularities, orbits, limit cycles, blowups..., and to
     * identify them.
     *
     * Given a results file, readTables() parses it and stores in the
     * WFStudy object all the information computed by Maple, so we can
     * use it for plotting everything from C++.
     */
    bool readTables(std::string basename);
    /**
     * Read the GCF for the vector field if there is one
     * @param  fp file where GCF info is stored
     * @return    @c true if no error or @c false if error
     */
    bool readGCF(FILE *fp);
    /**
     * Read vector field from a file
     *
     * @param  fp file where vector field info is stored
     * @param  vf struct where vector field is stored
     * @return    @c true if no error or @c false if error
     *
     * Function called by readTables()
     */
    bool readVectorField(FILE *fp, P4POLYNOM2 *vf);
    /**
     * Read vector field from a file in cylinder chart
     *
     * @param  fp file where vector field info is stored
     * @param  vf struct where vector field is stored
     * @return    @c true if no error or @c false if error
     *
     * Function called by readTables()
     */
    bool readVectorFieldCylinder(FILE *fp, P4POLYNOM3 *vf);
    /**
     * Read singularity info from file
     * @param  fp file with singularity info
     * @return    @c true if no error or @c false if error
     *
     * Function called by readTables(). This function calls
     * every specific <i>readSingularityPoint()</i> function
     * to parse and store all singularities found in the
     * present vector field.
     */
    bool readPoints(FILE *fp);
    /**
     * Read struct term1 from a file
     * @param  fp input file
     * @param  p  pointer to a struct term1
     * @param  N  number of terms of linked list to read
     * @return    @c true if no error or @c false if error
     *
     * This function is called by <i>readSingularityPoint()</i>
     * functions.
     */
    bool readTerm1(FILE *fp, P4POLYNOM1 p, int N);
    /**
     * Read struct term2 from a file
     * @param  fp input file
     * @param  p  pointer to a struct term2
     * @param  N  number of terms of linked list to read
     * @return    @c true if no error or @c false if error
     *
     * This function is called by <i>readSingularityPoint()</i>
     * functions.
     */
    bool readTerm2(FILE *fp, P4POLYNOM2 p, int N);
    /**
     * Read struct term3 from a file
     * @param  fp input file
     * @param  p  pointer to a struct term3
     * @param  N  number of terms of linked list to read
     * @return    @c true if no error or @c false if error
     *
     * This function is called by <i>readSingularityPoint()</i>
     * functions.
     */
    bool readTerm3(FILE *fp, P4POLYNOM3 p, int N);

    /**
     * Read saddle singularities from file
     * @param  fp input file
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readPoints().
     */
    bool readSaddlePoint(FILE *fp);
    /**
     * Read semi-elementary singularities from file
     * @param  fp input file
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readPoints().
     */
    bool readSemiElementaryPoint(FILE *fp);
    /**
     * Read strong foci singularities from file
     * @param  fp input file
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readPoints().
     */
    bool readStrongFocusPoint(FILE *fp);
    /**
     * Read weak foci singularities from file
     * @param  fp input file
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readPoints().
     */
    bool readWeakFocusPoint(FILE *fp);
    /**
     * Read degenerate singularities from file
     * @param  fp input file
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readPoints().
     */
    bool readDegeneratePoint(FILE *fp);
    /**
     * Read node singularities from file
     * @param  fp input file
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readPoints().
     */
    bool readNodePoint(FILE *fp);
    /**
     * Read blowup points from file
     * @param  fp input file
     * @param  b  blowup struct
     * @param  n  number of points to read
     * @return    @c true if no error or @c false if erro
     *
     * This function is called by readDegeneratePoint().
     */
    bool readBlowupPoints(FILE *fp, blow_up_points *b, int n);
    /**
     * Read blowup transformations from file
     * @param  fp    input file
     * @param  trans struct to store transformations
     * @param  n     number of transformations
     * @return       @c true if no error or @c false if erro
     *
     * This function is called by readBlowupPoints().
     */
    bool readTransformations(FILE *fp, transformations *trans, int n);

    /**
     * Setup coordinate transformations
     *
     * Depending on which view coordinates we want to use,
     * this function links the pointers to the actual functions that we
     * will need (see math_charts.cc, math_p4.cc, math_changedir.cc,
     * math_orbits.cc).
     */
    void setupCoordinateTransformations(void); // see math_p4.cpp

    // void dump( Wt::WString basename, Wt::WString info="" );

    // -----------------------------------------------------------------------
    //                  IMPLEMENTATION OF THE POINCARE CHARTS
    // -----------------------------------------------------------------------
    /**
     * R2 to Poincaré sphere
     * @param x      x coordinate
     * @param y      y coordinate
     * @param pcoord length 3 vector where sphere coordinates will be stored
     */
    void R2_to_psphere(double x, double y, double *pcoord);
    /**
     * Poincaré sphere to R2
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where R2 coordinates will be stored
     */
    void psphere_to_R2(double X, double Y, double Z, double *rcoord);
    /**
     * Projection of Poincaré sphere coordinates to circle
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param ucoord length 2 vector where R2 coordinates will be stored
     *
     * Results depend on the config_projection value
     */
    void psphere_ucircle(double X, double Y, double Z, double *ucoord);
    /**
     * Circle projection to Poincaré sphere
     * @param u      coordinate 1 of Poincaré sphere
     * @param v      coordinate 2 of Poincaré sphere
     * @param pcoord length 3 vector where sphere coordinates will be stored
     *
     * Results depend on the config_projection value
     */
    void ucircle_psphere(double u, double v, double *pcoord);
    /**
     * Finite coordinates to circle projection
     * @param x      coordinate 1 of plane
     * @param y      coordinate 2 of plane
     * @param ucoord length 2 vector where circle coordinates will be stored
     *
     * Results depend on the config_projection value
     */
    void finite_ucircle(double x, double y, double *ucoord);
    /**
     * Poincaré sphere to U1 chart coordinates
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where U1 coordinates will be stored
     */
    void psphere_to_U1(double X, double Y, double Z, double *rcoord);
    /**
     * Poincaré sphere to U2 chart coordinates
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where U2 coordinates will be stored
     */
    void psphere_to_U2(double X, double Y, double Z, double *rcoord);
    /**
     * Poincaré sphere to V1 chart coordinates
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where V1 coordinates will be stored
     */
    void psphere_to_V1(double X, double Y, double Z, double *rcoord);
    /**
     * Poincaré sphere to V2 chart coordinates
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where V2 coordinates will be stored
     */
    void psphere_to_V2(double X, double Y, double Z, double *rcoord);
    /**
     * U1 chart to Poincaré sphere coordinates
     * @param z1     coordinate 1 of U1 chart
     * @param z2     coordinate 2 of U1 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void U1_to_psphere(double z1, double z2, double *pcoord);
    /**
     * V1 chart to Poincaré sphere coordinates
     * @param z1     coordinate 1 of V1 chart
     * @param z2     coordinate 2 of V1 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void V1_to_psphere(double z1, double z2, double *pcoord);
    /**
     * U2 chart to Poincaré sphere coordinates
     * @param z1     coordinate 1 of U2 chart
     * @param z2     coordinate 2 of U2 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void U2_to_psphere(double z1, double z2, double *pcoord);
    /**
     * V2 chart to Poincaré sphere coordinates
     * @param z1     coordinate 1 of V2 chart
     * @param z2     coordinate 2 of V2 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void V2_to_psphere(double z1, double z2, double *pcoord);
    /**
     * U1 to Poincaré sphere and then change sign of coordinates
     * @param z1     coordinate 1 of U1 chart
     * @param z2     coordinate 2 of U1 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void VV1_to_psphere(double z1, double z2, double *pcoord);
    /**
     * Change sign of Poincaré sphere coords and then project on U1
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where U1 chart coordinates will be stored
     */
    void psphere_to_VV1(double X, double Y, double Z, double *rcoord);
    /**
     * U2 to Poincaré sphere and then change sign of coordinates
     * @param z1     coordinate 1 of U2 chart
     * @param z2     coordinate 2 of U2 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void VV2_to_psphere(double z1, double z2, double *pcoord);
    /**
     * Change sign of Poincaré sphere coords and then project on U2
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where U2 chart coordinates will be stored
     */
    void psphere_to_VV2(double X, double Y, double Z, double *rcoord);
    /**
     * V1 to Poincaré sphere and then change sign of coordinates
     * @param z1     coordinate 1 of V1 chart
     * @param z2     coordinate 2 of V1 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void UU1_to_psphere(double z1, double z2, double *pcoord);
    /**
     * Change sign of Poincaré sphere coords and then project on V1
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where V1 chart coordinates will be stored
     */
    void psphere_to_UU1(double X, double Y, double Z, double *rcoord);
    /**
     * V2 to Poincaré sphere and then change sign of coordinates
     * @param z1     coordinate 1 of V2 chart
     * @param z2     coordinate 2 of V2 chart
     * @param pcoord length 3 vector where Poincaré sphere coordinates will be
     * stored
     */
    void UU2_to_psphere(double z1, double z2, double *pcoord);
    /**
     * Change sign of Poincaré sphere coords and then project on V2
     * @param X      coordinate 1 of Poincaré sphere
     * @param Y      coordinate 2 of Poincaré sphere
     * @param Z      coordinate 3 of Poincaré sphere
     * @param rcoord length 2 vector where V2 chart coordinates will be stored
     */
    void psphere_to_UU2(double X, double Y, double Z, double *rcoord);
    /**
     * Change coords from cylinder to Poincaré-Lyapunov sphere
     * @param r      radius of cylinder coords
     * @param theta  angle of cylinder coords
     * @param pcoord pointer where new coords are stored
     */
    void cylinder_to_plsphere(double r, double theta, double *pcoord);
    // -----------------------------------------------------------------------
    //                      math_p4.cc FUNCTIONS
    // -----------------------------------------------------------------------
    /**
     * Set current step size of integration
     * @param curstep step size
     */
    void set_current_step(double curstep);
    // -----------------------------------------------------------------------
    //                      INTEGRATE BLOWUP FUNCTIONS
    // -----------------------------------------------------------------------
    /**
     * Evaluate blowup vector field
     * @param y point to evaluate
     * @param f vector for storing the result
     */
    void eval_blow_vec_field(double *y, double *f);
    // -----------------------------------------------------------------------
    //                      NUMERIC FUNCTIONS
    // -----------------------------------------------------------------------
    /**
     * Runge-Kutta order 7/8 numeric integration
     * @param deriv function to integrate, takes a member function of WVFStudy
     * @param y     integration limits
     * @param hh    initial step size
     * @param hmi   minimum step size
     * @param hma   maximum step size
     * @param e1    epsilon
     */
    void rk78(void (WVFStudy::*deriv)(double *, double *), double y[2],
              double *hh, double hmi, double hma, double e1);
    // -----------------------------------------------------------------------
    //                      math_gcf.cc FUNCTIONS
    // -----------------------------------------------------------------------
    /**
     * Specialized function for coordinate transformation used in Gcf
     * eval_U1_vec_field
     *
     * @param x      x coordinate
     * @param y      y coordinate
     * @param pcoord pointer to new coordinates
     */
    void rplane_plsphere0(double x, double y, double *pcoord);
    /**
     * Insert new point in GCF linked list
     * @param x0     1st coordinate
     * @param y0     2nd coordinate
     * @param z0     3rd coordinate
     * @param dashes dashes for @c orbits_points struct
     */
    void insert_gcf_point(double x0, double y0, double z0, int dashes);

  private:
    // void dumpSeparatrices( Wt::WTextArea * m, sep * separ, int margin );
    // void dumpSingularities( Wt::WTextArea * m, genericsingularity * p, const
    // char * type, bool longversion );
    // -----------------------------------------------------------------------
    //                  IMPLEMENTATION OF THE POINCARE CHARTS
    // -----------------------------------------------------------------------
    bool isvalid_psphereviewcoord(double u, double v, double *pcoord);
    bool isvalid_U1viewcoord(double u, double v, double *pcoord);
    bool isvalid_U2viewcoord(double u, double v, double *pcoord);
    bool isvalid_V1viewcoord(double u, double v, double *pcoord);
    bool isvalid_V2viewcoord(double u, double v, double *pcoord);
    bool isvalid_R2viewcoord(double u, double v, double *pcoord);
    void psphere_to_xyrevU1(double X, double Y, double Z, double *rcoord);
    void psphere_to_xyrevV1(double X, double Y, double Z, double *rcoord);
    void xyrevU1_to_psphere(double z1, double z2, double *pcoord);
    void xyrevV1_to_psphere(double z1, double z2, double *pcoord);
    // -----------------------------------------------------------------------
    //              IMPLEMENTATION OF THE POINCARE-LYPANOV CHARTS
    // -----------------------------------------------------------------------
    bool isvalid_plsphereviewcoord(double u, double v, double *pcoord);
    // -----------------------------------------------------------------------
    //                          ANNULUS_PLSPHERE
    // -----------------------------------------------------------------------
    void annulus_plsphere(double x, double y, double *p);
    // -----------------------------------------------------------------------
    //                          PLSPHERE_ANNULUS
    // -----------------------------------------------------------------------
    void plsphere_annulus(double x, double y, double z, double *u);
    // -----------------------------------------------------------------------
    //                          R2_TO_PLSPHERE
    // -----------------------------------------------------------------------
    void R2_to_plsphere(double x, double y, double *pcoord);
    // -----------------------------------------------------------------------
    //                          plsphere_to_R2
    // -----------------------------------------------------------------------
    void plsphere_to_R2(double ch, double u, double v, double *c);
    // -----------------------------------------------------------------------
    //                          FINITE_ANNULUS
    // -----------------------------------------------------------------------
    void finite_annulus(double x, double y, double *u);
    void U1_to_plsphere(double x0, double y0, double *pcoord);
    void xyrevU1_to_plsphere(double z1, double z2, double *pcoord);
    void V1_to_plsphere(double x0, double y0, double *pcoord);
    void xyrevV1_to_plsphere(double z1, double z2, double *pcoord);
    void U2_to_plsphere(double x0, double y0, double *pcoord);
    void V2_to_plsphere(double x0, double y0, double *pcoord);
    void identitytrf_R2(double x, double y, double *ucoord);
    double floatinfinity(void);
    void plsphere_to_U1(double ch, double x, double y, double *rcoord);
    void plsphere_to_xyrevU1(double ch, double x, double y, double *rcoord);
    void plsphere_to_U2(double ch, double x, double y, double *rcoord);
    void plsphere_to_V1(double ch, double x, double y, double *rcoord);
    void plsphere_to_xyrevV1(double ch, double x, double y, double *rcoord);
    void plsphere_to_V2(double ch, double x, double y, double *rcoord);
    // -----------------------------------------------------------------------
    //                      EVALUATION OF VECTOR FIELDS
    // -----------------------------------------------------------------------
    void eval_r_vec_field(double *y, double *f);
    void eval_U1_vec_field(double *y, double *f);
    void eval_U2_vec_field(double *y, double *f);
    void eval_V1_vec_field(double *y, double *f);
    void eval_V2_vec_field(double *y, double *f);
    void eval_vec_field_cyl(double *y, double *f);
    void default_finite_to_viewcoord(double x, double y, double *ucoord);
    bool default_sphere_to_viewcoordpair(double *p, double *q, double *u1,
                                         double *u2, double *u3, double *u4);
    bool psphere_to_viewcoordpair_discontinuousx(double *p, double *q,
                                                 double *u1, double *u2,
                                                 double *u3, double *u4);
    bool psphere_to_viewcoordpair_discontinuousy(double *p, double *q,
                                                 double *u1, double *u2,
                                                 double *u3, double *u4);
    bool plsphere_to_viewcoordpair_discontinuousx(double *p, double *q,
                                                  double *u1, double *u2,
                                                  double *u3, double *u4);
    bool plsphere_to_viewcoordpair_discontinuousy(double *p, double *q,
                                                  double *u1, double *u2,
                                                  double *u3, double *u4);
    // -----------------------------------------------------------------------
    //                      STATIC CHARTS FUNCTIONS
    // -----------------------------------------------------------------------
    // static void cylinder_to_U1( double r, double theta, double * c);
    // static void cylinder_to_U2( double r, double theta, double * c );
    // static void cylinder_to_V1( double r, double theta, double * c);
    // static void cylinder_to_V2( double r, double theta, double * c);
    void U1_to_cylinder(double u, double s, double *c);
    void U2_to_cylinder(double u, double s, double *c);
    void V1_to_cylinder(double u, double s, double *c);
    void V2_to_cylinder(double u, double s, double *c);
    double func_U1(double x);
    double dfunc_U1(double x);
    double func_U1_s0(double theta);
    double dfunc_U1_s0(double theta);
    double func_U2(double x);
    double dfunc_U2(double x);
    double func_U2_s0(double theta);
    double dfunc_U2_s0(double theta);
    double func(double z);
    double dfunc(double z);
    // -----------------------------------------------------------------------
    //                      NUMERIC FUNCTIONS
    // -----------------------------------------------------------------------
    void bisection(double (WVFStudy::*f)(double), double *x, double e);
    double regula_falsi(double (WVFStudy::*f)(double), double *x, double e);
    double newton(double (WVFStudy::*f)(double), double (WVFStudy::*df)(double),
                  double x, double e);
    double find_root(double (WVFStudy::*f)(double),
                     double (WVFStudy::*df)(double), double *value);
    // -----------------------------------------------------------------------
    //                      SEPARATRICE INTEGRATION FUNCTIONS
    // -----------------------------------------------------------------------
    void integrate_poincare_sep(double p0, double p1, double p2, double *pcoord,
                                double *hhi, int *type, int *color, int *dashes,
                                int *dir, double h_min, double h_max);
    void integrate_lyapunov_sep(double p0, double p1, double p2, double *pcoord,
                                double *hhi, int *type, int *color, int *dashes,
                                int *dir, double h_min, double h_max);
    // -----------------------------------------------------------------------
    //                      ORBIT INTEGRATION FUNCTIONS
    // -----------------------------------------------------------------------
    void integrate_poincare_orbit(double p0, double p1, double p2,
                                  double *pcoord, double *hhi, int *dashes,
                                  int *dir, double h_min, double h_max);
    void integrate_lyapunov_orbit(double p0, double p1, double p2,
                                  double *pcoord, double *hhi, int *dashes,
                                  int *dir, double h_min, double h_max);
    // -----------------------------------------------------------------------
    //                      math_p4.cc FUNCTIONS
    // -----------------------------------------------------------------------
    double eval_lc_poincare(double *pp, double, double, double);
    double eval_lc_lyapunov(double *pp, double, double, double);
    bool less_poincare(double *, double *);
    bool less_lyapunov(double *, double *);
    // -----------------------------------------------------------------------
    //                      CHANGE DIRECTION OF VECTOR FIELD
    // -----------------------------------------------------------------------
    int change_dir_poincare(double *p);
    int change_dir_lyapunov(double *p);
};

#define LINESTYLE_DASHES 1 ///< style for dashed lines in plot
#define LINESTYLE_POINTS 0 ///< style for dot lines in plot

#endif /*FILE_TAB_H*/
