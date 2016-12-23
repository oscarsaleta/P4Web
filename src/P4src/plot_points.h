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


#include <Wt/WPainter>


void win_plot_saddle( Wt::WPainter * p, int x, int y );
void win_plot_stablenode( Wt::WPainter * p, int x, int y );
void win_plot_unstablenode( Wt::WPainter * p, int x, int y );
void win_plot_weakfocus( Wt::WPainter * p, int x, int y );
void win_plot_stableweakfocus( Wt::WPainter * p, int x, int y );
void win_plot_unstableweakfocus( Wt::WPainter * p, int x, int y );
void win_plot_center( Wt::WPainter * p, int x, int y );
void win_plot_stablestrongfocus( Wt::WPainter * p, int x, int y );
void win_plot_unstablestrongfocus( Wt::WPainter * p, int x, int y );
void win_plot_sesaddlenode( Wt::WPainter * p, int x, int y );
void win_plot_sestablenode( Wt::WPainter * p, int x, int y );
void win_plot_seunstablenode( Wt::WPainter * p, int x, int y );
void win_plot_sesaddle( Wt::WPainter * p, int x, int y );
void win_plot_degen( Wt::WPainter * p, int x, int y );


#endif // PLOT_POINTS_H
