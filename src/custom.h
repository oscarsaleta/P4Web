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

#ifndef CUSTOM_H
#define CUSTOM_H

/*!
 * @brief This file contains several macro and struct definitions
 * @file custom.h
 */

#include "color.h"

/**
 * Macro for avoiding "unused variable" warnings. 
 * @param  x variable to avoid warnings of 
 * @return   cast to void
 *
 * Some functions are forced to admit certain arguments (e.g. for using them
 * in WVFStudy::rk78() calls).
 * This means that sometimes one of the arguments is not used, so we want to
 * avoid warnings from the compiler regarding these. 
 */
#define UNUSED(x) (void)(x)   //  to avoid warnings of unused variables in case we cannot avoid it

/**
 * Linked list of lines (pairs of points)
 */
struct P4POLYLINES
{
    double x1;          ///< coord 1 of point 1
    double y1;          ///< coord 2 of point 1
    double x2;          ///< coord 1 of point 2
    double y2;          ///< coord 2 of point 2
    P4POLYLINES * next; ///< next struct P4POLYLINES (linked list)
};

// parameters in the plot window (including parameters for the Runge-Kutta 7/8 method)

#define DEFAULT_INTCONFIG   INTCONFIG_ORIGINAL  ///< in case of a GCF: integrate using original vf
#define INTCONFIG_ORIGINAL  1                   ///< integrate using original VF
#define INTCONFIG_REDUCED   0                   ///< integrate using reduced VF when possible

#define DEFAULT_HMA     1.E-1       ///< default value for HMA (=MAXIMUM STEP SIZE)
#define MIN_HMA         1.E-32      ///< minimum value for HMA = 10^(-16)
#define MAX_HMA         10.0        ///< maximum value for HMA = 10^1

#define DEFAULT_HMI     1.E-6       ///< default value for HMI (=MINIMUM STEP SIZE)
#define MIN_HMI         1.E-32      ///< minimum value for HMI
#define MAX_HMI         10.0        ///< maximum value for HMI

#define DEFAULT_STEPSIZE    0.01    /**< default step size
                                    stepsize must be in between HMI and HMA */

#define DEFAULT_PROJECTION  -1.0    ///< default projection (Poincare sphere only)
#define MIN_PROJECTION      -1.E16  ///< projection can be VERY negative
#define MAX_PROJECTION      -1.E-32 ///< projection must be strictly negative

#define DEFAULT_TOLERANCE   1.E-8   ///< default tolerance
#define MIN_TOLERANCE       1.E-32  ///< minimum tolerance
#define MAX_TOLERANCE       1.0     ///< maximum tolerance

#define DEFAULT_INTPOINTS   200     ///< number of points during integration
#define MIN_INTPOINTS       1       ///< minimum number of points during integration
#define MAX_INTPOINTS       32767   ///< maximum number of points during integration

#define DEFAULT_LINESTYLE   LINESTYLE_DASHES    ///< choose between LINESTYLE_DASHES and LINESTYLE_POINTS

#define UPDATEFREQ_STEPSIZE 100     /**< after each 100 points of integration: update
                                    "current step size" field in this window. */


// limit cycles window:

#define DEFAULT_LCORBITS    1           ///< number of orbits to integrate before progress window is updated
#define MIN_LCORBITS        1           ///< when dividing transverse section length by
#define MAX_LCORBITS        32767       ///<  grid: number of orbits must lie between these values

#define DEFAULT_LCPOINTS    2000        ///< number of points to integrate EACH orbit
#define MIN_LCPOINTS        1           ///< minimum number of points to integrate EACH orbit
#define MAX_LCPOINTS        32767       ///< maximum number of points to integrate EACH orbit

#define DEFAULT_LCGRID      0.01        ///< division interval for the transverse section
#define MIN_LCGRID          1.E-16      ///< minimum division interval for the transverse section
#define MAX_LCGRID          1E16        ///< maximum division interval for the transverse section

// Defining colors.
//
// Choose one of the constants defined in COLOR.H

// Color of singular points:

#define CFOREGROUND     WHITE           ///< foreground
#define CBACKGROUND     BLACK           ///< background

#define CSADDLE_NODE    MAGENTA         ///< saddle-node
#define CSADDLE         GREEN2          ///< saddle
#define CNODE_S         BLUE            ///< stable node
#define CNODE_U         RED             ///< unstable node
#define CWEAK_FOCUS     CFOREGROUND     ///< weak focus
#define CWEAK_FOCUS_S   BLUE2           ///< stable weak focus
#define CWEAK_FOCUS_U   RED2            ///< unstable weak focus
#define CSTRONG_FOCUS_S BLUE            ///< stable strong focus
#define CSTRONG_FOCUS_U RED             ///< unstable strong focus
#define CCENTER         GREEN2          ///< center
#define CDEGEN          CFOREGROUND     ///< degenerated
#define CLINEATINFINITY CFOREGROUND     ///< color of poincare sphere

#define CSTABLE         BLUE            ///< stable separatrice
#define CUNSTABLE       RED             ///< unstable separatrice
#define CCENT_STABLE    BLUE2           ///< center-stable separatrice
#define CCENT_UNSTABLE  RED1            ///< center-unstable separatrice

#define CW_SEP          GOLD            ///< selected separatrice
#define CORBIT          YELLOW          ///< orbits (use GREEN1 when background is white)
#define CLIMIT          MAGENTA2        ///< limit cycles
#define CSING           GREEN           ///< curve of singularities



//#define   USE_SYSTEM_PRINTER          // comment when system printer fails

// defining symbol sizes:
// these are the sizes of the symbols in the plot window/legend window
//
// Note: these sizes must be even!

#define SYMBOLSIZE      6           ///< symbol size

#define SYMBOLHEIGHT    SYMBOLSIZE  ///< symbol height
#define SYMBOLWIDTH     SYMBOLSIZE  ///< symbol width


// MATHEMATICS

// during calculations, P4/Maple tries to simplify intermediate expressions.
// The simplify command could do well, but sometimes it is better to try to
// simplify expressions with radicals in it as well.  Unfortunately, we have
// found that radsimp contains a bug causing Maple to divide by 0 internally.
// The construction below is a workaround for this bug:

/**
 * Custom Maple simplify command.
 *
 * During calculations, Maple tries to simplify intermediate expressions.
 * The simplify command could do well, but sometimes it is better to try
 * to simplify expressions with radicals in it as well.
 * 
 * Unfortunately, we have found that radsimp contains a bug causing Maple
 * to divide by 0 internally.
 * 
 * This construction is a workaround for this bug.
 */
#define MAPLE_SIMPLIFY_EXPRESSIONS    \
        "proc(expr) try radsimp(simplify(expr),ratdenom) catch: simplify(expr) end try end";


#define ZCOORD          (sqrt(2)/2)     /**< Determine when the point (in the R2-chart)
                                        of the orbit goes to infinity.
                                        If z coord on the poincare sphere is less than
                                        ZCOORD then point goes to infinity */

#define RADIUS          0.25            ///< radius for the finite points
#define RADIUS2         (RADIUS*RADIUS) ///< square of radius


/**
 * Enum for the possible types of views
 *
 * This sets which chart are we studying and plotting
 */
enum TYPEOFVIEWS {
    TYPEOFVIEW_PLANE    = 0,       ///< plane R2
    TYPEOFVIEW_SPHERE   = 1,       ///< sphere
    TYPEOFVIEW_U1       = 2,       ///< U1 sphere chart
    TYPEOFVIEW_U2       = 3,       ///< U2 sphere chart
    TYPEOFVIEW_V1       = 4,       ///< V1 sphere chart
    TYPEOFVIEW_V2       = 5        ///< V2 sphere chart
};


#endif /* CUSTOM_H */
