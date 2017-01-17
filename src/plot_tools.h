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

#ifndef PLOT_TOOLS_H
#define PLOT_TOOLS_H

/*!
 * @brief Some plotting functions
 * @file plot_tools.h
 */

#include "win_sphere.h"

/**
 * Pointer to a function that plots a line in the sphere
 */
extern void (*plot_l)( WWinSphere *, double *, double *, int );
/**
 * Pointer to a function that plots a point in the sphere
 */
extern void (*plot_p)( WWinSphere *, double *, int );

/**
 * Plot a line in the sphere
 * @param sp    Sphere
 * @param p1    Line is given by two points (which are 2 element arrays)
 * @param p2    Line is given by two points (which are 2 element arrays)
 * @param color Color of the line (given by codes from color.h)
 */
void spherePlotLine( WWinSphere * sp, double * p1, double * p2, int color );
/**
 * Plot a point in the sphere
 * @param sp    Sphere
 * @param p     Point to plot (2 element array)
 * @param color Color (given by codes from color.h)
 */
void spherePlotPoint( WWinSphere * sp, double * p, int color );

//void spherePrintLine( WWinSphere * sp, double * p1, double * p2, int color );
//void spherePrintPoint( WWinSphere * sp, double * p, int color );

/**
 * Intersects a line with a rectangle
 * @param  x1   First point of line is (x1,y1)
 * @param  y1   First point of line is (x1,y1)
 * @param  x2   Second point of line is (x2,y2)
 * @param  y2   Second point of line is (x2,y2)
 * @param  xmin Rectangle is given by [xmin,xmax],[ymin,ymax]
 * @param  xmax Rectangle is given by [xmin,xmax],[ymin,ymax]
 * @param  ymin Rectangle is given by [xmin,xmax],[ymin,ymax]
 * @param  ymax Rectangle is given by [xmin,xmax],[ymin,ymax]
 * @return      @c true if line intersects the rectangle, @c false otherwise
 *
 * Changes the coordinates so that both endpoints are the endpoints
 * of the visible part of the line. Returns false if there is no visible part.
 */
bool lineRectangleIntersect( double &x1, double &y1, double &x2, double &y2,
                                        double xmin, double xmax, double ymin, double ymax );


#endif // PLOT_TOOLS_H
