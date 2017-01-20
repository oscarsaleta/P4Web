#include "HomeRight.h"

#include "file_tab.h"
#include "MyLogger.h"

#include <iostream>
#include <fstream>
#include <string>

#include <Wt/WBreak>
#include <Wt/WGroupBox>
#include <Wt/WMenuItem>

using namespace Wt;


HomeRight::HomeRight(WContainerWidget *parent) : WContainerWidget(parent)
{
    setId("HomeRight");
    setStyleClass("half-box-right");


    globalLogger__.debug("HomeRight :: setting up UI...");
    setupUI();
    globalLogger__.debug("HomeRight :: setting up connectors...");
    setupConnectors();

    tabWidget_->setCurrentIndex(0);

    globalLogger__.debug("HomeRight :: created correctly");
}

HomeRight::~HomeRight()
{
    /*
    // sphere
    delete sphere_;
    // output tab
    delete fullResButton_;
    delete finResButton_;
    delete infResButton_;
    delete clearOutputButton_;
    delete outputButtonsToolbar_;
    delete outputTextArea_;
    delete outputContainer_;*/
    // plot tab
    /*delete clearPlotButton_;
    delete plotPointsButton_;
    delete plotSeparatricesButton_;
    delete plotButtonsToolbar_;*/
    /*delete plotContainer_;
    // legend tab
    delete legend_;
    delete legendContainer_;
    // tab widget
    delete tabWidget_;*/

    globalLogger__.debug("HomeRight :: deleted correctly");
}

void HomeRight::setupUI()
{
    tabWidget_ = new WTabWidget(this);
    tabWidget_->setId("tabWidget_");

    // output tab ----
    outputContainer_ = new WContainerWidget();
    outputContainer_->setId("outputContainer_");
    tabWidget_->addTab(outputContainer_,WString::fromUTF8("Output"),WTabWidget::PreLoading);

    // text area where results are shown
    outputTextAreaContent_ = "";
    outputTextArea_ = new WTextArea(outputTextAreaContent_);
    outputTextArea_->setId("outputTextArea_");
    outputTextArea_->setReadOnly(true);
    outputTextArea_->resize(550,550);
    outputTextArea_->setMargin(5,Top);
    outputContainer_->addWidget(outputTextArea_);

    // buttons menu for choosing output
    outputButtonsToolbar_ = new WToolBar(outputContainer_);
    outputButtonsToolbar_->setId("outputButtonsToolbar_");
    outputButtonsToolbar_->setMargin(5,Top);
    outputButtonsToolbar_->setMargin(5,Bottom);

    fullResButton_ = new WPushButton("Full output");
    fullResButton_->setId("fullResButton_");
    fullResButton_->setStyleClass("btn-default btn");
    outputButtonsToolbar_->addButton(fullResButton_);

    finResButton_ = new WPushButton("Finite");
    finResButton_->setId("finResButton_");
    finResButton_->setStyleClass("btn-default btn");
    outputButtonsToolbar_->addButton(finResButton_);

    infResButton_ = new WPushButton("Infinite");
    infResButton_->setId("infResButton_");
    infResButton_->setStyleClass("btn-default btn");
    outputButtonsToolbar_->addButton(infResButton_);

    outputButtonsToolbar_->addSeparator();

    clearOutputButton_ = new WPushButton("Clear");
    clearOutputButton_->setId("clearOutputButton_");
    clearOutputButton_->setStyleClass("btn-warning btn");
    outputButtonsToolbar_->addButton(clearOutputButton_);


    // plot tab ----
    plotContainer_ =  new WContainerWidget();
    plotContainer_->setId("plotContainer_");
    tabWidget_->addTab(plotContainer_,WString::fromUTF8("Plot"),WTabWidget::PreLoading);

    // sphere (plot region)
    //sphere_ = new WWinSphere(plotContainer_,550,550);
    //sphere_->setId("sphere_");
    //sphere_->setMargin(5,Top);
    //plotContainer_->addWidget(sphere_);

    //TODO: add plot separatrices, orbits, etc buttons?
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
    
    // legend tab ----
    legendContainer_ = new WContainerWidget();
    legendContainer_->setId("legendContainer_");
    tabWidget_->addTab(legendContainer_,WString::fromUTF8("Legend"),WTabWidget::PreLoading);

    // legend image
    legend_ = new WImage(WLink("resources/p4legend.png"), legendContainer_);
    legend_->setAlternateText("Plot legend");
    legend_->setId("legend_");
    legend_->setMargin(5,Top);
    legendContainer_->addWidget(legend_);

    globalLogger__.debug("HomeRight :: UI set up");

}

void HomeRight::setupConnectors()
{
    // output buttons
    fullResButton_->clicked().connect(this,&HomeRight::fullResults);
    finResButton_->clicked().connect(this,&HomeRight::showFinResults);
    infResButton_->clicked().connect(this,&HomeRight::showInfResults);
    clearOutputButton_->clicked().connect(this,&HomeRight::clearResults);
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
    resultsFile.open((fileName_+".res").c_str());
    if (resultsFile.is_open()) {
        while(getline(resultsFile,line))
            fullResults_ += line + "\n";
        resultsFile.close();
    }

    // read finite singular points results
    finResults_ = "";
    resultsFile.open((fileName_+"_fin.res").c_str());
    if(resultsFile.is_open()) {
        while (getline(resultsFile,line))
            finResults_ += line + "\n";
        resultsFile.close();
    }
    
    // add title for infinite region (missing in inf.res)
    infResults_ = "AT THE INFINITE REGION \n";
    // read infinite singular points results
    resultsFile.open((fileName_+"_inf.res").c_str());
    if(resultsFile.is_open()) {
        while(getline(resultsFile,line))
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
    globalLogger__.debug("HomeRight :: showing full results");
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
    globalLogger__.debug("HomeRight :: cleared results");
}

void HomeRight::onPlot(std::string basename)
{ 
    if (sphere_ != nullptr) {
        delete sphere_;
        sphere_ = nullptr;
    }
    sphere_ = new WWinSphere(plotContainer_,550,550,basename);
    sphere_->setId("sphere_");
    sphere_->setMargin(5,Top);
    plotContainer_->addWidget(sphere_);

    sphere_->update();
    tabWidget_->setCurrentIndex(1);
    globalLogger__.debug("HomeRight :: reacted to onPlot signal");
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