#include "HomeLeft.h"

#include <Wt/WBreak>
#include <Wt/WFileUpload>
#include <Wt/WGroupBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLength>
#include <Wt/WLineEdit>

using namespace Wt;

HomeLeft::HomeLeft(WContainerWidget *parent) : WContainerWidget(parent)
{
    // set CSS class for inline 50% of the screen
    setId("HomeLeft");
    setStyleClass(WString::fromUTF8("half-box"));

    // File upload box
    fileUploadBox_ = new WGroupBox(this);
    fileUploadBox_->setId("fileUploadBox_");
    fileUploadBox_->setTitle(tr("homeleft.fuploadboxtitle"));
    addWidget(fileUploadBox_);

    fileUploadButton_ = new WPushButton("Load",fileUploadBox_);
    fileUploadButton_->setId("fileUploadButton_");
    fileUploadButton_->disable();
    fileUploadButton_->setInline(true);
    fileUploadBox_->addWidget(fileUploadButton_);

    //fileUploadBox_->addWidget(new WBreak(fileUploadBox_));
    
    fileUploadWidget_ = new WFileUpload(this);
    fileUploadWidget_->setId("fileUploadWidget_");
    fileUploadWidget_->setFileTextSize(20);
    fileUploadWidget_->setFilters(".inp");
    fileUploadWidget_->setInline(true);
    fileUploadBox_->addWidget(fileUploadWidget_);

    //fileUploadButton_->resize(fileUploadWidget_->width(),fileUploadButton_->height());

    // add connectors
    fileUploadWidget_->changed().connect(std::bind([=] () {
        fileUploadButton_->enable();
    }));
    fileUploadButton_->clicked().connect(fileUploadWidget_,&WFileUpload::upload);
    fileUploadWidget_->uploaded().connect(this,&HomeLeft::fileUploaded);
    fileUploadWidget_->uploaded().connect(this,&HomeLeft::fileTooLarge);
    

    addWidget(new WBreak());

    // Equation box
    equationsBox_ = new WGroupBox(this);
    equationsBox_->setId("equationsBox_");
    equationsBox_->setTitle(tr("homeleft.equationboxtitle"));
    addWidget(equationsBox_);

    xLabel_ = new WText(WString::tr("homeleft.xprimelabel"), equationsBox_);
    xLabel_->setId("xLabel_");
    equationsBox_->addWidget(xLabel_);

    xEquationInput_ = new WLineEdit();
    xEquationInput_->setId("xEquationInput_");
    xEquationInput_->setStyleClass(WString::fromUTF8("equation-editor"));
    equationsBox_->addWidget(xEquationInput_);

    equationsBox_->addWidget(new WBreak);

    yLabel_ = new WText(WString::tr("homeleft.yprimelabel"), equationsBox_);
    yLabel_->setId("yLabel_");
    equationsBox_->addWidget(yLabel_);

    yEquationInput_ = new WLineEdit();
    yEquationInput_->setId("yEquationInput_");
    yEquationInput_->setStyleClass(WString::fromUTF8("equation-editor"));
    equationsBox_->addWidget(yEquationInput_);

    equationsBox_->addWidget(new WBreak);

    evalButton_ = new WPushButton("Evaluate",equationsBox_);
    evalButton_->setId("evalButton_");
    equationsBox_->addWidget(evalButton_);

}

HomeLeft::~HomeLeft()
{
    delete fileUploadBox_;
    delete equationsBox_;
}

void HomeLeft::fileUploaded()
{
    fileUploadButton_->disable();
    fileUploadName_ = fileUploadWidget_->spoolFileName();
    fileUploadButton_->setText(WString::fromUTF8(fileUploadName_));
}

void HomeLeft::fileTooLarge()
{

}