#include "math_p4.h"

#include "file_tab.h"
#include "math_charts.h"
/*#include "win_intparams.h"
#include "win_main.h"
#include "win_plot.h"*/

#include <cmath>


bool less_poincare( double * p1, double * p2 )
{
    if((p1[0]*p2[2])<(p2[0]*p1[2]))
        return 1;
    if(((p1[0]*p2[2])==(p2[0]*p1[2])) && ((p1[1]*p2[2])<(p2[1]*p1[2])))
        return  true ;
    return  false ;
}

double eval_lc_poincare( double * pp,double a, double b, double c )
{
    return (a*pp[0]+b*pp[1]+c*pp[2]);
}

double eval_lc_lyapunov( double * pp,double a, double b, double c )
{
    if( pp[0] )
        return (  a * pow(pp[1],VFResults.double_q)*cos(pp[2])
                + b * pow(pp[1],VFResults.double_p)*sin(pp[2])
                + c * pow(pp[1],VFResults.double_p_plus_q) );
    else
        return ( a * pp[1] + b * pp[2] + c );
}
 
bool less_lyapunov(double * p1, double * p2 )
{
    double u[2],v[2];

    plsphere_annulus(p1[0],p1[1],p1[2],u);
    plsphere_annulus(p2[0],p2[1],p2[2],v);
    if(u[0]<v[0])
        return 1;
    if((u[0]==v[0]) && (u[1]<v[1]))
        return 1;
    return 0;
}  

/*void set_current_step( double curstep )
{
    VFResults.config_currentstep = curstep;
    
    if( p4startdlg != nullptr )
        if( p4startdlg->Plot_Window != nullptr )
            if( p4startdlg->Plot_Window->IntParams_Window != nullptr )
                p4startdlg->Plot_Window->IntParams_Window->SetCurrentStep( curstep );

}*/
