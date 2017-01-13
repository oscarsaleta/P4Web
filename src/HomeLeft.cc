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
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WFileResource>
#include <Wt/WFileUpload>
#include <Wt/WGroupBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLength>
#include <Wt/WLineEdit>
#include <Wt/WString>

using namespace Wt;

HomeLeft::HomeLeft(WContainerWidget *parent) : WContainerWidget(parent), evaluatedSignal_(this), errorSignal_(this), onPlotSignal_(this)
{
    // set CSS class for inline 50% of the screen
    setId("HomeLeft");
    setStyleClass(WString::fromUTF8("half-box"));

    // set UI and connect signals
    setupUI();
    setupConnectors();

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
    fileUploadBox_->setTitle(tr("homeleft.fuploadboxtitle"));
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
    // input validation
    std::string extension = fileUploadWidget_->clientFileName().toUTF8().substr(fileUploadWidget_->clientFileName().toUTF8().find_last_of(".")+1);
    if (extension != "inp") {
        errorSignal_.emit("Filetype not accepted.");
        globalLogger__.warning("HomeLeft :: client tried to upload a not supported file type");
        return;
    }

    fileUploadName_ = fileUploadWidget_->spoolFileName();

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
            errorSignal_.emit("End-Of-File reached prematurely.");
            globalLogger__.error("HomeLeft :: EOF while reading input file uploaded with name "+fileUploadName_);
            fileUploadName_ = "";
        } else if (f.bad()) {
            errorSignal_.emit("Input/Output error.");
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

void HomeLeft::fileTooLarge()
{
    errorSignal_.emit("File too large.");
    globalLogger__.warning("HomeLeft :: Client tried to upload file too large.");
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

    if (mplFile.is_open())
        fillMapleScript(fileUploadName_,mplFile);
}

void HomeLeft::fillMapleScript(std::string fname, std::ofstream &f)
{

    WString str_bindir = "/usr/local/p4/bin/";
    WString str_p4m = str_bindir+"p4.m";
    WString str_tmpdir = "/tmp/";
    WString str_lypexe = "lyapunov";
    WString str_sepexe = "separatrice";
    WString str_exeprefix = "";
    WString str_platform = "LINUX";
    WString str_sumtablepath = "/usr/local/p4/sumtables/";
    WString str_removecmd = "rm";
    WString str_simplify = "false";
    WString str_simplifycmd = "proc(expr) try radsimp(simplify(expr),ratdenom) catch: simplify(expr) end try end";
    WString str_critpoints = "0";
    WString str_saveall = "false";
    WString str_vectable = fname+"_vec.tab";
    WString str_fintab = fname+"_fin.tab";
    WString str_finres = fname+"_fin.res";
    WString str_inftab = fname+"_inf.tab";
    WString str_infres = fname+"_inf.res";
    WString str_userf = "[ "+xEquationInput_->text()+", "+yEquationInput_->text()+" ]";
    WString str_gcf = "0";
    WString str_numeric = "true";
    WString str_epsilon = "0.01";
    WString str_testsep = "false";
    WString str_precision = "8";
    WString str_precision0 = "0";
    WString str_taylor = "6";
    WString str_numericlevel = "10";
    WString str_maxlevel = "20";
    WString str_weaklevel = "0";
    WString str_userp = "1";
    WString str_userq = "1";

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
    if ( /*!study_.readTables(fileUploadName_)*/ fileUploadName_.empty() ) {
        errorSignal_.emit("Cannot read results, evaluate a vector field first.\n");
    } else {
        //study_.setupCoordinateTransformations();

        globalLogger__.debug("HomeLeft :: sending onPlot signal");
        onPlotSignal_.emit(fileUploadName_);
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