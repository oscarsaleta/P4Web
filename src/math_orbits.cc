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

#include "math_orbits.h"

#include "custom.h"
#include "math_p4.h"
//#include "math_charts.h"
//#include "math_numerics.h"
#include "math_polynom.h"
#include "plot_tools.h"
#include "win_sphere.h"

#include <cmath>


// -----------------------------------------------------------------------
//                      INTEGRATEORBIT
// -----------------------------------------------------------------------
//
// dir = -1: backwards, dir=0: continue, dir=+1: forwards

void integrateOrbit( WWinSphere * sphere, int dir )
{
    struct orbits_points *sep;
    double pcoord[3],ucoord[2];

    if ( dir == 0 ) {
        // continue orbit button has been pressed
     
        dir = sphere->study_->current_orbit->current_f_orbits->dir;

        copy_x_into_y( sphere->study_->current_orbit->current_f_orbits->pcoord, pcoord );
        sphere->study_->current_orbit->current_f_orbits->next_point =
                    integrate_orbit( sphere, pcoord,sphere->study_->config_currentstep,dir,CORBIT, sphere->study_->config_intpoints,&sep);
        
        sphere->study_->current_orbit->current_f_orbits = sep;
        return;
    }

    copy_x_into_y(sphere->study_->current_orbit->pcoord,pcoord);
    ((sphere->study_)->*(sphere->study_->sphere_to_R2))(pcoord[0],pcoord[1],pcoord[2],ucoord);
    if ( sphere->study_->config_kindvf == INTCONFIG_ORIGINAL )
        if (eval_term2(sphere->study_->gcf,ucoord) < 0)
            dir = -dir;

    if ( sphere->study_->current_orbit->f_orbits == nullptr ) {
        sphere->study_->current_orbit->f_orbits =
                    integrate_orbit(sphere,pcoord,sphere->study_->config_step,dir,CORBIT,sphere->study_->config_intpoints,&sep);
    } else {
        sphere->study_->current_orbit->current_f_orbits->next_point = new orbits_points;
        //(struct orbits_points *) malloc(sizeof(struct orbits_points));
        sphere->study_->current_orbit->current_f_orbits = sphere->study_->current_orbit->current_f_orbits->next_point;
        copy_x_into_y(pcoord,sphere->study_->current_orbit->current_f_orbits->pcoord);
        sphere->study_->current_orbit->current_f_orbits->dashes = 0;
        sphere->study_->current_orbit->current_f_orbits->color = CORBIT;
        sphere->study_->current_orbit->current_f_orbits->dir = dir;
        sphere->study_->current_orbit->current_f_orbits->next_point = 
                    integrate_orbit(sphere,pcoord,sphere->study_->config_step,dir,CORBIT,sphere->study_->config_intpoints,&sep);  
    }
    sphere->study_->current_orbit->current_f_orbits=sep;
}

//// -----------------------------------------------------------------------
////                      STARTORBIT
//// -----------------------------------------------------------------------
///* R=0 then point selected in the drawing canvas else in the orbit window */

bool startOrbit( WWinSphere * sphere, double x, double y, bool R )
{
    double pcoord[3];
    double ucoord[2];

    if ( sphere->study_->first_orbit == nullptr ) {
        sphere->study_->first_orbit = new orbits;//(struct orbits *)malloc( sizeof(struct orbits) );
        sphere->study_->current_orbit = sphere->study_->first_orbit;
    } else {
        sphere->study_->current_orbit->next_orbit = new orbits;//(struct orbits *)malloc( sizeof(struct orbits) );
        sphere->study_->current_orbit = sphere->study_->current_orbit->next_orbit;
    }
    if ( R )
        ((sphere->study_)->*(sphere->study_->R2_to_sphere))(x,y,pcoord);
    else
        ((sphere->study_)->*(sphere->study_->viewcoord_to_sphere))(x,y,pcoord); 

    copy_x_into_y( pcoord, sphere->study_->current_orbit->pcoord );
    sphere->study_->current_orbit->color = CORBIT;
    sphere->study_->current_orbit->f_orbits = nullptr;
    sphere->study_->current_orbit->next_orbit = nullptr;

    ((sphere->study_)->*(sphere->study_->sphere_to_viewcoord))( pcoord[0], pcoord[1], pcoord[2], ucoord );
    sphere->drawPoint( ucoord[0], ucoord[1], CORBIT );

    return true;
}

//// -----------------------------------------------------------------------
////                      DRAWORBIT
//// -----------------------------------------------------------------------

void drawOrbit( WWinSphere * sphere, double * pcoord, struct orbits_points * points, int color )
{
    double pcoord1[3];

    copy_x_into_y( pcoord, pcoord1 );
    (*plot_p)( sphere, pcoord, color );
    
    while ( points!=nullptr ) {
        if (points->dashes) {
            (*plot_l)(sphere, pcoord1,points->pcoord,color);
        } else {
            (*plot_p)(sphere, points->pcoord,color);
        }

        copy_x_into_y(points->pcoord,pcoord1);

        points = points->next_point;
    }
}

//// -----------------------------------------------------------------------
////                      DRAWORBITS
//// -----------------------------------------------------------------------

void drawOrbits( WWinSphere * sphere )
{
    struct orbits * orbit;

    for ( orbit = sphere->study_->first_orbit; orbit != nullptr; orbit = orbit->next_orbit ) {
        drawOrbit( sphere, orbit->pcoord, orbit->f_orbits, orbit->color );
    }
}

//// -----------------------------------------------------------------------
////                      DELETELASTORBIT
//// -----------------------------------------------------------------------

void deleteLastOrbit( WWinSphere * sphere )
{
    struct orbits *orbit1,*orbit2;

    if ( sphere->study_->current_orbit == nullptr )
        return;

    orbit2 = sphere->study_->current_orbit;
    drawOrbit( sphere, orbit2->pcoord, orbit2->f_orbits, sphere->spherebgcolor );

    if ( sphere->study_->first_orbit == sphere->study_->current_orbit ) {
        sphere->study_->first_orbit = nullptr;
        sphere->study_->current_orbit = nullptr;
    } else {
        orbit1 = sphere->study_->first_orbit;
    
        do {
            sphere->study_->current_orbit = orbit1;
            orbit1 = orbit1->next_orbit;
        } while( orbit1 != orbit2 );
        
        sphere->study_->current_orbit->next_orbit = nullptr;
    }
    sphere->study_->deleteOrbitPoint( orbit2->f_orbits );
    delete orbit2;
    orbit2 = nullptr;
}


/*integrate poincare sphere case p=q=1 */
void WVFStudy::integrate_poincare_orbit( double p0, double p1, double p2, double * pcoord,
                              double * hhi, int * dashes, int * dir, double h_min, double h_max)
{
    double y[2],theta;

    *dashes=true; *dir=1;
    if (pcoord[2]>ZCOORD) {
        psphere_to_R2(p0,p1,p2,y);
        rk78(&WVFStudy::eval_r_vec_field,y,hhi,h_min,h_max,config_tolerance);
        R2_to_psphere(y[0],y[1],pcoord);
    } else {
        theta=atan2(fabs(p1),fabs(p0));
        if ((theta<PI_DIV4) && (theta>-PI_DIV4)) {
            if (p0>0) {
                psphere_to_U1(p0,p1,p2,y);
                rk78(&WVFStudy::eval_U1_vec_field,y,hhi,h_min,h_max,config_tolerance);
                    if (y[1]>=0 || !singinf)
                        U1_to_psphere(y[0],y[1],pcoord);
                    else {
                        VV1_to_psphere(y[0],y[1],pcoord);
                        if (dir_vec_field==1) {
                            *dir=-1; *hhi=-(*hhi);
                        }
                        psphere_to_V1(pcoord[0],pcoord[1],pcoord[2],y);
                        *dashes=false;
                    }
            } else {
                psphere_to_V1(p0,p1,p2,y);
                rk78(&WVFStudy::eval_V1_vec_field,y,hhi,h_min,h_max,config_tolerance);
                if (y[1]>=0 || !singinf)
                    V1_to_psphere(y[0],y[1],pcoord);
                else {
                    UU1_to_psphere(y[0],y[1],pcoord);
                    if (dir_vec_field==1) {
                        *dir=-1; *hhi=-(*hhi);
                    }
                    psphere_to_U1(pcoord[0],pcoord[1],pcoord[2],y);
                    *dashes=false;
                }
            }
        } else {
            if (p1>0) {
                psphere_to_U2(p0,p1,p2,y);
                rk78(&WVFStudy::eval_U2_vec_field,y,hhi,h_min,h_max,config_tolerance);
                if (y[1]>=0 || !singinf)
                    U2_to_psphere(y[0],y[1],pcoord);
                else {
                    VV2_to_psphere(y[0],y[1],pcoord);
                    if (dir_vec_field==1) {
                        *dir=-1;*hhi=-(*hhi);
                    }
                    psphere_to_V2(pcoord[0],pcoord[1],pcoord[2],y);
                    *dashes=false;
                }
            } else {
                psphere_to_V2(p0,p1,p2,y);
                rk78(&WVFStudy::eval_V2_vec_field,y,hhi,h_min,h_max,config_tolerance);
                if (y[1]>=0 || !singinf)
                    V2_to_psphere(y[0],y[1],pcoord);
                else {
                    UU2_to_psphere(y[0],y[1],pcoord);
                    if (dir_vec_field==1) {
                        *dir=-1;*hhi=-(*hhi);
                    }
                    psphere_to_U2(pcoord[0],pcoord[1],pcoord[2],y);
                    *dashes=false;
                }
            }
        }
    }
}

/* integrate on the Poincare-Lyapunov sphere */
void WVFStudy::integrate_lyapunov_orbit( double p0, double p1, double p2, double * pcoord,
                              double * hhi, int * dashes, int * dir, double h_min, double h_max)
{
    double y[2];

    *dashes=true;
    *dir=1;
    if (p0==0) {
        y[0]=p1;y[1]=p2;
        rk78(&WVFStudy::eval_r_vec_field,y,hhi,h_min,h_max,config_tolerance);
        R2_to_plsphere(y[0],y[1],pcoord);
    } else {
        y[0]=p1; y[1]=p2;
        rk78(&WVFStudy::eval_vec_field_cyl,y,hhi,h_min,h_max,config_tolerance); 
        if (y[1]>=TWOPI)y[1]-=TWOPI;
        cylinder_to_plsphere(y[0],y[1],pcoord);
    } 
}

struct orbits_points * integrate_orbit(WWinSphere * sphere, double pcoord[3],double step,int dir,int color,
    int points_to_int,struct orbits_points **orbit)
{
    int i,d,h;
    int dashes;
    double hhi;
    double pcoord2[3],h_min,h_max;
    struct orbits_points *first_orbit=nullptr,*last_orbit=nullptr;

    hhi=(double)dir*step;
    h_min=sphere->study_->config_hmi;
    h_max=sphere->study_->config_hma;
    copy_x_into_y(pcoord,pcoord2);
    for (i=1;i<=points_to_int;++i) {
        ((sphere->study_)->*(sphere->study_->integrate_sphere_orbit))
            (pcoord[0],pcoord[1],pcoord[2],pcoord,&hhi,&dashes,&d,h_min,h_max);     
         
        if ( (i % UPDATEFREQ_STEPSIZE) == 0 )
            sphere->study_->set_current_step(fabs(hhi));

        if (last_orbit==nullptr) {
            first_orbit = new orbits_points;
            last_orbit = first_orbit;
            h=dir;
        } else {
            last_orbit->next_point = new orbits_points;
            h=last_orbit->dir;                          
            last_orbit=last_orbit->next_point; 
        }
  
        copy_x_into_y(pcoord,last_orbit->pcoord);
        last_orbit->color=color;
        last_orbit->dashes=dashes * sphere->study_->config_dashes;
        last_orbit->dir=d*h;
        last_orbit->next_point=nullptr;
        if (dashes * sphere->study_->config_dashes)
            (*plot_l)(sphere,pcoord,pcoord2,color);
        else
            (*plot_p)(sphere,pcoord,color);
        copy_x_into_y(pcoord,pcoord2); 
    }
    sphere->study_->set_current_step(fabs(hhi));
    *orbit=last_orbit;
    return(first_orbit);   
}
