#include "HomeRight.h"

#include <Wt/WContainerWidget>
#include <Wt/WGroupBox>
#include <Wt/WTextArea>

using namespace Wt;


HomeRight::HomeRight(WContainerWidget *parent) : WStackedWidget(parent)
{
    setId("HomeRight");
    setStyleClass("half-box");

    outputBox_ = new WGroupBox(this);
    outputBox_->setId("outputBox_");
    outputBox_->setTitle(tr("homeright.outputboxtitle"));
    addWidget(outputBox_);

    outputTextArea_ = new WTextArea("Output goes here");
    outputTextArea_->setId("outputTextArea_");
    outputTextArea_->setReadOnly(true);
    outputBox_->addWidget(outputTextArea_);
}

HomeRight::~HomeRight()
{
    delete outputTextArea_;
}