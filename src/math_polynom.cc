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

// -----------------------------------------------------------------------
//
//                      ROUTINES TO HANDLE POLYNOMIALS
//
// -----------------------------------------------------------------------


#include "math_polynom.h"

#include "file_tab.h"

#include <cmath>

using namespace Wt;


// -----------------------------------------------------------------------
//                              EVAL_TERM1
// -----------------------------------------------------------------------
//
// Calculates p(t) for a polynomial p and a value t.

double eval_term1( P4POLYNOM1 p, double t )
{
    double s;

    s = 0;
    while( p != nullptr )
    {
        if( p->exp != 0 )
            s += (p->coeff) * pow(t, (double)(p->exp) );
        else
            s += p->coeff;

        p = p->next_term1;
    }

    return s;
}

// -----------------------------------------------------------------------
//                              EVAL_TERM2
// -----------------------------------------------------------------------
//
// Calculates f(x,y) for a polynomial f and values x and y.
//
// value refers to an array containing x and y: value[0]=x, value[1]=y

double eval_term2( P4POLYNOM2 f, double * value )
{
    double s;

    s = 0;
    while( f != nullptr )
    {
        if( f->exp_x != 0 && f->exp_y != 0 )
            s += f->coeff * pow( value[0], (double)(f->exp_x) )
                          * pow( value[1], (double)(f->exp_y) );
        else if( f->exp_x != 0 )
            s += f->coeff * pow( value[0], (double)(f->exp_x) );
        else if( f->exp_y != 0 )
            s += f->coeff * pow( value[1], (double)(f->exp_y) );
        else
            s += f->coeff;

        f = f->next_term2;
    }

    return s;
}

// -----------------------------------------------------------------------
//                              EVAL_TERM3
// -----------------------------------------------------------------------
//
// Calculates F( r, cos(theta), sin(theta) ) for a polynomial f and values
// of r and theta.
//
// value refers to an array containing r and theta: value[0]=r, value[1]=theta

double eval_term3( P4POLYNOM3 F, double * value )
{ 
    double s;
    double t;
    double Co, Si;

    s = 0;
    Co = cos( value[1] );
    Si = sin( value[1] );

    while( F != nullptr )
    {
        if( F->exp_r != 0 )
            t = pow( value[0], (double)(F->exp_r) );
        else
            t = 1.0;

        if( F->exp_Co )
            t *= pow( Co, (double)(F->exp_Co) );

        if( F->exp_Si )
            t *= pow( Si, (double)(F->exp_Si) );

        s += t * F->coeff;
        F = F->next_term3;
    }

    return s;
}

// -----------------------------------------------------------------------
//                              DELETE_TERM1
// -----------------------------------------------------------------------

void delete_term1( P4POLYNOM1 p )
{
    P4POLYNOM1 q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_term1;
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                              DELETE_TERM2
// -----------------------------------------------------------------------

void delete_term2( P4POLYNOM2 p )
{
    P4POLYNOM2 q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_term2;
        delete q;
        q = nullptr;
    }
}

// -----------------------------------------------------------------------
//                              DELETE_TERM3
// -----------------------------------------------------------------------

void delete_term3( P4POLYNOM3 p )
{
    P4POLYNOM3 q;

    while( p != nullptr )
    {
        q = p;
        p = p->next_term3;
        delete q;
        q = nullptr;
    }
}

