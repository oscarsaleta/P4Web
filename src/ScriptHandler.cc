#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "ScriptHandler.h"

#include "MyLogger.h"
#include "file_tab.h"
#include "math_p4.h"
#include "math_polynom.h"

#include <cstdlib>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

std::string randomFileName(std::string prefix, std::string suffix)
{
    std::string fullname;
    prefix += "XXXXXX" + suffix;
    std::vector<char> dst_prefix(prefix.begin(), prefix.end());
    dst_prefix.push_back('\0');

    int fd = mkstemps(&dst_prefix[0], suffix.length());
    if (fd != -1) {
        prefix.assign(dst_prefix.begin(),
                      dst_prefix.end() - 1 - suffix.length());
        fullname = prefix + suffix;
        close(fd);
    }

    globalLogger__.debug("ScriptHandler :: created temp file " + fullname);

    return prefix;
}

bool prepareMapleFile(std::string &fname, mapleParamsStruct &prms)
{
    globalLogger__.debug("ScriptHandler :: received order to prepare script " +
                         fname);
    FILE *mplFile;

    if (fname.empty())
        fname = randomFileName(TMP_DIR, ".mpl");

    mplFile = fopen((fname + ".mpl").c_str(), "w");

    // repair file permissions
    std::string command = "chmod 644 " + fname + ".mpl";
    system(command.c_str());

    prms.str_vectable = fname + "_vec.tab";
    prms.str_fintab = fname + "_fin.tab";
    prms.str_finres = fname + "_fin.res";
    prms.str_inftab = fname + "_inf.tab";
    prms.str_infres = fname + "_inf.res";

    if (mplFile != nullptr) {
        fillMapleScript(mplFile, prms);
        fclose(mplFile);
        globalLogger__.debug("ScriptHandler :: prepared Maple file " + fname);
        return true;
    } else {
        globalLogger__.error("ScriptHandler :: cannot prepare Maple file");
        return false;
    }
}

void fillMapleScript(FILE *f, mapleParamsStruct prms)
{
    fprintf(f, "restart;\n");
    fprintf(f, "read( \"%s\" ):\n", prms.str_p4m.c_str());
    fprintf(f, "user_bindir := \"%s\":\n", prms.str_bindir.c_str());
    fprintf(f, "user_tmpdir := \"%s\":\n", prms.str_tmpdir.c_str());
    fprintf(f, "user_lypexe := \"%s\":\n", prms.str_lypexe.c_str());
    fprintf(f, "user_lypexe_mpf := \"%s\":\n", prms.str_lypexe_mpf.c_str());
    fprintf(f, "user_sepexe := \"%s\":\n", prms.str_sepexe.c_str());
    fprintf(f, "user_exeprefix := \"%s\":\n", prms.str_exeprefix.c_str());
    fprintf(f, "user_platform := \"%s\":\n", prms.str_platform.c_str());
    fprintf(f, "user_sumtablepath := \"%s\":\n", prms.str_sumtablepath.c_str());
    fprintf(f, "user_removecmd := \"%s\":\n", prms.str_removecmd.c_str());
    fprintf(f, "user_simplify := %s:\n", prms.str_simplify.c_str());
    fprintf(f, "user_simplifycmd := %s:\n", prms.str_simplifycmd.c_str());
    fprintf(f, "all_crit_points := %s:\n", prms.str_critpoints.c_str());
    fprintf(f, "save_all := %s:\n", prms.str_saveall.c_str());
    fprintf(f, "vec_table := \"%s\":\n", prms.str_vectable.c_str());
    fprintf(f, "finite_table := \"%s\":\n", prms.str_fintab.c_str());
    fprintf(f, "finite_res := \"%s\":\n", prms.str_finres.c_str());
    fprintf(f, "infinite_table := \"%s\":\n", prms.str_inftab.c_str());
    fprintf(f, "infinite_res := \"%s\":\n", prms.str_infres.c_str());
    fprintf(f, "if (type(parse(\"%s\"),polynom)) then\n", prms.str_xeq.c_str());
    fprintf(f, "  if (type(parse(\"%s\"),polynom)) then\n",
            prms.str_yeq.c_str());
    fprintf(f, "    user_f := %s:\n", prms.str_userf.c_str());
    fprintf(f, "  else `quit(1)` end if:\n");
    fprintf(f, "else `quit(1)` end if:\n");
    fprintf(f, "if (type(parse(\"%s\"),polynom)) then\n", prms.str_gcf.c_str());
    fprintf(f, "  user_gcf := %s:\n", prms.str_gcf.c_str());
    fprintf(f, "else `quit(1)` end if:\n");
    fprintf(f, "user_numeric := %s:\n", prms.str_numeric.c_str());
    fprintf(f, "epsilon := %s:\n", prms.str_epsilon.c_str());
    fprintf(f, "test_sep := %s:\n", prms.str_testsep.c_str());
    fprintf(f, "user_precision := %s:\n", prms.str_precision.c_str());
    fprintf(f, "user_precision0 := %s:\n", prms.str_precision0.c_str());
    fprintf(f, "taylor_level := %s:\n", prms.str_taylor.c_str());
    fprintf(f, "numeric_level := %s:\n", prms.str_numericlevel.c_str());
    fprintf(f, "max_level := %s:\n", prms.str_maxlevel.c_str());
    fprintf(f, "weakness_level := %s:\n", prms.str_weaklevel.c_str());
    fprintf(f, "user_p := %s:\n", prms.str_userp.c_str());
    fprintf(f, "user_q := %s:\n", prms.str_userq.c_str());
    fprintf(f, "try timelimit(%s,p4main()) catch:\n", prms.time_limit.c_str());
    fprintf(f, "printf( \"! Error (%%a) %%a\\n\", lastexception[1], "
               "lastexception[2] );\n");
    fprintf(f, "finally: closeallfiles();\n");
    fprintf(
        f, "if normalexit=0 then `quit`(0); else `quit(1)` end if: end try:\n");

    globalLogger__.debug("ScriptHandler :: filled Maple file");
}

siginfo_t evaluateMapleScript(std::string fname, int maxtime)
{
    globalLogger__.debug(
        "ScriptHandler :: Will fork Maple process for script " + fname);
    pid_t pid = fork();
    if (pid < 0) {
        globalLogger__.error("HomeLeft :: error forking Maple thread.");
        siginfo_t pinfo;
        pinfo.si_pid = -1;
        pinfo.si_code = -1;
        return pinfo;
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
        char *aux = new char[fname.length() + 1];
        std::strcpy(aux, fname.c_str());
        std::strcat(aux, ".mpl");
        commands.push_back(aux);
        commands.push_back(nullptr);
        // output from this thread goes to "fname.res"
        int fd =
            open((fname + ".res").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        dup2(fd, 1);
        // execute command
        char **command = commands.data();
        int status = execvp(command[0], command);
        // return status; // NOTE needed?
    } else {
        siginfo_t infop;
        infop.si_pid = 0;
        for (int tries=0; tries<maxtime; tries++) {
            waitid(P_PID, pid, &infop, WEXITED | WSTOPPED | WNOHANG);
            if (infop.si_pid != 0) {
                globalLogger__.debug("ScriptHandler :: forked Maple execution finished");
                return infop;
            }
            delay(1000);
        }
        char *aux = new char[128];
        strcat(aux,"kill ");
        strcat(aux,std::to_string(pid).c_str());
        strcat(aux,"\0");
        globalLogger__.error("ScriptHandler :: "+ std::string(aux));
        system(aux);
        globalLogger__.error("ScriptHandler :: Maple execution took too much time");
        infop.si_status = -2;
        infop.si_code = -2;
        return infop;
    }
}

bool fillSaveFile(std::string fname, mapleParamsStruct prms)
{
    FILE *fp = fopen(fname.c_str(), "w");

    if (fp != nullptr) {
        fprintf(fp, "0\n"); // typeofstudy
        fprintf(
            fp, "%s\n",
            (prms.str_numeric == std::string("true") ? "1" : "0")); // numeric
        fprintf(fp, "%s\n", prms.str_precision.c_str());            // precision
        fprintf(fp, "%s\n", prms.str_epsilon.c_str());              // epsilon
        fprintf(
            fp, "%s\n",
            (prms.str_testsep == std::string("true") ? "1" : "0")); // test sep
        fprintf(fp, "%s\n", prms.str_taylor.c_str());       // taylor level
        fprintf(fp, "%s\n", prms.str_numericlevel.c_str()); // numeric level
        fprintf(fp, "%s\n", prms.str_maxlevel.c_str());     // max levels
        fprintf(fp, "%s\n", prms.str_weaklevel.c_str());    // weakness level
        fprintf(fp, "%s\n", prms.str_userp.c_str());        // p
        fprintf(fp, "%s\n", prms.str_userq.c_str());        // q
        fprintf(fp, "%s\n", prms.str_xeq.c_str());          // x'
        fprintf(fp, "%s\n", prms.str_yeq.c_str());          // y'
        fprintf(fp, "%s\n", prms.str_gcf.c_str());          // gcf
        fprintf(fp, "0\n");                                 // numparams

        fclose(fp);
        return true;
    }
}

bool prepareGcf(std::string fname, P4POLYNOM2 f, double y1, double y2,
                int precision, int numpoints)
{
    int i;
    char buf[100];

    // open original maple script (will get overwritten?!)
    FILE *fp = fopen(std::string(fname + ".mpl").c_str(), "w");
    if (fp != nullptr) {
        fprintf(fp, "restart;\n");
        fprintf(fp, "read( \"%s\" ):\n",
                std::string(std::string(P4_BINDIR) + std::string("p4gcf.m"))
                    .c_str());
        fprintf(fp, "user_file := \"%s\":\n",
                std::string(fname + "_gcf.tab").c_str());
        fprintf(fp, "user_numpoints := %d:\n", numpoints);
        fprintf(fp, "user_x1 := -1.0:\n");
        fprintf(fp, "user_x2 := 1.0:\n");
        fprintf(fp, "user_y1 := %g:\n", y1);
        fprintf(fp, "user_y2 := %g:\n", y2);
        fprintf(fp, "u := x:\n");
        fprintf(fp, "v := y:\n");
        fprintf(fp, "user_f := ");
        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm2(buf, f, (i == 0) ? true : false, "x", "y"));
            f = f->next_term2;
        }
        if (i == 0)
            fprintf(fp, "0:\n");
        else
            fprintf(fp, ":\n");

        fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                    "`quit`(0); else `quit(1)` end if: end try:\n");

        globalLogger__.debug("ScriptHandler :: prepared GCF file " + fname);
        fclose(fp);
        return true;
    }
    globalLogger__.error("ScriptHandler :: cannot prepare GCF file");
    return false;
}

bool prepareGcf_LyapunovCyl(std::string fname, P4POLYNOM3 f, double theta1,
                            double theta2, int precision, int numpoints)
{
    char buf[100];
    int i;

    /*f = VFResults.gcf_C;*/

    FILE *fp = fopen(std::string(fname + ".mpl").c_str(), "w");

    if (fp != nullptr) {
        fprintf(fp, "restart;\n");
        fprintf(fp, "read(\"%s\"):\n",
                std::string(std::string(P4_BINDIR) + std::string("p4gcf.m"))
                    .c_str());
        fprintf(fp, "user_file := \"%s\":\n",
                std::string(fname + "_gcf.tab").c_str());
        fprintf(fp, "user_numpoints := %d:\n", numpoints);
        fprintf(fp, "user_x1 := 0.0:\n");
        fprintf(fp, "user_x2 := 1.0:\n");
        fprintf(fp, "user_y1 := %g:\n", theta1);
        fprintf(fp, "user_y2 := %g:\n", theta2);
        fprintf(fp, "u := cos(y):\n");
        fprintf(fp, "v := sin(y):\n");
        fprintf(fp, "user_f := ");
        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm3(buf, f, (i == 0) ? true : false, "x", "U", "V"));
            f = f->next_term3;
        }
        if (i == 0)
            fprintf(fp, "0:\n");
        else
            fprintf(fp, ":\n");

        fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                    "`quit`(0); else `quit(1)` end if: end try:\n");

        globalLogger__.debug("ScriptHandler :: prepared GCF_LyapunovCyl file " +
                             fname);
        fclose(fp);
        return true;
    }
    globalLogger__.error(
        "ScriptHandler :: cannot prepare GCF_LyapunovCyl file");
    return false;
}

bool prepareGcf_LyapunovR2(std::string fname, P4POLYNOM2 f, int precision,
                           int numpoints)
{
    char buf[100];
    int i;

    // f = VFResults.gcf;

    FILE *fp = fopen(std::string(fname + ".mpl").c_str(), "w");

    if (fp != nullptr) {
        fprintf(fp, "restart;\n");
        fprintf(fp, "read( \"%s\" ):\n",
                std::string(std::string(P4_BINDIR) + std::string("p4gcf.m"))
                    .c_str());
        fprintf(fp, "user_file := \"%s\":\n",
                std::string(fname + "_gcf.tab").c_str());
        fprintf(fp, "user_numpoints := %d:\n", numpoints);
        fprintf(fp, "user_x1 := 0.0:");
        fprintf(fp, "user_x2 := 1.0:");
        fprintf(fp, "user_y1 := 0.0:");
        fprintf(fp, "user_y2 := %g:\n", TWOPI);
        fprintf(fp, "u := x*cos(y):");
        fprintf(fp, "v := y*sin(y):");
        fprintf(fp, "user_f := ");
        for (i = 0; f != nullptr; i++) {
            fprintf(fp, "%s",
                    printterm2(buf, f, (i == 0) ? true : false, "U", "V"));
            f = f->next_term2;
        }
        if (i == 0)
            fprintf(fp, "0:\n");
        else
            fprintf(fp, ":\n");

        fprintf(fp, "try FindSingularities() finally: if returnvalue=0 then "
                    "`quit`(0); else `quit(1)` end if: end try:");

        globalLogger__.debug("ScriptHandler :: prepared GCF_LyapunovR2 file " +
                             fname);
        fclose(fp);
        return true;
    }
    globalLogger__.error("ScriptHandler :: cannot prepare GCF_LyapunovR2 file");
    return false;
}

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) ||                 \
    defined(__WINDOWS__) || defined(__TOS_WIN__)

#include <windows.h>

inline void delay(unsigned long ms) { Sleep(ms); }

#else /* presume POSIX */

#include <unistd.h>

inline void delay(unsigned long ms) { usleep(ms * 1000); }

#endif