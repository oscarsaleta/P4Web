//#include <qprocess.h>
#pragma once

#include <Wt/WObject>
#include <Wt/WString>

#include "custom.h"

class QTextEdit;
class QGcfDlg;
class QPushButton;
class QTextStream;

#define TYPEOFSTUDY_ALL     0
#define TYPEOFSTUDY_FIN     1
#define TYPEOFSTUDY_INF     2
#define TYPEOFSTUDY_ONE     3

#define PACKAGE_MAPLE       1
#define PACKAGE_REDUCE      0

class WInputVF : public WObject
{
public:
    WString filename;

    int symbolicpackage;        // 0 for reduce, 1 for maple
    int typeofstudy;            // 0, 1, 2, 3 = all, inf, fin, one

    bool numeric;
    int precision;
    int precision0;
    WString epsilon;
    bool testsep;
    int taylorlevel;
    int numericlevel;
    int maxlevel;
    int weakness;
    WString x0;
    WString y0;
    int p;
    int q;

    WString xdot;
    WString ydot;
    WString gcf;
    WString EvalFile;
    WString EvalFile2;
    
    QProcess * EvalProcess;
    QTextEdit * ProcessText;
    QPushButton * ProcessButton;
    QPushButton * ProcessClearButton;

    int numparams;
    WString parlabel[MAXNUMPARAMS];
    WString parvalue[MAXNUMPARAMS];

    bool changed;               // set when data needs to be saved
    bool evaluated;             // set when data has been evaluated
    bool evaluating;            // set while evaluating
    bool cleared;               // initial state, when records are clear
    bool evaluatinggcf;         // true when evaluation is of GCF kind
    bool processfailed;         // true when process failed;
    WString processError;       // only relevant when processfailed=true
    QGcfDlg * gcfDlg;

public:
    QInputVF();
    
    WString getfilename( void ) const;              // filename.inp
    WString getbarefilename( void ) const;          // filename
    WString getfilename_finresults( void ) const;   // filename_fin.res
    WString getfilename_infresults( void ) const;   // filename_inf.res
    WString getfilename_fintable( void ) const;     // filename_fin.tab
    WString getfilename_inftable( void ) const;     // filename_inf.tab
    WString getfilename_vectable( void ) const;     // filename_vec.tab
    WString getfilename_gcf( void ) const;          // filename_gcf.tab (temporary file)
    WString getfilename_gcfresults( void ) const;   // filename_gcf.res (temporary file, only used in case of reduce)
    WString getreducefilename( void ) const;        // filename.red
    WString getmaplefilename( void ) const;         // filename.mpl
    WString getrunfilename( void ) const;           // filename.run

    bool load( void );
    bool save( void );
    void reset( void );
    bool checkevaluated( void );

    static bool FileExists( WString );

    void PrepareReduceParameters( QTextStream * );
    void PrepareReduceVectorField( QTextStream * );
    void PrepareMapleParameters( QTextStream * );
    void PrepareMapleVectorField( QTextStream * );
    WString booleanString( int value ) const;
    WString ConvertMapleUserParameterLabels( WString );
    WString ConvertReduceUserParameterLabels( WString );

    void PrepareFile( QTextStream * ); // used by Prepare()

    void Prepare( void );
    void Evaluate( void );

    bool PrepareGcf( struct term2 * f, double, double, int, int );
    bool PrepareGcf_LyapunovCyl( double, double, int, int );
    bool PrepareGcf_LyapunovR2( int, int );
    bool EvaluateGcf( void );
    void CreateProcessWindow( void );

public slots:
    void finishEvaluation( int );
    void catchProcessError( QProcess::ProcessError );
    void ReadProcessStdout( void );
    void OnTerminateButton( void );
    void OnClearButton( void );
    void finishGcfEvaluation( void );
};

extern QInputVF * ThisVF;
