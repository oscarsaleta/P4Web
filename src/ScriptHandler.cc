/*  This file is part of WP4 (http://github.com/oscarsaleta/WP4)
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  WP4 is free software: you can redistribute it and/or modify
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

#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "ScriptHandler.h"

#include "MyLogger.h"
#include "custom.h"
#include "file_tab.h"
#include "math_p4.h"
#include "math_polynom.h"

#include <cctype>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

ScriptHandler::ScriptHandler()
{
    str_bindir_ = P4_BINDIR;
    str_p4m_ = str_bindir_ + "p4.m";
    str_tmpdir_ = TMP_DIR;
    str_lypexe_ = "lyapunov";
    str_lypexe_mpf_ = "lyapunov_mpf";
    str_sepexe_ = "separatrice";
    str_exeprefix_ = "";
    str_platform_ = "LINUX";
    str_sumtablepath_ = str_bindir_ + "../sumtables/";
    str_removecmd_ = "rm";
    str_simplify_ = "false";
    str_simplifycmd_ = MAPLE_SIMPLIFY_EXPRESSIONS;
}

std::string ScriptHandler::randomFileName(std::string prefix,
                                          std::string suffix)
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

    g_globalLogger.debug("ScriptHandler :: created temp file " + fullname +
                         " and prefix is " + prefix);

    return prefix;
}

bool ScriptHandler::prepareMapleFile(std::string &fname)
{
    g_globalLogger.debug("ScriptHandler :: received order to prepare script " +
                         fname);
    FILE *mplFile;

    if (fname.empty())
        fname = randomFileName(TMP_DIR, ".mpl");

    mplFile = fopen((fname + ".mpl").c_str(), "w");

    // repair file permissions
    std::string command = "chmod 644 " + fname + ".mpl";
    system(command.c_str());

    str_vectable_ = fname + "_vec.tab";
    str_fintab_ = fname + "_fin.tab";
    str_finres_ = fname + "_fin.res";
    str_inftab_ = fname + "_inf.tab";
    str_infres_ = fname + "_inf.res";

    if (mplFile != nullptr) {
        fillMapleScript(mplFile);
        fclose(mplFile);
        g_globalLogger.debug("ScriptHandler :: prepared Maple file " + fname);
        return true;
    } else {
        g_globalLogger.error("ScriptHandler :: cannot prepare Maple file");
        return false;
    }
}

void ScriptHandler::fillMapleScript(FILE *f)
{
    if (!paramLabels_.empty()) {
        changeParameterNames(str_xeq_, str_yeq_, str_gcf_);
        str_userf_ = "[" + str_xeq_ + "," + str_yeq_ + "]";
    }

    fprintf(f, "restart;\n");
    fprintf(f, "read( \"%s\" ):\n", str_p4m_.c_str());
    fprintf(f, "user_bindir := \"%s\":\n", str_bindir_.c_str());
    fprintf(f, "user_tmpdir := \"%s\":\n", str_tmpdir_.c_str());
    fprintf(f, "user_lypexe := \"%s\":\n", str_lypexe_.c_str());
    fprintf(f, "user_lypexe_mpf := \"%s\":\n", str_lypexe_mpf_.c_str());
    fprintf(f, "user_sepexe := \"%s\":\n", str_sepexe_.c_str());
    fprintf(f, "user_exeprefix := \"%s\":\n", str_exeprefix_.c_str());
    fprintf(f, "user_platform := \"%s\":\n", str_platform_.c_str());
    fprintf(f, "user_sumtablepath := \"%s\":\n", str_sumtablepath_.c_str());
    fprintf(f, "user_removecmd := \"%s\":\n", str_removecmd_.c_str());
    fprintf(f, "user_simplify := %s:\n", str_simplify_.c_str());
    fprintf(f, "user_simplifycmd := %s:\n", str_simplifycmd_.c_str());
    fprintf(f, "all_crit_points := %s:\n", str_critpoints_.c_str());
    fprintf(f, "save_all := %s:\n", str_saveall_.c_str());
    fprintf(f, "vec_table := \"%s\":\n", str_vectable_.c_str());
    fprintf(f, "finite_table := \"%s\":\n", str_fintab_.c_str());
    fprintf(f, "finite_res := \"%s\":\n", str_finres_.c_str());
    fprintf(f, "infinite_table := \"%s\":\n", str_inftab_.c_str());
    fprintf(f, "infinite_res := \"%s\":\n", str_infres_.c_str());
    fprintf(f, "if (type(parse(\"%s\"),polynom)) then\n", str_xeq_.c_str());
    fprintf(f, "  if (type(parse(\"%s\"),polynom)) then\n", str_yeq_.c_str());
    fprintf(f, "    user_f := %s:\n", str_userf_.c_str());
    fprintf(f, "  else `quit(1)` end if:\n");
    fprintf(f, "else `quit(1)` end if:\n");
    fprintf(f, "if (type(parse(\"%s\"),polynom)) then\n", str_gcf_.c_str());
    fprintf(f, "  user_gcf := %s:\n", str_gcf_.c_str());
    fprintf(f, "else `quit(1)` end if:\n");

    writeMapleParameters(f);

    fprintf(f, "user_numeric := %s:\n", str_numeric_.c_str());
    fprintf(f, "epsilon := %s:\n", str_epsilon_.c_str());
    fprintf(f, "test_sep := %s:\n", str_testsep_.c_str());
    fprintf(f, "user_precision := %s:\n", str_precision_.c_str());
    fprintf(f, "user_precision0 := %s:\n", str_precision0_.c_str());
    fprintf(f, "taylor_level := %s:\n", str_taylor_.c_str());
    fprintf(f, "numeric_level := %s:\n", str_numericlevel_.c_str());
    fprintf(f, "max_level := %s:\n", str_maxlevel_.c_str());
    fprintf(f, "weakness_level := %s:\n", str_weaklevel_.c_str());
    fprintf(f, "user_p := %s:\n", str_userp_.c_str());
    fprintf(f, "user_q := %s:\n", str_userq_.c_str());
    fprintf(f, "try timelimit(%s,p4main()) catch:\n", time_limit_.c_str());
    fprintf(f, "printf( \"! Error (%%a) %%a\\n\", lastexception[1], "
               "lastexception[2] );\n");
    fprintf(f, "finally: closeallfiles();\n");
    fprintf(
        f, "if normalexit=0 then `quit`(0); else `quit(1)` end if: end try:\n");

    g_globalLogger.debug("ScriptHandler :: filled Maple file");
}

siginfo_t ScriptHandler::evaluateMapleScript(std::string fname, int maxtime)
{
    g_globalLogger.debug(
        "ScriptHandler :: Will fork Maple process for script " + fname);
    pid_t pid = fork();
    if (pid < 0) {
        g_globalLogger.error("HomeLeft :: error forking Maple thread.");
        siginfo_t pinfo;
        pinfo.si_pid = -1;
        pinfo.si_code = -1;
        return pinfo;
    } else if (pid == 0) {
        // create vector of arguments for execvp
        std::vector<char *> commands;
#ifdef ANTZ
        commands.push_back("ssh");
        commands.push_back("p4@a05");
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
        for (int tries = 0; tries < maxtime; tries++) {
            waitid(P_PID, pid, &infop, WEXITED | WSTOPPED | WNOHANG);
            if (infop.si_pid != 0) {
                g_globalLogger.debug(
                    "ScriptHandler :: forked Maple execution finished");
                return infop;
            }
            delay(1000);
        }
        std::string aux("pkill -TERM -P " + std::to_string(pid));
        g_globalLogger.error("ScriptHandler :: " + aux);
        system(aux.c_str());
        kill(pid, SIGTERM);
        g_globalLogger.error(
            "ScriptHandler :: Maple execution took too much time");
        infop.si_status = -2;
        infop.si_code = -2;
        return infop;
    }
}

/*
    Structure of .inp file:

    - integer 0,1,2,3  (type of singularities: all, finite, ...)
    - parameters:
        * integer 0,1 symbolic/numeric
        * integer precision
        * string epsilon
        * integer testsep
        * integer taylorlevel
        * integer numericlevel
        * integer maxlevel
        * integer weakness
        * only if type of singularity is ONE:
            + string x0
            + string y0
        * otherwhise:
            + integer P
            + integer Q

    - vector field:
        * string xdot
        * string ydot
        * string gcf
        * integer numparams
        * for each parameter:
            + string label
            + string value

    - optional: integer precision0
*/
bool ScriptHandler::fillSaveFile(std::string fname)
{
    g_globalLogger.debug("ScriptHandler :: filling save file...");
    FILE *fp = fopen(fname.c_str(), "w");

    if (fp != nullptr) {
        fprintf(fp, "0\n"); // typeofstudy_
        fprintf(fp, "%s\n",
                (str_numeric_ == std::string("true") ? "1" : "0")); // numeric
        fprintf(fp, "%s\n", str_precision_.c_str());                // precision
        fprintf(fp, "%s\n", str_epsilon_.c_str());                  // epsilon
        fprintf(fp, "%s\n",
                (str_testsep_ == std::string("true") ? "1" : "0")); // test sep
        fprintf(fp, "%s\n", str_taylor_.c_str());       // taylor level
        fprintf(fp, "%s\n", str_numericlevel_.c_str()); // numeric level
        fprintf(fp, "%s\n", str_maxlevel_.c_str());     // max levels
        fprintf(fp, "%s\n", str_weaklevel_.c_str());    // weakness level
        fprintf(fp, "%s\n", str_userp_.c_str());        // p
        fprintf(fp, "%s\n", str_userq_.c_str());        // q
        fprintf(fp, "%s\n", str_xeq_.c_str());          // x'
        fprintf(fp, "%s\n", str_yeq_.c_str());          // y'
        fprintf(fp, "%s\n", str_gcf_.c_str());          // gcf
        if (paramLabels_.empty()) {
            fprintf(fp, "0\n"); // numparams
        } else {
            fprintf(fp, "%lu\n", paramLabels_.size()); // numparams
            writeMapleParameters(fp);
        }

        fclose(fp);
        return true;
    }
}

bool ScriptHandler::prepareGcf(std::string fname, P4POLYNOM2 f, double y1,
                               double y2, int precision, int numpoints)
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

        g_globalLogger.debug("ScriptHandler :: prepared GCF file " + fname);
        fclose(fp);
        return true;
    }
    g_globalLogger.error("ScriptHandler :: cannot prepare GCF file");
    return false;
}

bool ScriptHandler::prepareGcf_LyapunovCyl(std::string fname, P4POLYNOM3 f,
                                           double theta1, double theta2,
                                           int precision, int numpoints)
{
    char buf[100];
    int i;

    /*f = VFResults.gcf_C_;*/

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

        g_globalLogger.debug("ScriptHandler :: prepared GCF_LyapunovCyl file " +
                             fname);
        fclose(fp);
        return true;
    }
    g_globalLogger.error(
        "ScriptHandler :: cannot prepare GCF_LyapunovCyl file");
    return false;
}

bool ScriptHandler::prepareGcf_LyapunovR2(std::string fname, P4POLYNOM2 f,
                                          int precision, int numpoints)
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

        g_globalLogger.debug("ScriptHandler :: prepared GCF_LyapunovR2 file " +
                             fname);
        fclose(fp);
        return true;
    }
    g_globalLogger.error("ScriptHandler :: cannot prepare GCF_LyapunovR2 file");
    return false;
}

void ScriptHandler::changeParameterNames(std::string &xeq, std::string &yeq,
                                         std::string &gcf)
{
    xeq = convertLabelsFromString(xeq);
    yeq = convertLabelsFromString(yeq);
    gcf = convertLabelsFromString(gcf);

    std::vector<std::string>::iterator it;
    for (it = paramValues_.begin(); it != paramValues_.end(); it++) {
        *it = convertLabelsFromString(*it);
    }
    for (it = paramLabels_.begin(); it != paramLabels_.end(); it++) {
        *it = convertLabelsFromString(*it);
    }
    return;
}

std::string ScriptHandler::convertLabelsFromString(std::string target)
{
    std::string aux;
    std::string currentLabel, newLabel;
    int i, j;

    std::vector<std::string>::iterator it;
    for (it = paramLabels_.begin(); it != paramLabels_.end(); it++) {
        currentLabel = *it;
        newLabel = currentLabel + "_";
        if (currentLabel.empty())
            continue;
        g_globalLogger.debug("ScriptHandler :: looking for word " +
                             currentLabel + " in " + target);
        while ((i = findIndexOfWordInTarget(target, currentLabel, 0)) != -1) {
            aux = target.substr(0, i);
            aux += newLabel;
            aux += target.substr(i + currentLabel.length());
            target = aux;
        }
    }
    return target;
}

int ScriptHandler::findIndexOfWordInTarget(std::string target, std::string word,
                                           int start)
{
    int i, j;
    // search for first occurrence of substring word in target
    while ((i = target.find(word, start)) != std::string::npos) {
        // next time we will search from this point if needed
        start = i + 1;
        // check if substring is the beginning of a word
        // if the char before our occurrence is not a mathematical operator,
        // the word is a substring from a bigger one
        j = i;
        if (j > 0) {
            g_globalLogger.debug("char is " + std::string(&target[j - 1]));
            if (isdigit(target[j - 1]) || isalpha(target[j - 1]) ||
                target[j - 1] == '_') {
                continue;
            }
        }
        // check if substring is end of a word
        j += word.length();
        if (j < target.length()) {
            g_globalLogger.debug("char is " + std::string(&target[j]));
            if (isdigit(target[j]) || isalpha(target[j]) || target[j] == '_') {
                continue;
            }
        }
        // if we get through, we found a case
        break;
    }
    if (i == std::string::npos)
        return -1;
    return i;
}

void ScriptHandler::writeMapleParameters(FILE *f)
{
    if (!paramLabels_.empty()) {
        std::vector<std::string>::iterator it1;
        std::vector<std::string>::iterator it2;
        for (it1 = paramLabels_.begin(), it2 = paramValues_.begin();
             it1 < paramLabels_.end(), it2 < paramValues_.end(); ++it1, ++it2) {
            fprintf(f, "if (type(parse(\"%s\"),polynom)) then\n", it1->c_str());
            fprintf(f, "  if (type(parse(\"%s\"),polynom)) then\n",
                    it2->c_str());
            if (!str_numeric_.empty() && stringToBool(str_numeric_))
                fprintf(f, "    %s := evalf(%s):\n", it1->c_str(),
                        it2->c_str());
            else
                fprintf(f, "    %s := %s:\n", it1->c_str(), it2->c_str());
            fprintf(f, "  else `quit(1)` end if:\n");
            fprintf(f, "else `quit(1)` end if:\n");
        }
    }
}

bool ScriptHandler::stringToBool(std::string s)
{
    if (s == "true")
        return true;
    else
        return false;
}

void ScriptHandler::prepareCurveTable(std::string fname)
{
    FILE *f;
    char buf[100];

    f = fopen((fname + "_curve_prep.mpl").c_str(), "w");

    if (f != nullptr) {
        fprintf(f, "restart;\n");
        fprintf(f, "read( \"%s\" ):\n", str_p4m_.c_str());
        fprintf(f, "user_bindir := \"%s\":\n", str_bindir_.c_str());
        fprintf(f, "user_tmpdir := \"%s\":\n", str_tmpdir_.c_str());
        fprintf(f, "user_exeprefix := \"%s\":\n", str_exeprefix_.c_str());
        fprintf(f, "user_platform := \"%s\":\n", str_platform_.c_str());
        fprintf(f, "user_removecmd := \"%s\":\n", str_removecmd_.c_str());
        fprintf(f, "user_simplify := %s:\n", str_simplify_.c_str());
        fprintf(f, "user_simplifycmd := %s:\n", str_simplifycmd_.c_str());
        fprintf(f, "all_crit_points := %s:\n", str_critpoints_.c_str());

        str_curvetable_ = fname + "_veccurve.tab";
        system(std::string("rm -f " + str_curvetable_).c_str());
        fprintf(f, "curve_table := \"%s\":\n", str_curvetable_.c_str());

        str_curve_ = convertLabelsFromString(str_curve_);
        fprintf(f, "user_curve := %s\n:", str_curve_.c_str());

        writeMapleParameters(f);

        fprintf(f, "user_numeric := %s:\n", str_numeric_.c_str());
        fprintf(f, "epsilon := %s:\n", str_epsilon_.c_str());
        fprintf(f, "test_sep := %s:\n", str_testsep_.c_str());
        fprintf(f, "user_precision := %s:\n", str_precision_.c_str());
        fprintf(f, "user_precision0 := %s:\n", str_precision0_.c_str());
        fprintf(f, "taylor_level := %s:\n", str_taylor_.c_str());
        fprintf(f, "numeric_level := %s:\n", str_numericlevel_.c_str());
        fprintf(f, "max_level := %s:\n", str_maxlevel_.c_str());
        fprintf(f, "weakness_level := %s:\n", str_weaklevel_.c_str());

        if (false /*typeofstudy_ == TYPEOFSTUDY_ONE*/) {
            fprintf(f, "user_p := 1:\n");
            fprintf(f, "user_q := 1:\n");
            fprintf(f, "x0 := %s:\n", str_x0_.c_str());
            fprintf(f, "y0 := %s:\n", str_y0_.c_str());
            fprintf(f, "x_min := x0+(%f):\n", (float)(X_MIN));
            fprintf(f, "x_min := x0+(%f):\n", (float)(X_MIN));
            fprintf(f, "x_max := x0+(%f):\n", (float)(X_MAX));
            fprintf(f, "y_min := y0+(%f):\n", (float)(Y_MIN));
            fprintf(f, "y_max := y0+(%f):\n", (float)(Y_MAX));
        } else {
            fprintf(f, "user_p:=%s:\n", str_userp_.c_str());
            fprintf(f, "user_q:=%s:\n", str_userq_.c_str());
        }

        fprintf(f, "try prepareCurve() catch:\n"
                   "printf( \"! Error (\%%a) \%%a\\n\", lastexception[1], "
                   "lastexception[2] );\n"
                   "finally: closeallfiles();\n"
                   "if normalexit=0 then `quit`(0); else `quit(1)` end if: end "
                   "try:\n");
        ;
    }
    fclose(f);
}

bool ScriptHandler::prepareCurve(std::string fname, P4POLYNOM2 f, double y1,
                                 double y2, int precision, int numpoints)
{
    int i;
    char buf[100];

    // open original maple script (will get overwritten?!)
    FILE *fp = fopen(std::string(fname + ".mpl").c_str(), "w");
    if (fp != nullptr) {
        fprintf(fp, "restart;\n");
        fprintf(fp, "read( \"%s\" ):\n", (str_bindir_ + "/p4gcf.m").c_str());
        fprintf(fp, "user_file := \"%s\":\n",
                std::string(fname + "_curve.tab").c_str());
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

        g_globalLogger.debug("ScriptHandler :: prepared curve file " + fname);
        fclose(fp);
        return true;
    }
    g_globalLogger.error("ScriptHandler :: cannot prepare curve file");
    return false;
}

bool ScriptHandler::prepareCurve_LyapunovCyl(std::string fname, P4POLYNOM3 f,
                                             double theta1, double theta2,
                                             int precision, int numpoints)
{
    char buf[100];
    int i;

    FILE *fp = fopen(std::string(fname + ".mpl").c_str(), "w");

    if (fp != nullptr) {
        fprintf(fp, "restart;\n");
        fprintf(fp, "read(\"%s\"):\n", (str_bindir_ + "/p4gcf.m").c_str());
        fprintf(fp, "user_file := \"%s\":\n",
                std::string(fname + "_curve.tab").c_str());
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

        g_globalLogger.debug(
            "ScriptHandler :: prepared curve_LyapunovCyl file " + fname);
        fclose(fp);
        return true;
    }
    g_globalLogger.error(
        "ScriptHandler :: cannot prepare curve_LyapunovCyl file");
    return false;
}

bool ScriptHandler::prepareCurve_LyapunovR2(std::string fname, P4POLYNOM2 f,
                                            int precision, int numpoints)
{
    char buf[100];
    int i;

    FILE *fp = fopen(std::string(fname + ".mpl").c_str(), "w");

    if (fp != nullptr) {
        fprintf(fp, "restart;\n");
        fprintf(fp, "read( \"%s\" ):\n", (str_bindir_ + "/p4gcf.m").c_str());
        fprintf(fp, "user_file := \"%s\":\n",
                std::string(fname + "_curve.tab").c_str());
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

        g_globalLogger.debug(
            "ScriptHandler :: prepared curve_LyapunovR2 file " + fname);
        fclose(fp);
        return true;
    }
    g_globalLogger.error(
        "ScriptHandler :: cannot prepare curve_LyapunovR2 file");
    return false;
}