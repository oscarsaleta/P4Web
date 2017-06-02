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
#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

/*!
 * @brief This file implements file/script creation and execution
 * @file ScriptHandler.h
 *
 * The ScriptHandler.h functions are implemented as global methods
 * for creating files with random names and forking processes that
 * execute the Maple scripts, as well as handling the file filling
 * in some casses.
 */

#include "file_tab.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef ANTZ
#define MAPLE_PATH "/usr/share/maple11/bin/maple" ///< path to Maple executable
#define P4_BINDIR "/home/p4/p4/bin/"              ///< path to P4 Maple scripts
#define TMP_DIR "/home/p4/tmp/"                   ///< path to tmp folder
#else
#define MAPLE_PATH "/home/osr/maple2015/bin/maple" ///< path to Maple executable
#define P4_BINDIR "/usr/local/p4/bin/"             ///< path to P4 Maple scripts
#define TMP_DIR "/tmp/"                            ///< path to tmp folder
#endif

#define X_MIN -1
#define X_MAX 1
#define Y_MIN -1
#define Y_MAX 1

/**
 * Struct that stores all the Maple execution parameters for WP4
 */
class ScriptHandler
{
  public:
    /**
     * Constructor method
     */
    ScriptHandler();
    /**
     * Destructor method
     */
    ~ScriptHandler(){};

    /**
     * directory where maple scripts are located
     */
    std::string str_bindir_;
    /**
     * name of p4.m script
     */
    std::string str_p4m_;
    /**
     * path to temporary directory for storing text files
     */
    std::string str_tmpdir_;
    /**
     * path to lyapunov C++ executable
     */
    std::string str_lypexe_;
    /**
     * path to lyapunov C++ multiprecision executable
     */
    std::string str_lypexe_mpf_;
    /**
     * path to separatrice C++ executable
     */
    std::string str_sepexe_;
    /**
     * prefix to executable files
     */
    std::string str_exeprefix_;
    /**
     * platform where the software runs
     */
    std::string str_platform_;
    /**
     * path to sumtable folder
     */
    std::string str_sumtablepath_;
    /**
     * remove command in this specific platform
     */
    std::string str_removecmd_;
    /**
     * flag to use custom simplify command
     */
    std::string str_simplify_;
    /**
     * custom simplify command
     */
    std::string str_simplifycmd_;
    /**
     * which critical points are to be studied
     */
    std::string str_critpoints_;
    /**
     * turn on/off save all feature
     */
    std::string str_saveall_;
    /**
     * name of vec.tab file
     */
    std::string str_vectable_;
    /**
     * name of curve.tab file
     */
    std::string str_curvetable_;
    /**
     * name of isoclines.tab file
     */
    std::string str_isoctable_;
    /**
     * name of fin.tab file
     */
    std::string str_fintab_;
    /**
     * name of fin.res file
     */
    std::string str_finres_;
    /**
     * name of inf.tab file
     */
    std::string str_inftab_;
    /**
     * name of inf.res file
     */
    std::string str_infres_;
    /**
     * x' polynomial
     */
    std::string str_xeq_;
    /**
     * y' polynomial
     */
    std::string str_yeq_;
    /**
     * vector field [x',y']
     */
    std::string str_userf_;
    /**
     * gcf polynomial
     */
    std::string str_gcf_;
    /**
     * turn numeric mode on/off
     */
    std::string str_numeric_;
    /**
     * epsilon setting
     */
    std::string str_epsilon_;
    /**
     * turn separatrice testing on/off
     */
    std::string str_testsep_;
    /**
     * accuracy setting
     */
    std::string str_precision_;
    /**
     * precision setting
     */
    std::string str_precision0_;
    /**
     * taylor order for separatrice testing
     */
    std::string str_taylor_;
    /**
     * start working numerically from this order
     */
    std::string str_numericlevel_;
    /**
     * max taylor order
     */
    std::string str_maxlevel_;
    /**
     * weakness level for Lyapunov constants
     */
    std::string str_weaklevel_;
    /**
     * PL weight
     */
    std::string str_userp_;
    /**
     * PL weight
     */
    std::string str_userq_;
    /**
     * time limit for Maple execution
     */
    std::string time_limit_;
    /**
     * string with the curve equation
     */
    std::string str_curve_;
    std::string str_x0_;
    std::string str_y0_;
    /**
     * Vector of strings for labels
     */
    std::vector<std::string> paramLabels_;
    /**
     * Vector of strings for values
     */
    std::vector<std::string> paramValues_;

    /**
     * Generate a random name for a temp file
     *
     * @param prefix path to file plus some prefix to the name
     * @param suffix suffix after random part of name and file extension
     * @return       a string containing the name generated
     */
    std::string randomFileName(std::string prefix, std::string suffix);

    /**
     * Prepare Maple script for evaluation
     *
     * @param fname name of file, can be empty and the function will set it
     * @return      @c true if file was successfully created, @c false otherwise
     *
     * Opens file and fills it (through calling fillMapleScript())
     */
    bool prepareMapleFile(std::string &fname);

    /**
     * Fill a Maple script with the parameters and commands for evaluation
     *
     * @param f     file stream to write to
     *
     * Called from prepareMapleFile()
     */
    void fillMapleScript(FILE *f);

    /**
     * Evaluate a Maple script
     *
     * @param fname   filename of Maple script
     * @param maxtime max number of seconds for execution
     * @return        return status of the forked process
     *
     * Forks a Maple process and waits for it to finish
     */
    siginfo_t evaluateMapleScript(std::string fname, int maxtime);

    /**
     * Create a file that contains the execution parameters
     *
     * @param fname  filename of temp file to serve to client
     * @param prms   struct that contains the current parameters of execution
     * @param labels string vector of parameter labels
     * @param values string vector of parameter values
     * @return       @c true if file was successfully created, @c false
     * otherwise
     *
     * This format is compatible with P4. If a user is not logged in, every
     * option will be saved as the default options.
     */
    bool fillSaveFile(std::string fname);

    /**
     * Prepare files in case of calculating GCF in plane/U1/U2 charts.
     *
     * @param fname     name of file where to write
     * @param f         terms of GCF
     * @param y1        y coord of point 1
     * @param y2        y coord of point 2
     * @param precision precision of integration
     * @param numpoints number of points to compute
     * @return          @c true if file was successfully prepared, @c false
     * otherwise
     *
     * This is only called in case of Poincare-compactification (weights p=q=1)
     */
    bool prepareGcf(std::string fname, P4POLYNOM2 f, double y1, double y2,
                    int precision, int numpoints);

    /**
     * Prepare files in case of calculating GCF in charts near infinity.
     *
     * @param fname     name of file where to write
     * @param f         terms of GCF
     * @param theta1    theta coord of point 1
     * @param theta2    theta coord of point 2
     * @param precision precision of integration
     * @param numpoints number of points to compute
     * @return          @c true if file was successfully prepared, @c false
     * otherwise
     *
     * This is only called in case of Poincare-Lyapunov compactification
     * (weights (p,q) !=(1,1))
     */
    bool prepareGcf_LyapunovCyl(std::string fname, P4POLYNOM3 f, double theta1,
                                double theta2, int precision, int numpoints);

    /**
     * Prepare files in case of calculating GCF in charts near infinity.
     *
     * @param fname     file name of user Maple script
     * @param f         terms of GCF
     * @param precision precision of integration
     * @param numpoints number of points to compute
     * @return          @c true if file was successfully created, @c false
     * otherwise
     *
     * This is only called in case of Poincare-Lyapunov compactification
     * (weights (p,q) !=(1,1)).
     * Same as preparegcf, except for the "u := " and "v := " assignments,
     * and the fact that one always refers to the same object WVFStudy.gcf_,
     * and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
     */
    bool prepareGcf_LyapunovR2(std::string fname, P4POLYNOM2 f, int precision,
                               int numpoints);

    /**
     * Find a word in a target string and make sure it is the full word
     *
     * @param target string to look for word in
     * @param word   word to find as substring
     * @param start  index where to start looking
     * @return       the first index where we found a match
     *
     * If we find word in target, we need to check if it is preceded by
     * a mathematical operator (i.e., not a digit, letter or underscore)
     * and followed by a mathematical operator (same as before).
     */
    int findIndexOfWordInTarget(std::string target, std::string word,
                                int start = 0);

    /**
     * Prepare the Maple script that will convert a curve into a table
     *
     * @param fname name of file where to write
     */
    void prepareCurveTable(std::string fname);

    /**
     * Prepare files in case of calculating curve in plane/U1/U2 charts.
     *
     * @param fname     name of file where to write
     * @param f         terms of curve
     * @param y1        y coord of point 1
     * @param y2        y coord of point 2
     * @param precision precision of integration
     * @param numpoints number of points to compute
     * @return          @c true if file was successfully prepared, @c false
     * otherwise
     *
     * This is only called in case of Poincare-compactification (weights p=q=1)
     */
    bool prepareCurve(std::string fname, P4POLYNOM2 f, double y1, double y2,
                      int precision, int numpoints);

    /**
     * Prepare files in case of calculating curve in charts near infinity.
     *
     * @param fname     name of file where to write
     * @param f         terms of curve
     * @param theta1    theta coord of point 1
     * @param theta2    theta coord of point 2
     * @param precision precision of integration
     * @param numpoints number of points to compute
     * @return          @c true if file was successfully prepared, @c false
     * otherwise
     *
     * This is only called in case of Poincare-Lyapunov compactification
     * (weights (p,q) !=(1,1))
     */
    bool prepareCurve_LyapunovCyl(std::string fname, P4POLYNOM3 f,
                                  double theta1, double theta2, int precision,
                                  int numpoints);

    /**
     * Prepare files in case of calculating curve in charts near infinity.
     *
     * @param fname     file name of user Maple script
     * @param f         terms of curve
     * @param precision precision of integration
     * @param numpoints number of points to compute
     * @return          @c true if file was successfully created, @c false
     * otherwise
     *
     * This is only called in case of Poincare-Lyapunov compactification
     * (weights (p,q) !=(1,1)).
     * Same as preparecurve, except for the "u := " and "v := " assignments,
     * and the fact that one always refers to the same object curve_vector_.r2,
     * and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
     */
    bool prepareCurve_LyapunovR2(std::string fname, P4POLYNOM2 f, int precision,
                                 int numpoints);

  private:
    /**
     * Wait for a certain amount of milliseconds, pausing the program
     *
     * @param ms number of milliseconds to pause execution for
     *
     * The implementation depends on wether the system is Windows or Linux,
     * and
     * although this program is thought for Linux, we implemented both, just
     * in
     * case.
     */
    inline void delay(unsigned long ms) { usleep(ms * 1000); }

    /**
     * Change name of parameters from "something" to "something_", to avoid
     * conflict
     *
     * @param xeq    string that contains x'
     * @param yeq    string that contains y'
     * @param gcf    string that contains the GCF
     *
     * The change has to be done recurrently (substitute every label in the
     * values
     * vector), and also to x', y', and the GCF.
     */
    void changeParameterNames(std::string &xeq, std::string &yeq,
                              std::string &gcf);
    /**
     * Add a _ to the end of all labels present in target
     *
     * @param target string to be modified
     * @return       the modified string
     *
     * If the labels vector is {a,b,c} and the target is a*b*2, then
     * this function returns a_*b_*2
     */
    std::string convertLabelsFromString(std::string target);

    void writeMapleParameters(FILE *f);
    bool stringToBool(std::string);
};

#endif // SCRIPTHANDLER_H