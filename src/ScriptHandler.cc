#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "ScriptHandler.h"

#include "file_tab.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "MyLogger.h"

#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

std::string randomFileName(std::string prefix, std::string suffix)
{
    std::string fullname;
    prefix += "XXXXXX" + suffix;
    std::vector<char> dst_prefix(prefix.begin(), prefix.end());
    dst_prefix.push_back('\0');

    int fd = mkstemps(&dst_prefix[0],suffix.length());
    if (fd != -1) {
        prefix.assign(dst_prefix.begin(), dst_prefix.end()-1-suffix.length());
        fullname = prefix+suffix;
        close(fd);
    }

    globalLogger__.debug("ScriptHandler :: created temp file "+fullname);

    return prefix;
}

bool prepareMapleFile(std::string fname, mapleParamsStruct &prms)
{
    std::ofstream mplFile;

    if (fname.empty())
        fname = randomFileName(TMP_DIR,".mpl");

    mplFile.open((fname+".mpl").c_str(),std::fstream::trunc);
    
    // repair file permissions
    std::string command = "chmod 644 "+fname+".mpl";
    system(command.c_str());

    prms.str_vectable = fname+"_vec.tab";
    prms.str_fintab = fname+"_fin.tab";
    prms.str_finres = fname+"_fin.res";
    prms.str_inftab = fname+"_inf.tab";
    prms.str_infres = fname+"_inf.res";

    if (mplFile.is_open()) {
        fillMapleScript(mplFile,prms);
        mplFile.close();
        globalLogger__.debug("ScriptHandler :: prepared Maple file " + fname);
        return true;
    } else {
        globalLogger__.error("ScriptHandler :: cannot prepare Maple file");
        return false;
    }
}

void fillMapleScript(std::ofstream &f, mapleParamsStruct prms)
{
    f << "restart;" << std::endl;
    f << "read( \"" << prms.str_p4m << "\" );" << std::endl;
    f << "user_bindir := \"" << prms.str_bindir << "\":" << std::endl;
    f << "user_tmpdir := \"" << prms.str_tmpdir << "\":" << std::endl;
    f << "user_lypexe := \"" << prms.str_lypexe << "\":" << std::endl;
    f << "user_sepexe := \"" << prms.str_sepexe << "\":" << std::endl;
    f << "user_exeprefix := \"" << prms.str_exeprefix << "\":" << std::endl;
    f << "user_platform := \"" << prms.str_platform << "\":" << std::endl;
    f << "user_sumtablepath := \"" << prms.str_sumtablepath << "\":" << std::endl;
    f << "user_removecmd := \"" << prms.str_removecmd << "\":" << std::endl;
    f << "user_simplify := " << prms.str_simplify << ":" << std::endl;
    f << "user_simplifycmd := " << prms.str_simplifycmd << ":" << std::endl;
    f << "all_crit_points := " << prms.str_critpoints << ":" << std::endl;
    f << "save_all := " << prms.str_saveall << ":" << std::endl;
    f << "vec_table := \"" << prms.str_vectable << "\":" << std::endl;
    f << "finite_table := \"" << prms.str_fintab << "\":" << std::endl;
    f << "finite_res := \"" << prms.str_finres << "\":" << std::endl;
    f << "infinite_table := \"" << prms.str_inftab << "\":" << std::endl;
    f << "infinite_res := \"" << prms.str_infres << "\":" << std::endl;
    f << "if (type(parse(\"" << prms.str_xeq << "\"),polynom)) then" << std::endl;
    f << "  if (type(parse(\"" << prms.str_yeq << "\"),polynom)) then" << std::endl;
    f << "    user_f := " << prms.str_userf << ":" << std::endl;
    f << "  else `quit(1)` end if:" << std::endl;
    f << "else `quit(1)` end if:" << std::endl;
    f << "user_gcf := " << prms.str_gcf << ":" << std::endl;
    f << "user_numeric :=" << prms.str_numeric << ":" << std::endl;
    f << "epsilon := " << prms.str_epsilon << ":" << std::endl;
    f << "test_sep := " << prms.str_testsep << ":" << std::endl;
    f << "user_precision := " << prms.str_precision << ":" << std::endl;
    f << "user_precision0 := " << prms.str_precision0 << ":" << std::endl;
    f << "taylor_level := " << prms.str_taylor << ":" << std::endl;
    f << "numeric_level := " << prms.str_numericlevel << ":" << std::endl;
    f << "max_level := " << prms.str_maxlevel << ":" << std::endl;
    f << "weakness_level := " << prms.str_weaklevel << ":" << std::endl;
    f << "user_p := " << prms.str_userp << ":" << std::endl;
    f << "user_q := " << prms.str_userq << ":" << std::endl;
    f << "try timelimit("<<prms.time_limit<<",p4main()) catch:"  << std::endl
        << "printf( \"! Error (\%a) \%a\\n\", lastexception[1], lastexception[2] );\n"
        << "finally: closeallfiles();\n"
        << "if normalexit=0 then `quit`(0); else `quit(1)` end if: end try:\n";
    globalLogger__.debug("ScriptHandler :: filled Maple file");
}

int evaluateMapleScript (std::string fname)
{
    pid_t pid = fork();
    if (pid < 0) {
        globalLogger__.error("HomeLeft :: error forking Maple thread.");
        return pid;
    } else if (pid == 0) {
        // create vector of arguments for execvp
        std::vector<char *> commands;
#ifdef ANTZ
        commands.push_back("ssh");
        commands.push_back("p4@a01");
        commands.push_back("-i");
        commands.push_back("/var/www/claus_ssh/idrsa-1");
#endif
        commands.push_back(MAPLE_PATH);
        commands.push_back("-T ,1048576"); // 1GB memory limit
        char *aux = new char [fname.length()+1];
        std::strcpy(aux,fname.c_str());
        std::strcat(aux,".mpl");
        commands.push_back(aux);
        commands.push_back(nullptr);
        // output from this thread goes to "fname.res"
        int fd = open((fname+".res").c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);
        dup2(fd,1);
        // execute command
        char **command = commands.data();
        int status = execvp(command[0],command);
        return status; //NOTE needed?
    } else {
        int status;
        waitpid(pid,&status,0);
        globalLogger__.debug("ScriptHandler :: forked Maple execution finished");
        return status;
    }
}

bool fillSaveFile(std::string fname, mapleParamsStruct prms)
{
    std::ofstream fp(fname.c_str(),std::ios::trunc);

    if (fp.is_open()) {
        fp << 0                                                 << std::endl;   // typeofstudy
        fp << (prms.str_numeric == std::string("true") ? 1 : 0) << std::endl;   // numeric
        fp << prms.str_precision                                << std::endl;   // precision
        fp << prms.str_epsilon                                  << std::endl;   // epsilon
        fp << (prms.str_testsep == std::string("true") ? 1 : 0) << std::endl;   // test sep
        fp << prms.str_taylor                                   << std::endl;   // taylor level
        fp << prms.str_numericlevel                             << std::endl;   // numeric level
        fp << prms.str_maxlevel                                 << std::endl;   // max levels
        fp << prms.str_weaklevel                                << std::endl;   // weakness level
        fp << prms.str_userp                                    << std::endl;   // p
        fp << prms.str_userq                                    << std::endl;   // q
        fp << prms.str_xeq                                      << std::endl;   // x'
        fp << prms.str_yeq                                      << std::endl;   // y'
        fp << prms.str_gcf                                      << std::endl;   // gcf
        fp << 0                                                 << std::endl;   // numparams

        fp.close();
        return true;
    }
}

bool prepareGcf(std::string fname, P4POLYNOM2 f, double y1, double y2, int precision, int numpoints )
{
    int i;
    char buf[100];
    
    // open original maple script (will get overwritten?!)
    std::ofstream fp(std::string(fname+".mpl").c_str(), std::ios::trunc);
    if (fp.is_open()) {
        fp << "restart;" << std::endl;
        fp << "read( \"" << std::string(P4_BINDIR) << "p4gcf.m" << "\" ):" << std::endl;
        fp << "user_file := \"" << fname+"_gcf.tab" << "\":" << std::endl;
        fp << "user_numpoints := " << std::to_string(numpoints) << ":" << std::endl;
        fp << "user_x1 := -1.0:" << std::endl;
        fp << "user_x2 := 1.0:" << std::endl;
        fp << "user_y1 := " << std::to_string(y1) << ":" << std::endl;
        fp << "user_y2 := " << std::to_string(y2) << ":" << std::endl;
        fp << "u := x:" << std::endl;
        fp << "v := y:" << std::endl;
        fp << "user_f := ";
        for (i=0; f!=nullptr; i++) {
            fp <<  printterm2( buf, f, (i==0) ? true : false, "x", "y" );
            f = f->next_term2;
        }
        if (i==0)
            fp << "0:" << std::endl;
        else
            fp << ":" << std::endl;
        
        fp << "try FindSingularities() finally: if returnvalue=0 then `quit`(0); else `quit(1)` end if: end try:" << std::endl;

        globalLogger__.debug("ScriptHandler :: prepared GCF file "+fname);
        fp.close();
        return true;
    }
    globalLogger__.error("ScriptHandler :: cannot prepare GCF file");
    return false;   
}


bool prepareGcf_LyapunovCyl(std::string fname, P4POLYNOM3 f, double theta1, double theta2, int precision, int numpoints )
{
    char buf[100];
    int i;

    /*f = VFResults.gcf_C;*/

    std::ofstream fp(std::string(fname+".mpl").c_str(), std::ios::trunc);

    if (fp.is_open()) {
        fp << "restart;" << std::endl;
        fp << "read(\"" << std::string(P4_BINDIR) << "p4gcf.m" << "\" ):" << std::endl;
        fp << "user_file := \"" << fname+"_gcf.tab" << "\":" << std::endl;
        fp << "user_numpoints := " << std::to_string(numpoints) << ":" << std::endl;
        fp << "user_x1 := 0.0:" << std::endl;
        fp << "user_x2 := 1.0:" << std::endl;
        fp << "user_y1 := " << std::to_string(theta1) << ":" << std::endl;
        fp << "user_y2 := " << std::to_string(theta2) << ":" << std::endl;
        fp << "u := cos(y):" << std::endl;
        fp << "v := sin(y):" << std::endl;
        fp << "user_f := ";
        for (i=0; f!=nullptr; i++) {
            fp <<  printterm3( buf, f, (i==0) ? true : false, "x", "U", "V" );
            f = f->next_term3;
        }
        if (i==0)
            fp << "0:" << std::endl;
        else
            fp << ":" << std::endl;
        
        fp << "try FindSingularities() finally: if returnvalue=0 then `quit`(0); else `quit(1)` end if: end try:" << std::endl;
        
        globalLogger__.debug("ScriptHandler :: prepared GCF_LyapunovCyl file "+fname);
        fp.close();
        return true;
    }
    globalLogger__.error("ScriptHandler :: cannot prepare GCF_LyapunovCyl file");
    return false; 
}


bool prepareGcf_LyapunovR2( std::string fname, P4POLYNOM2 f, int precision, int numpoints )
{
    char buf[100];
    int i;

    //f = VFResults.gcf;

    std::ofstream fp(std::string(fname+".mpl").c_str(),std::ios::trunc);

    if (fp.is_open()) {
        fp << "restart;" << std::endl;
        fp << "read( \"" + std::string(P4_BINDIR) << "p4gcf.m" << "\" ):" << std::endl;
        fp << "user_file := \"" + fname+"_gcf.tab" << "\":" << std::endl;
        fp << "user_numpoints := " + std::to_string(numpoints) << ":" << std::endl;
        fp << "user_x1 := 0.0:" << std::endl;
        fp << "user_x2 := 1.0:" << std::endl;
        fp << "user_y1 := 0.0:" << std::endl;
        fp << "user_y2 := " << std::to_string(TWOPI) << ":" << std::endl;
        fp << "u := x*cos(y):" << std::endl;
        fp << "v := y*sin(y):" << std::endl;
        fp << "user_f := ";

        for( i = 0; f != nullptr; i++ ) {
            fp << printterm2( buf, f, (i==0) ? true : false, "U", "V" );
            f = f->next_term2;
        }
        if( i == 0 )
            fp << "0:" << std::endl;
        else
            fp <<":" << std::endl;
    
        fp << "try FindSingularities() finally: if returnvalue=0 then `quit`(0); else `quit(1)` end if: end try:" << std::endl;

        globalLogger__.debug("ScriptHandler :: prepared GCF_LyapunovR2 file "+fname);
        fp.close();
        return true;
    }
    globalLogger__.error("ScriptHandler :: cannot prepare GCF_LyapunovR2 file");    
    return false;
}
