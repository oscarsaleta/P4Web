/*  P4 (Polynomial Planar Phase Portraits) WEB VERSION SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4Web
 *
 *  Copyright (C) 2016  O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
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

#include "HomeLeft.h"

#include "custom.h"
#include "MyLogger.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <Wt/WAnchor>
#include <Wt/WAnimation>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WButtonGroup>
#include <Wt/WDoubleSpinBox>
#include <Wt/WDoubleValidator>
#include <Wt/WFileResource>
#include <Wt/WFileUpload>
#include <Wt/WGroupBox>
#include <Wt/WLabel>
#include <Wt/WLength>
#include <Wt/WLineEdit>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WRadioButton>
#include <Wt/WSpinBox>

using namespace Wt;

HomeLeft::HomeLeft(WContainerWidget *parent) :
    WContainerWidget(parent),
    evaluatedSignal_(this),
    errorSignal_(this),
    onPlotSignal_(this),
    loggedIn_(false),
    settingsBox_(nullptr)
{

    // set CSS class for inline 50% of the screen
    setId("HomeLeft");
    setStyleClass(WString::fromUTF8("half-box-left"));

    // set UI and connect signals
    setupUI();
    setupConnectors();

    // set up maple parameters that will not change
    str_bindir = "/usr/local/p4/bin/";
    str_p4m = str_bindir+"p4.m";
    str_tmpdir = "/tmp/";
    str_lypexe = "lyapunov";
    str_sepexe = "separatrice";
    str_exeprefix = "";
    str_platform = "LINUX";
    str_sumtablepath = "/usr/local/p4/sumtables/";
    str_removecmd = "rm";
    str_simplify = "false";
    str_simplifycmd = MAPLE_SIMPLIFY_EXPRESSIONS;

    globalLogger__.debug("HomeLeft :: created correctly");
}

HomeLeft::~HomeLeft()
{
    delete fileUploadWidget_;
    delete fileUploadBox_;

    delete xLabel_;
    delete yLabel_;
    delete xEquationInput_;
    delete yEquationInput_;
    delete evalButton_;
    delete plotButton_;
    delete saveButton_;
    delete equationsBox_;

    globalLogger__.debug("HomeLeft :: deleted correctly");

}

void HomeLeft::setupUI()
{

    // File upload box
    fileUploadBox_ = new WGroupBox(this);
    fileUploadBox_->setId("fileUploadBox_");
    fileUploadBox_->setTitle(WString::tr("homeleft.fuploadboxtitle"));
    addWidget(fileUploadBox_);

    fileUploadWidget_ = new WFileUpload(fileUploadBox_);
    fileUploadWidget_->setId("fileUploadWidget_");
    fileUploadWidget_->setFileTextSize(30);
    fileUploadWidget_->setFilters(".inp");
    fileUploadWidget_->setInline(true);
    fileUploadBox_->addWidget(fileUploadWidget_);

    addWidget(new WBreak());

    // Equation boxes
    equationsBox_ = new WGroupBox(this);
    equationsBox_->setId("equationsBox_");
    equationsBox_->setTitle(WString::tr("homeleft.equationboxtitle"));
    addWidget(equationsBox_);

    xLabel_ = new WLabel(WString::tr("homeleft.xprimelabel"), equationsBox_);
    xLabel_->setId("xLabel_");
    //equationsBox_->addWidget(xLabel_);

    xEquationInput_ = new WLineEdit();
    xEquationInput_->setId("xEquationInput_");
    xEquationInput_->setStyleClass(WString::fromUTF8("equation-editor"));
    xLabel_->setBuddy(xEquationInput_);
    equationsBox_->addWidget(xEquationInput_);

    equationsBox_->addWidget(new WBreak);

    yLabel_ = new WLabel(WString::tr("homeleft.yprimelabel"), equationsBox_);
    yLabel_->setId("yLabel_");
    //equationsBox_->addWidget(yLabel_);

    yEquationInput_ = new WLineEdit();
    yEquationInput_->setId("yEquationInput_");
    yEquationInput_->setStyleClass(WString::fromUTF8("equation-editor"));
    yLabel_->setBuddy(yEquationInput_);
    equationsBox_->addWidget(yEquationInput_);

    equationsBox_->addWidget(new WBreak);

    // eval button
    evalButton_ = new WPushButton("Evaluate",equationsBox_);
    evalButton_->setId("evalButton_");
    evalButton_->setStyleClass("btn btn-primary");
    evalButton_->setInline(true);
    equationsBox_->addWidget(evalButton_);

    // plot button
    plotButton_ = new WPushButton("Plot",equationsBox_);
    plotButton_->setId("plotButton_");
    plotButton_->setStyleClass("btn btn-default");
    plotButton_->setInline(true);
    plotButton_->setMargin(5,Left);
    equationsBox_->addWidget(plotButton_);

    // save file button (actually, it's a WAnchor)
    saveButton_ = new WAnchor(equationsBox_);
    saveButton_->setId("saveButton_");
    saveButton_->setStyleClass("btn btn-default");
    saveButton_->setText("Save");
    saveButton_->setInline(true);
    saveButton_->setMargin(5,Left);
    saveButton_->setDisabled(true);
    equationsBox_->addWidget(saveButton_);

    clearButton_ = new WPushButton("Clear",equationsBox_);
    clearButton_->setId("clearButton_");
    clearButton_->setStyleClass("btn btn-warning");
    clearButton_->setInline(true);
    clearButton_->setMargin(5,Left);
    equationsBox_->addWidget(clearButton_);

    globalLogger__.debug("HomeLeft :: UI set up");

}

void HomeLeft::setupConnectors()
{
    // file upload
    fileUploadWidget_->changed().connect(fileUploadWidget_,&WFileUpload::upload);
    fileUploadWidget_->uploaded().connect(this,&HomeLeft::fileUploaded);
    fileUploadWidget_->fileTooLarge().connect(this,&HomeLeft::fileTooLarge);

    // buttons
    evalButton_->clicked().connect(this,&HomeLeft::evaluate);
    plotButton_->clicked().connect(this,&HomeLeft::onPlot);
    clearButton_->clicked().connect(std::bind([=] () {
        xEquationInput_->setText(std::string());
        yEquationInput_->setText(std::string());
    }));

    // enable buttons if line edits have content
    xEquationInput_->textInput().connect(std::bind([=] () {
        if (!yEquationInput_->text().empty()) {
            evalButton_->setEnabled(true);
            plotButton_->setEnabled(true);
            prepareSaveFile();
        }
    }));
    yEquationInput_->textInput().connect(std::bind([=] () {
        if (!xEquationInput_->text().empty()) {
            evalButton_->setEnabled(true);
            plotButton_->setEnabled(true);
            prepareSaveFile();
        }
    }));

    globalLogger__.debug("HomeLeft :: connectors set up");
}


void HomeLeft::fileUploaded()
{
    xEquationInput_->setText(std::string());
    yEquationInput_->setText(std::string());
    // input validation
    std::string extension = fileUploadWidget_->clientFileName().toUTF8().substr(fileUploadWidget_->clientFileName().toUTF8().find_last_of(".")+1);
    if (extension != "inp") {
        errorSignal_.emit("Filetype not accepted.");
        globalLogger__.warning("HomeLeft :: client tried to upload a not supported file type");
        return;
    }

    fileUploadName_ = fileUploadWidget_->spoolFileName();

    parseInputFile();
    
}

void HomeLeft::fileTooLarge()
{
    errorSignal_.emit("File too large.");
    globalLogger__.warning("HomeLeft :: Client tried to upload file too large.");
}


void HomeLeft::parseInputFile()
{
    // read file and fill xEquationInput and yEquationInput
    std::ifstream f;
    std::string line;
    f.open(fileUploadName_.c_str());
    if (f.is_open()) {
        int i=0;
        while (std::getline(f,line)) {
            if (i==11)
                xEquationInput_->setText(WString::fromUTF8(line));
            else if (i==12)
                yEquationInput_->setText(WString::fromUTF8(line));
        i++;    
        }
        if (f.eof() && i<12) {
            errorSignal_.emit("Invalid input file.");
            globalLogger__.error("HomeLeft :: EOF while reading input file uploaded with name "+fileUploadName_);
            fileUploadName_ = "";
        } else if (f.bad()) {
            errorSignal_.emit("Invalid input file.");
            globalLogger__.error("HomeLeft :: I/O error while reading input file uploaded with name "+fileUploadName_);
            fileUploadName_ = "";
        } else {
            prepareSaveFile();
            errorSignal_.emit("File uploaded. Press the Evaluate button to start computing.");
            globalLogger__.info("HomeLeft :: Input file uploaded with name "+fileUploadName_);
        }
        f.close();
    }
}


std::string HomeLeft::openTempStream(std::string prefix, std::string suffix, std::ofstream &f)
{
    std::string fullname;
    prefix += "/XXXXXX" + suffix;
    std::vector<char> dst_prefix(prefix.begin(), prefix.end());
    dst_prefix.push_back('\0');

    int fd = mkstemps(&dst_prefix[0],4);
    if (fd != -1) {
        prefix.assign(dst_prefix.begin(), dst_prefix.end()-1-4);
        fullname = prefix+suffix;
        f.open(fullname.c_str(), std::ios_base::trunc | std::ios_base::out);
        close(fd);
    }

    globalLogger__.debug("HomeLeft :: created temp file "+fullname);

    return prefix;
}

void HomeLeft::prepareMapleFile()
{
    std::ofstream mplFile;

    if (!fileUploadName_.empty()) {
        mplFile.open((fileUploadName_+".mpl").c_str(),
            std::ios_base::trunc | std::ios_base::out);
    } else {
        fileUploadName_ = openTempStream("/tmp",".mpl",mplFile);
    }

    if (!loggedIn_) {
        str_critpoints = "0";
        str_saveall = "false";
        str_vectable = fileUploadName_+"_vec.tab";
        str_fintab = fileUploadName_+"_fin.tab";
        str_finres = fileUploadName_+"_fin.res";
        str_inftab = fileUploadName_+"_inf.tab";
        str_infres = fileUploadName_+"_inf.res";
        str_userf = "[ "+xEquationInput_->text()+", "+yEquationInput_->text()+" ]";
        str_gcf = "0";
        str_numeric = "true";
        str_epsilon = "0.01";
        str_testsep = "false";
        str_precision = "8";
        str_precision0 = "0";
        str_taylor = "6";
        str_numericlevel = "10";
        str_maxlevel = "20";
        str_weaklevel = "0";
        str_userp = "1";
        str_userq = "1";
    } else {
        str_critpoints = "0";
        str_saveall = "false";
        str_vectable = fileUploadName_+"_vec.tab";
        str_fintab = fileUploadName_+"_fin.tab";
        str_finres = fileUploadName_+"_fin.res";
        str_inftab = fileUploadName_+"_inf.tab";
        str_infres = fileUploadName_+"_inf.res";
        str_userf = "[ "+xEquationInput_->text()+", "+yEquationInput_->text()+" ]";
        str_gcf = "0";
        str_numeric = "true";
        str_epsilon = "0.01";
        str_testsep = "false";
        str_precision = "8";
        str_precision0 = "0";
        str_taylor = "6";
        str_numericlevel = "10";
        str_maxlevel = "20";
        str_weaklevel = "0";
        str_userp = "1";
        str_userq = "1";
    }

    if (mplFile.is_open())
        fillMapleScript(fileUploadName_,mplFile);
}

void HomeLeft::fillMapleScript(std::string fname, std::ofstream &f)
{
    f << "restart;" << std::endl;
    f << "read( \"" << str_p4m << "\" );" << std::endl;
    f << "user_bindir := \"" << str_bindir << "\":" << std::endl;
    f << "user_tmpdir := \"" << str_tmpdir << "\":" << std::endl;
    f << "user_lypexe := \"" << str_lypexe << "\":" << std::endl;
    f << "user_sepexe := \"" << str_sepexe << "\":" << std::endl;
    f << "user_exeprefix := \"" << str_exeprefix << "\":" << std::endl;
    f << "user_platform := \"" << str_platform << "\":" << std::endl;
    f << "user_sumtablepath := \"" << str_sumtablepath << "\":" << std::endl;
    f << "user_removecmd := \"" << str_removecmd << "\":" << std::endl;
    f << "user_simplify := " << str_simplify << ":" << std::endl;
    f << "user_simplifycmd := " << str_simplifycmd << ":" << std::endl;
    f << "all_crit_points := " << str_critpoints << ":" << std::endl;
    f << "save_all := " << str_saveall << ":" << std::endl;
    f << "vec_table := \"" << str_vectable << "\":" << std::endl;
    f << "finite_table := \"" << str_fintab << "\":" << std::endl;
    f << "finite_res := \"" << str_finres << "\":" << std::endl;
    f << "infinite_table := \"" << str_inftab << "\":" << std::endl;
    f << "infinite_res := \"" << str_infres << "\":" << std::endl;
    f << "user_f := " << str_userf << ":" << std::endl;
    f << "user_gcf := " << str_gcf << ":" << std::endl;
    f << "user_numeric :=" << str_numeric << ":" << std::endl;
    f << "epsilon := " << str_epsilon << ":" << std::endl;
    f << "test_sep := " << str_testsep << ":" << std::endl;
    f << "user_precision := " << str_precision << ":" << std::endl;
    f << "user_precision0 := " << str_precision0 << ":" << std::endl;
    f << "taylor_level := " << str_taylor << ":" << std::endl;
    f << "numeric_level := " << str_numericlevel << ":" << std::endl;
    f << "max_level := " << str_maxlevel << ":" << std::endl;
    f << "weakness_level := " << str_weaklevel << ":" << std::endl;
    f << "user_p := " << str_userp << ":" << std::endl;
    f << "user_q := " << str_userq << ":" << std::endl;
    f << "try p4main() catch:"  << std::endl
        << "printf( \"! Error (\%a) \%a\\n\", lastexception[1], lastexception[2] );\n"
        << "finally: closeallfiles();\n"
        << "if normalexit=0 then `quit`(0); else `quit(1)` end if: end try:\n";

    globalLogger__.info("HomeLeft :: filled Maple script "+fname);
}

void HomeLeft::evaluate()
{
    if (xEquationInput_->text().empty() || yEquationInput_->text().empty()) {
        errorSignal_.emit("Cannot evaluate yet, insert a vector field in the input forms.");
        return;
    }

    prepareMapleFile();

    std::string command = "maple -z --secure-read=/tmp/*,/usr/local/p4/bin/*,/usr/local/p4/sum_tables/* --secure-write=/tmp/*,/usr/local/p4/sum_tables/* --secure-extcall=/usr/local/p4/bin/lyapunov,/usr/local/p4/bin/separatrice "+fileUploadName_+".mpl > "+fileUploadName_+".res";
    globalLogger__.debug("HomeLeft :: Maple system command: "+command);

    int status = system(command.c_str());
    if (status == 0) {
        evaluatedSignal_.emit(fileUploadName_);
        globalLogger__.info("HomeLeft :: Maple script successfully executed");
    } else {
        errorSignal_.emit("Maple error.");
        globalLogger__.error("HomeLeft :: error during Maple script execution");
    }
    
}


void HomeLeft::prepareSaveFile()
{
    if (fileUploadName_.empty()) {
        std::ofstream saveFile;
        if (saveFileName_.empty()) {
            saveFileName_ = openTempStream("/tmp",".txt",saveFile);
            saveFileName_ += ".txt";
        } else
            saveFile.open(saveFileName_.c_str());
        saveFile << 0 << std::endl;
        saveFile << 1 << std::endl;
        saveFile << 8 << std::endl;
        saveFile << 0.01 << std::endl;
        saveFile << 0 << std::endl;
        saveFile << 6 << std::endl;
        saveFile << 10 << std::endl;
        saveFile << 20 << std::endl;
        saveFile << 4 << std::endl;
        saveFile << 1 << std::endl;
        saveFile << 1 << std::endl;
        saveFile << xEquationInput_->text() << std::endl;
        saveFile << yEquationInput_->text() << std::endl;
        saveFile << 0 << std::endl;
        saveFile << 0 << std::endl;
        saveFile.close();
    } else {
        saveFileName_ = fileUploadName_;
    }

    saveFileResource_ = new WFileResource(saveFileName_);

    char date[100];
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::strftime(date,sizeof(date),"%F_%R",std::localtime(&now));
    
    saveFileResource_->suggestFileName("P4vf_"+std::string(date)+".inp");
    saveButton_->setLink(saveFileResource_);
    saveButton_->setDisabled(false);

}

void HomeLeft::onPlot()
{
    if ( fileUploadName_.empty() ) {
        errorSignal_.emit("Cannot read results, evaluate a vector field first.\n");
    } else {
        globalLogger__.debug("HomeLeft :: sending onPlot signal");
        onPlotSignal_.emit(fileUploadName_);
    }
}


void HomeLeft::showSettings()
{
    loggedIn_ = true;
    if (settingsBox_ != nullptr) {
        delete settingsBox_;
        settingsBox_ = nullptr;
    }
    settingsBox_ = new WGroupBox(this);
    settingsBox_->setId("settingsBox_");
    settingsBox_->setTitle(WString::tr("homeleft.settingsboxtitle"));
    settingsBox_->setMargin(15,Top);
    addWidget(settingsBox_);

    WRadioButton *button;
    WLabel *label;

    // calculations
    calculationsBtnGroup_ = new WButtonGroup(settingsBox_);
    label = new WLabel("Calculations:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.calculations"),XHTMLText);
    button = new WRadioButton("Algebraic",settingsBox_);
    button->addStyleClass("radio-button");
    //button->setMargin(10,Left|Right);
    label->setBuddy(button);
    calculationsBtnGroup_->addButton(button,Algebraic);
    button = new WRadioButton("Numeric",settingsBox_);
    button->addStyleClass("radio-button");
    calculationsBtnGroup_->addButton(button,Numeric);
    calculationsBtnGroup_->setCheckedButton(calculationsBtnGroup_->button(Numeric));

    // separatrices
    separatricesBtnGroup_ = new WButtonGroup(settingsBox_);
    label = new WLabel("Test separatrices:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.separatrices"),XHTMLText);
    label->setMargin(20,Left);
    button = new WRadioButton("Yes",settingsBox_);
    button->addStyleClass("radio-button");
    //button->setMargin(10,Left|Right);
    label->setBuddy(button);
    separatricesBtnGroup_->addButton(button,Yes);
    button = new WRadioButton("No",settingsBox_);
    button->addStyleClass("radio-button");
    separatricesBtnGroup_->addButton(button,No);
    separatricesBtnGroup_->setCheckedButton(separatricesBtnGroup_->button(No));

    new WBreak(settingsBox_);

    // accuracy
    label = new WLabel("Accuracy:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.accuracy"),XHTMLText);
    accuracySpinBox_ = new WSpinBox(settingsBox_);
    accuracySpinBox_->setStyleClass("spin-box");
    accuracySpinBox_->setRange(1,16);
    accuracySpinBox_->setValue(8);
    accuracySpinBox_->setInline(true);
    label->setBuddy(accuracySpinBox_);

    // precision
    label = new WLabel("Precision:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.precision"),XHTMLText);
    label->setMargin(20,Left);
    precisionSpinBox_ = new WSpinBox(settingsBox_);
    precisionSpinBox_->setStyleClass("spin-box");
    precisionSpinBox_->setRange(0,8);
    precisionSpinBox_->setValue(0);
    label->setBuddy(precisionSpinBox_);

    new WBreak(settingsBox_);

    // epsilon
    label = new WLabel("Epsilon:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.epsilon"),XHTMLText);
    epsilonSpinBox_ = new WDoubleSpinBox(settingsBox_);
    epsilonSpinBox_->setStyleClass("spin-box");
    epsilonSpinBox_->setDecimals(2);
    epsilonSpinBox_->setSingleStep(0.01);
    epsilonSpinBox_->setValue(0.01);
    label->setBuddy(epsilonSpinBox_);

    // level of approximation
    label = new WLabel("Level of approximation:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.level-approximation"),XHTMLText);
    label->setMargin(20,Left);
    levAppSpinBox_ = new WSpinBox(settingsBox_);
    levAppSpinBox_->setStyleClass("spin-box");
    levAppSpinBox_->setRange(0,10);
    levAppSpinBox_->setValue(6);
    label->setBuddy(levAppSpinBox_);

    new WBreak(settingsBox_);

    // numeric level
    label = new WLabel("Numeric level:",settingsBox_);
    label->setToolTip(WString::tr("tooltip.numeric-level"),XHTMLText);
    numericLevelSpinBox_ = new WSpinBox(settingsBox_);
    numericLevelSpinBox_->setStyleClass("spin-box");
    numericLevelSpinBox_->setRange(5,15);
    numericLevelSpinBox_->setValue(8);
    label->setBuddy(numericLevelSpinBox_);

    // maximum level
    

    /*Wt::WSpinBox        *maxLevelSpinBox_;
    Wt::WSpinBox        *maxWeakLevelSpinBox_;
    Wt::WSpinBox        *PLWeightPSpinBox_;
    Wt::WSpinBox        *PLWeightQSpinBox_;*/

}

void HomeLeft::hideSettings()
{
    loggedIn_ = false;
    if (settingsBox_ != nullptr) {        
        delete settingsBox_;
        settingsBox_ = nullptr;
    }
}



// signals

Signal<std::string>& HomeLeft::evaluatedSignal()
{
    return evaluatedSignal_;
}

Signal<std::string>& HomeLeft::errorSignal()
{
    return errorSignal_;
}

Signal <std::string>& HomeLeft::onPlotSignal()
{
    return onPlotSignal_;
}