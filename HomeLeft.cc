#include "HomeLeft.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <Wt/WBreak>
#include <Wt/WFileUpload>
#include <Wt/WGroupBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLength>
#include <Wt/WLineEdit>
#include <Wt/WString>

using namespace Wt;

HomeLeft::HomeLeft(WContainerWidget *parent) : WContainerWidget(parent), evaluated_(this)
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
    fileUploadButton_->setEnabled(false);
    fileUploadButton_->setInline(true);
    fileUploadButton_->setMargin(5,Bottom);
    fileUploadBox_->addWidget(fileUploadButton_);
    
    fileUploadWidget_ = new WFileUpload(this);
    fileUploadWidget_->setId("fileUploadWidget_");
    fileUploadWidget_->setFileTextSize(30);
    fileUploadWidget_->setFilters(".inp");
    fileUploadWidget_->setInline(true);
    fileUploadBox_->addWidget(fileUploadWidget_);

    // add connectors
    fileUploadWidget_->changed().connect(std::bind([=] () {
        fileUploadButton_->setEnabled(true);
    }));
    fileUploadButton_->clicked().connect(fileUploadWidget_,&WFileUpload::upload);
    fileUploadWidget_->uploaded().connect(this,&HomeLeft::fileUploaded);
    fileUploadWidget_->uploaded().connect(this,&HomeLeft::fileTooLarge);
    
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
    evalButton_->setEnabled(false);
    equationsBox_->addWidget(evalButton_);

    evalButton_->clicked().connect(this,&HomeLeft::evaluate);

    // save file button and line edit for filename
    saveButton_ = new WPushButton("Save",equationsBox_);
    saveButton_->setId("saveButton_");
    saveButton_->setStyleClass("btn btn-default");
    saveButton_->setInline(true);
    saveButton_->setMargin(10,Left);
    saveButton_->setEnabled(false);
    equationsBox_->addWidget(saveButton_);
    saveButton_->hide();

    saveButton_->clicked().connect(this,&HomeLeft::saveFile);

    // enable buttons if line edits have content
    xEquationInput_->textInput().connect(std::bind([=] () {
        if (!yEquationInput_->text().empty()) {
            evalButton_->setEnabled(true);
            saveButton_->setEnabled(true);
        }
    }));
    yEquationInput_->textInput().connect(std::bind([=] () {
        if (!xEquationInput_->text().empty()) {
            evalButton_->setEnabled(true);
            saveButton_->setEnabled(true);
        }
    }));



}

HomeLeft::~HomeLeft()
{
    delete fileUploadBox_;
    delete equationsBox_;
}

void HomeLeft::fileUploaded()
{
    fileUploadButton_->setEnabled(false);
    fileUploadName_ = fileUploadWidget_->spoolFileName();
    /* aqui toca llegir el fitxer i omplir els camps */

    std::ifstream f;
    std::string line;
    f.open(fileUploadName_.c_str());
    if (f.is_open()) {
        for (int i=0; i<=12; i++) {
            getline(f,line);
            if (i==11)
                xEquationInput_->setText(WString::fromUTF8(line));
            else if (i==12)
                yEquationInput_->setText(WString::fromUTF8(line));
        }
        evalButton_->setEnabled(true);
    }
    
}

void HomeLeft::fileTooLarge()
{

}


void HomeLeft::saveFile()
{

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
        fullname = prefix+".mpl";
        f.open(fullname.c_str(), std::ios_base::trunc | std::ios_base::out);
        close(fd);
    }
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
// TODO: llegir del fitxer uploaded el camp i ficar-lo als LineEdits i al .mpl

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
    //f << "user_lypexe_mpf := \"" << user_lypexe_mpf << "\":" << std::endl;
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
}

// TODO: executar arxiu temporal!

void HomeLeft::evaluate()
{
    prepareMapleFile();

    std::string command = "maple "+fileUploadName_+".mpl > "+fileUploadName_+".res";
    system(command.c_str());
    
    //evalButton_->setText(WString::fromUTF8(fileUploadName_));

    evaluated_.emit(42,fileUploadName_);
    fileUploadName_="";
}

