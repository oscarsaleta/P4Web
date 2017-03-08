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

#ifndef HOMELEFT_H
#define HOMELEFT_H

#include "ScriptHandler.h"

#define ACCURACY_DEFAULT 8
#define PRECISION_DEFAULT 0
#define EPSILON_DEFAULT 0.01
#define APPROX_DEFAULT 6
#define NUMERIC_DEFAULT 10
#define MAXIMUM_DEFAULT 20
#define WEAKNESS_DEFAULT 4
#define P_DEFAULT 1
#define Q_DEFAULT 1


/*!
 * @brief Left side of UI
 * @file HomeLeft.h
 * @author Oscar Saleta
 */
#include <Wt/WContainerWidget>


#include <Wt/WSignal>

/** 
 * This class holds the UI from the left side of the website
 * 
 * @class HomeLeft
 * 
 * This class derives from a WContainerWidget, and it serves as a container
 * for several elements, including an upload widget, two input forms for
 * entering the vector field (x' and y'), and three buttons (evaluate,
 * plot and save).
 *
 * When a button is pressed, a signal is sent and #HomeRight receives it
 * and acts accordingly. The connection between HomeLeft and HomeRight is
 * made in #MainUI.
 */
class HomeLeft : public Wt::WContainerWidget
{
public:
    /** 
     * Constructor method for HomeLeft
     * @param parent Parent container widget
     */
    HomeLeft(Wt::WContainerWidget *parent = 0);
    /** 
     * Destructor method for HomeLeft
     */
    ~HomeLeft();

    /**
     * Show additional settings for a logged in user
     */
    void showSettings();
    /**
     * Hide additional settings (when a user logs out)
     */
    void hideSettings();

    /**
     * Show orbit dialog when plot is clicked
     *
     * Additionally, fill x0 and y0 forms with the coordinates for the
     * point, if these are valid coordinates for the current view.
     * 
     * @param clickValid @c true if coords are valid, @c false otherwise
     * @param x          x coordinate
     * @param y          y coordinate
     */
    void showOrbitsDialog( bool clickValid, double x, double y );

    /**
     * Method that sends a signal when a vector field is evaluated by Maple
     */
    Wt::Signal<std::string>& evaluatedSignal() {  return evaluatedSignal_; }
    /**
     * Method that sends a signal to print some message in the output text area from #HomeRight
     */
    Wt::Signal<std::string>& errorSignal() { return errorSignal_; }
    /**
     * Method that sends a signal when the plot button is pressed in order to display a plot
     *
     * This specific signal is sent when a sphere plot is issued
     */
    Wt::Signal<std::string,double>& onPlotSphereSignal() { return onPlotSphereSignal_; }
    /**
     * Method that sends a signal when the plot button is pressed in order to display a plot
     *
     * This specific signal is sent when a plane or chart plot is issued
     */
    Wt::Signal<std::string,int,double,double,double,double>& onPlotPlaneSignal() { return onPlotPlaneSignal_; }
    /**
     * Orbit integration signal
     *
     * The int can be -1 (backwards), 0 (continue) or 1 (forwards). The doubles are the
     * coordinates
     */
    Wt::Signal<int,double,double>& orbitIntegrateSignal() { return orbitIntegrateSignal_; }
    /**
     * Signal to delete orbits
     *
     * The int can be 1 (delete last) or 0 (delete all)
     */
    Wt::Signal<int>& orbitDeleteSignal() { return orbitDeleteSignal_; }
    /**
     * Signal to reset everything
     *
     * We need to send a signal so HomeRight also resets its UI.
     * The int is just a dummy value because we cannot sent empty
     * signals for some reason.
     */
    Wt::Signal<int>& resetSignal() { return resetSignal_; }
    /**
     * Signal to compute gcf
     *
     * Only sent if there is a gcf. Makes HomeRight call the 
     * needed functions of the sphere to compute the gcf
     */
    Wt::Signal<std::string,int,int,int>& gcfSignal() { return gcfSignal_; }

    /* MAPLE FILE PARAMETERS */
    mapleParamsStruct mplParams; 

private:
    bool evaluated_;

    /* PUBLIC UI (no need to log in) */
    Wt::WGroupBox       *equationsBox_;
    Wt::WFileUpload     *fileUploadWidget_;
    std::string         fileUploadName_;
    Wt::WLineEdit       *xEquationInput_;
    Wt::WLineEdit       *yEquationInput_;
    Wt::WLineEdit       *gcfEquationInput_;
    Wt::WPushButton     *evalButton_;
    Wt::WPushButton     *plotButton_;
    Wt::WPushButton     *prepSaveButton_;
    Wt::WPushButton     *resetButton_;

    Wt::WAnchor         *saveAnchor_;
    std::string         saveFileName_;
    Wt::WFileResource   *saveFileResource_;

    Wt::WTabWidget      *tabs_;

    /* PRIVATE UI (log in needed) */
    // evaluation parameters
    Wt::WContainerWidget    *settingsContainer_;
    Wt::WButtonGroup        *calculationsBtnGroup_;
    enum Calculations       { Algebraic = 0, Numeric = 1 };
    Wt::WButtonGroup        *separatricesBtnGroup_;
    enum Separatrices       { Yes = 0, No = 1 };
    Wt::WSpinBox            *accuracySpinBox_;
    Wt::WSpinBox            *precisionSpinBox_;
    Wt::WDoubleSpinBox      *epsilonSpinBox_;
    Wt::WSpinBox            *levAppSpinBox_;
    Wt::WSpinBox            *numericLevelSpinBox_;
    Wt::WSpinBox            *maxLevelSpinBox_;
    Wt::WSpinBox            *maxWeakLevelSpinBox_;
    Wt::WSpinBox            *PLWeightPSpinBox_;
    Wt::WSpinBox            *PLWeightQSpinBox_;
    // view settings
    Wt::WContainerWidget    *viewContainer_;
    Wt::WComboBox           *viewComboBox_;
    Wt::WLineEdit           *viewProjection_;
    Wt::WLineEdit           *viewMinX_;
    Wt::WLineEdit           *viewMinY_;
    Wt::WLineEdit           *viewMaxX_;
    Wt::WLineEdit           *viewMaxY_;
    // orbits dialog
    Wt::WContainerWidget    *orbitsContainer_;
    Wt::WLineEdit           *orbitsXLineEdit_;
    Wt::WLineEdit           *orbitsYLineEdit_;
    Wt::WPushButton         *orbitsForwardsBtn_;
    Wt::WPushButton         *orbitsContinueBtn_;
    Wt::WPushButton         *orbitsBackwardsBtn_;
    Wt::WPushButton         *orbitsDeleteOneBtn_;
    Wt::WPushButton         *orbitsDeleteAllBtn_;
    bool                    orbitsStartSelected_;
    // gcf dialog
    Wt::WContainerWidget    *gcfContainer_;
    Wt::WButtonGroup        *gcfAppearanceBtnGrp_;
    enum Appearance         { Dots = 0, Dashes = 1 };
    Wt::WSpinBox            *gcfNPointsSpinBox_;
    Wt::WSpinBox            *gcfPrecisionSpinBox_;
    Wt::WPushButton         *gcfPlotBtn_;


    /* SIGNALS */
    Wt::Signal<std::string> evaluatedSignal_;
    Wt::Signal<std::string> errorSignal_;
    Wt::Signal<std::string,double> onPlotSphereSignal_;
    Wt::Signal<std::string,int,double,double,double,double> onPlotPlaneSignal_;
    Wt::Signal<int,double,double> orbitIntegrateSignal_;
    Wt::Signal<int> orbitDeleteSignal_;
    Wt::Signal<int> resetSignal_;
    Wt::Signal<std::string,int,int,int> gcfSignal_;

    /* FUNCTIONS */
    // sets up public UI
    void setupUI();
    // sets up connectors for buttons, forms, etc
    void setupConnectors();
    // resets all parameters
    void resetUI();
    // what to do when file is uploaded
    void fileUploaded();
    // what to do when uploaded file is too large
    void fileTooLarge();
    // read uploaded file
    void parseInputFile();
    // run maple on the script
    void evaluate();
    // write a tmp save file in server for download
    void prepareSaveFile();
    void allowSaveFile();
    // what to do when plot button is pressed
    void onPlot();
    // set default/widget evaluation parameters
    void setParams();
    // react to button presses in orbits tab
    void onOrbitsForwardsBtn();
    void onOrbitsBackwardsBtn();
    void onOrbitsContinueBtn();
    void onOrbitsDeleteOneBtn();
    void onOrbitsDeleteAllBtn();
    // react to button presses in gcf tab
    void onPlotGcfBtn();

    bool loggedIn_;

};

#endif // HOMELEFT_H