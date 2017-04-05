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

#ifdef ANTZ
#define MAPLE_PATH "/usr/share/maple11/bin/maple" ///< path to Maple executable
#define P4_BINDIR "/home/p4/p4/bin/"              ///< path to P4 Maple scripts
#define TMP_DIR "/home/p4/tmp/"                   ///< path to tmp folder
#else
#define MAPLE_PATH "/home/osr/maple2015/bin/maple" ///< path to Maple executable
#define P4_BINDIR "/usr/local/p4/bin/"             ///< path to P4 Maple scripts
#define TMP_DIR "/tmp/"                            ///< path to tmp folder
#endif

/**
 * Struct that stores all the Maple execution parameters for WP4
 */
struct mapleParamsStruct {
    /**
     * directory where maple scripts are located
     */
    std::string str_bindir;
    /**
     * name of p4.m script
     */
    std::string str_p4m;
    /**
     * path to temporary directory for storing text files
     */
    std::string str_tmpdir;
    /**
     * path to lyapunov C++ executable
     */
    std::string str_lypexe;
    /**
     * path to lyapunov C++ multiprecision executable
     */
    std::string str_lypexe_mpf;
    /**
     * path to separatrice C++ executable
     */
    std::string str_sepexe;
    /**
     * prefix to executable files
     */
    std::string str_exeprefix;
    /**
     * platform where the software runs
     */
    std::string str_platform;
    /**
     * path to sumtable folder
     */
    std::string str_sumtablepath;
    /**
     * remove command in this specific platform
     */
    std::string str_removecmd;
    /**
     * flag to use custom simplify command
     */
    std::string str_simplify;
    /**
     * custom simplify command
     */
    std::string str_simplifycmd;
    /**
     * which critical points are to be studied
     */
    std::string str_critpoints;
    /**
     * turn on/off save all feature
     */
    std::string str_saveall;
    /**
     * name of vec.tab file
     */
    std::string str_vectable;
    /**
     * name of fin.tab file
     */
    std::string str_fintab;
    /**
     * name of fin.res file
     */
    std::string str_finres;
    /**
     * name of inf.tab file
     */
    std::string str_inftab;
    /**
     * name of inf.res file
     */
    std::string str_infres;
    /**
     * x' polynomial
     */
    std::string str_xeq;
    /**
     * y' polynomial
     */
    std::string str_yeq;
    /**
     * vector field [x',y']
     */
    std::string str_userf;
    /**
     * gcf polynomial
     */
    std::string str_gcf;
    /**
     * turn numeric mode on/off
     */
    std::string str_numeric;
    /**
     * epsilon setting
     */
    std::string str_epsilon;
    /**
     * turn separatrice testing on/off
     */
    std::string str_testsep;
    /**
     * accuracy setting
     */
    std::string str_precision;
    /**
     * precision setting
     */
    std::string str_precision0;
    /**
     * taylor order for separatrice testing
     */
    std::string str_taylor;
    /**
     * start working numerically from this order
     */
    std::string str_numericlevel;
    /**
     * max taylor order
     */
    std::string str_maxlevel;
    /**
     * weakness level for Lyapunov constants
     */
    std::string str_weaklevel;
    /**
     * PL weight
     */
    std::string str_userp;
    /**
     * PL weight
     */
    std::string str_userq;
    /**
     * time limit for Maple execution
     */
    std::string time_limit;
};

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
 * @param prms  struct containing all the parameters for Maple evaluation
 * @return      @c true if file was successfully created, @c false otherwise
 *
 * Opens file and fills it (through calling fillMapleScript())
 */
bool prepareMapleFile(std::string &fname, mapleParamsStruct &prms);
/**
 * Fill a Maple script with the parameters and commands for evaluation
 *
 * @param f     file stream to write to
 * @param prms  struct containing all the parameters of the script
 *
 * Called from prepareMapleFile()
 */
void fillMapleScript(FILE *f, mapleParamsStruct prms);
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
 * @param fname filename of temp file to serve to client
 * @param prms  struct that contains the current parameters of execution
 * @return      @c true if file was successfully created, @c false otherwise
 *
 * This format is compatible with P4. If a user is not logged in, every
 * parameter
 * will be saved as the default parameters.
 */
bool fillSaveFile(std::string fname, mapleParamsStruct prms);

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
 * This is only called in case of Poincare-Lyapunov compactification (weights
 * (p,q) !=(1,1))
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
 * @return          @c true if file was successfully created, @c false otherwise
 *
 * This is only called in case of Poincare-Lyapunov compactification (weights
 * (p,q) !=(1,1)).
 * Same as preparegcf, except for the "u := " and "v := " assignments,
 * and the fact that one always refers to the same function VFResults.gcf,
 * and the fact that the x and y intervals are [0,1] and [0,2Pi] resp.
 */
bool prepareGcf_LyapunovR2(std::string fname, P4POLYNOM2 f, int precision,
                           int numpoints);

inline void delay(unsigned long ms);

#endif