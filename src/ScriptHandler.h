#include "file_tab.h"

#ifdef ANTZ
#define MAPLE_PATH "/usr/share/maple11/bin/maple"
#define P4_BINDIR "/home/p4/p4/bin/"
#define TMP_DIR "/home/p4/tmp/"
#else
#define MAPLE_PATH "/home/osr/maple2015/bin/maple"
#define P4_BINDIR "/usr/local/p4/bin/"
#define TMP_DIR "/tmp/"
#endif

/**
 * Struct that stores all the Maple execution parameters for WP4
 */
struct mapleParamsStruct {
    std::string str_bindir;
    std::string str_p4m;
    std::string str_tmpdir;
    std::string str_lypexe;
    std::string str_sepexe;
    std::string str_exeprefix;
    std::string str_platform;
    std::string str_sumtablepath;
    std::string str_removecmd;
    std::string str_simplify;
    std::string str_simplifycmd;
    std::string str_critpoints;
    std::string str_saveall;
    std::string str_vectable;
    std::string str_fintab;
    std::string str_finres;
    std::string str_inftab;
    std::string str_infres;
    std::string str_xeq;
    std::string str_yeq;
    std::string str_userf;
    std::string str_gcf;
    std::string str_numeric;
    std::string str_epsilon;
    std::string str_testsep;
    std::string str_precision;
    std::string str_precision0;
    std::string str_taylor;
    std::string str_numericlevel;
    std::string str_maxlevel;
    std::string str_weaklevel;
    std::string str_userp;
    std::string str_userq;
    std::string time_limit;
};

/**
 * Generate a random name for a temp file
 *
 * @param prefix path to file plus some prefix to the name
 * @param suffix suffix after random part of name and extension
 */
std::string randomFileName(std::string prefix, std::string suffix);
/**
 * Prepare Maple script for evaluation
 * 
 * Opens file and fills it (through calling fillMapleScript())
 * @param fname name of file
 * @param prms  struct containing all the parameters for Maple evaluation
 */
bool prepareMapleFile(std::string fname, mapleParamsStruct& prms);
/**
 * Fill a Maple script with the parameters and commands for evaluation
 * 
 * Called from prepareMapleFile()
 * @param f     file stream to write to
 * @param prms  struct containing all the parameters of the script
 */
void fillMapleScript(std::ofstream& f, mapleParamsStruct prms);
/**
 * Evaluate a Maple script
 *
 * Forks a Maple process and waits for it to finish
 * @param fname filename of Maple script
 */
int evaluateMapleScript(std::string fname);
/**
 * Create a file that contains the execution parameters
 * 
 * This format is compatible with P4. If a user is not logged in, every parameter
 * will be saved as the default parameters.
 * @param prms struct that contains the current parameters of execution
 */
void fillSaveFile(mapleParamsStruct prms); //TODO: mirar inputs i outputs necessaris

//TODO copiar codi de file_vf.cpp i adaptar
/**
 * Prepare files in case of calculating GCF in plane/U1/U2 charts.
 * 
 * This is only called in case of Poincare-compactification (weights p=q=1)
 * @param fname     name of file where to write
 * @param f         terms of GCF
 * @param y1        y coord of point 1
 * @param y2        y coord of point 2
 * @param precision precision of integration
 * @param numpoints number of points to compute
 */
bool prepareGcf(std::string fname, P4POLYNOM2 f, double y1, double y2, int precision, int numpoints );
/**
 * Prepare files in case of calculating GCF in charts near infinity.
 * 
 * This is only called in case of Poincare-Lyapunov compactification (weights (p,q) !=(1,1))
 * @param fname     name of file where to write
 * @param f         terms of GCF
 * @param theta1    theta coord of point 1
 * @param theta2    theta coord of point 2
 * @param precision precision of integration
 * @param numpoints number of points to compute
 */
bool prepareGcf_LyapunovCyl(std::string fname, P4POLYNOM3 f, double theta1, double theta2, int precision, int numpoints );
/**
 * Prepare files in case of calculating GCF in charts near infinity.
 *
 * This is only called in case of Poincare-Lyapunov compactification (weights (p,q) !=(1,1)).
 * Same as preparegcf, except for the "u := " and "v := " assignments,
 * and the fact that one always refers to the same function VFResults.gcf,
 * and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
 * @param fname     file name of user Maple script
 * @param f         terms of GCF
 * @param precision precision of integration
 * @param numpoints number of points to compute
 */
bool prepareGcf_LyapunovR2( std::string fname, P4POLYNOM2 f, int precision, int numpoints );
