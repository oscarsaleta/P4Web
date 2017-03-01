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

#include "math_gcf.h"

#include "custom.h"
//#include "file_vf.h"
#include "math_p4.h"
//#include "math_charts.h"
#include "plot_tools.h"

#include <cmath>


// static global variables
static int GcfTask = EVAL_GCF_NONE;             // TODO fer membre de WWinSphere?
//static QWinSphere * GcfSphere = nullptr;
static int GcfDashes = 0;
static bool GcfError = false;

// non-static global variables
orbits_points * last_gcf_point = nullptr; // TODO: fer membre de WVFStudy?

// static functions
static void insert_gcf_point( double x0, double y0, double z0, int dashes );
static bool ReadTaskResults( int ); // , int, int, int );
static bool read_gcf( void (*chart)(double,double,double *) );

// function definitions
bool WWinSphere::evalGcfStart( int dashes, int points, int precis )
{
    if ( study_->gcf_points != nullptr ) {
        /* here we just paint in black over the previous gcf,
           we can simply reset the plot or something */
        //sp->prepareDrawing();
        //draw_gcf( sp, study_->gcf_points, CBACKGROUND, GcfDashes );
        //sp->finishDrawing();
        study_->deleteOrbitPoint( study_->gcf_points );
        study_->gcf_points = nullptr;
    }

    if ( study_->plweights )
        GcfTask = EVAL_GCF_LYP_R2;
    else
        GcfTask = EVAL_GCF_R2;

    GcfError = false;
    //GcfSphere = sp;
    GcfDashes = dashes;
    // TODO si això ho ha de fer homeright, haurem d'enviar una senyal
    return runTask( GcfTask, points, precis );
}

// returns true when finished.  Then run EvalGCfFinish to see if error occurred or not
bool WWinSphere::evalGcfContinue( int points, int prec )
{
    if ( GcfTask == EVAL_GCF_NONE )
        return true;

    if ( !ReadTaskResults( GcfTask ) ) {
        GcfError = true;
        return true;
    }
    GcfTask++;
    if ( GcfTask == EVAL_GCF_FINISHPOINCARE || GcfTask == EVAL_GCF_FINISHLYAPUNOV ) {
        return true;
    }
    
    if ( !runTask( GcfTask, points, prec ) ) {
        GcfError = true;
        return true;
    }

    return false; // still busy
}

bool WWinSphere::evalGcfFinish( void )      // return false in case an error occured
{
    if ( GcfTask != EVAL_GCF_NONE ) {
        // TODO: aqui vol pintar ja....
        /*GcfSphere->prepareDrawing();
        draw_gcf( GcfSphere, VFResults.gcf_points, CSING, 1 );
        GcfSphere->finishDrawing();*/

        GcfTask = EVAL_GCF_NONE;

        if ( GcfError ) {
            GcfError = false;
            return false;
        }
    }
    return true;
}

bool runTask( int task, int points, int prec )
{
    bool value;

    switch( task ) {
    case EVAL_GCF_R2:
    // TODO: either send signal to homeleft to prepare file or prepare it
    // in homeright or in sphere
    // la signal seria <P4POLYNOM,int,int,int,int>
        value = ThisVF->prepareGcf( /*FIXME: sphere_->*/study_->gcf, -1, 1, prec, points );
        break;
    case EVAL_GCF_U1:
        value = ThisVF->prepareGcf(study_->gcf_U1, 0, 1, prec, points);
        break;
    case EVAL_GCF_V1:
        value = ThisVF->prepareGcf(study_->gcf_U1, -1, 0, prec, points);
        break;
    case EVAL_GCF_U2:
        value = ThisVF->prepareGcf(study_->gcf_U2, 0, 1, prec, points);
        break;
    case EVAL_GCF_V2:
        value = ThisVF->prepareGcf(study_->gcf_U2, -1, 0, prec, points);
        break;
    case EVAL_GCF_LYP_R2:
        value = ThisVF->prepareGcf_LyapunovR2(prec, points);
        break;
    case EVAL_GCF_CYL1:
        value = ThisVF->prepareGcf_LyapunovCyl(-PI_DIV4,PI_DIV4, prec, points);
        break;
    case EVAL_GCF_CYL2:
        value = ThisVF->prepareGcf_LyapunovCyl( PI_DIV4, PI-PI_DIV4, prec, points);
        break;
    case EVAL_GCF_CYL3:
        value = ThisVF->prepareGcf_LyapunovCyl( PI-PI_DIV4, PI+PI_DIV4, prec, points);
        break;
    case EVAL_GCF_CYL4:
        value = ThisVF->prepareGcf_LyapunovCyl(-PI+PI_DIV4,-PI_DIV4, prec, points);
        break;
    default:
        value = false;
        break;
    }

    if ( value )
        return ThisVF->evaluateGcf();
    else
        return false;
}

// TODO: can this be static?
void rplane_plsphere0( double x, double y, double * pcoord )
{
    R2_to_plsphere(x*cos(y),x*sin(y),pcoord);
}

static bool ReadTaskResults( int task ) // , int points, int prec, int memory )
{
    bool value;

    switch( task )
    {
    case EVAL_GCF_R2:       value = read_gcf( R2_to_psphere ); break;
    case EVAL_GCF_U1:       value = read_gcf( U1_to_psphere ); break;
    case EVAL_GCF_V1:       value = read_gcf( VV1_to_psphere ); break;
    case EVAL_GCF_U2:       value = read_gcf( U2_to_psphere ); break;
    case EVAL_GCF_V2:       value = read_gcf( VV2_to_psphere ); break;
    case EVAL_GCF_LYP_R2:   value = read_gcf( rplane_plsphere0 ); break;
                            // here: the old herc files contained (the equivalent of)
                            // value = read_gcf( rplane_psphere );
    case EVAL_GCF_CYL1:     value = read_gcf( cylinder_to_plsphere ); break;
    case EVAL_GCF_CYL2:     value = read_gcf( cylinder_to_plsphere ); break;
    case EVAL_GCF_CYL3:     value = read_gcf( cylinder_to_plsphere ); break;
    case EVAL_GCF_CYL4:     value = read_gcf( cylinder_to_plsphere ); break;
    default:
        value = false;
    }
    
    return value;
}

void WWinSphere::draw_gcf( orbits_points * sep, int color, int dashes )
{
    double pcoord[3];

    while( sep != nullptr )
    {
        if (sep->dashes && dashes)
            (*plot_l)(this,pcoord,sep->pcoord,color);
        else
            (*plot_p)(this,sep->pcoord,color);
        copy_x_into_y(sep->pcoord,pcoord);

        sep=sep->next_point;
    }
} 

void WVFStudy::insert_gcf_point( double x0, double y0, double z0, int dashes )
{
    if ( gcf_points != nullptr  ) {
        last_gcf_point->next_point = new orbits_points;
        last_gcf_point = last_gcf_point->next_point; 
    } else {
        last_gcf_point = new orbits_points;
        gcf_points = last_gcf_point;
    }

    last_gcf_point->pcoord[0] = x0;
    last_gcf_point->pcoord[1] = y0;
    last_gcf_point->pcoord[2] = z0;
    
    last_gcf_point->dashes = dashes;
    last_gcf_point->color = CSING;
    last_gcf_point->next_point = nullptr;
}

static bool read_gcf( void (*chart)(double,double,double *) )
{
    int t;
    int k;
    FILE * fp;
    double x, y;
    double pcoord[3];
    int d,c;

    // TODO: change name of file, el reb la funció?
    fp = fopen( QFile::encodeName( ThisVF->getfilename_gcf() ), "r" );
    if ( fp == nullptr )
        return false;

    k=0;
    while( 1 )
    {
        d=0;
        while( fscanf( fp, "%lf %lf", &x, &y ) == 2 )
        {
            k++;
            chart(x,y,pcoord);
            insert_gcf_point( pcoord[0], pcoord[1], pcoord[2], d);
            //d=1;
            d=GcfDashes;
        }
        for( c = getc(fp); isspace(c); )
            c =getc(fp);
        if ( c != ',' )
            break;
    }
    
    fclose(fp);
    return true;
}
