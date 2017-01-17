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
/*  P4 (Polynomial Planar Phase Portraits) WEB VERSION SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4Web
 *
 *  Copyright (C) 2016  O. Saleta
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

#ifndef MATH_POLYNOM_H
#define MATH_POLYNOM_H

/*!
 * @brief Polynomial related functions
 * @file math_polynom.h
 */

#include "file_tab.h"

/**
 * Calculates p(t) for a polynomial p and a value t
 * @param  p     Polynomial p
 * @param  value Value t
 * @return       Result p(t)
 */
double eval_term1( P4POLYNOM1 p, double value);
/**
 * Calculates f(x,y) for a polynomial f and values x and y.
 * @param  f     Polynomial f
 * @param  value Array (x,y)
 * @return       Result f(x,y)
 */
double eval_term2( P4POLYNOM2 f, double * value);
/**
 * Calculates F( r, cos(theta), sin(theta) ) for a polynomial F
 * and values of r and theta
 * @param  F     Polynomial F
 * @param  value Array (r,theta)
 * @return       Result F(r,cos(theta),sin(theta))
 */
double eval_term3( P4POLYNOM3 F, double * value);

/**
 * Delete a one variable polynomial
 * @param p polynomial
 */
void delete_term1( P4POLYNOM1 p );
/**
 * Delete a two variables polynomial
 * @param p polynomial
 */
void delete_term2( P4POLYNOM2 p );
/**
 * Delete a three variables polynomial
 * @param p polynomial
 */
void delete_term3( P4POLYNOM3 p );



#endif // MATH_POLYNOM_H
