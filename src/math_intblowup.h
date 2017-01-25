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

#ifndef MATH_INTBLOWUP_H
#define MATH_INTBLOWUP_H

/*!
 * @brief File that contains the function for integrating a blowup
 * @file math_intblowup.h
 */

#include "file_tab.h"
#include "win_sphere.h"

/**
 * Integrate blowup separatrices for a non elementary singular point
 * @param  spherewnd sphere object of the study
 * @param  pcoord2   coordinates in Poincaré sphere
 * @param  de_sep    blowup separatrices
 * @param  step      step size for integration
 * @param  dir       direction for integration
 * @param  type      stability type of singularity
 * @param  orbit     linked list where last orbit will be stored
 * @param  chart     chart in which to perform computations
 * @return           pointer to first orbit
 */
orbits_points * integrate_blow_up( WWinSphere * spherewnd, double * pcoord2,
                    blow_up_points * de_sep, double step, int dir, int type,
                    orbits_points ** orbit, int chart );

#endif // MATH_INTBLOWUP_H
