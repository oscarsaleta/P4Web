#include "HomeRight.h"

#include "P4src/file_tab.h"

#include <iostream>
#include <fstream>
#include <string>

#include <Wt/WBreak>
#include <Wt/WGroupBox>
#include <Wt/WMenuItem>

using namespace Wt;


HomeRight::HomeRight(WContainerWidget *parent) : WContainerWidget(parent), flag_(0)
{
    setId("HomeRight");
    setStyleClass("half-box");

    tabWidget_ = new WTabWidget(this);
    tabWidget_->setId("tabWidget_");

    // output widget
    outputContainer_ = new WContainerWidget();
    outputContainer_->setId("outputContainer_");
    tabWidget_->addTab(outputContainer_,WString::fromUTF8("Output"),WTabWidget::PreLoading);

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

    clearButton_ = new WPushButton("Clear");
    clearButton_->setId("clearButton_");
    clearButton_->setStyleClass("btn-warning btn");
    //outputContainer_->addWidget(clearButton_);
    outputButtonsToolbar_->addButton(clearButton_);

    fullResButton_->clicked().connect(this,&HomeRight::fullResults);
    finResButton_->clicked().connect(this,&HomeRight::showFinResults);
    infResButton_->clicked().connect(this,&HomeRight::showInfResults);

    clearButton_->clicked().connect(std::bind([=] () {
        outputTextArea_->setText("");
        outputTextAreaContent_ = "";
    }));

    // text area where results are shown
    outputTextAreaContent_ = "";
    outputTextArea_ = new WTextArea(outputTextAreaContent_);
    outputTextArea_->setId("outputTextArea_");
    outputTextArea_->setReadOnly(true);
    outputContainer_->addWidget(outputTextArea_);


    // plot tab
    plotContainer_ =  new WContainerWidget();
    plotContainer_->setId("plotContainer_");
    tabWidget_->addTab(plotContainer_,WString::fromUTF8("Plot"),WTabWidget::PreLoading);

    // region w
    plotRegion_ = new PlotRegion(plotContainer_,550,550);
    plotRegion_->setId("plotRegion_");
    plotRegion_->setMargin(5,Top);
    plotContainer_->addWidget(plotRegion_);

    plotButtonToolBar_ = new WToolBar(plotContainer_);
    plotButtonToolBar_->setId("plotButtonToolBar_");
    plotButtonToolBar_->setMargin(5,Top);

    plotButton_ = new WPushButton("Plot");
    plotButton_->setId("plotButton_");
    plotButton_->setStyleClass("btn-default btn");
    plotButtonToolBar_->addButton(plotButton_);

    // TODO: ficar metode onPlot o equivalent.
    //plotButton_->clicked().connect(this,&HomeRight::onPlot);

    //plotContainer_->addWidget(plotButtonToolBar_);


    tabWidget_->setCurrentIndex(0);


}

HomeRight::~HomeRight()
{
    delete outputTextArea_;
    delete clearButton_;
    delete outputContainer_;

    delete plotContainer_;

    delete tabWidget_;
}

void HomeRight::readResults(int k, std::string fileName)
{
    if (k!=0) {
        fullResults_ = "Unauthorised operation. IP will be logged.";
    } else {
        std::ifstream resultsFile;
        std::string line;

        fileName_ = fileName;
        flag_ = 0;

        // read full results
        fullResults_ = "";
        resultsFile.open((fileName_+".res").c_str());
        if (resultsFile.is_open()) {
            while(getline(resultsFile,line))
                fullResults_ += line + "\n";
            resultsFile.close();
        } else {
            flag_ = 1;
        }

        // read finite singular points results
        finResults_ = "";
        resultsFile.open((fileName_+"_fin.res").c_str());
        if(resultsFile.is_open()) {
            while (getline(resultsFile,line))
                finResults_ += line + "\n";
            resultsFile.close();
        } else {
            flag_ = 1;
        }
        
        // add title for infinite region (missing in inf.res)
        infResults_ = "AT THE INFINITE REGION \n";
        // read infinite singular points results
        resultsFile.open((fileName_+"_inf.res").c_str());
        if(resultsFile.is_open()) {
            while(getline(resultsFile,line))
                infResults_ += line + "\n";
            resultsFile.close();
        } else {
            flag_ = 1;
        }
    }

    fullResults();
}

void HomeRight::fullResults()
{
    if (flag_ == 0) {
        outputTextAreaContent_ = fullResults_;
        outputTextArea_->setText(outputTextAreaContent_);
    }
}

void HomeRight::showFinResults()
{
    if (flag_ == 0) {
        outputTextAreaContent_ = finResults_;
        outputTextArea_->setText(outputTextAreaContent_);
    }
}

void HomeRight::showInfResults()
{
    if (flag_ == 0) {
        outputTextAreaContent_ = infResults_;
        outputTextArea_->setText(outputTextAreaContent_);
    }
}


/*void HomeRight::onPlot()
{
    VFResults.deleteVF();
    if (VFResults.readTables)
}*/