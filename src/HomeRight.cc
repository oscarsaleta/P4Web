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

#include "delete_pointer.h"
#include "file_tab.h"
#include "MyLogger.h"
#include "WSphere.h"

#include <fstream>
#include <iostream>
#include <string>

#include <Wt/WBreak>
#include <Wt/WEvent>
#include <Wt/WGroupBox>
#include <Wt/WImage>
#include <Wt/WLineEdit>
#include <Wt/WMenuItem>
#include <Wt/WPushButton>
#include <Wt/WScrollArea>
#include <Wt/WTabWidget>
#include <Wt/WTemplate>
#include <Wt/WToolBar>

using namespace Wt;

HomeRight::HomeRight(WContainerWidget *parent)
    : WContainerWidget(parent), plotCaption_(nullptr), sphere_(nullptr),
      orbitStarted_(false), loggedIn_(false)
{
    setId("HomeRight");
    setStyleClass("half-box-right");

    globalLogger__.debug("HomeRight :: setting up UI...");
    setupUI();
    globalLogger__.debug("HomeRight :: setting up connectors...");
    setupConnectors();

    // tabWidget_->setCurrentIndex(0);

    /* vectors for parameters */
    /*templatesVector_ = new std::vector<WTemplate *>();
    leLabelsVector_ = new std::vector<WLineEdit *>();
    leValuesVector_ = new std::vector<WLineEdit *>();*/

    globalLogger__.debug("HomeRight :: created correctly");
}

HomeRight::~HomeRight()
{
    // output tab
    delete_pointer(fullResButton_);
    delete_pointer(finResButton_);
    delete_pointer(infResButton_);
    delete_pointer(clearOutputButton_);
    delete_pointer(outputButtonsToolbar_);
    delete_pointer(outputTextArea_);
    delete_pointer(outputContainer_);

    // plot tab
    delete_pointer(sphere_);
    delete_pointer(plotCaption_);
    delete_pointer(plotContainer_);

    // parameters tab
    delete_pointer(addParamBtn_);
    delete_pointer(delParamBtn_);
    std::vector<std::string>().swap(paramLabels_);
    std::vector<std::string>().swap(paramValues_);
    leLabelsVector_.clear();
    leValuesVector_.clear();
    templatesVector_.clear();
    delete_pointer(paramsScrollArea_);

    // tab widget
    delete_pointer(tabWidget_);


    globalLogger__.debug("HomeRight :: deleted correctly");
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
    globalLogger__.debug("HomeRight :: UI set up");
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

    globalLogger__.debug("HomeRight :: connectors set up");
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
    globalLogger__.debug("HomeRight :: showing output panel");
}

void HomeRight::showFinResults()
{
    outputTextAreaContent_ = finResults_;
    outputTextArea_->setText(outputTextAreaContent_);
    globalLogger__.debug("HomeRight :: showing finite results");
}

void HomeRight::showInfResults()
{
    outputTextAreaContent_ = infResults_;
    outputTextArea_->setText(outputTextAreaContent_);
    globalLogger__.debug("HomeRight :: showing infinite results");
}

void HomeRight::clearResults()
{
    outputTextArea_->setText("");
    outputTextAreaContent_ = "";
    globalLogger__.debug("HomeRight :: cleared output panel");
}

void HomeRight::onSpherePlot(std::string basename, double projection)
{
    delete_pointer(sphere_);

    sphere_ = new WSphere(plotContainer_, 550, 550, basename, projection);
    setupSphereAndPlot();
}

void HomeRight::onPlanePlot(std::string basename, int type, double minx,
                            double maxx, double miny, double maxy)
{
    delete_pointer(sphere_);

    sphere_ = new WSphere(plotContainer_, 550, 550, basename, type, minx, maxx,
                          miny, maxy);
    setupSphereAndPlot();
}

void HomeRight::setupSphereAndPlot()
{
    sphere_->setId("sphere_");
    sphere_->setMargin(5, Top);
    plotContainer_->addWidget(sphere_);

    delete_pointer(plotCaption_);

    plotCaption_ = new WText(plotContainer_);
    plotCaption_->setId("plotCaption_");
    plotContainer_->addWidget(plotCaption_);

    sphere_->hoverSignal().connect(this, &HomeRight::mouseMovedEvent);
    sphere_->errorSignal().connect(this, &HomeRight::printError);
    sphere_->clickedSignal().connect(this, &HomeRight::sphereClicked);

    sphere_->update();
    tabWidget_->setCurrentIndex(1);
    globalLogger__.debug("HomeRight :: reacted to onPlot signal");
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
    delete_pointer(sphere_);

    outputTextAreaContent_ = std::string();
    outputTextArea_->setText(outputTextAreaContent_);

    if (loggedIn_)
        tabWidget_->setCurrentIndex(2);
    else
        tabWidget_->setCurrentIndex(0);
}

void HomeRight::onOrbitsIntegrate(int dir, double x0, double y0)
{
    if (dir == 1 || dir == -1)
        orbitStarted_ = sphere_->startOrbit(x0, y0, true);

    if (dir == 1)
        globalLogger__.debug("HomeRight :: integrating forwards...");
    else if (dir == -1)
        globalLogger__.debug("HomeRight :: integrating backwards...");
    else
        globalLogger__.debug("HomeRight :: continuing integration...");

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
        sphere_->study_->first_orbit == nullptr ||
        sphere_->study_->current_orbit == nullptr)
        return;

    if (flag == 0) {
        sphere_->study_->deleteOrbit(sphere_->study_->first_orbit);
        sphere_->study_->first_orbit = nullptr;
    } else if (flag == 1)
        sphere_->deleteLastOrbit();

    sphere_->plotDone_ = false;
    sphere_->update();
    if (tabWidget_->currentIndex() != 1)
        tabWidget_->setCurrentIndex(1);
}

void HomeRight::onGcfEval(std::string fname, int pointdash, int npoints,
                          int prec)
{
    globalLogger__.debug("HomeRight :: received gcf signal with fname " +
                         fname);
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
    WTemplate *t = new WTemplate(WString::tr("template.params"),
                                 paramsScrollAreaContainer_);
    t->addFunction("id", WTemplate::Functions::id);
    templatesVector_.push_back(boost::shared_ptr<WTemplate>(t));

    WLineEdit *label = new WLineEdit(paramsScrollAreaContainer_);
    leLabelsVector_.push_back(boost::shared_ptr<WLineEdit>(label));
    t->bindWidget("label", label);
    WLineEdit *value = new WLineEdit(paramsScrollAreaContainer_);
    leValuesVector_.push_back(boost::shared_ptr<WLineEdit>(value));
    t->bindWidget("value", value);
}

void HomeRight::addParameterWithValue(std::string strlabel,
                                      std::string strvalue)
{
    WTemplate *t = new WTemplate(WString::tr("template.params"),
                                 paramsScrollAreaContainer_);
    t->addFunction("id", WTemplate::Functions::id);
    templatesVector_.push_back(boost::shared_ptr<WTemplate>(t));

    WLineEdit *label = new WLineEdit(paramsScrollAreaContainer_);
    label->setText(strlabel);
    leLabelsVector_.push_back(boost::shared_ptr<WLineEdit>(label));
    t->bindWidget("label", label);
    WLineEdit *value = new WLineEdit(paramsScrollAreaContainer_);
    value->setText(strvalue);
    leValuesVector_.push_back(boost::shared_ptr<WLineEdit>(value));
    t->bindWidget("value", value);

    if (tabWidget_->currentIndex() != 2)
        tabWidget_->setCurrentIndex(2);
}

void HomeRight::delParameter()
{
    if (!leLabelsVector_.empty()) {
        boost::shared_ptr<WLineEdit> label = leLabelsVector_.back();
        if (label != nullptr) {
            label.reset();
            leLabelsVector_.pop_back();
        }
    }
    if (!leValuesVector_.empty()) {
        boost::shared_ptr<WLineEdit> value = leValuesVector_.back();
        if (value != nullptr) {
            value.reset();
            leValuesVector_.pop_back();
        }
    }
    if (!templatesVector_.empty()) {
        boost::shared_ptr<WTemplate> t = templatesVector_.back();
        if (t != nullptr) {
            t.reset();
            templatesVector_.pop_back();
        }
    }
}

void HomeRight::showParamsTab()
{
    loggedIn_ = true;
    tabWidget_->setTabHidden(2, false);
    if (tabWidget_->currentIndex() != 2)
        tabWidget_->setCurrentIndex(2);
}

void HomeRight::hideParamsTab()
{
    loggedIn_ = false;
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
    if (paramLabels_.size() != 0) {
        std::vector<std::string>().swap(paramLabels_);
    }
    if (paramValues_.size() != 0) {
        std::vector<std::string>().swap(paramValues_);
    }

    std::vector<boost::shared_ptr<WLineEdit>>::const_iterator it1;
    std::vector<boost::shared_ptr<WLineEdit>>::const_iterator it2;
    for (it1 = leLabelsVector_.begin(), it2 = leValuesVector_.begin();
         it1 != leLabelsVector_.end(), it2 != leValuesVector_.end();
         it1++, it2++) {
        if ((*it1)->text().empty() || (*it2)->text().empty())
            continue;
        paramLabels_.push_back((*it1)->text().toUTF8());
        paramValues_.push_back((*it2)->text().toUTF8());
    }
}
