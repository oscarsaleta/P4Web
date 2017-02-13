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
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "HomeLeft.h"

#include "custom.h"
#include "MyLogger.h"

#include <boost/filesystem.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <Wt/WAnchor>
//#include <Wt/WAnimation>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WButtonGroup>
#include <Wt/WComboBox>
#include <Wt/WDoubleSpinBox>
#include <Wt/WDoubleValidator>
#include <Wt/WFileResource>
#include <Wt/WFileUpload>
#include <Wt/WGroupBox>
#include <Wt/WImage>
#include <Wt/WLabel>
#include <Wt/WLength>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WRadioButton>
#include <Wt/WSpinBox>
#include <Wt/WTabWidget>
#include <Wt/WTemplate>

using namespace Wt;

HomeLeft::HomeLeft(WContainerWidget *parent) :
    WContainerWidget(parent),
    loggedIn_(false),
    settingsContainer_(nullptr),
    viewContainer_(nullptr),
    orbitsContainer_(nullptr),
    orbitsStartSelected_(false),
    evaluated_(false)
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
    str_tmpdir = TMP_DIR;
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
    delete xEquationInput_;
    delete yEquationInput_;
    delete evalButton_;
    delete plotButton_;
    delete equationsBox_;

    globalLogger__.debug("HomeLeft :: deleted correctly");

}

void HomeLeft::setupUI()
{

    WLabel *label;

    // Equation boxes
    equationsBox_ = new WGroupBox(this);
    equationsBox_->setId("equationsBox_");
    equationsBox_->setTitle(WString::tr("homeleft.equationboxtitle"));
    addWidget(equationsBox_);

    WTemplate *t = new WTemplate(WString::tr("template.homeleft-default"),equationsBox_);
    t->addFunction("id",WTemplate::Functions::id);

    fileUploadWidget_ = new WFileUpload(equationsBox_);
    fileUploadWidget_->setId("fileUploadWidget_");
    fileUploadWidget_->setFileTextSize(30);
    fileUploadWidget_->setFilters(".inp");
    fileUploadWidget_->setInline(true);
    t->bindWidget("upload",fileUploadWidget_);

    xEquationInput_ = new WLineEdit();
    xEquationInput_->setId("xEquationInput_");
    yEquationInput_ = new WLineEdit();
    yEquationInput_->setId("yEquationInput_");
    t->bindString("vf-tooltip",WString::tr("tooltip.vectorfield"));
    t->bindWidget("xeq",xEquationInput_);
    t->bindWidget("yeq",yEquationInput_);

    // eval button
    evalButton_ = new WPushButton("Evaluate",equationsBox_);
    evalButton_->setId("evalButton_");
    evalButton_->setStyleClass("btn btn-primary");
    evalButton_->setInline(true);
    evalButton_->setToolTip(WString::tr("tooltip.homeleft-eval-button"));
    t->bindWidget("eval",evalButton_);

    // plot button
    plotButton_ = new WPushButton("Plot",equationsBox_);
    plotButton_->setId("plotButton_");
    plotButton_->setStyleClass("btn btn-default");
    plotButton_->setInline(true);
    plotButton_->setToolTip(WString::tr("tooltip.homeleft-plot-button"));
    t->bindWidget("plot",plotButton_);

    // prepare save file button
    prepSaveButton_ = new WPushButton("Prepare save file",equationsBox_);
    prepSaveButton_->setId("prepSaveButton_");
    prepSaveButton_->setStyleClass("btn btn-default");
    prepSaveButton_->setInline(true);
    prepSaveButton_->setToolTip(WString::tr("tooltip.homeleft-save-button"));
    t->bindWidget("prep-save",prepSaveButton_);

    // save file anchor
    saveAnchor_ = new WAnchor(equationsBox_);
    saveAnchor_->setId("saveAnchor_");
    saveAnchor_->setStyleClass("btn btn-default");
    saveAnchor_->setText("Download save file");
    saveAnchor_->setInline(true);
    saveAnchor_->setToolTip(WString::tr("tooltip.homeleft-save-button"));
    saveAnchor_->hide();
    t->bindWidget("down-save",saveAnchor_);

    resetButton_ = new WPushButton("Reset",equationsBox_);
    resetButton_->setId("resetButton_");
    resetButton_->setStyleClass("btn btn-warning");
    resetButton_->setInline(true);
    resetButton_->setToolTip(WString::tr("tooltip.homeleft-clear-button"));
    t->bindWidget("reset",resetButton_);

    new WBreak(this);

    /* tab widget for legend et al */
    tabs_ = new WTabWidget(this);
    // legend tab ----
    WContainerWidget *legendContainer = new WContainerWidget();
    legendContainer->setId("legendContainer");
    tabs_->addTab(legendContainer,WString::fromUTF8("Legend"),WTabWidget::PreLoading);
    addWidget(tabs_);

    t = new WTemplate(WString::tr("template.legend"),legendContainer);
    t->addFunction("id",WTemplate::Functions::id);

    // legend image
    WImage *legend = new WImage(WLink("resources/p4legend.png"), legendContainer);
    legend->setAlternateText("Plot legend");
    legend->setId("legend");
    //legend->setMargin(5,Top);
    //legendContainer->addWidget(legend);
    t->bindWidget("img",legend);


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
    resetButton_->clicked().connect(this,&HomeLeft::resetUI);
    prepSaveButton_->clicked().connect(this,&HomeLeft::prepareSaveFile);

    saveAnchor_->clicked().connect(std::bind([=] () {
        saveAnchor_->hide();
        prepSaveButton_->show();
        delete saveFileResource_;
    }));

    globalLogger__.debug("HomeLeft :: connectors set up");
}


void HomeLeft::fileUploaded()
{
    globalLogger__.debug("HomeLeft :: file uploaded, proceeding to read...");

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

#ifdef ANTZ
    globalLogger__.debug("HomeLeft :: (ANTZ) copying uploaded file to "+std::string(TMP_DIR));
    // copy file to home tmp dir
    try {
        boost::filesystem::copy_file(fileUploadName_, TMP_DIR+fileUploadName_.substr(5));
        fileUploadName_ = TMP_DIR+fileUploadName_.substr(5);
    } catch (const boost::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
#endif

    evaluated_ = false;

    parseInputFile();

    if (loggedIn_)
        tabs_->setCurrentWidget(settingsContainer_);

    globalLogger__.debug("HomeLeft :: file read correctly.");
    
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
        if (loggedIn_) {
            while (std::getline(f,line)) {
                switch(i) {
                case 1:
                    if (line == "0")
                        calculationsBtnGroup_->setCheckedButton(calculationsBtnGroup_->button(Algebraic));
                    else
                        calculationsBtnGroup_->setCheckedButton(calculationsBtnGroup_->button(Numeric));
                    break;
                case 2:
                    accuracySpinBox_->setValue((stoi(line)>0)?stoi(line):ACCURACY_DEFAULT);
                    break;
                case 3:
                    epsilonSpinBox_->setValue((stoi(line)>0)?stoi(line):EPSILON_DEFAULT);
                    break;
                case 4:
                    if (line == "0") { 
                        separatricesBtnGroup_->setCheckedButton(separatricesBtnGroup_->button(No));
                        levAppSpinBox_->disable();
                        numericLevelSpinBox_->disable();
                        maxLevelSpinBox_->disable();
                    } else {
                        separatricesBtnGroup_->setCheckedButton(separatricesBtnGroup_->button(Yes));
                        levAppSpinBox_->enable();
                        numericLevelSpinBox_->enable();
                        maxLevelSpinBox_->enable();
                    }
                    break;
                case 5:
                    levAppSpinBox_->setValue((stoi(line)>0)?stoi(line):APPROX_DEFAULT);
                    break;
                case 6:
                    numericLevelSpinBox_->setValue((stoi(line)>0)?stoi(line):NUMERIC_DEFAULT);
                    break;
                case 7:
                    maxLevelSpinBox_->setValue((stoi(line)>0)?stoi(line):MAXIMUM_DEFAULT);
                    break;
                case 8:
                    maxWeakLevelSpinBox_->setValue((stoi(line)>0)?stoi(line):WEAKNESS_DEFAULT);
                    break;
                case 9:
                    PLWeightPSpinBox_->setValue((stoi(line)>0)?stoi(line):P_DEFAULT);
                    break;
                case 10:
                    PLWeightQSpinBox_->setValue((stoi(line)>0)?stoi(line):Q_DEFAULT);
                    break;
                case 11:
                    xEquationInput_->setText(line);
                    break;
                case 12:
                    yEquationInput_->setText(line);
                    break;
                }
                i++;
            }
        } else {
            while(std::getline(f,line)) {
                if (i==11)
                    xEquationInput_->setText(WString::fromUTF8(line));
                else if (i==12)
                    yEquationInput_->setText(WString::fromUTF8(line));
                i++;
            }
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
            //prepareSaveFile();
            errorSignal_.emit("File uploaded. Press the Evaluate button to start computing.");
            globalLogger__.debug("HomeLeft :: Input file uploaded with name "+fileUploadName_);
        }
        f.close();
    }
}


std::string HomeLeft::openTempStream(std::string prefix, std::string suffix/*, std::ofstream &f*/)
{
    std::string fullname;
    prefix += "/XXXXXX" + suffix;
    std::vector<char> dst_prefix(prefix.begin(), prefix.end());
    dst_prefix.push_back('\0');

    int fd = mkstemps(&dst_prefix[0],4);
    if (fd != -1) {
        prefix.assign(dst_prefix.begin(), dst_prefix.end()-1-4);
        fullname = prefix+suffix;
        //f.open(fullname.c_str(), std::ios_base::trunc | std::ios_base::out);
        close(fd);
    }

    globalLogger__.debug("HomeLeft :: created temp file "+fullname);

    return prefix;
}

void HomeLeft::setParams()
{
    if (!loggedIn_) {
        str_critpoints = "0";
        str_saveall = "false";
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
        time_limit = "30";
    } else {
        str_critpoints = "0";
        str_saveall = "false";
        str_gcf = "0";
        str_numeric = (calculationsBtnGroup_->checkedId() == Numeric) ? "true" : "false";
        str_epsilon = std::to_string(epsilonSpinBox_->value());
        str_testsep = (separatricesBtnGroup_->checkedId() == No) ? "false" : "true";
        str_precision = std::to_string(accuracySpinBox_->value());
        str_precision0 = std::to_string(precisionSpinBox_->value());
        str_taylor = std::to_string(levAppSpinBox_->value());
        str_numericlevel = std::to_string(numericLevelSpinBox_->value());
        str_maxlevel = std::to_string(maxLevelSpinBox_->value());
        str_weaklevel = std::to_string(maxWeakLevelSpinBox_->value());
        str_userp = std::to_string(PLWeightPSpinBox_->value());
        str_userq = std::to_string(PLWeightQSpinBox_->value());
        time_limit = "60";
    }
}

void HomeLeft::prepareMapleFile()
{
    std::ofstream mplFile;

    if (fileUploadName_.empty())
        fileUploadName_ = openTempStream(TMP_DIR,".mpl"/*,mplFile*/);

    mplFile.open((fileUploadName_+".mpl").c_str(),std::fstream::trunc|std::fstream::out);

    str_vectable = fileUploadName_+"_vec.tab";
    str_fintab = fileUploadName_+"_fin.tab";
    str_finres = fileUploadName_+"_fin.res";
    str_inftab = fileUploadName_+"_inf.tab";
    str_infres = fileUploadName_+"_inf.res";
    str_userf = "[ "+xEquationInput_->text()+", "+yEquationInput_->text()+" ]";

    setParams();

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
    f << "if (type(parse(\"" << xEquationInput_->text() << "\"),polynom)) then" << std::endl;
    f << "  if (type(parse(\"" << yEquationInput_->text() << "\"),polynom)) then" << std::endl;
    f << "    user_f := " << str_userf << ":" << std::endl;
    f << "  else `quit(1)` end if:" << std::endl;
    f << "else `quit(1)` end if:" << std::endl;
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
    f << "try timelimit("<<time_limit<<",p4main()) catch:"  << std::endl
        << "printf( \"! Error (\%a) \%a\\n\", lastexception[1], lastexception[2] );\n"
        << "finally: closeallfiles();\n"
        << "if normalexit=0 then `quit`(0); else `quit(1)` end if: end try:\n";

    globalLogger__.debug("HomeLeft :: filled Maple script "+fname);
}

void HomeLeft::evaluate()
{
    if (xEquationInput_->text().empty() || yEquationInput_->text().empty()) {
        errorSignal_.emit("Cannot evaluate yet, insert a vector field in the input forms.");
        return;
    }

    //errorSignal_.emit("Evaluating vector field...");

    evaluated_ = true;

    prepareMapleFile();

    /*std::string command = "batch <<< '"+std::string(MAPLE_PATH)+" -T ,1048576 "+fileUploadName_+".mpl > "+fileUploadName_+".res'";
    int status = system(command.c_str());
    if (status == 0) {
        evaluatedSignal_.emit(fileUploadName_);
        globalLogger__.info("HomeLeft :: Maple script successfully executed");
    } else {
        errorSignal_.emit("Error during Maple script execution");
        globalLogger__.error("HomeLeft :: Maple error: "+std::to_string(status));
    }*/

    pid_t pid = fork();
    if (pid < 0) {
        errorSignal_.emit("Error creating Maple thread");
        globalLogger__.error("HomeLeft :: error during Maple script execution");
    } else if (pid == 0) {
        // create vector of arguments for execvp
        std::vector<char *> commands;
#ifdef ANTZ
        commands.push_back("ssh");
        commands.push_back("a01");
        commands.push_back("-i");
        commands.push_back("/var/www/claus_ssh/idrsa-1");
        //commands.push_back("'");
#endif
        commands.push_back(MAPLE_PATH);
        commands.push_back("-T ,1048576"); // 1GB memory limit
        char *aux = new char [fileUploadName_.length()+1];
        std::strcpy(aux,fileUploadName_.c_str());
        std::strcat(aux,".mpl");
        commands.push_back(aux);
/*#ifdef ANTZ
        commands.push_back("'");
#endif*/
        commands.push_back(nullptr);
        // output from this thread goes to "fileUploadName_.res"
        int fd = open((fileUploadName_+".res").c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);
        dup2(fd,1);
        // execute command
        char **command = commands.data();
        int status = execvp(command[0],command);
    } else {
        int status;
        waitpid(pid,&status,0);
        if (status == 0) {
            evaluatedSignal_.emit(fileUploadName_);
            globalLogger__.info("HomeLeft :: Maple script successfully executed");
        } else {
            errorSignal_.emit("Error during Maple script execution");
            globalLogger__.error("HomeLeft :: Maple error: "+std::to_string(status));
        }
    }
}



void HomeLeft::prepareSaveFile()
{
    if (xEquationInput_->text().empty() || yEquationInput_->text().empty()) {
        errorSignal_.emit("Cannot prepare a Maple script without introducing a vector field first.");
        globalLogger__.error("HomeLeft :: tried to save without entering a vector field");
        return;
    }

    std::ofstream saveFile;

    if (fileUploadName_.empty()) {
        if (saveFileName_.empty()) {
            saveFileName_ = openTempStream(TMP_DIR,".txt"/*,saveFile*/);
            saveFileName_ += ".txt";
        }
    } else
        saveFileName_ = fileUploadName_;

    saveFile.open(saveFileName_.c_str(),std::fstream::out|std::fstream::trunc);

    setParams();

    saveFile << 0                               << std::endl;   // typeofstudy
    saveFile << (str_numeric == WString("true") ? 1 : 0) << std::endl;   // numeric
    saveFile << str_precision                   << std::endl;   // precision
    saveFile << str_epsilon                     << std::endl;   // epsilon
    saveFile << (str_testsep == WString("true") ? 1 : 0) << std::endl;   // test sep
    saveFile << str_taylor                      << std::endl;   // taylor level
    saveFile << str_numericlevel                << std::endl;   // numeric level
    saveFile << str_maxlevel                    << std::endl;   // max levels
    saveFile << str_weaklevel                   << std::endl;   // weakness level
    saveFile << str_userp                       << std::endl;   // p
    saveFile << str_userq                       << std::endl;   // q
    saveFile << xEquationInput_->text()         << std::endl;   // x'
    saveFile << yEquationInput_->text()         << std::endl;   // y'
    saveFile << str_gcf                         << std::endl;   // gcf
    saveFile << 0                               << std::endl;   // numparams

    saveFile.close();

    saveFileResource_ = new WFileResource(saveFileName_);

    char date[100];
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::strftime(date,sizeof(date),"%F_%R",std::localtime(&now));
    
    saveFileResource_->suggestFileName("WP4_"+std::string(date)+".inp");
    saveAnchor_->setLink(saveFileResource_);

    prepSaveButton_->hide();
    //saveAnchor_->clicked().emit(WMouseEvent());
    saveAnchor_->show();

}


void HomeLeft::onPlot()
{
    if ( !evaluated_ ) {
        errorSignal_.emit("Cannot read results, evaluate a vector field first.\n");
    } else {
        globalLogger__.debug("HomeLeft :: sending onPlot signal");
        if (!loggedIn_)
            onPlotSphereSignal_.emit(fileUploadName_,-1);
        else {
            if (viewComboBox_->currentIndex() == 0) {
                onPlotSphereSignal_.emit(fileUploadName_,std::stod(viewProjection_->text()));
            } else {
                onPlotPlaneSignal_.emit(fileUploadName_,viewComboBox_->currentIndex(),
                std::stod(viewMinX_->text()),
                std::stod(viewMaxX_->text()),
                std::stod(viewMinY_->text()),
                std::stod(viewMaxY_->text()));    
            }
            tabs_->setCurrentWidget(viewContainer_);
        }
    }
}


void HomeLeft::showSettings()
{
    loggedIn_ = true;
    if (settingsContainer_ != nullptr) {
        delete settingsContainer_;
        settingsContainer_ = nullptr;
    }
    if (viewContainer_ != nullptr) {
        delete viewContainer_;
        viewContainer_ = nullptr;
    }
    

    WRadioButton *button;
    WDoubleValidator *validator;
    WTemplate *t;


    /* evaluation parameters */
    settingsContainer_ = new WContainerWidget(this);
    settingsContainer_->setId("settingsContainer_");
    tabs_->addTab(settingsContainer_,WString("Evaluation parameters"),WTabWidget::PreLoading);

    t = new WTemplate(WString::tr("template.homeleft-parameters"),settingsContainer_);
    t->addFunction("id",WTemplate::Functions::id);

    // calculations
    calculationsBtnGroup_ = new WButtonGroup(settingsContainer_);
    button = new WRadioButton("Algebraic",settingsContainer_);
    button->setInline(true);
    t->bindWidget("calc-algebraic",button);
    calculationsBtnGroup_->addButton(button,Algebraic);
    button = new WRadioButton("Numeric",settingsContainer_);
    button->setInline(true);
    calculationsBtnGroup_->addButton(button,Numeric);
    calculationsBtnGroup_->setCheckedButton(calculationsBtnGroup_->button(Numeric));
    t->bindString("calc-tooltip",WString::tr("tooltip.calculations"));
    t->bindWidget("calc-numeric",button);

    // separatrices
    separatricesBtnGroup_ = new WButtonGroup(settingsContainer_);
    button = new WRadioButton("Yes",settingsContainer_);
    button->setInline(true);
    t->bindWidget("sep-yes",button);
    separatricesBtnGroup_->addButton(button,Yes);
    button = new WRadioButton("No",settingsContainer_);
    button->setInline(true);
    separatricesBtnGroup_->addButton(button,No);
    separatricesBtnGroup_->setCheckedButton(separatricesBtnGroup_->button(No));
    t->bindString("sep-tooltip",WString::tr("tooltip.separatrices"));
    t->bindWidget("sep-no",button);


    // accuracy
    accuracySpinBox_ = new WSpinBox(settingsContainer_);
    accuracySpinBox_->setRange(1,14);
    accuracySpinBox_->setValue(8);
    accuracySpinBox_->setInline(true);
    t->bindWidget("acc",accuracySpinBox_);
    t->bindString("acc-tooltip",WString::tr("tooltip.accuracy"));

    // precision
    precisionSpinBox_ = new WSpinBox(settingsContainer_);
    precisionSpinBox_->setRange(0,8);
    precisionSpinBox_->setValue(0);
    t->bindWidget("pre",precisionSpinBox_);
    t->bindString("pre-tooltip",WString::tr("tooltip.precision"));

    // epsilon
    epsilonSpinBox_ = new WDoubleSpinBox(settingsContainer_);
    epsilonSpinBox_->setDecimals(2);
    epsilonSpinBox_->setSingleStep(0.01);
    epsilonSpinBox_->setValue(0.01);
    epsilonSpinBox_->setRange(0.01,0.3);
    t->bindWidget("eps",epsilonSpinBox_);
    t->bindString("eps-tooltip",WString::tr("tooltip.epsilon"));


    // level of approximation
    levAppSpinBox_ = new WSpinBox(settingsContainer_);
    levAppSpinBox_->setRange(0,10);
    levAppSpinBox_->setValue(6);
    t->bindWidget("app",levAppSpinBox_);
    t->bindString("app-tooltip",WString::tr("tooltip.approximation-level"));

    // numeric level
    numericLevelSpinBox_ = new WSpinBox(settingsContainer_);
    numericLevelSpinBox_->setRange(5,15);
    numericLevelSpinBox_->setValue(8);
    t->bindWidget("num",numericLevelSpinBox_);
    t->bindString("num-tooltip",WString::tr("tooltip.numeric-level"));


    // maximum level
    maxLevelSpinBox_ = new WSpinBox(settingsContainer_);
    maxLevelSpinBox_->setRange(15,25);
    maxLevelSpinBox_->setValue(20);
    t->bindWidget("max",maxLevelSpinBox_);
    t->bindString("max-tooltip",WString::tr("tooltip.maximum-level"));
    
    // max weakness level
    maxWeakLevelSpinBox_ = new WSpinBox(settingsContainer_);
    maxWeakLevelSpinBox_->setRange(0,8);
    maxWeakLevelSpinBox_->setValue(4);
    t->bindWidget("weak",maxWeakLevelSpinBox_);
    t->bindString("weak-tooltip",WString::tr("tooltip.maximum-weakness-level"));


    // p q
    PLWeightPSpinBox_ = new WSpinBox(settingsContainer_);
    PLWeightPSpinBox_->setRange(1,10);
    PLWeightPSpinBox_->setValue(1);
    t->bindWidget("p",PLWeightPSpinBox_);
    t->bindString("pq-tooltip",WString::tr("tooltip.poincare-lyapunov-weights"));
    
    PLWeightQSpinBox_ = new WSpinBox(settingsContainer_);
    PLWeightQSpinBox_->setRange(1,10);
    PLWeightQSpinBox_->setValue(1);
    t->bindWidget("q",PLWeightQSpinBox_);

    // enable separatrice test parameters only if separatrice testing is on Yes
    levAppSpinBox_->disable();
    numericLevelSpinBox_->disable();
    maxLevelSpinBox_->disable();
    separatricesBtnGroup_->checkedChanged().connect(std::bind([=] () {
        if (separatricesBtnGroup_->checkedId()==No) {
            levAppSpinBox_->disable();
            numericLevelSpinBox_->disable();
            maxLevelSpinBox_->disable();
        } else {
            levAppSpinBox_->enable();
            numericLevelSpinBox_->enable();
            maxLevelSpinBox_->enable();
        }
    }));



    /* view settings */
    viewContainer_ = new WContainerWidget(this);
    viewContainer_->setId("viewContainer_");
    tabs_->addTab(viewContainer_,WString::fromUTF8("View settings"),WTabWidget::PreLoading);

    // type of view
    t = new WTemplate(WString::tr("template.homeleft-view"),viewContainer_);
    t->addFunction("id",&WTemplate::Functions::id);

    viewComboBox_ = new WComboBox(viewContainer_);
    viewComboBox_->setId("viewComboBox_");
    viewComboBox_->addItem("Spherical");
    viewComboBox_->addItem("Planar");
    viewComboBox_->addItem("U1");
    viewComboBox_->addItem("V1");
    viewComboBox_->addItem("U2");
    viewComboBox_->addItem("V2");
    t->bindString("selectview-tooltip",WString::tr("tooltip.view-select"));
    t->bindWidget("selectview",viewComboBox_);

    // projection
    viewProjection_ = new WLineEdit(viewContainer_);
    viewProjection_->setText("-1");
    t->bindString("projection-tooltip",WString::tr("tooltip.view-projection"));
    t->bindWidget("projection",viewProjection_);
    validator = new WDoubleValidator(-1e16,-1e-16);
    validator->setMandatory(true);
    viewProjection_->setValidator(validator);

    viewMinX_ = new WLineEdit(viewContainer_);
    viewMinX_->setText("-1");
    viewMinX_->disable();
    t->bindString("minx-tooltip",WString::tr("tooltip.view-minx"));
    t->bindWidget("minx",viewMinX_);

    viewMaxX_ = new WLineEdit(viewContainer_);
    viewMaxX_->setText("1");
    viewMaxX_->disable();
    t->bindString("maxx-tooltip",WString::tr("tooltip.view-maxx"));
    t->bindWidget("maxx",viewMaxX_);

    viewMinY_ = new WLineEdit(viewContainer_);
    viewMinY_->setText("-1");
    viewMinY_->disable();
    t->bindString("miny-tooltip",WString::tr("tooltip.view-miny"));
    t->bindWidget("miny",viewMinY_);

    viewMaxY_ = new WLineEdit(viewContainer_);
    viewMaxY_->setText("1");
    viewMaxY_->disable();
    t->bindString("maxy-tooltip",WString::tr("tooltip.view-maxy"));
    t->bindWidget("maxy",viewMaxY_);

    // enable view range boxes only if sphere is not selected
    viewComboBox_->changed().connect(std::bind([=] () {
        switch(viewComboBox_->currentIndex()) {
        case 0:
            viewProjection_->enable();
            viewMinX_->disable();
            viewMinY_->disable();
            viewMaxX_->disable();
            viewMaxY_->disable();
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            viewProjection_->disable();
            viewMinX_->enable();
            viewMinY_->enable();
            viewMaxX_->enable();
            viewMaxY_->enable();
            break;
        }
    }));



    // orbits integration
    orbitsContainer_ = new WContainerWidget(this);
    orbitsContainer_->setId("orbitsContainer_");
    tabs_->addTab(orbitsContainer_,"Orbit integration");

    t = new WTemplate(WString::tr("template.orbits-dialog"),orbitsContainer_);
    t->addFunction("id",WTemplate::Functions::id);

    orbitsXLineEdit_ = new WLineEdit(orbitsContainer_);
    t->bindWidget("x",orbitsXLineEdit_);
    t->bindString("point-label",WString::tr("tooltip.orbits-selected-point"));
    orbitsYLineEdit_ = new WLineEdit(orbitsContainer_);
    t->bindWidget("y",orbitsYLineEdit_);

    orbitsForwardsBtn_ = new WPushButton("Forwards",orbitsContainer_);
    orbitsContinueBtn_ = new WPushButton("Continue",orbitsContainer_);
    orbitsContinueBtn_->disable();
    orbitsBackwardsBtn_ = new WPushButton("Backwards",orbitsContainer_);
    t->bindWidget("fw",orbitsForwardsBtn_);
    t->bindWidget("cnt",orbitsContinueBtn_);
    t->bindWidget("bw",orbitsBackwardsBtn_);

    orbitsDeleteOneBtn_ = new WPushButton("Delete last orbit",orbitsContainer_);
    orbitsDeleteOneBtn_->setStyleClass("btn btn-warning");
    orbitsDeleteOneBtn_->disable();
    orbitsDeleteAllBtn_ = new WPushButton("Delete all orbits",orbitsContainer_);
    orbitsDeleteAllBtn_->setStyleClass("btn btn-danger");
    orbitsDeleteAllBtn_->disable();
    t->bindWidget("dl",orbitsDeleteOneBtn_);
    t->bindWidget("da",orbitsDeleteAllBtn_);

    // enable delete orbits and continue if integrate button has been pressed
    orbitsForwardsBtn_->clicked().connect(this,&HomeLeft::onOrbitsForwardsBtn);
    orbitsBackwardsBtn_->clicked().connect(this,&HomeLeft::onOrbitsBackwardsBtn);
    orbitsContinueBtn_->clicked().connect(this,&HomeLeft::onOrbitsContinueBtn);
    orbitsDeleteOneBtn_->clicked().connect(this,&HomeLeft::onOrbitsDeleteOneBtn);
    orbitsDeleteAllBtn_->clicked().connect(this,&HomeLeft::onOrbitsDeleteAllBtn);


    tabs_->setCurrentWidget(settingsContainer_);

}

void HomeLeft::hideSettings()
{
    loggedIn_ = false;
    if (settingsContainer_ != nullptr) {
        tabs_->removeTab(settingsContainer_);
        delete settingsContainer_;
        settingsContainer_ = nullptr;
    }
    if (viewContainer_ != nullptr) {
        tabs_->removeTab(viewContainer_);
        delete viewContainer_;
        viewContainer_ = nullptr;
    }
    if (orbitsContainer_ != nullptr) {
        tabs_->removeTab(orbitsContainer_);
        delete orbitsContainer_;
        orbitsContainer_ = nullptr;
    }
}

void HomeLeft::resetUI()
{
    xEquationInput_->setText(std::string());
    yEquationInput_->setText(std::string());
    if (loggedIn_) {
        hideSettings();
        showSettings();
    }
}

void HomeLeft::showOrbitsDialog( bool clickValid, double x, double y )
{
    if (!loggedIn_)
        return;

    tabs_->setCurrentWidget(orbitsContainer_);

    if (clickValid) {
        orbitsXLineEdit_->setText(std::to_string(x));
        orbitsYLineEdit_->setText(std::to_string(y));
        orbitsForwardsBtn_->enable();
        orbitsBackwardsBtn_->enable();
        orbitsContinueBtn_->disable();
    }

}

void HomeLeft::onOrbitsForwardsBtn()
{
    if (!evaluated_ || orbitsXLineEdit_->text().empty() || orbitsYLineEdit_->text().empty())
        return;
    
    orbitsStartSelected_ = true;
    //if (orbitIntegrationStarted_)

    orbitsContinueBtn_->enable();
    orbitsDeleteOneBtn_->enable();
    orbitsDeleteAllBtn_->enable();
    orbitsForwardsBtn_->disable();

    globalLogger__.debug("HomeLeft :: orbit start ("+orbitsXLineEdit_->text().toUTF8()+","+
        orbitsYLineEdit_->text().toUTF8()+")");
    orbitIntegrateSignal_.emit(1,std::stod(orbitsXLineEdit_->text()),std::stod(orbitsYLineEdit_->text()));
}

void HomeLeft::onOrbitsBackwardsBtn()
{
    if (!evaluated_ || orbitsXLineEdit_->text().empty() || orbitsYLineEdit_->text().empty())
        return;
    
    orbitsStartSelected_ = true;
    //if (orbitIntegrationStarted_)

    orbitsContinueBtn_->enable();
    orbitsDeleteOneBtn_->enable();
    orbitsDeleteAllBtn_->enable();
    orbitsBackwardsBtn_->disable();

    orbitIntegrateSignal_.emit(-1,std::stod(orbitsXLineEdit_->text()),std::stod(orbitsYLineEdit_->text()));
}

void HomeLeft::onOrbitsContinueBtn()
{
    if (orbitsStartSelected_ /*&& orbitIntegrationStarted_*/) {
        orbitIntegrateSignal_.emit(0,0.0,0.0);
    }
}

void HomeLeft::onOrbitsDeleteOneBtn()
{
    //orbitDeleteSignal_.emit(1);
}

void HomeLeft::onOrbitsDeleteAllBtn()
{
    //orbitDeleteSignal_.emit(0);
}
