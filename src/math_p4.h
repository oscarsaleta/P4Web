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

#ifndef MATH_P4_H
#define MATH_P4_H

/*!
 * @brief Some mathematical-related definitions and macros
 * @file math_p4.h
 */

#include "file_tab.h"

#define MIN_FLOAT   -1.0E32     ///< minimum single precision floating point
#define MAX_FLOAT   1.0E32      ///< maximum single precision floating point

#define p4_finite std::isfinite /**< macro for accessing the standard function
                                that checks if a number is finite */

#define PI 3.1415926535897932384626433832   ///< Pi
#define PI_DIV2 (PI/2.)                     ///< Pi/2
#define PI_DIV4 (PI/4.)                     ///< Pi/4
#define TWOPI (2.0*PI)                      ///< 2*Pi

#define __minus_one_to_q    pow(-1.0,double_q)  ///< (-1)^q
#define __minus_one_to_p    pow(-1.0,double_p)  ///< (-1)^p
#define __one_over_p        (1.0/double_p)      ///< 1/p
#define __one_over_q        (1.0/double_q)      ///< 1/q

/**
 * Copy a length 3 array into another
 * @param x input array
 * @param y copy array
 */
void copy_x_into_y( double * x, double * y );


#endif /* MATH_P4_H */
