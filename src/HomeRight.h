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

#pragma once

/*!
 * @brief Right side of the UI
 * @file HomeRight.h
 * @author Oscar Saleta
 */
#include <Wt/WContainerWidget>

#include "ScriptHandler.h"

#include <Wt/WString>

class WSphere;

/**
 * This class holds the UI from the right side of the website
 *
 * @class HomeRight
 *
 * This class derives from a WContainerWidget, and it serves as a container
 * for a WTabWidget (which is, in turn, a container widget organised by tabs).
 *
 * There are three tabs: <b>Output</b>, <b>Plot</b> and <b>Legend</b>.
 *
 * The <b>Output</b> tab has a text area where output generated by Maple is
 * shown
 * after an evaluation of a vector field, or where messages for the user
 * are printed. It also has 4 buttons: <i>Full results</i>, <i>Finite</i>,
 * <i>Infinite</i> and <i>Clear</i>. The three first buttons control
 * which results are shown in the output area, while the 4th button clears it.
 *
 * The <b>Plot</b> tab has a WPaintedWidget, which is a widget in which we can
 * paint.
 * This tab holds the Poincaré sphere plots for the phase portraits of the
 * evaluated vector field. The plotting work is done by the #WSphere class,
 * which is instantiated inside HomeRight by a private @c WSphere @c sphere_
 * object.
 *
 * The <b>Legend</b> tab simply holds a static image picked from the original
 * P4.
 * It shows the legend of the plots (kinds of singular points, etc).
 *
 * There are three methods in this class that are designed to act
 * upon the receival of a signal from #HomeLeft.
 */
class HomeRight : public Wt::WContainerWidget
{
  public:
    /**
     * Constructor method for HomeRight
     */
    HomeRight(Wt::WContainerWidget *parent = 0, ScriptHandler *s = 0);
    /**
     * Destructor method for HomeRight
     */
    ~HomeRight();

    /**
     * Read results from Maple .res files
     *
     * This method is linked in #MainUI to the evaluated signal from #HomeLeft,
     * so when a vector field is evaluated, HomeRight automatically displays
     * the results of the computations in the output text area.
     *
     * @param fname Filename where Maple has written the results of the vector
     * field evaluation
     */
    void readResults(std::string fname);
    /**
     * Print a custom message in the output area
     *
     * This method does not only print errors, even though the name suggests
     * so. It can also be used to print any other string in the output area,
     * such as suggestions or warnings.
     *
     * @param message String that will be printed in the output area
     */
    void printError(std::string message);
    /**
     * Method that triggers a plot in the plot tab
     *
     * This method is connected to the Plot button of HomeLeft by the onPlot
     * signal. Here there is no "plotting" work, everything is handled by
     * #WSphere. This method only triggers an @c update of the @c sphere_
     * object.
     *
     * @param basename      This is a filename for the Maple results file. The
     * #WSphere class needs this file to parse it and paint accordingly.
     * @param projection    projection for the sphere view
     */
    void onSpherePlot(std::string basename, double projection);
    /**
     * Method that triggers a plot in the plot tab
     *
     * This method is connected to the Plot button of HomeLeft by the onPlot
     * signal. Here there is no "plotting" work, everything is handled by
     * #WSphere. This method only triggers an @c update of the @c sphere_
     * object.
     *
     * @param basename This is a filename for the Maple results file. The
     * #WSphere class needs this file to parse it and paint accordingly.
     * @param type     type of view (R2,U1,U2,V1,V2)
     * @param minx     min x for plot range
     * @param maxx     max x for plot range
     * @param miny     min y for plot range
     * @param maxy     max y for plot range
     */
    void onPlanePlot(std::string basename, int type, double minx, double maxx,
                     double miny, double maxy);
    /**
     * React to a mouse event to set the caption of the plot
     *
     * Sets the caption to the name of the view and the coordinates of the
     * mouse cursor, if these are correct.
     *
     * Actually, the work is not done here. This function simply sets the
     * text of the caption widget to the caption WString (member of
     * the WSphere *study_ object).
     *
     * @param caption string to write as caption
     */
    void mouseMovedEvent(Wt::WString caption);
    /**
     * React to a click on the sphere by printing the coordinates below
     *
     * @param clickValid wether the click was in valid coordinates
     * @param x          x coordinate
     * @param y          y coordinate
     */
    void sphereClicked(bool clickValid, double x, double y);
    /**
     * Send signal of click on plot so we can start orbit integration
     */
    Wt::Signal<bool, double, double> &sphereClickedSignal()
    {
        return sphereClickedSignal_;
    }
    /**
     * Send signal of confirmation for computed curve
     */
    Wt::Signal<bool> &curveConfirmedSignal() { return curveConfirmedSignal_; }
    /**
     * Send signal of confirmation for computed isocline
     */
    Wt::Signal<bool> &isoclineConfirmedSignal()
    {
        return isoclineConfirmedSignal_;
    }
    /**
     * React to an orbit integration request
     *
     * The @c dir argument tells the direction of integration. If
     * dir=0, then we just continue the integration in the last
     * direction used.
     *
     * @param dir direction of integration (-1, 0 or 1)
     * @param x0  x coordinate of starting point
     * @param y0  y coordinate of starting point
     */
    void onOrbitsIntegrate(int dir, double x0, double y0);
    /**
     * React to an orbit delete request
     *
     * @param flag can be 0 (delete all) or 1 (delete last)
     */
    void onOrbitsDelete(int flag);
    /**
     * React to reset signal from HomeLeft
     *
     * Delete sphere, clean outputand reset view to output tab
     *
     * @param dummy not needed but signals cannot be empty
     */
    void onReset(int dummy);
    /**
     * React to the GCF evaluation signal
     *
     * Set GCF variables in sphere (npoints, precision and dashes)
     * and call for an update on the widget.
     *
     * @param fname     name of file used in first maple evaluation
     * @param pointdash plot points (0) or dashes (1) for gcf
     * @param npoints   number of points to plot
     * @param prec      precision of computations for zeros of gcf
     */
    void onGcfEval(std::string fname, int pointdash, int npoints, int prec);
    /**
     * React to the curve plot signal
     *
     * Set curve variables in sphere (npoints, precision, dashes), call study to
     * read the curve table, evaluate the curve in the different charts, and
     * finally plot the curve in the sphere
     *
     * @param fname     name of file used in first maple evaluation
     * @param pointdash plot points (0) or dashes (1) for curve
     * @param npoints   number of points to plot
     * @param prec      precision of computations for zeros of curve
     */
    void onCurvePlot(std::string fname, int pointdash, int npoints, int prec);
    /**
     * React to a curve delete request
     *
     * @param flag can be 0 (delete all) or 1 (delete last)
     */
    void onCurvesDelete(int flag);
    /**
     * React to the isocline plot signal
     *
     * Set isocline variables in sphere (npoints, precision, dashes), call study
     * to
     * read the isocline table, evaluate the isocline in the different charts,
     * and
     * finally plot the isocline in the sphere
     *
     * @param fname     name of file used in first maple evaluation
     * @param pointdash plot points (0) or dashes (1) for isocline
     * @param npoints   number of points to plot
     * @param prec      precision of computations for zeros of isocline
     */
    void onIsoclinePlot(std::string fname, int pointdash, int npoints,
                        int prec);
    /**
     * React to an isocline delete request
     *
     * @param flag can be 0 (delete all) or 1 (delete last)
     */
    void onIsoclinesDelete(int flag);
    /**
     * Add a parameter to the list and fill the label and value
     *
     * This function will be called from HomeLeft when reading an input
     * file with parameters.
     *
     * @param label name of the parameter
     * @param value value of the parameter
     */
    void addParameterWithValue(std::string label, std::string value);
    /**
     * Set loggedIn_ = true and show parameters tab
     */
    void showParamsTab();
    /**
     * Delete all parameters and hide the tab
     *
     * Also, if @c logout = @c true, set @c loggedIn_ to false
     *
     * @param logout boolean flag to set the login status to false if needed
     */
    void hideParamsTab(bool logout = false);
    /**
     * Create string vectors from parameter WLineEdits
     */
    void refreshParamStringVectors();
    /**
     * Refresh the plot into a sphere
     *
     * @param proj projection for the plot
     */
    void refreshPlotSphere(double proj);
    /**
     * Refresh the plot into a plane
     *
     * @param type identifies if the plot is R2, U1, U2, V1, or V2
     * @param minx min x coord
     * @param maxx max x coord
     * @param miny min y coord
     * @param maxy max y coord
     */
    void refreshPlotPlane(int type, double minx, double maxx, double miny,
                          double maxy);

    /**
     * Vector of strings for labels
     */
    std::vector<std::string> paramLabels_;

    /**
     * Vector of strings for values
     */
    std::vector<std::string> paramValues_;

  private:
    /* script handler */
    ScriptHandler *scriptHandler_;

    bool loggedIn_;
    bool orbitStarted_;
    int projection_;
    double viewMinX_;
    double viewMaxX_;
    double viewMinY_;
    double viewMaxY_;

    /* OBJECTS */

    Wt::WTabWidget *tabWidget_;

    // parameters tab
    Wt::WContainerWidget *paramsContainer_;
    Wt::WContainerWidget *paramsScrollArea_;
    Wt::WContainerWidget *paramsScrollAreaContainer_;
    Wt::WPushButton *addParamBtn_;
    Wt::WPushButton *delParamBtn_;
    std::vector<Wt::WLineEdit *> leLabelsVector_;
    std::vector<Wt::WLineEdit *> leValuesVector_;
    std::vector<Wt::WTemplate *> templatesVector_;

    // output tab
    Wt::WContainerWidget *outputContainer_;
    Wt::WTextArea *outputTextArea_;
    Wt::WString outputTextAreaContent_;
    Wt::WToolBar *outputButtonsToolbar_;
    std::unique_ptr<Wt::WPushButton> fullResButton_;
    std::unique_ptr<Wt::WPushButton> finResButton_;
    std::unique_ptr<Wt::WPushButton> infResButton_;
    std::unique_ptr<Wt::WPushButton> clearOutputButton_;

    std::string fileName_;
    std::string fullResults_;
    std::string finResults_;
    std::string infResults_;

    // plot tab
    std::unique_ptr<WSphere> sphere_;
    std::string sphereBasename_;

    Wt::WContainerWidget *plotContainer_;
    Wt::WText *plotCaption_;
    /*Wt::WToolBar            *plotButtonsToolbar_;
    Wt::WPushButton         *clearPlotButton_;
    Wt::WPushButton         *plotPointsButton_;
    Wt::WPushButton         *plotSeparatricesButton_;*/

    /* FUNCTIONS */

    void setupUI();
    void setupConnectors();

    // parameters functions
    void addParameter();
    void delParameter();

    // output functions
    void fullResults();
    void showFinResults();
    void showInfResults();
    void clearResults();

    // plot functions
    void setupSphereAndPlot();

    void sphereClicked(Wt::WMouseEvent e);

    /*void plotSingularPoints();
    void plotSeparatrices();
    void clearPlot();*/

    /* SIGNALS */

    Wt::Signal<bool, double, double> sphereClickedSignal_;
    Wt::Signal<bool> curveConfirmedSignal_;
    Wt::Signal<bool> isoclineConfirmedSignal_;
};
