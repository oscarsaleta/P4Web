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

std::string randomFileName(std::string prefix, std::string suffix);
bool prepareMapleFile(std::string fname, mapleParamsStruct& prms);
void fillMapleScript(std::ofstream& f, mapleParamsStruct prms);
int evaluateMapleScript(std::string fname);
void fillSaveFile(mapleParamsStruct prms); //TODO: mirar inputs i outputs necessaris

//TODO copiar codi de file_vf.cpp i adaptar
bool prepareGcf( P4POLYNOM2 f, double, double, int, int );
bool prepareGcf_LyapunovCyl( double, double, int, int );
bool prepareGcf_LyapunovR2( int precision, int numpoints);
