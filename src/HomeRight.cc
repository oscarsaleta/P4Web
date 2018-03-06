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
#include "HomeRight.h"

#include "MyLogger.h"
#include "WSphere.h"
#include "file_tab.h"

#include <fstream>

#include <Wt/WLineEdit>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WScrollArea>
#include <Wt/WTabWidget>
#include <Wt/WTemplate>
#include <Wt/WTextArea>
#include <Wt/WToolBar>

using namespace Wt;

HomeRight::HomeRight(WContainerWidget *parent, ScriptHandler *s)
    : WContainerWidget(parent)
{
    setId("HomeRight");
    setStyleClass("half-box-right");
    scriptHandler_ = s;
    sphere_ = nullptr;

    plotCaption_ = nullptr;

    loggedIn_ = false;
    orbitStarted_ = false;

    g_globalLogger.debug("[HomeRight] setting up UI...");
    setupUI();
    g_globalLogger.debug("[HomeRight] setting up connectors...");
    setupConnectors();
    g_globalLogger.debug("[HomeRight] created correctly");
}

HomeRight::~HomeRight()
{
    // output tab
    if (fullResButton_ != nullptr) {
        delete fullResButton_;
        fullResButton_ = nullptr;
    }
    if (finResButton_ != nullptr) {
        delete finResButton_;
        finResButton_ = nullptr;
    }
    if (infResButton_ != nullptr) {
        delete infResButton_;
        infResButton_ = nullptr;
    }
    if (clearOutputButton_ != nullptr) {
        delete clearOutputButton_;
        clearOutputButton_ = nullptr;
    }
    if (outputButtonsToolbar_ != nullptr) {
        delete outputButtonsToolbar_;
        outputButtonsToolbar_ = nullptr;
    }
    if (outputTextArea_ != nullptr) {
        delete outputTextArea_;
        outputTextArea_ = nullptr;
    }
    if (outputContainer_ != nullptr) {
        delete outputContainer_;
        outputContainer_ = nullptr;
    }

    // plot tab
    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }
    if (plotCaption_ != nullptr) {
        delete plotCaption_;
        plotCaption_ = nullptr;
    }
    if (plotContainer_ != nullptr) {
        delete plotContainer_;
        plotContainer_ = nullptr;
    }

    // parameters tab
    if (addParamBtn_ != nullptr) {
        delete addParamBtn_;
        addParamBtn_ = nullptr;
    }
    if (delParamBtn_ != nullptr) {
        delete delParamBtn_;
        delParamBtn_ = nullptr;
    }
    std::vector<std::string>().swap(paramLabels_);
    std::vector<std::string>().swap(paramValues_);
    leLabelsVector_.clear();
    leValuesVector_.clear();
    templatesVector_.clear();
    if (paramsScrollArea_ != nullptr) {
        delete paramsScrollArea_;
        paramsScrollArea_ = nullptr;
    }

    // tab widget
    if (tabWidget_ != nullptr) {
        delete tabWidget_;
        tabWidget_ = nullptr;
    }

    g_globalLogger.debug("[HomeRight] deleted correctly");
}

void HomeRight::setupUI()
{
    tabWidget_ = new WTabWidget(this);
    tabWidget_->setId("tabWidget_");

    // output tab ----
    outputContainer_ = new WContainerWidget();
    outputContainer_->setId("outputContainer_");
    tabWidget_->addTab(outputContainer_, WString::fromUTF8("Output"),
                       WTabWidget::PreLoading);

    // text area where results are shown
    outputTextAreaContent_ = "";
    outputTextArea_ = new WTextArea(outputTextAreaContent_);
    outputTextArea_->setId("outputTextArea_");
    outputTextArea_->setReadOnly(true);
    outputTextArea_->setMinimumSize(550, 550);
    outputTextArea_->resize(WLength::Auto, 550);
    outputTextArea_->setMargin(5, Top);
    outputContainer_->addWidget(outputTextArea_);

    // buttons menu for choosing output
    outputButtonsToolbar_ = new WToolBar(outputContainer_);
    outputButtonsToolbar_->setId("outputButtonsToolbar_");
    outputButtonsToolbar_->setMargin(5, Top);
    outputButtonsToolbar_->setMargin(5, Bottom);

    fullResButton_ = new WPushButton("Full output");
    fullResButton_->setId("fullResButton_");
    fullResButton_->setStyleClass("btn-default btn");
    fullResButton_->setToolTip(WString::tr("tooltip.homeright-full-button"));
    outputButtonsToolbar_->addButton(fullResButton_);

    finResButton_ = new WPushButton("Finite");
    finResButton_->setId("finResButton_");
    finResButton_->setStyleClass("btn-default btn");
    finResButton_->setToolTip(WString::tr("tooltip.homeright-finite-button"));
    outputButtonsToolbar_->addButton(finResButton_);

    infResButton_ = new WPushButton("Infinite");
    infResButton_->setId("infResButton_");
    infResButton_->setStyleClass("btn-default btn");
    infResButton_->setToolTip(WString::tr("tooltip.homeright-infinite-button"));
    outputButtonsToolbar_->addButton(infResButton_);

    outputButtonsToolbar_->addSeparator();

    clearOutputButton_ = new WPushButton("Clear");
    clearOutputButton_->setId("clearOutputButton_");
    clearOutputButton_->setStyleClass("btn-warning btn");
    clearOutputButton_->setToolTip(
        WString::tr("tooltip.homeright-clear-button"));
    outputButtonsToolbar_->addButton(clearOutputButton_);

    // plot tab ----
    plotContainer_ = new WContainerWidget();
    plotContainer_->setId("plotContainer_");
    tabWidget_->addTab(plotContainer_, WString::fromUTF8("Plot"),
                       WTabWidget::PreLoading);

    // TODO: add plot separatrices, orbits, etc buttons?
    /*plotButtonsToolbar_ = new WToolBar(plotContainer_);
    plotButtonsToolbar_->setId("plotButtonsToolbar_");
    plotButtonsToolbar_->setMargin(5,Top);
    plotButtonsToolbar_->setMargin(5,Bottom);

    plotPointsButton_ = new WPushButton("Singular points");
    plotPointsButton_->setId("plotPointsButton_");
    plotPointsButton_->setStyleClass("btn-default btn");
    plotButtonsToolbar_->addButton(plotPointsButton_);

    plotSeparatricesButton_ = new WPushButton("Separatrices");
    plotSeparatricesButton_->setId("plotSeparatricesButton_");
    plotSeparatricesButton_->setStyleClass("btn-default btn");
    plotButtonsToolbar_->addButton(plotSeparatricesButton_);

    plotButtonsToolbar_->addSeparator();

    clearPlotButton_ = new WPushButton("Clear");
    clearPlotButton_->setId("clearPlotButton_");
    clearPlotButton_->setStyleClass("btn-warning btn");
    plotButtonsToolbar_->addButton(clearPlotButton_);

*/

    // parameters tab ------
    paramsContainer_ = new WContainerWidget();
    tabWidget_->addTab(paramsContainer_, WString::fromUTF8("Parameters"),
                       WTabWidget::LazyLoading);

    addParamBtn_ = new WPushButton("Add", paramsContainer_);
    addParamBtn_->setId("addParamBtn_");
    addParamBtn_->setStyleClass("btn btn-primary");
    addParamBtn_->setMargin(15, Top);
    addParamBtn_->setMargin(15, Bottom);
    addParamBtn_->setToolTip(WString::tr("tooltip.homeright-addparam-button"));

    delParamBtn_ = new WPushButton("Remove", paramsContainer_);
    delParamBtn_->setId("delParamBtn_");
    delParamBtn_->setStyleClass("btn btn-danger");
    delParamBtn_->setToolTip(WString::tr("tooltip.homeright-delparam-button"));
    delParamBtn_->setMargin(15, Top);
    delParamBtn_->setMargin(15, Bottom);
    delParamBtn_->setMargin(10, Left);

    paramsScrollArea_ = new WScrollArea(paramsContainer_);
    paramsScrollAreaContainer_ = new WContainerWidget();
    paramsScrollArea_->setWidget(paramsScrollAreaContainer_);
    paramsScrollArea_->setHorizontalScrollBarPolicy(
        WScrollArea::ScrollBarAlwaysOff);
    paramsScrollArea_->setMinimumSize(550, 550);
    paramsScrollArea_->resize(WLength::Auto, 550);

    tabWidget_->setCurrentIndex(0);
    tabWidget_->setTabHidden(2, true);
    g_globalLogger.debug("[HomeRight] UI set up");
}

void HomeRight::setupConnectors()
{
    // parameters buttons
    addParamBtn_->clicked().connect(this, &HomeRight::addParameter);
    delParamBtn_->clicked().connect(this, &HomeRight::delParameter);
    // output buttons
    fullResButton_->clicked().connect(this, &HomeRight::fullResults);
    finResButton_->clicked().connect(this, &HomeRight::showFinResults);
    infResButton_->clicked().connect(this, &HomeRight::showInfResults);
    clearOutputButton_->clicked().connect(this, &HomeRight::clearResults);
    // plot buttons
    /*plotPointsButton_->clicked().connect(this,&HomeRight::plotSingularPoints);
    plotSeparatricesButton_->clicked().connect(this,&HomeRight::plotSeparatrices);
    clearPlotButton_->clicked().connect(this,&HomeRight::clearPlot);*/

    g_globalLogger.debug("[HomeRight] connectors set up");
}

void HomeRight::readResults(std::string fileName)
{
    tabWidget_->setCurrentIndex(0);

    std::ifstream resultsFile;
    std::string line;

    fileName_ = fileName;

    // read full results
    fullResults_ = "";
    resultsFile.open((fileName_ + ".res").c_str());
    if (resultsFile.is_open()) {
        while (getline(resultsFile, line))
            fullResults_ += line + "\n";
        resultsFile.close();
    }
    // read finite singular points results
    finResults_ = "";
    resultsFile.open((fileName_ + "_fin.res").c_str());
    if (resultsFile.is_open()) {
        while (getline(resultsFile, line))
            finResults_ += line + "\n";
        resultsFile.close();
    }
    // add title for infinite region (missing in inf.res)
    infResults_ = "AT INFINITY \n";
    // read infinite singular points results
    resultsFile.open((fileName_ + "_inf.res").c_str());
    if (resultsFile.is_open()) {
        while (getline(resultsFile, line))
            infResults_ += line + "\n";
        resultsFile.close();
    }

    fullResults();
}

void HomeRight::printError(std::string error)
{
    fullResults_ = error;
    fullResults();
}

void HomeRight::fullResults()
{
    outputTextAreaContent_ = fullResults_;
    outputTextArea_->setText(outputTextAreaContent_);
    tabWidget_->setCurrentIndex(0);
    g_globalLogger.debug("[HomeRight] showing output panel");
}

void HomeRight::showFinResults()
{
    outputTextAreaContent_ = finResults_;
    outputTextArea_->setText(outputTextAreaContent_);
    g_globalLogger.debug("[HomeRight] showing finite results");
}

void HomeRight::showInfResults()
{
    outputTextAreaContent_ = infResults_;
    outputTextArea_->setText(outputTextAreaContent_);
    g_globalLogger.debug("[HomeRight] showing infinite results");
}

void HomeRight::clearResults()
{
    outputTextArea_->setText("");
    outputTextAreaContent_ = "";
    g_globalLogger.debug("[HomeRight] cleared output panel");
}

void HomeRight::onSpherePlot(std::string basename, double projection)
{
    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }
    sphereBasename_ = basename;
    sphere_ = new WSphere(plotContainer_, scriptHandler_, 550, 550, basename,
                          projection);
    setupSphereAndPlot();
    g_globalLogger.debug("[HomeRight] reacted to onSpherePlot signal");
}

void HomeRight::onPlanePlot(std::string basename, int type, double minx,
                            double maxx, double miny, double maxy)
{
    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }
    sphereBasename_ = basename;
    sphere_ = new WSphere(plotContainer_, scriptHandler_, 550, 550, basename,
                          type, minx, maxx, miny, maxy);
    setupSphereAndPlot();
    g_globalLogger.debug("[HomeRight] reacted to onPlanePlot signal");
}

void HomeRight::setupSphereAndPlot()
{
    sphere_->setId("sphere_");
    sphere_->setMargin(5, Top);
    plotContainer_->addWidget(sphere_);

    if (plotCaption_ != nullptr) {
        delete plotCaption_;
        plotCaption_ = nullptr;
    }

    plotCaption_ = new WText(plotContainer_);
    plotCaption_->setId("plotCaption_");
    plotContainer_->addWidget(plotCaption_);

    sphere_->hoverSignal().connect(this, &HomeRight::mouseMovedEvent);
    sphere_->errorSignal().connect(this, &HomeRight::printError);
    sphere_->clickedSignal().connect(this, &HomeRight::sphereClicked);

    sphere_->update();
    tabWidget_->setCurrentIndex(1);
}

void HomeRight::mouseMovedEvent(WString caption)
{
    plotCaption_->setText(caption);
}

void HomeRight::sphereClicked(bool clickValid, double x, double y)
{
    sphereClickedSignal_.emit(clickValid, x, y);
}

void HomeRight::onReset(int dummy)
{
    g_globalLogger.debug("[HomeRight] Deleting sphere...");
    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }

    outputTextAreaContent_ = std::string();
    outputTextArea_->setText(outputTextAreaContent_);

    if (loggedIn_) {
        g_globalLogger.debug("[HomeRight] Hiding params tab...");
        hideParamsTab();
        g_globalLogger.debug("[HomeRight] Showing params tab...");
        showParamsTab();
        tabWidget_->setCurrentIndex(2);
    } else {
        tabWidget_->setCurrentIndex(0);
    }
}

void HomeRight::onOrbitsIntegrate(int dir, double x0, double y0)
{
    if (dir == 1 || dir == -1)
        orbitStarted_ = sphere_->startOrbit(x0, y0, true);

    if (dir == 1)
        g_globalLogger.debug("[HomeRight] integrating forwards...");
    else if (dir == -1)
        g_globalLogger.debug("[HomeRight] integrating backwards...");
    else
        g_globalLogger.debug("[HomeRight] continuing integration...");

    sphere_->integrateOrbit(dir);

    // update with flag PaintUpdate so widget is not cleared before painting
    // orbit
    sphere_->update(PaintUpdate);
    if (tabWidget_->currentIndex() != 1)
        tabWidget_->setCurrentIndex(1);
}

void HomeRight::onOrbitsDelete(int flag)
{
    if (sphere_ == nullptr || sphere_->study_ == nullptr ||
        sphere_->study_->orbit_vector_.empty())
        return;

    if (flag == 0)
        sphere_->study_->orbit_vector_.clear();
    else if (flag == 1)
        sphere_->deleteLastOrbit();

    sphere_->plotDone_ = false;
    sphere_->update();
    if (tabWidget_->currentIndex() != 1)
        tabWidget_->setCurrentIndex(1);
}

void HomeRight::onGcfEval(std::string fname, int pointdash, int npoints,
                          int prec)
{
    g_globalLogger.debug("[HomeRight] received gcf signal with fname " + fname);
    if (sphere_ == nullptr)
        return;

    sphere_->gcfEval_ = true;
    sphere_->gcfDashes_ = pointdash;
    sphere_->gcfFname_ = fname;
    sphere_->gcfNPoints_ = npoints;
    sphere_->gcfPrec_ = prec;
    sphere_->plotDone_ = false;
    sphere_->update(PaintUpdate);
    if (tabWidget_->currentIndex() != 1)
        tabWidget_->setCurrentIndex(1);
}

/*void HomeRight::clearPlot()
{
    sphere_->clearPlot();
}

void HomeRight::plotSingularPoints()
{
    return;
}

void HomeRight::plotSeparatrices()
{
    return;
}*/

void HomeRight::addParameter()
{
    auto t = std::make_unique<WTemplate>(WString::tr("template.params"),
                                         paramsScrollAreaContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    auto label = std::make_unique<WLineEdit>(paramsScrollAreaContainer_);
    t->bindWidget("label", label);
    leLabelsVector_.push_back(std::move(label));

    auto value = std::make_unique<WLineEdit>(paramsScrollAreaContainer_);
    t->bindWidget("value", value);
    leValuesVector_.push_back(std::move(value));

    templatesVector_.push_back(std::move(t));
}

void HomeRight::addParameterWithValue(std::string strlabel,
                                      std::string strvalue)
{
    auto t = std::make_unique<WTemplate>(WString::tr("template.params"),
                                         paramsScrollAreaContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    auto label = std::make_unique<WLineEdit>(paramsScrollAreaContainer_);
    label->setText(strlabel);
    t->bindWidget("label", label);
    leLabelsVector_.push_back(std::move(label));

    auto value = std::make_unique<WLineEdit>(paramsScrollAreaContainer_);
    value->setText(strvalue);
    t->bindWidget("value", value);
    leValuesVector_.push_back(std::move(value));

    templatesVector_.push_back(std::move(t));

    if (tabWidget_->currentIndex() != 2)
        tabWidget_->setCurrentIndex(2);
}

void HomeRight::delParameter()
{
    if (!leLabelsVector_.empty()) {
        leLabelsVector_.pop_back();
    }
    if (!leValuesVector_.empty()) {
        leValuesVector_.pop_back();
    }
    if (!templatesVector_.empty()) {
        templatesVector_.pop_back();
    }
}

void HomeRight::showParamsTab()
{
    loggedIn_ = true;
    tabWidget_->setTabHidden(2, false);
    if (tabWidget_->currentIndex() != 2)
        tabWidget_->setCurrentIndex(2);
}

void HomeRight::hideParamsTab(bool logout)
{
    if (logout)
        loggedIn_ = false;
    /* in case there are parameters defined, remove them */
    if (!scriptHandler_->paramLabels_.empty()) {
        std::vector<std::string>().swap(scriptHandler_->paramLabels_);
    }
    if (!scriptHandler_->paramValues_.empty()) {
        std::vector<std::string>().swap(scriptHandler_->paramValues_);
    }
    leLabelsVector_.clear();
    leValuesVector_.clear();
    templatesVector_.clear();
    tabWidget_->setTabHidden(2, true);
    if (tabWidget_->currentIndex() != 2)
        tabWidget_->setCurrentIndex(2);
}

void HomeRight::refreshParamStringVectors()
{
    if (loggedIn_ == false)
        return;

    // correctly delete and free all memory from these vectors
    // if they are not empty
    if (!scriptHandler_->paramLabels_.empty()) {
        std::vector<std::string>().swap(scriptHandler_->paramLabels_);
    }
    if (!scriptHandler_->paramValues_.empty()) {
        std::vector<std::string>().swap(scriptHandler_->paramValues_);
    }

    for (auto it1 = std::begin(leLabelsVector_),
              auto it2 = std::begin(leValuesVector_),
              it1 != std::end(leLabelsVector_),
              it2 != std::end(leValuesVector_), ++it1, ++it2) {
        if ((*it1)->text().empty() || (*it2)->text().empty())
            continue;
        scriptHandler_->paramLabels_.push_back((*it1)->text().toUTF8());
        scriptHandler_->paramValues_.push_back((*it2)->text().toUTF8());
    }
}

void HomeRight::onCurvePlot(std::string fname, int pointdash, int npoints,
                            int prec)
{
    g_globalLogger.debug("[HomeRight] received curve signal with fname " +
                         fname);

    if (sphere_ == nullptr) {
        return;
        curveConfirmedSignal_.emit(false);
    }

    // 1. read curve tables
    if (!sphere_->study_->readCurve(fname)) {
        g_globalLogger.error("[HomeRight] cannot read curve");
        curveConfirmedSignal_.emit(false);
        return;
    }

    // 2. create and execute maple scripts for charts
    sphere_->curveDashes_ = pointdash;
    sphere_->curveFname_ = fname;
    sphere_->curveNPoints_ = npoints;
    sphere_->curvePrec_ = prec;
    sphere_->plotDone_ = false;

    int result =
        sphere_->evalCurveStart(sphere_->curveFname_, sphere_->curveDashes_,
                                sphere_->curveNPoints_, sphere_->curvePrec_);
    if (!result) {
        g_globalLogger.error("[HomeRight] cannot evaluate curve");
        curveConfirmedSignal_.emit(false);
        return;
    } else {
        int i = 0;
        do {
            result = sphere_->evalCurveContinue(sphere_->curveFname_,
                                                CURVE_POINTS, CURVE_PRECIS);
            if (sphere_->curveError_) {
                g_globalLogger.error("[HomeRight] error while computing "
                                     "evalCurveContinue at step: " +
                                     std::to_string(i));
                curveConfirmedSignal_.emit(false);
                return;
            }
            i++;
        } while (!result);
        // finish evaluation
        result = sphere_->evalCurveFinish();
        if (!result) {
            g_globalLogger.error("[HomeRight] error in evalCurveFinish");
            curveConfirmedSignal_.emit(false);
            return;
        } else {
            g_globalLogger.debug("[HomeRight] computed curve");
            curveConfirmedSignal_.emit(true);
        }
    }

    // 3. plot
    sphere_->update(PaintUpdate);

    // 4. Focus plot tab
    tabWidget_->setCurrentIndex(1);

    return;
}

void HomeRight::onCurvesDelete(int flag)
{
    if (sphere_ == nullptr || sphere_->study_ == nullptr ||
        sphere_->study_->curve_vector_.empty())
        return;

    if (flag == 0) {
        sphere_->study_->curve_vector_.clear();
    } else if (flag == 1)
        sphere_->study_->curve_vector_.pop_back();

    sphere_->study_->deleteOrbitPoint(sphere_->study_->last_curves_point_);
    sphere_->study_->last_curves_point_ = nullptr;

    sphere_->plotDone_ = false;
    sphere_->update();
    if (tabWidget_->currentIndex() != 1)
        tabWidget_->setCurrentIndex(1);
}

void HomeRight::onIsoclinePlot(std::string fname, int pointdash, int npoints,
                               int prec)
{
    g_globalLogger.debug("[HomeRight] received isocline signal with fname " +
                         fname);

    if (sphere_ == nullptr) {
        isoclineConfirmedSignal_.emit(false);
        return;
    }

    // 1. read isocline tables
    if (!sphere_->study_->readIsoclines(fname)) {
        g_globalLogger.error("[HomeRight] cannot read isocline");
        isoclineConfirmedSignal_.emit(false);
        return;
    }

    // 2. create and execute maple scripts for charts
    sphere_->isoclineDashes_ = pointdash;
    sphere_->isoclineFname_ = fname;
    sphere_->isoclineNPoints_ = npoints;
    sphere_->isoclinePrec_ = prec;
    sphere_->plotDone_ = false;

    int result = sphere_->evalIsoclineStart(
        sphere_->isoclineFname_, sphere_->isoclineDashes_,
        sphere_->isoclineNPoints_, sphere_->isoclinePrec_);
    if (!result) {
        g_globalLogger.error("[HomeRight] cannot evaluate isocline");
        isoclineConfirmedSignal_.emit(false);
    } else {
        int i = 0;
        do {
            result = sphere_->evalIsoclineContinue(sphere_->isoclineFname_,
                                                   CURVE_POINTS, CURVE_PRECIS);
            if (sphere_->isoclineError_) {
                g_globalLogger.error("[HomeRight] error while computing "
                                     "evalIsoclineContinue at step: " +
                                     std::to_string(i));
                isoclineConfirmedSignal_.emit(false);
                return;
            }
            i++;
        } while (!result);
        // finish evaluation
        result = sphere_->evalIsoclineFinish();
        if (!result) {
            g_globalLogger.error("[HomeRight] error in evalIsoclineFinish");
            isoclineConfirmedSignal_.emit(false);
        } else {
            g_globalLogger.debug("[HomeRight] computed isocline");
            isoclineConfirmedSignal_.emit(true);
        }
    }

    // 3. assign color and plot
    int nisocs = (sphere_->study_->isocline_vector_.size() - 1) % 4;
    sphere_->study_->isocline_vector_.back().color = CISOC + nisocs;
    sphere_->update(PaintUpdate);

    // 4. Focus plot tab
    tabWidget_->setCurrentIndex(1);
    return;
}

void HomeRight::onIsoclinesDelete(int flag)
{
    if (sphere_ == nullptr || sphere_->study_ == nullptr ||
        sphere_->study_->isocline_vector_.empty())
        return;

    if (flag == 0) {
        sphere_->study_->isocline_vector_.clear();
    } else if (flag == 1)
        sphere_->study_->isocline_vector_.pop_back();

    sphere_->study_->deleteOrbitPoint(sphere_->study_->last_isoclines_point_);
    sphere_->study_->last_isoclines_point_ = nullptr;

    sphere_->plotDone_ = false;
    sphere_->update();
    if (tabWidget_->currentIndex() != 1)
        tabWidget_->setCurrentIndex(1);
}

void HomeRight::refreshPlotSphere(double p)
{
    WVFStudy *study;
    if (sphere_->study_ != nullptr) {
        g_globalLogger.debug("[HomeRight] copying study");
        study = new WVFStudy(*(sphere_->study_));
    } else {
        g_globalLogger.debug("[HomeRight] using null study");
        study = nullptr;
    }

    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }

    sphere_ = new WSphere(plotContainer_, scriptHandler_, 550, 550,
                          sphereBasename_, p, study);
    setupSphereAndPlot();
}

void HomeRight::refreshPlotPlane(int type, double minx, double maxx,
                                 double miny, double maxy)
{
    WVFStudy *study;
    if (sphere_->study_ != nullptr) {
        g_globalLogger.debug("[HomeRight] copying study");
        study = new WVFStudy(*(sphere_->study_));
    } else {
        g_globalLogger.debug("[HomeRight] using null study");
        study = nullptr;
    }

    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }

    sphere_ = new WSphere(plotContainer_, scriptHandler_, 550, 550,
                          sphereBasename_, type, minx, maxx, miny, maxy, study);
    setupSphereAndPlot();
}
