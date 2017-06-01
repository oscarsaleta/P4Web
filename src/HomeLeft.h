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

/*!
 * @brief Left side of UI
 * @file HomeLeft.h
 * @author Oscar Saleta
 */

#include "MainUI.h"
#include "ScriptHandler.h"

#include <Wt/WContainerWidget>
#include <Wt/WSignal>

/**
 * Maximum number of parameters
 */
#define PARAMS_MAX 99
/**
 * Minimum value for accuracy setting
 */
#define ACCURACY_MIN 1
/**
 * Maximum value for accuracy setting
 */
#define ACCURACY_MAX 14
/**
 * Default value for accuracy setting
 */
#define ACCURACY_DEFAULT 8
/**
 * Minimum value for precision setting
 */
#define PRECISION_MIN 0
/**
 * Maximum value for precision setting
 */
#define PRECISION_MAX 15
/**
 * Default value for precision setting
 */
#define PRECISION_DEFAULT 0
/**
 * Minimum value for epsilon setting
 */
#define EPSILON_MIN 0.01
/**
 * Maximum value for epsilon setting
 */
#define EPSILON_MAX 0.3
/**
 * Default value for epsilon setting
 */
#define EPSILON_DEFAULT 0.01
/**
 * Minimum value for level of approximation setting
 */
#define APPROX_MIN 1
/**
 * Maximum value for level of approximation setting
 */
#define APPROX_MAX 10
/**
 * Default value for level of approximation setting
 */
#define APPROX_DEFAULT 6
/**
 * Minimum value for level of numeric level setting
 */
#define NUMERIC_MIN 5
/**
 * Maximum value for level of numeric level setting
 */
#define NUMERIC_MAX 15
/**
 * Default value for level of numeric level setting
 */
#define NUMERIC_DEFAULT 10
/**
 * Minimum value for level of max Taylor order setting
 */
#define MAXIMUM_MIN 15
/**
 * Maximum value for level of max Taylor order setting
 */
#define MAXIMUM_MAX 25
/**
 * Default value for level of max Taylor order setting
 */
#define MAXIMUM_DEFAULT 20
/**
 * Minimum value for level of weakness level setting
 */
#define WEAKNESS_MIN 0
/**
 * Maximum value for level of weakness level setting
 */
#define WEAKNESS_MAX 8
/**
 * Default value for level of weakness level setting
 */
#define WEAKNESS_DEFAULT 4
/**
 * Minimum value for level of Poincaré-Lyapunov weights setting
 */
#define PQ_MIN 1
/**
 * Maximum value for level of Poincaré-Lyapunov weights setting
 */
#define PQ_MAX 10
/**
 * Default value for level of Poincaré-Lyapunov weights setting
 */
#define PQ_DEFAULT 1

/**
 * Default projection setting for sphere plot
 */
#define PROJECTION_DEFAULT -1

/**
 * Minimum value for number of points in gcf
 */
#define GCF_NP_MIN 40
/**
 * Maximum value for number of points in gcf
 */
#define GCF_NP_MAX 40000
/**
 * Default value for number of points in gcf
 */
#define GCF_NP_DEFAULT 40
/**
 * Minimum value for zero precision in gcf
 */
#define GCF_PREC_MIN 8
/**
 * Maximum value for zero precision in gcf
 */
#define GCF_PREC_MAX 16
/**
 * Default value for zero precision in gcf
 */
#define GCF_PREC_DEFAULT 12

/**
 * Minimum value for number of points in curves
 */
#define CURVES_NP_MIN 40
/**
 * Maximum value for number of points in curves
 */
#define CURVES_NP_MAX 40000
/**
 * Default value for number of points in curves
 */
#define CURVES_NP_DEFAULT 400
/**
 * Minimum value for zero precision in curves
 */
#define CURVES_PREC_MIN 8
/**
 * Maximum value for zero precision in curves
 */
#define CURVES_PREC_MAX 16
/**
 * Default value for zero precision in curves
 */
#define CURVES_PREC_DEFAULT 12

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
    HomeLeft(Wt::WContainerWidget *parent = 0,
             ScriptHandler *scriptHandler = 0);
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
    void showOrbitsDialog(bool clickValid, double x, double y);

    /**
     * Method that sends a signal when a vector field is evaluated by Maple
     */
    Wt::Signal<std::string> &evaluatedSignal() { return evaluatedSignal_; }
    /**
     * Method that sends a signal to print some message in the output text area
     * from #HomeRight
     */
    Wt::Signal<std::string> &textSignal() { return textSignal_; }
    /**
     * Method that sends a signal when the plot button is pressed in order to
     * display a plot
     *
     * This specific signal is sent when a sphere plot is issued
     */
    Wt::Signal<std::string, double> &onPlotSphereSignal()
    {
        return onPlotSphereSignal_;
    }
    /**
     * Method that sends a signal when the plot button is pressed in order to
     * display a plot
     *
     * This specific signal is sent when a plane or chart plot is issued
     */
    Wt::Signal<std::string, int, double, double, double, double> &
    onPlotPlaneSignal()
    {
        return onPlotPlaneSignal_;
    }
    /**
     * Orbit integration signal
     *
     * The int can be -1 (backwards), 0 (continue) or 1 (forwards). The doubles
     * are the
     * coordinates
     */
    Wt::Signal<int, double, double> &orbitIntegrateSignal()
    {
        return orbitIntegrateSignal_;
    }
    /**
     * Signal to delete orbits
     *
     * The int can be 1 (delete last) or 0 (delete all)
     */
    Wt::Signal<int> &orbitDeleteSignal() { return orbitDeleteSignal_; }
    /**
     * Signal to reset everything
     *
     * We need to send a signal so HomeRight also resets its UI.
     * The int is just a dummy value because we cannot sent empty
     * signals for some reason.
     */
    Wt::Signal<int> &resetSignal() { return resetSignal_; }
    /**
     * Signal to compute gcf
     *
     * Only sent if there is a gcf. Makes HomeRight call the
     * needed functions of the sphere to compute the gcf
     */
    Wt::Signal<std::string, int, int, int> &gcfSignal() { return gcfSignal_; }
    /**
     * Signal to add a pair (label,value) to the parameters list
     *
     * The parameters list is held in HomeRight
     */
    Wt::Signal<std::string, std::string> &addParameterSignal()
    {
        return addParameterSignal_;
    }
    /**
     * Signal to plot curve
     *
     * Is sent after pressing plot. This causes the curve table to be read, and
     * then to perform all the chained executions for different charts, and
     * finally to plot the curve.
     */
    Wt::Signal<std::string, int, int, int> &plotCurveSignal()
    {
        return plotCurveSignal_;
    }

    /* MainUI parent */
    MainUI *parent_;

  private:
    bool loggedIn_;       // tells if a user is logged in
    bool evaluated_;      // tells if the vf has been evaluated
    bool evaluatedCurve_; // tells if a curve has been evaluated
    int nParams_;         // tells number of parameters added by user

    /* Script Handler */
    ScriptHandler *scriptHandler_;

    /* PUBLIC UI (no need to log in) */
    Wt::WGroupBox *equationsBox_;
    Wt::WFileUpload *fileUploadWidget_;
    std::string fileUploadName_;
    Wt::WLineEdit *xEquationInput_;
    Wt::WLineEdit *yEquationInput_;
    Wt::WLineEdit *gcfEquationInput_;
    Wt::WPushButton *evalButton_;
    Wt::WPushButton *plotButton_;
    Wt::WPushButton *prepSaveButton_;
    Wt::WPushButton *resetButton_;

    Wt::WAnchor *saveAnchor_;
    std::string saveFileName_;
    Wt::WFileResource *saveFileResource_;

    Wt::WTabWidget *tabs_;

    /* PRIVATE UI (log in needed) */
    // evaluation parameters tab
    Wt::WContainerWidget *settingsContainer_;
    Wt::WButtonGroup *calculationsBtnGroup_;
    enum Calculations { Algebraic = 0, Numeric = 1 };
    Wt::WButtonGroup *separatricesBtnGroup_;
    enum Separatrices { Yes = 0, No = 1 };
    Wt::WSpinBox *accuracySpinBox_;
    Wt::WSpinBox *precisionSpinBox_;
    Wt::WDoubleSpinBox *epsilonSpinBox_;
    Wt::WSpinBox *levAppSpinBox_;
    Wt::WSpinBox *numericLevelSpinBox_;
    Wt::WSpinBox *maxLevelSpinBox_;
    Wt::WSpinBox *maxWeakLevelSpinBox_;
    Wt::WSpinBox *PLWeightPSpinBox_;
    Wt::WSpinBox *PLWeightQSpinBox_;
    // view settings tab
    Wt::WContainerWidget *viewContainer_;
    Wt::WComboBox *viewComboBox_;
    Wt::WLineEdit *viewProjection_;
    Wt::WLineEdit *viewMinX_;
    Wt::WLineEdit *viewMinY_;
    Wt::WLineEdit *viewMaxX_;
    Wt::WLineEdit *viewMaxY_;
    // orbits tab
    Wt::WContainerWidget *orbitsContainer_;
    Wt::WLineEdit *orbitsXLineEdit_;
    Wt::WLineEdit *orbitsYLineEdit_;
    Wt::WPushButton *orbitsForwardsBtn_;
    Wt::WPushButton *orbitsContinueBtn_;
    Wt::WPushButton *orbitsBackwardsBtn_;
    Wt::WPushButton *orbitsDeleteOneBtn_;
    Wt::WPushButton *orbitsDeleteAllBtn_;
    bool orbitsStartSelected_;
    // gcf tab
    Wt::WContainerWidget *gcfContainer_;
    Wt::WButtonGroup *gcfAppearanceBtnGrp_;
    enum Appearance { Dots = 0, Dashes = 1 };
    Wt::WSpinBox *gcfNPointsSpinBox_;
    Wt::WSpinBox *gcfPrecisionSpinBox_;
    Wt::WPushButton *gcfPlotBtn_;
    // curves tab
    Wt::WContainerWidget *curvesContainer_;
    Wt::WLineEdit *curvesLineEdit_;
    Wt::WButtonGroup *curvesAppearanceBtnGrp_;
    Wt::WSpinBox *curvesNPointsSpinBox_;
    Wt::WSpinBox *curvesPrecisionSpinBox_;
    Wt::WPushButton *curvesEvalBtn_;
    Wt::WPushButton *curvesPlotBtn_;
    Wt::WPushButton *curvesDelOneBtn_;
    Wt::WPushButton *curvesDelAllBtn_;

    /* SIGNALS */
    Wt::Signal<std::string> evaluatedSignal_;
    Wt::Signal<std::string> textSignal_;
    Wt::Signal<std::string, double> onPlotSphereSignal_;
    Wt::Signal<std::string, int, double, double, double, double>
        onPlotPlaneSignal_;
    Wt::Signal<int, double, double> orbitIntegrateSignal_;
    Wt::Signal<int> orbitDeleteSignal_;
    Wt::Signal<int> resetSignal_;
    Wt::Signal<std::string, int, int, int> gcfSignal_;
    Wt::Signal<std::string, std::string> addParameterSignal_;
    Wt::Signal<std::string, int, int, int> plotCurveSignal_;

    /* FUNCTIONS */
    // sets up public UI
    void setupUI();
    // sets up connectors for buttons, forms, etc
    void setupConnectors();
    // resets all options and parameters
    void resetUI();
    // what to do when file is uploaded
    void fileUploaded();
    // what to do when uploaded file is too large
    void fileTooLarge();
    // read uploaded file
    void parseInputFile();
    // add parameter to homeright list
    void addParameterToList(std::string label, std::string value);
    // run maple on the script
    void evaluate();
    // write a tmp save file in server for download
    void prepareSaveFile();
    void allowSaveFile();
    // what to do when plot button is pressed
    void onPlot();
    // set default/widget evaluation parameters
    void setOptions();
    // react to button presses in orbits tab
    void onOrbitsDialogChange();
    void onOrbitsForwardsBtn();
    void onOrbitsBackwardsBtn();
    void onOrbitsContinueBtn();
    void onOrbitsDeleteOneBtn();
    void onOrbitsDeleteAllBtn();
    // react to button presses in gcf tab
    void onPlotGcfBtn();
    // show an error message box
    void showErrorBox(Wt::WString message);
    // react to button clicks in curves tab
    void onEvalCurvesBtn();
    void onPlotCurvesBtn();
};

#endif // HOMELEFT_H