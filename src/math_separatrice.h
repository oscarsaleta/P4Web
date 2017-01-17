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

#ifndef MATH_SEPARATRICE_H
#define MATH_SEPARATRICE_H

/*!
 * @brief Functions for integrating and plotting separatrices
 * @file math_separatrice.h
 */

#include "file_tab.h"
#include "win_sphere.h"


/*extern void (*change_epsilon)( WWinSphere *, double );
extern void (*start_plot_sep)( WWinSphere * );
extern void (*cont_plot_sep)( WWinSphere * );
extern void (*plot_next_sep)( WWinSphere * );
extern void (*select_next_sep)( WWinSphere * );

void start_plot_saddle_sep( WWinSphere * spherewnd );
void cont_plot_saddle_sep( WWinSphere * spherewnd );
void plot_next_saddle_sep( WWinSphere * spherewnd );
void select_next_saddle_sep( WWinSphere * spherewnd );
void change_epsilon_saddle( WWinSphere * spherewnd, double e );

void start_plot_se_sep( WWinSphere * spherewnd );
void cont_plot_se_sep( WWinSphere * spherewnd );
void plot_next_se_sep( WWinSphere * spherewnd );
void select_next_se_sep( WWinSphere * spherewnd );
void change_epsilon_se( WWinSphere * spherewnd, double e);

void start_plot_de_sep( WWinSphere * spherewnd );
void cont_plot_de_sep( WWinSphere * spherewnd );
void plot_next_de_sep( WWinSphere * spherewnd );
void select_next_de_sep( WWinSphere * spherewnd );
void change_epsilon_de( WWinSphere * spherewnd, double e );*/

/**
 * Compute all separatrices for all singularities
 * @param spherewnd sphere object where the study and plot are stored
 */
void plot_all_sep( WWinSphere * spherewnd );
/**
 * Draw separatrice
 * @param spherewnd sphere object
 * @param sep       separatrice
 *
 * This function draws a separatrice with its default color
 */
void draw_sep( WWinSphere * spherewnd, orbits_points *sep );
/**
 * Draw separatrice with specified color
 * @param spherewnd sphere object
 * @param sep       separatrice
 * @param color     color (given by definitions from colors.h)
 *
 * Same as draw_sep() but with a custom color
 */
void draw_selected_sep( WWinSphere * spherewnd, orbits_points *sep, int color);

/**
 * Find color for a P4POLYNOM2 of a given type at a given point
 * @param f     polynomial
 * @param type  type of stability
 * @param y     point
 * @return      color code
 */
int findSepColor2(P4POLYNOM2 f, int type, double y[2]);
/**
 * Find color for a P4POLYNOM3 of a given type at a given point
 * @param f     polynomial
 * @param type  type of stability
 * @param y     point
 * @return      color code
 */
int findSepColor3(P4POLYNOM3 f, int type, double y[2]);

/**
 * Change type from stable to unstable (or from unstable to stable)
 * @param  type type of stability
 * @return      opposite type of stability
 */
int change_type(int type);

/**
 * Separatrices for saddle or saddle-node singularities
 * @param  spherewnd sphere object
 * @param  x0        integration limits
 * @param  y0        integration limits
 * @param  a11       transformation matrix
 * @param  a12       transformation matrix
 * @param  a21       transformation matrix
 * @param  a22       transformation matrix
 * @param  epsilon   radius for boundary around singularity where to start integration
 * @param  sep1      Taylor approximation of invariant manifold of singularity
 * @param  orbit     linked list where integrated separatrice is stored
 * @param  chart     chart in which integration is performed
 * @return           pointer to first element of the linked list that is
 *                           the integrated separatrice
 *
 * When the singularity is a saddle or saddle-node, first we use the Taylor
 * approximation of the invariant manifold until we meet the boundary of a 
 * circle of radius epsilon.
 *
 * Then we integrate using WVFStudy::rk78() (calling WVFStudy::integrate_poincare_sep()
 * or WVFStudy::integrate_lyapunov_sep() depending on which sphere are we working on).
 */
orbits_points * plot_separatrice(WWinSphere * spherewnd, double x0, double y0,double a11, double a12,
                                        double a21, double a22, double epsilon, sep * sep1,
                                        orbits_points ** orbit, short int chart);

/**
 * Apply a list of transformations to a point
 * @param trans linked list of transformations
 * @param x0    initial coordinates
 * @param y0    initial coordinates
 * @param point array for storing final coordinates
 */
void make_transformations(transformations * trans, double x0, double y0, double * point);


#endif // MATH_SEPARATRICE_H
