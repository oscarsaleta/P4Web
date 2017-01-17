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

#ifndef PLOT_POINTS_H
#define PLOT_POINTS_H

/*!
 * @brief Functions for plotting singular points
 * @file plot_points.h
 *
 * Each singular point has a shape and color that identifies them.
 */

#include <Wt/WPainter>

/**
 * Plot a saddle
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_saddle( Wt::WPainter * p, int x, int y );
/**
 * Plot a stable node
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_stablenode( Wt::WPainter * p, int x, int y );
/**
 * Plot an unstable node
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_unstablenode( Wt::WPainter * p, int x, int y );
/**
 * Plot a weak focus of unknown properties
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_weakfocus( Wt::WPainter * p, int x, int y );
/**
 * Plot a stable weak focus
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_stableweakfocus( Wt::WPainter * p, int x, int y );
/**
 * Plot an unstable weak focus
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_unstableweakfocus( Wt::WPainter * p, int x, int y );
/**
 * Plot a center
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_center( Wt::WPainter * p, int x, int y );
/**
 * Plot a stable strong focus
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_stablestrongfocus( Wt::WPainter * p, int x, int y );
/**
 * Plot an unstable strong focus
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_unstablestrongfocus( Wt::WPainter * p, int x, int y );
/**
 * Plot a semi-elementary saddle-node
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_sesaddlenode( Wt::WPainter * p, int x, int y );
/**
 * Plot a semi-elementary stable node
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_sestablenode( Wt::WPainter * p, int x, int y );
/**
 * Plot a saddle
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_seunstablenode( Wt::WPainter * p, int x, int y );
/**
 * Plot a semi-elementary saddle
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_sesaddle( Wt::WPainter * p, int x, int y );
/**
 * Plot a degenerate singular point
 * @param p Painter object that does the drawing on screen
 * @param x Coordinate 1 of singular point
 * @param y Coordinate 2 of singular point
 */
void win_plot_degen( Wt::WPainter * p, int x, int y );

#endif // PLOT_POINTS_H
