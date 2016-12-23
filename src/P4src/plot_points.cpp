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

#include "plot_points.h"

#include "custom.h"

using namespace Wt;

// -----------------------------------------------------------------------
//                          PLOT SINGULAR POINTS
// -----------------------------------------------------------------------
//
// This uses the pixel-coordinate system, i.e. any transformations should
// be done beforehand.

void win_plot_saddle( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSADDLE) );
    p->setBrush( QXFIGCOLOR(CSADDLE) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_stablenode( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_S) );
    p->setBrush( QXFIGCOLOR(CNODE_S) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_unstablenode( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_U) );
    p->setBrush( QXFIGCOLOR(CNODE_U) );
    p->drawRect( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, SYMBOLWIDTH, SYMBOLHEIGHT );
}

void win_plot_weakfocus( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS) );
    WPointF qpa[4];
    qpa[0].setX(x);
    qpa[0].setY(y-SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y);
    qpa[2].setX(x);
    qpa[2].setY(y+SYMBOLHEIGHT/2);
    qpa[3].setX(x-SYMBOLWIDTH/2);
    qpa[3].setY(y);

    p->drawPolygon( &qpa[0], 4 );
}

void win_plot_stableweakfocus( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS_S) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS_S) );
        WPointF qpa[4];
    qpa[0].setX(x);
    qpa[0].setY(y-SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y);
    qpa[2].setX(x);
    qpa[2].setY(y+SYMBOLHEIGHT/2);
    qpa[3].setX(x-SYMBOLWIDTH/2);
    qpa[3].setY(y);

    p->drawPolygon( &qpa[0], 4 );
}

void win_plot_unstableweakfocus( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CWEAK_FOCUS_U) );
    p->setBrush( QXFIGCOLOR(CWEAK_FOCUS_U) );
        WPointF qpa[4];
    qpa[0].setX(x);
    qpa[0].setY(y-SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y);
    qpa[2].setX(x);
    qpa[2].setY(y+SYMBOLHEIGHT/2);
    qpa[3].setX(x-SYMBOLWIDTH/2);
    qpa[3].setY(y);

    p->drawPolygon( &qpa[0], 4 );
}

void win_plot_center( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CCENTER) );
    p->setBrush( QXFIGCOLOR(CCENTER) );
        WPointF qpa[4];
    qpa[0].setX(x);
    qpa[0].setY(y-SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y);
    qpa[2].setX(x);
    qpa[2].setY(y+SYMBOLHEIGHT/2);
    qpa[3].setX(x-SYMBOLWIDTH/2);
    qpa[3].setY(y);

    p->drawPolygon( &qpa[0], 4 );
}

void win_plot_stablestrongfocus( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSTRONG_FOCUS_S) );
    p->setBrush( QXFIGCOLOR(CSTRONG_FOCUS_S) );
        WPointF qpa[4];
    qpa[0].setX(x);
    qpa[0].setY(y-SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y);
    qpa[2].setX(x);
    qpa[2].setY(y+SYMBOLHEIGHT/2);
    qpa[3].setX(x-SYMBOLWIDTH/2);
    qpa[3].setY(y);

    p->drawPolygon( &qpa[0], 4 );
}

void win_plot_unstablestrongfocus( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSTRONG_FOCUS_U) );
    p->setBrush( QXFIGCOLOR(CSTRONG_FOCUS_U) );
        WPointF qpa[4];
    qpa[0].setX(x);
    qpa[0].setY(y-SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y);
    qpa[2].setX(x);
    qpa[2].setY(y+SYMBOLHEIGHT/2);
    qpa[3].setX(x-SYMBOLWIDTH/2);
    qpa[3].setY(y);

    p->drawPolygon( &qpa[0], 4 );
}

void win_plot_sesaddlenode( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSADDLE_NODE) );
    p->setBrush( QXFIGCOLOR(CSADDLE_NODE) );
    WPointF qpa[3];
    qpa[0].setX(x-SYMBOLWIDTH/2);
    qpa[0].setY(y+SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y+SYMBOLHEIGHT/2);
    qpa[2].setX(x);
    qpa[2].setY(y-SYMBOLHEIGHT/2);

    p->drawPolygon( &qpa[0], 3 );
}

void win_plot_sestablenode( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_S) );
    p->setBrush( QXFIGCOLOR(CNODE_S) );
    WPointF qpa[3];
    qpa[0].setX(x-SYMBOLWIDTH/2);
    qpa[0].setY(y+SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y+SYMBOLHEIGHT/2);
    qpa[2].setX(x);
    qpa[2].setY(y-SYMBOLHEIGHT/2);

    p->drawPolygon( &qpa[0], 3 );
}

void win_plot_seunstablenode( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CNODE_U) );
    p->setBrush( QXFIGCOLOR(CNODE_U) );
    WPointF qpa[3];
    qpa[0].setX(x-SYMBOLWIDTH/2);
    qpa[0].setY(y+SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y+SYMBOLHEIGHT/2);
    qpa[2].setX(x);
    qpa[2].setY(y-SYMBOLHEIGHT/2);

    p->drawPolygon( &qpa[0], 3 );
}

void win_plot_sesaddle( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CSADDLE) );
    p->setBrush( QXFIGCOLOR(CSADDLE) );
    WPointF qpa[3];
    qpa[0].setX(x-SYMBOLWIDTH/2);
    qpa[0].setY(y+SYMBOLHEIGHT/2);
    qpa[1].setX(x+SYMBOLWIDTH/2);
    qpa[1].setY(y+SYMBOLHEIGHT/2);
    qpa[2].setX(x);
    qpa[2].setY(y-SYMBOLHEIGHT/2);

    p->drawPolygon( &qpa[0], 3 );
}

void win_plot_degen( WPainter * p, int x, int y )
{
    p->setPen( QXFIGCOLOR(CDEGEN) );
    p->drawLine( x-SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, x+SYMBOLWIDTH/2, y+SYMBOLHEIGHT/2 );
    p->drawLine( x+SYMBOLWIDTH/2, y-SYMBOLHEIGHT/2, x-SYMBOLWIDTH/2, y+SYMBOLHEIGHT/2 );
}
