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

#ifndef COLOR_H
#define COLOR_H

/*!
 * @brief Definitions for colors
 * @file color.h
 */

#include <Wt/WColor>


//
// XFIG 3.1 DEFAULT COLOR TABLE
//

#define BLACK           0       ///< pure black
#define BLUE            1       ///< pure blue
#define GREEN           2       ///< pure green
#define CYAN            3       ///< pure cyan
#define RED             4       ///< pure red
#define MAGENTA         5       ///< pure magenta
#define YELLOW          6       ///< pure yellow
#define WHITE           7       ///< pure white
#define BLUE1           8       ///< shades of blue (darkest)
#define BLUE2           9       ///< shades of blue (dark)
#define BLUE3           10      ///< shades of blue (light)
#define BLUE4           11      ///< shades of blue (lightest)
#define GREEN1          12      ///< shades of green (dark)
#define GREEN2          13      ///< shades of green (normal)
#define GREEN3          14      ///< shades of green (light)
#define CYAN1           15      ///< shades of cyan (dark)
#define CYAN2           16      ///< shades of cyan (normal)
#define CYAN3           17      ///< shades of cyan (light)
#define RED1            18      ///< shades of red (dark)
#define RED2            19      ///< shades of red (normal)
#define RED3            20      ///< shades of red (light)
#define MAGENTA1        21      ///< shades of magenta (dark)
#define MAGENTA2        22      ///< shades of magenta (normal)
#define MAGENTA3        23      ///< shades of magenta (light)
#define BROWN1          24      ///< shades of brown (dark)
#define BROWN2          25      ///< shades of brown (normal)
#define BROWN3          26      ///< shades of brown (light)
#define PINK1           27      ///< shades of pink (darkest)
#define PINK2           28      ///< shades of pink (dark)
#define PINK3           29      ///< shades of pink (light)
#define PINK4           30      ///< shades of pink (lightest)
#define GOLD            31      ///< gold


#define NUMXFIGCOLORS   32              ///< number of colors
#define DARKGRAY        NUMXFIGCOLORS   ///< dark gray

/**
 * Macro for defining a color
 * @param  b int (color code as defined in this file)
 * @return   object of class Wt::WColor with rgb as given by XFigToRGB
 */
#define QXFIGCOLOR(x)   Wt::WColor( XFigToRGB[x].r, XFigToRGB[x].g, XFigToRGB[x].b )

/**
 * Struct that holds the RGB value of a color
 */
struct P4RGBITEM
{
    int r;  ///< red
    int g;  ///< green
    int b;  ///< blue
};

/**
 * Global struct array (one for each color defined in this file)
 */
extern P4RGBITEM XFigToRGB[NUMXFIGCOLORS];


#endif /* COLOR_H */
