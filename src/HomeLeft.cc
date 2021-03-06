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
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "HomeLeft.h"

#include "MyLogger.h"
#include "ScriptHandler.h"
#include "custom.h"

#include <boost/filesystem.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

#include <Wt/WAnchor>
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
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WRadioButton>
#include <Wt/WSpinBox>
#include <Wt/WTabWidget>
#include <Wt/WTemplate>

using namespace Wt;

HomeLeft::HomeLeft(WContainerWidget *parent, ScriptHandler *scriptHandler)
    : WContainerWidget(parent), settingsContainer_(nullptr),
      viewContainer_(nullptr), orbitsContainer_(nullptr)
{
    loggedIn_ = false;
    evaluated_ = false;
    plotted_ = false;

    evaluatedCurve_ = false;
    nCurves_ = 0;

    evaluatedIsocline_ = false;
    nIsoclines_ = 0;

    // set CSS class for inline 50% of the screen
    setId("HomeLeft");
    setStyleClass(WString::fromUTF8("half-box-left"));

    scriptHandler_ = scriptHandler;

    // set UI and connect signals
    setupUI();
    setupConnectors();

    g_globalLogger.debug("[HomeLeft] created correctly");
}

HomeLeft::~HomeLeft()
{
    // main widget

    if (fileUploadWidget_ != nullptr) {
        delete fileUploadWidget_;
        fileUploadWidget_ = nullptr;
    }

    if (xEquationInput_ != nullptr) {
        delete xEquationInput_;
        xEquationInput_ = nullptr;
    }
    if (yEquationInput_ != nullptr) {
        delete yEquationInput_;
        yEquationInput_ = nullptr;
    }
    if (gcfEquationInput_ != nullptr) {
        delete gcfEquationInput_;
        gcfEquationInput_ = nullptr;
    }

    if (evalButton_ != nullptr) {
        delete evalButton_;
        evalButton_ = nullptr;
    }
    if (plotButton_ != nullptr) {
        delete plotButton_;
        plotButton_ = nullptr;
    }
    if (prepSaveButton_ != nullptr) {
        delete prepSaveButton_;
        prepSaveButton_ = nullptr;
    }
    if (saveAnchor_ != nullptr) {
        delete saveAnchor_;
        saveAnchor_ = nullptr;
    }
    if (resetButton_ != nullptr) {
        delete resetButton_;
        resetButton_ = nullptr;
    }

    if (equationsBox_ != nullptr) {
        delete equationsBox_;
        equationsBox_ = nullptr;
    }

    // tab widget

    if (loggedIn_)
        hideSettings();
    if (tabs_ != nullptr) {
        delete tabs_;
        tabs_ = nullptr;
    }

    g_globalLogger.debug("[HomeLeft] deleted correctly");
}

void HomeLeft::setupUI()
{
    // Equation boxes
    equationsBox_ = new WGroupBox(this);
    equationsBox_->setId("equationsBox_");
    equationsBox_->setTitle(WString::tr("homeleft.equationboxtitle"));
    addWidget(equationsBox_);

    WTemplate *t =
        new WTemplate(WString::tr("template.homeleft-default"), equationsBox_);
    t->addFunction("id", WTemplate::Functions::id);

    fileUploadWidget_ = new WFileUpload(equationsBox_);
    fileUploadWidget_->setId("fileUploadWidget_");
    fileUploadWidget_->setFileTextSize(30);
    fileUploadWidget_->setFilters(".inp");
    fileUploadWidget_->setInline(true);
    t->bindWidget("upload", fileUploadWidget_);

    xEquationInput_ = new WLineEdit();
    xEquationInput_->setId("xEquationInput_");
    yEquationInput_ = new WLineEdit();
    yEquationInput_->setId("yEquationInput_");
    gcfEquationInput_ = new WLineEdit();
    gcfEquationInput_->setId("gcfEquationInput_");
    t->bindString("vf-tooltip", WString::tr("tooltip.vectorfield"));
    t->bindWidget("xeq", xEquationInput_);
    t->bindWidget("yeq", yEquationInput_);
    t->bindString("gcf-tooltip", WString::tr("tooltip.gcf"));
    t->bindWidget("gcf", gcfEquationInput_);

    /* Buttons */
    // eval button
    evalButton_ = new WPushButton("Evaluate", equationsBox_);
    evalButton_->setId("evalButton_");
    evalButton_->setStyleClass("btn btn-primary");
    evalButton_->setInline(true);
    evalButton_->setToolTip(WString::tr("tooltip.homeleft-eval-button"));
    t->bindWidget("eval", evalButton_);

    // plot button
    plotButton_ = new WPushButton("Plot", equationsBox_);
    plotButton_->setId("plotButton_");
    plotButton_->setStyleClass("btn btn-default");
    plotButton_->setInline(true);
    plotButton_->setToolTip(WString::tr("tooltip.homeleft-plot-button"));
    t->bindWidget("plot", plotButton_);

    // prepare save file button
    prepSaveButton_ = new WPushButton("Prepare save file", equationsBox_);
    prepSaveButton_->setId("prepSaveButton_");
    prepSaveButton_->setStyleClass("btn btn-default");
    prepSaveButton_->setInline(true);
    prepSaveButton_->setToolTip(WString::tr("tooltip.homeleft-save-button"));
    t->bindWidget("prep-save", prepSaveButton_);

    // save file anchor
    saveAnchor_ = new WAnchor(equationsBox_);
    saveAnchor_->setId("saveAnchor_");
    saveAnchor_->setStyleClass("btn btn-default");
    saveAnchor_->setText("Download save file");
    saveAnchor_->setInline(true);
    saveAnchor_->setToolTip(WString::tr("tooltip.homeleft-save-button"));
    saveAnchor_->hide();
    t->bindWidget("down-save", saveAnchor_);

    resetButton_ = new WPushButton("Reset", equationsBox_);
    resetButton_->setId("resetButton_");
    resetButton_->setStyleClass("btn btn-warning");
    resetButton_->setInline(true);
    resetButton_->setToolTip(WString::tr("tooltip.homeleft-clear-button"));
    t->bindWidget("reset", resetButton_);

    new WBreak(this);

    /* tab widget for legend et al */
    tabs_ = new WTabWidget(this);
    // legend tab ----
    WContainerWidget *legendContainer = new WContainerWidget();
    legendContainer->setId("legendContainer");
    tabs_->addTab(legendContainer, WString::fromUTF8("Legend"),
                  WTabWidget::PreLoading);
    addWidget(tabs_);

    t = new WTemplate(WString::tr("template.legend"), legendContainer);
    t->addFunction("id", WTemplate::Functions::id);

    // legend image
    WImage *legend =
        new WImage(WLink("resources/p4legend.png"), legendContainer);
    legend->setAlternateText("Plot legend");
    legend->setId("legend");
    legend->resize(400, 300);
    t->bindWidget("img", legend);

    g_globalLogger.debug("[HomeLeft] UI set up");
}

void HomeLeft::setupConnectors()
{
    // file upload
    fileUploadWidget_->changed().connect(fileUploadWidget_,
                                         &WFileUpload::upload);
    fileUploadWidget_->uploaded().connect(this, &HomeLeft::fileUploaded);
    fileUploadWidget_->fileTooLarge().connect(this, &HomeLeft::fileTooLarge);

    // buttons
    evalButton_->clicked().connect(this, &HomeLeft::evaluate);
    plotButton_->clicked().connect(this, &HomeLeft::onPlot);
    resetButton_->clicked().connect(this, &HomeLeft::resetUI);
    prepSaveButton_->clicked().connect(this, &HomeLeft::prepareSaveFile);

    saveAnchor_->clicked().connect(std::bind([=]() {
        saveAnchor_->hide();
        prepSaveButton_->show();
        delete saveFileResource_;
    }));

    g_globalLogger.debug("[HomeLeft] connectors set up");
}

void HomeLeft::fileUploaded()
{
    g_globalLogger.debug("[HomeLeft] input file uploaded");
    g_globalLogger.debug("[HomeLeft] resetting UI before parsing file");

    resetUI();
    // input validation
    std::string extension = fileUploadWidget_->clientFileName().toUTF8().substr(
        fileUploadWidget_->clientFileName().toUTF8().find_last_of(".") + 1);
    if (extension != "inp") {
        g_globalLogger.warning(
            "[HomeLeft] client tried to upload a not supported file type");
        errorSignal_.emit("Filetype not accepted.");
        return;
    }

    fileUploadName_ = fileUploadWidget_->spoolFileName();

#ifdef ANTZ
    g_globalLogger.debug("[HomeLeft] [ANTZ] copying uploaded file to " +
                         std::string(TMP_DIR));

    try {
        boost::filesystem::copy_file(fileUploadName_,
                                     TMP_DIR + fileUploadName_.substr(5));
        fileUploadName_ = TMP_DIR + fileUploadName_.substr(5);
    } catch (const boost::filesystem::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
#endif

    evaluated_ = false;

    parseInputFile();

    if (loggedIn_)
        tabs_->setCurrentWidget(settingsContainer_);

    g_globalLogger.debug("[HomeLeft] file read correctly.");
}

void HomeLeft::fileTooLarge()
{
    g_globalLogger.warning("[HomeLeft] Client tried to upload file too large.");
    errorSignal_.emit("File too large.");
}

void HomeLeft::parseInputFile()
{
    // read file and fill xEquationInput and yEquationInput
    std::ifstream f;
    std::string line;
    bool hasParams = false;

    f.open(fileUploadName_.c_str());

    if (f.is_open()) {
        int i = 0;
        if (loggedIn_) {
            // while (std::getline(f, line)) {
            for (i = 0; i < 15; i++) {
                std::getline(f, line);
                if (line.empty()) {
                    g_globalLogger.error("[HomeLeft] error reading input file, "
                                         "expected more lines.");
                    break;
                }
                switch (i) {
                case 0:
                    // what is line 0?
                    break;
                case 1:
                    if (line == "0")
                        calculationsBtnGroup_->setCheckedButton(
                            calculationsBtnGroup_->button(Algebraic));
                    else
                        calculationsBtnGroup_->setCheckedButton(
                            calculationsBtnGroup_->button(Numeric));
                    break;
                case 2:
                    accuracySpinBox_->setValue(
                        (stoi(line) > 0) ? stoi(line) : ACCURACY_DEFAULT);
                    break;
                case 3:
                    if (line[0] == '.')
                        line = "0" + line;
                    epsilonSpinBox_->setValue(
                        (stod(line) > 0) ? stod(line) : EPSILON_DEFAULT);
                    break;
                case 4:
                    if (line == "0") {
                        separatricesBtnGroup_->setCheckedButton(
                            separatricesBtnGroup_->button(No));
                        levAppSpinBox_->disable();
                        numericLevelSpinBox_->disable();
                        maxLevelSpinBox_->disable();
                    } else {
                        separatricesBtnGroup_->setCheckedButton(
                            separatricesBtnGroup_->button(Yes));
                        levAppSpinBox_->enable();
                        numericLevelSpinBox_->enable();
                        maxLevelSpinBox_->enable();
                    }
                    break;
                case 5:
                    levAppSpinBox_->setValue((stoi(line) > 0) ? stoi(line)
                                                              : APPROX_DEFAULT);
                    break;
                case 6:
                    numericLevelSpinBox_->setValue(
                        (stoi(line) > 0) ? stoi(line) : NUMERIC_DEFAULT);
                    break;
                case 7:
                    maxLevelSpinBox_->setValue(
                        (stoi(line) > 0) ? stoi(line) : MAXIMUM_DEFAULT);
                    break;
                case 8:
                    maxWeakLevelSpinBox_->setValue(
                        (stoi(line) > 0) ? stoi(line) : WEAKNESS_DEFAULT);
                    break;
                case 9:
                    PLWeightPSpinBox_->setValue((stoi(line) > 0) ? stoi(line)
                                                                 : PQ_DEFAULT);
                    break;
                case 10:
                    PLWeightQSpinBox_->setValue((stoi(line) > 0) ? stoi(line)
                                                                 : PQ_DEFAULT);
                    break;
                case 11:
                    xEquationInput_->setText(line);
                    break;
                case 12:
                    yEquationInput_->setText(line);
                    break;
                case 13:
                    gcfEquationInput_->setText(line);
                    break;
                case 14:
                    if (stoi(line) > 0 && stoi(line) < PARAMS_MAX) {
                        nParams_ = stoi(line);
                    } else {
                        nParams_ = 0;
                    }
                    for (int j = 0; j < nParams_; j++) {
                        std::string label, value;
                        std::getline(f, label);
                        if (label.empty()) {
                            g_globalLogger.error("[HomeLeft] error reading "
                                                 "input file, wrong number of "
                                                 "parameters.");
                            break;
                        }
                        std::getline(f, value);
                        if (value.empty()) {
                            g_globalLogger.error("[HomeLeft] error reading "
                                                 "input file, wrong number of "
                                                 "parameters.");
                            break;
                        }
                        addParameterToList(label, value);
                        hasParams = true;
                    }
                    break;
                default:
                    break;
                }
            }
        } else {
            i = 0;
            while (std::getline(f, line)) {
                if (i == 11)
                    xEquationInput_->setText(WString::fromUTF8(line));
                else if (i == 12)
                    yEquationInput_->setText(WString::fromUTF8(line));
                else if (i == 13)
                    gcfEquationInput_->setText(WString::fromUTF8(line));
                i++;
            }
        }
        if (f.eof() && i < 12) {
            g_globalLogger.error(
                "[HomeLeft] EOF while reading input file uploaded with name " +
                fileUploadName_);
            errorSignal_.emit("Invalid input file.");
            fileUploadName_ = std::string();
        } else if (f.bad()) {
            g_globalLogger.error("[HomeLeft] I/O error while reading input "
                                 "file uploaded with name " +
                                 fileUploadName_);
            errorSignal_.emit("Invalid input file.");
            fileUploadName_ = std::string();
        } else {
            // prepareSaveFile();
            g_globalLogger.debug("[HomeLeft] Input file uploaded with name " +
                                 fileUploadName_);
            if (!hasParams) {
                textSignal_.emit("File uploaded. Press the Evaluate button to "
                                 "start computing.");
            }
        }
        f.close();
    }
}

void HomeLeft::setOptions()
{
    parent_->getMapleParams();

    scriptHandler_->str_xeq_ = xEquationInput_->text().toUTF8();
    scriptHandler_->str_yeq_ = yEquationInput_->text().toUTF8();
    scriptHandler_->str_gcf_ = gcfEquationInput_->text().empty()
                                   ? "0"
                                   : gcfEquationInput_->text().toUTF8();
    scriptHandler_->str_userf_ = "[ " + scriptHandler_->str_xeq_ + ", " +
                                 scriptHandler_->str_yeq_ + " ]";
    if (!loggedIn_) {
        scriptHandler_->str_critpoints_ = "0";
        scriptHandler_->str_saveall_ = "false";
        scriptHandler_->str_numeric_ = "true";
        scriptHandler_->str_epsilon_ = "0.01";
        scriptHandler_->str_testsep_ = "false";
        scriptHandler_->str_precision_ = "8";
        scriptHandler_->str_precision0_ = "0";
        scriptHandler_->str_taylor_ = "6";
        scriptHandler_->str_numericlevel_ = "10";
        scriptHandler_->str_maxlevel_ = "20";
        scriptHandler_->str_weaklevel_ = "0";
        scriptHandler_->str_userp_ = "1";
        scriptHandler_->str_userq_ = "1";
        scriptHandler_->time_limit_ = "30";
    } else {
        // validators
        double dval;
        int ival;
        // set options
        scriptHandler_->str_critpoints_ = "0";
        scriptHandler_->str_saveall_ = "false";
        scriptHandler_->str_numeric_ =
            (calculationsBtnGroup_->checkedId() == Numeric) ? "true" : "false";
        dval = epsilonSpinBox_->value();
        if (dval < EPSILON_MIN || dval > EPSILON_MAX) {
            scriptHandler_->str_epsilon_ = std::to_string(EPSILON_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_epsilon out of bounds, "
                                   "setting to default value");
            epsilonSpinBox_->setValue(EPSILON_DEFAULT);
        } else {
            scriptHandler_->str_epsilon_ = std::to_string(dval);
        }
        scriptHandler_->str_testsep_ =
            (separatricesBtnGroup_->checkedId() == No) ? "false" : "true";
        ival = accuracySpinBox_->value();
        if (ival < ACCURACY_MIN || ival > ACCURACY_MAX) {
            scriptHandler_->str_precision_ = std::to_string(ACCURACY_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_precision out of bounds, "
                                   "setting to default value");
            accuracySpinBox_->setValue(ACCURACY_DEFAULT);
        } else {
            scriptHandler_->str_precision_ = std::to_string(ival);
        }
        ival = precisionSpinBox_->value();
        if (ival < PRECISION_MIN || ival > PRECISION_MAX) {
            scriptHandler_->str_precision0_ = std::to_string(PRECISION_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_precision0 out of bounds, "
                                   "setting to default value");
            precisionSpinBox_->setValue(PRECISION_DEFAULT);
        } else {
            scriptHandler_->str_precision0_ = std::to_string(ival);
        }
        ival = levAppSpinBox_->value();
        if (ival < APPROX_MIN || ival > APPROX_MAX) {
            scriptHandler_->str_taylor_ = std::to_string(APPROX_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_taylor out of bounds, "
                                   "setting to default value");
            levAppSpinBox_->setValue(APPROX_DEFAULT);
        } else {
            scriptHandler_->str_taylor_ = std::to_string(ival);
        }
        ival = numericLevelSpinBox_->value();
        if (ival < NUMERIC_MIN || ival > NUMERIC_MAX) {
            scriptHandler_->str_numericlevel_ = std::to_string(NUMERIC_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_numericlevel out of "
                                   "bounds, setting to default value");
            numericLevelSpinBox_->setValue(NUMERIC_DEFAULT);
        } else {
            scriptHandler_->str_numericlevel_ = std::to_string(ival);
        }
        ival = maxLevelSpinBox_->value();
        if (ival < MAXIMUM_MIN || ival > MAXIMUM_MAX) {
            scriptHandler_->str_maxlevel_ = std::to_string(MAXIMUM_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_maxlevel out of bounds, "
                                   "setting to default value");
            maxLevelSpinBox_->setValue(MAXIMUM_DEFAULT);
        } else {
            scriptHandler_->str_maxlevel_ = std::to_string(ival);
        }
        ival = maxWeakLevelSpinBox_->value();
        if (ival < WEAKNESS_MIN || ival > WEAKNESS_MAX) {
            scriptHandler_->str_weaklevel_ = std::to_string(WEAKNESS_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_weaklevel out of bounds, "
                                   "setting to default value");
            maxWeakLevelSpinBox_->setValue(WEAKNESS_DEFAULT);
        } else {
            scriptHandler_->str_weaklevel_ = std::to_string(ival);
        }
        ival = PLWeightPSpinBox_->value();
        if (ival < PQ_MIN || ival > PQ_MAX) {
            scriptHandler_->str_userp_ = std::to_string(PQ_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_userp out of bounds, "
                                   "setting to default value");
            PLWeightPSpinBox_->setValue(PQ_DEFAULT);
        } else {
            scriptHandler_->str_userp_ = std::to_string(ival);
        }
        ival = PLWeightQSpinBox_->value();
        if (ival < PQ_MIN || ival > PQ_MAX) {
            scriptHandler_->str_userq_ = std::to_string(PQ_DEFAULT);
            g_globalLogger.warning("[HomeLeft] str_userq out of bounds, "
                                   "setting to default value");
            PLWeightQSpinBox_->setValue(PQ_DEFAULT);
        } else {
            scriptHandler_->str_userq_ = std::to_string(ival);
        }
        scriptHandler_->time_limit_ = "180";
    }
}

void HomeLeft::evaluate()
{
    evaluated_ = false;
    // validate options
    if (xEquationInput_->text().empty() || yEquationInput_->text().empty()) {
        errorSignal_.emit(
            "Cannot evaluate yet, insert a vector field in the input forms.");
        return;
    }

    evaluated_ = true;

    setOptions();
    if (scriptHandler_->prepareMapleFile(fileUploadName_)) {
        g_globalLogger.debug("[HomeLeft] filled Maple script " +
                             fileUploadName_);
    } else {
        g_globalLogger.error("[HomeLeft] Error creating Maple script.");
        errorSignal_.emit("Error creating Maple script.");
        return;
    }

    siginfo_t status = scriptHandler_->evaluateMapleScript(
        fileUploadName_, stoi(scriptHandler_->time_limit_));
    if (status.si_status == 0) {
        g_globalLogger.debug("[HomeLeft] Maple script executed");
        evaluatedSignal_.emit(fileUploadName_);
    } else {
        if (status.si_code == CLD_EXITED) {
            g_globalLogger.error("[HomeLeft] Maple error");
            evaluatedSignal_.emit(fileUploadName_);
            // errorSignal_.emit("Maple process terminated prematurely (.");
        } else if (status.si_code == CLD_KILLED) {
            g_globalLogger.error("[HomeLeft] Maple process killed by system");
            errorSignal_.emit("Maple process killed by system.");
        } else if (status.si_code == -2) {
            g_globalLogger.error(
                "[HomeLeft] Maple computation ran out of time");
            errorSignal_.emit("Computation ran out of time");
        } else {
            g_globalLogger.error("[HomeLeft] unkwnown error in Maple process");
            errorSignal_.emit("Unknown error when creating Maple process.");
        }
    }
}

void HomeLeft::prepareSaveFile()
{
    if (xEquationInput_->text().empty() || yEquationInput_->text().empty()) {
        errorSignal_.emit("Cannot prepare a Maple script without introducing a "
                     "vector field first.");
        g_globalLogger.error(
            "[HomeLeft] tried to save without entering a vector field");
        return;
    }

    // std::ofstream saveFile;

    if (fileUploadName_.empty()) {
        if (saveFileName_.empty()) {
            saveFileName_ = scriptHandler_->randomFileName(TMP_DIR, ".txt");
            saveFileName_ += ".txt";
        }
    } else
        saveFileName_ = fileUploadName_;

    setOptions();
    if (!scriptHandler_->fillSaveFile(saveFileName_)) {
        g_globalLogger.error("Cannot create save file " + saveFileName_);
        errorSignal_.emit("Could not create save file. You can notify this "
                     "error at osr@mat.uab.cat, sorry for the "
                     "inconvenience.");
        return;
    }

    saveFileResource_ = new WFileResource(saveFileName_);

    char date[100];
    std::time_t now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::strftime(date, sizeof(date), "%F_%R", std::localtime(&now));

    saveFileResource_->suggestFileName("WP4_" + std::string(date) + ".inp");
    saveAnchor_->setLink(saveFileResource_);

    prepSaveButton_->hide();
    // saveAnchor_->clicked().emit(WMouseEvent());
    saveAnchor_->show();
}

void HomeLeft::onPlot()
{
    if (!evaluated_) {
        errorSignal_.emit("Cannot read results, evaluate a vector field first.");
    } else {
        g_globalLogger.debug("[HomeLeft] sending onPlot signal");
        if (!loggedIn_)
            onPlotSphereSignal_.emit(fileUploadName_, -1);
        else {
            if (viewComboBox_->currentIndex() == 0) {
                double proj;
                try {
                    proj = std::stod(viewProjection_->text());
                } catch (...) {
                    proj = PROJECTION_DEFAULT;
                    viewProjection_->setText(
                        std::to_string(PROJECTION_DEFAULT));
                    g_globalLogger.warning("[HomeLeft] bad view settings, "
                                           "setting to default value");
                }
                onPlotSphereSignal_.emit(fileUploadName_, proj);
            } else {
                double minx, maxx, miny, maxy;
                try {
                    minx = std::stod(viewMinX_->text());
                } catch (...) {
                    minx = -1;
                    viewMinX_->setText("-1");
                    g_globalLogger.warning("[HomeLeft] bad view settings, "
                                           "setting to default value");
                }
                try {
                    maxx = std::stod(viewMaxX_->text());
                } catch (...) {
                    maxx = 1;
                    viewMaxX_->setText("1");
                    g_globalLogger.warning("[HomeLeft] bad view settings, "
                                           "setting to default value");
                }
                try {
                    miny = std::stod(viewMinY_->text());
                } catch (...) {
                    miny = -1;
                    viewMinY_->setText("-1");
                    g_globalLogger.warning("[HomeLeft] bad view settings, "
                                           "setting to default value");
                }
                try {
                    maxy = std::stod(viewMaxY_->text());
                } catch (...) {
                    maxy = 1;
                    viewMaxY_->setText("1");
                    g_globalLogger.warning("[HomeLeft] bad view settings, "
                                           "setting to default value");
                }
                onPlotPlaneSignal_.emit(fileUploadName_,
                                        viewComboBox_->currentIndex(), minx,
                                        maxx, miny, maxy);
            }
            plotted_ = true;
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

    /*
     * evaluation options
     */
    settingsContainer_ = new WContainerWidget(this);
    settingsContainer_->setId("settingsContainer_");
    tabs_->addTab(settingsContainer_,
                  WString::tr("homeleft.evaluation-options"),
                  WTabWidget::PreLoading);

    t = new WTemplate(WString::tr("template.homeleft-options"),
                      settingsContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    // calculations
    calculationsBtnGroup_ = new WButtonGroup(settingsContainer_);
    button = new WRadioButton("Algebraic", settingsContainer_);
    button->setInline(true);
    t->bindWidget("calc-algebraic", button);
    calculationsBtnGroup_->addButton(button, Algebraic);
    button = new WRadioButton("Numeric", settingsContainer_);
    button->setInline(true);
    calculationsBtnGroup_->addButton(button, Numeric);
    calculationsBtnGroup_->setCheckedButton(
        calculationsBtnGroup_->button(Numeric));
    t->bindString("calc-tooltip", WString::tr("tooltip.calculations"));
    t->bindWidget("calc-numeric", button);

    // separatrices
    separatricesBtnGroup_ = new WButtonGroup(settingsContainer_);
    button = new WRadioButton("Yes", settingsContainer_);
    button->setInline(true);
    t->bindWidget("sep-yes", button);
    separatricesBtnGroup_->addButton(button, Yes);
    button = new WRadioButton("No", settingsContainer_);
    button->setInline(true);
    separatricesBtnGroup_->addButton(button, No);
    separatricesBtnGroup_->setCheckedButton(separatricesBtnGroup_->button(No));
    t->bindString("sep-tooltip", WString::tr("tooltip.separatrices"));
    t->bindWidget("sep-no", button);

    // accuracy
    accuracySpinBox_ = new WSpinBox(settingsContainer_);
    accuracySpinBox_->setRange(ACCURACY_MIN, ACCURACY_MAX);
    accuracySpinBox_->setValue(ACCURACY_DEFAULT);
    accuracySpinBox_->setInline(true);
    t->bindWidget("acc", accuracySpinBox_);
    t->bindString("acc-tooltip", WString::tr("tooltip.accuracy"));

    // precision
    precisionSpinBox_ = new WSpinBox(settingsContainer_);
    precisionSpinBox_->setRange(PRECISION_MIN, PRECISION_MAX);
    precisionSpinBox_->setValue(PRECISION_DEFAULT);
    t->bindWidget("pre", precisionSpinBox_);
    t->bindString("pre-tooltip", WString::tr("tooltip.precision"));

    // epsilon
    epsilonSpinBox_ = new WDoubleSpinBox(settingsContainer_);
    epsilonSpinBox_->setDecimals(2);
    epsilonSpinBox_->setSingleStep(0.01);
    epsilonSpinBox_->setRange(EPSILON_MIN, EPSILON_MAX);
    epsilonSpinBox_->setValue(EPSILON_DEFAULT);
    t->bindWidget("eps", epsilonSpinBox_);
    t->bindString("eps-tooltip", WString::tr("tooltip.epsilon"));

    // level of approximation
    levAppSpinBox_ = new WSpinBox(settingsContainer_);
    levAppSpinBox_->setRange(APPROX_MIN, APPROX_MAX);
    levAppSpinBox_->setValue(APPROX_DEFAULT);
    t->bindWidget("app", levAppSpinBox_);
    t->bindString("app-tooltip", WString::tr("tooltip.approximation-level"));

    // numeric level
    numericLevelSpinBox_ = new WSpinBox(settingsContainer_);
    numericLevelSpinBox_->setRange(NUMERIC_MIN, NUMERIC_MAX);
    numericLevelSpinBox_->setValue(NUMERIC_DEFAULT);
    t->bindWidget("num", numericLevelSpinBox_);
    t->bindString("num-tooltip", WString::tr("tooltip.numeric-level"));

    // maximum level
    maxLevelSpinBox_ = new WSpinBox(settingsContainer_);
    maxLevelSpinBox_->setRange(MAXIMUM_MIN, MAXIMUM_MAX);
    maxLevelSpinBox_->setValue(MAXIMUM_DEFAULT);
    t->bindWidget("max", maxLevelSpinBox_);
    t->bindString("max-tooltip", WString::tr("tooltip.maximum-level"));

    // max weakness level
    maxWeakLevelSpinBox_ = new WSpinBox(settingsContainer_);
    maxWeakLevelSpinBox_->setRange(WEAKNESS_MIN, WEAKNESS_MAX);
    maxWeakLevelSpinBox_->setValue(WEAKNESS_DEFAULT);
    t->bindWidget("weak", maxWeakLevelSpinBox_);
    t->bindString("weak-tooltip",
                  WString::tr("tooltip.maximum-weakness-level"));

    // p q
    PLWeightPSpinBox_ = new WSpinBox(settingsContainer_);
    PLWeightPSpinBox_->setRange(PQ_MIN, PQ_MAX);
    PLWeightPSpinBox_->setValue(PQ_DEFAULT);
    t->bindWidget("p", PLWeightPSpinBox_);
    t->bindString("pq-tooltip",
                  WString::tr("tooltip.poincare-lyapunov-weights"));

    PLWeightQSpinBox_ = new WSpinBox(settingsContainer_);
    PLWeightQSpinBox_->setRange(PQ_MIN, PQ_MAX);
    PLWeightQSpinBox_->setValue(PQ_DEFAULT);
    t->bindWidget("q", PLWeightQSpinBox_);

    // enable separatrice test options only if separatrice testing is on Yes
    levAppSpinBox_->disable();
    numericLevelSpinBox_->disable();
    maxLevelSpinBox_->disable();
    separatricesBtnGroup_->checkedChanged().connect(std::bind([=]() {
        if (separatricesBtnGroup_->checkedId() == No) {
            levAppSpinBox_->disable();
            numericLevelSpinBox_->disable();
            maxLevelSpinBox_->disable();
        } else {
            levAppSpinBox_->enable();
            numericLevelSpinBox_->enable();
            maxLevelSpinBox_->enable();
        }
    }));

    /*
     * view settings
     */
    viewContainer_ = new WContainerWidget(this);
    viewContainer_->setId("viewContainer_");
    tabs_->addTab(viewContainer_, WString::fromUTF8("View settings"),
                  WTabWidget::PreLoading);

    // type of view
    t = new WTemplate(WString::tr("template.homeleft-view"), viewContainer_);
    t->addFunction("id", &WTemplate::Functions::id);

    viewComboBox_ = new WComboBox(viewContainer_);
    viewComboBox_->setId("viewComboBox_");
    viewComboBox_->addItem("Spherical");
    viewComboBox_->addItem("Planar");
    viewComboBox_->addItem("U1");
    viewComboBox_->addItem("V1");
    viewComboBox_->addItem("U2");
    viewComboBox_->addItem("V2");
    t->bindString("selectview-tooltip", WString::tr("tooltip.view-select"));
    t->bindWidget("selectview", viewComboBox_);

    // projection
    viewProjection_ = new WLineEdit(viewContainer_);
    viewProjection_->setText("-1");
    t->bindString("projection-tooltip", WString::tr("tooltip.view-projection"));
    t->bindWidget("projection", viewProjection_);
    validator = new WDoubleValidator(-1e16, -1e-16);
    validator->setMandatory(true);
    viewProjection_->setValidator(validator);

    viewMinX_ = new WLineEdit(viewContainer_);
    viewMinX_->setText("-1");
    viewMinX_->disable();
    t->bindString("minx-tooltip", WString::tr("tooltip.view-minx"));
    t->bindWidget("minx", viewMinX_);

    viewMaxX_ = new WLineEdit(viewContainer_);
    viewMaxX_->setText("1");
    viewMaxX_->disable();
    t->bindString("maxx-tooltip", WString::tr("tooltip.view-maxx"));
    t->bindWidget("maxx", viewMaxX_);

    viewMinY_ = new WLineEdit(viewContainer_);
    viewMinY_->setText("-1");
    viewMinY_->disable();
    t->bindString("miny-tooltip", WString::tr("tooltip.view-miny"));
    t->bindWidget("miny", viewMinY_);

    viewMaxY_ = new WLineEdit(viewContainer_);
    viewMaxY_->setText("1");
    viewMaxY_->disable();
    t->bindString("maxy-tooltip", WString::tr("tooltip.view-maxy"));
    t->bindWidget("maxy", viewMaxY_);

    // enable view range boxes only if sphere is not selected
    viewComboBox_->changed().connect(std::bind([=]() {
        switch (viewComboBox_->currentIndex()) {
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

    // refresh button
    refreshPlotButton_ = new WPushButton("Refresh plot", viewContainer_);
    t->bindWidget("btn", refreshPlotButton_);

    // connect refresh button to refresh plot signal
    refreshPlotButton_->clicked().connect(this, &HomeLeft::onRefreshPlotBtn);

    /*
     * orbits integration
     */
    orbitsContainer_ = new WContainerWidget(this);
    orbitsContainer_->setId("orbitsContainer_");
    tabs_->addTab(orbitsContainer_, "Orbits");

    t = new WTemplate(WString::tr("template.homeleft-orbits"),
                      orbitsContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    orbitsXLineEdit_ = new WLineEdit(orbitsContainer_);
    t->bindWidget("x", orbitsXLineEdit_);
    t->bindString("point-label", WString::tr("tooltip.orbits-selected-point"));
    orbitsYLineEdit_ = new WLineEdit(orbitsContainer_);
    t->bindWidget("y", orbitsYLineEdit_);

    // reenable forward/backward and disable continue when point is modified
    // manually
    orbitsXLineEdit_->changed().connect(this, &HomeLeft::onOrbitsDialogChange);
    orbitsYLineEdit_->changed().connect(this, &HomeLeft::onOrbitsDialogChange);

    orbitsForwardsBtn_ = new WPushButton("Forwards", orbitsContainer_);
    orbitsContinueBtn_ = new WPushButton("Continue", orbitsContainer_);
    orbitsContinueBtn_->disable();
    orbitsBackwardsBtn_ = new WPushButton("Backwards", orbitsContainer_);
    t->bindWidget("fw", orbitsForwardsBtn_);
    t->bindWidget("cnt", orbitsContinueBtn_);
    t->bindWidget("bw", orbitsBackwardsBtn_);

    orbitsDeleteOneBtn_ =
        new WPushButton("Delete last orbit", orbitsContainer_);
    orbitsDeleteOneBtn_->setStyleClass("btn btn-warning");
    orbitsDeleteOneBtn_->disable();
    orbitsDeleteAllBtn_ =
        new WPushButton("Delete all orbits", orbitsContainer_);
    orbitsDeleteAllBtn_->setStyleClass("btn btn-danger");
    orbitsDeleteAllBtn_->disable();
    t->bindWidget("dl", orbitsDeleteOneBtn_);
    t->bindWidget("da", orbitsDeleteAllBtn_);

    // enable delete orbits and continue if integrate button has been pressed
    orbitsForwardsBtn_->clicked().connect(this, &HomeLeft::onOrbitsForwardsBtn);
    orbitsBackwardsBtn_->clicked().connect(this,
                                           &HomeLeft::onOrbitsBackwardsBtn);
    orbitsContinueBtn_->clicked().connect(this, &HomeLeft::onOrbitsContinueBtn);
    orbitsDeleteOneBtn_->clicked().connect(this,
                                           &HomeLeft::onOrbitsDeleteOneBtn);
    orbitsDeleteAllBtn_->clicked().connect(this,
                                           &HomeLeft::onOrbitsDeleteAllBtn);

    /*
     * gcf settings
     */
    gcfContainer_ = new WContainerWidget(this);
    gcfContainer_->setId("gcfContainer_");
    tabs_->addTab(gcfContainer_, "Gcf");

    t = new WTemplate(WString::tr("template.homeleft-gcf"), gcfContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    // appearance
    gcfAppearanceBtnGrp_ = new WButtonGroup(gcfContainer_);
    button = new WRadioButton("Dots", gcfContainer_);
    button->setInline(true);
    t->bindWidget("gcf-dots", button);
    t->bindString("gcf-tooltip-dots", WString::tr("tooltip.appearance-dots"));
    gcfAppearanceBtnGrp_->addButton(button, Dots);
    button = new WRadioButton("Dashes", gcfContainer_);
    button->setInline(true);
    gcfAppearanceBtnGrp_->addButton(button, Dashes);
    gcfAppearanceBtnGrp_->setCheckedButton(
        gcfAppearanceBtnGrp_->button(Dashes));
    t->bindWidget("gcf-dashes", button);
    t->bindString("gcf-tooltip-dashes",
                  WString::tr("tooltip.appearance-dashes"));

    // n points
    gcfNPointsSpinBox_ = new WSpinBox(gcfContainer_);
    gcfNPointsSpinBox_->setRange(GCF_NP_MIN, GCF_NP_MAX);
    gcfNPointsSpinBox_->setValue(GCF_NP_DEFAULT);
    t->bindWidget("nps", gcfNPointsSpinBox_);
    t->bindString("gcf-tooltip-nps", WString::tr("tooltip.npoints"));

    // precision
    gcfPrecisionSpinBox_ = new WSpinBox(gcfContainer_);
    gcfPrecisionSpinBox_->setRange(GCF_PREC_MIN, GCF_PREC_MAX);
    gcfPrecisionSpinBox_->setValue(GCF_PREC_DEFAULT);
    t->bindWidget("gcf-prc", gcfPrecisionSpinBox_);
    t->bindString("gcf-tooltip-prc", WString::tr("tooltip.gcf-prc"));

    // plot gcf button
    gcfPlotBtn_ = new WPushButton("Plot GCF", gcfContainer_);
    t->bindWidget("gcf-btn", gcfPlotBtn_);

    // connect gcf plot button to function
    gcfPlotBtn_->clicked().connect(this, &HomeLeft::onPlotGcfBtn);

    /*
     * Curves
     */
    curvesContainer_ = new WContainerWidget(this);
    curvesContainer_->setId("curvesContainer_");
    tabs_->addTab(curvesContainer_, "Curves");

    t = new WTemplate(WString::tr("template.homeleft-curves"),
                      curvesContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    // curve equation
    curvesLineEdit_ = new WLineEdit(curvesContainer_);
    t->bindWidget("curve-eqn", curvesLineEdit_);
    t->bindString("curve-tooltip-eqn", WString::tr("tooltip.curve-eqn"));

    // appearance
    curvesAppearanceBtnGrp_ = new WButtonGroup(curvesContainer_);
    t->bindString("curve-tooltip-appearance",
                  WString::tr("tooltip.appearance"));
    button = new WRadioButton("Dots", curvesContainer_);
    button->setInline(true);
    t->bindWidget("curve-dots", button);
    t->bindString("curve-tooltip-dots", WString::tr("tooltip.appearance-dots"));
    curvesAppearanceBtnGrp_->addButton(button, Dots);
    button = new WRadioButton("Dashes", curvesContainer_);
    button->setInline(true);
    curvesAppearanceBtnGrp_->addButton(button, Dashes);
    curvesAppearanceBtnGrp_->setCheckedButton(
        curvesAppearanceBtnGrp_->button(Dashes));
    t->bindWidget("curve-dashes", button);
    t->bindString("curve-tooltip-dashes",
                  WString::tr("tooltip.appearance-dashes"));

    // n points
    curvesNPointsSpinBox_ = new WSpinBox(curvesContainer_);
    curvesNPointsSpinBox_->setRange(CURVES_NP_MIN, CURVES_NP_MAX);
    curvesNPointsSpinBox_->setValue(CURVES_NP_DEFAULT);
    t->bindWidget("nps", curvesNPointsSpinBox_);
    t->bindString("curve-tooltip-nps", WString::tr("tooltip.npoints"));

    // precision
    curvesPrecisionSpinBox_ = new WSpinBox(curvesContainer_);
    curvesPrecisionSpinBox_->setRange(CURVES_PREC_MIN, CURVES_PREC_MAX);
    curvesPrecisionSpinBox_->setValue(CURVES_PREC_DEFAULT);
    t->bindWidget("curve-prc", curvesPrecisionSpinBox_);
    t->bindString("curve-tooltip-prc", WString::tr("tooltip.prc"));

    // plot curve button
    curvesPlotBtn_ = new WPushButton("Plot curve", curvesContainer_);
    t->bindWidget("curve-btn-plot", curvesPlotBtn_);
    t->bindString("curve-tooltip-plot", WString::tr("tooltip.curve-plot"));

    // delete one button
    curvesDelOneBtn_ = new WPushButton("Delete last curve", curvesContainer_);
    curvesDelOneBtn_->setStyleClass("btn btn-warning");
    curvesDelOneBtn_->disable();
    t->bindWidget("curve-btn-del-one", curvesDelOneBtn_);
    t->bindString("curve-tooltip-del-one",
                  WString::tr("tooltip.curve-del-one"));

    // delete all button
    curvesDelAllBtn_ = new WPushButton("Delete all curves", curvesContainer_);
    curvesDelAllBtn_->setStyleClass("btn btn-danger");
    curvesDelAllBtn_->disable();
    t->bindWidget("curve-btn-del-all", curvesDelAllBtn_);
    t->bindString("curve-tooltip-del-all",
                  WString::tr("tooltip.curve-del-all"));

    // connect buttons to functions
    curvesPlotBtn_->clicked().connect(this, &HomeLeft::onPlotCurvesBtn);
    curvesDelOneBtn_->clicked().connect(this, &HomeLeft::onDelOneCurvesBtn);
    curvesDelAllBtn_->clicked().connect(this, &HomeLeft::onDelAllCurvesBtn);

    /*
     * Isoclines
     */
    isoclinesContainer_ = new WContainerWidget(this);
    isoclinesContainer_->setId("isoclinesContainer_");
    tabs_->addTab(isoclinesContainer_, "Isoclines");

    t = new WTemplate(WString::tr("template.homeleft-isoclines"),
                      isoclinesContainer_);
    t->addFunction("id", WTemplate::Functions::id);

    // isocline slope
    isoclinesLineEdit_ = new WLineEdit(isoclinesContainer_);
    t->bindWidget("isocline-slope", isoclinesLineEdit_);
    t->bindString("isocline-tooltip-slope",
                  WString::tr("tooltip.isocline-slope"));

    // appearance
    isoclinesAppearanceBtnGrp_ = new WButtonGroup(isoclinesContainer_);
    t->bindString("isocline-tooltip-appearance",
                  WString::tr("tooltip.appearance"));
    button = new WRadioButton("Dots", isoclinesContainer_);
    button->setInline(true);
    t->bindWidget("isocline-dots", button);
    t->bindString("isocline-tooltip-dots",
                  WString::tr("tooltip.appearance-dots"));
    isoclinesAppearanceBtnGrp_->addButton(button, Dots);
    button = new WRadioButton("Dashes", isoclinesContainer_);
    button->setInline(true);
    isoclinesAppearanceBtnGrp_->addButton(button, Dashes);
    isoclinesAppearanceBtnGrp_->setCheckedButton(
        isoclinesAppearanceBtnGrp_->button(Dashes));
    t->bindWidget("isocline-dashes", button);
    t->bindString("isocline-tooltip-dashes",
                  WString::tr("tooltip.appearance-dashes"));

    // n points
    isoclinesNPointsSpinBox_ = new WSpinBox(isoclinesContainer_);
    isoclinesNPointsSpinBox_->setRange(CURVES_NP_MIN, CURVES_NP_MAX);
    isoclinesNPointsSpinBox_->setValue(CURVES_NP_DEFAULT);
    t->bindWidget("nps", isoclinesNPointsSpinBox_);
    t->bindString("isocline-tooltip-nps", WString::tr("tooltip.npoints"));

    // precision
    isoclinesPrecisionSpinBox_ = new WSpinBox(isoclinesContainer_);
    isoclinesPrecisionSpinBox_->setRange(CURVES_PREC_MIN, CURVES_PREC_MAX);
    isoclinesPrecisionSpinBox_->setValue(CURVES_PREC_DEFAULT);
    t->bindWidget("isocline-prc", isoclinesPrecisionSpinBox_);
    t->bindString("isocline-tooltip-prc", WString::tr("tooltip.prc"));

    // plot isocline button
    isoclinesPlotBtn_ = new WPushButton("Plot isocline", isoclinesContainer_);
    t->bindWidget("isocline-btn-plot", isoclinesPlotBtn_);
    t->bindString("isocline-tooltip-plot",
                  WString::tr("tooltip.isocline-plot"));

    // delete one button
    isoclinesDelOneBtn_ =
        new WPushButton("Delete last isocline", isoclinesContainer_);
    isoclinesDelOneBtn_->setStyleClass("btn btn-warning");
    isoclinesDelOneBtn_->disable();
    t->bindWidget("isocline-btn-del-one", isoclinesDelOneBtn_);
    t->bindString("isocline-tooltip-del-one",
                  WString::tr("tooltip.isocline-del-one"));

    // delete all button
    isoclinesDelAllBtn_ =
        new WPushButton("Delete all isoclines", isoclinesContainer_);
    isoclinesDelAllBtn_->setStyleClass("btn btn-danger");
    isoclinesDelAllBtn_->disable();
    t->bindWidget("isocline-btn-del-all", isoclinesDelAllBtn_);
    t->bindString("isocline-tooltip-del-all",
                  WString::tr("tooltip.isocline-del-all"));

    // connect buttons to functions
    isoclinesPlotBtn_->clicked().connect(this, &HomeLeft::onPlotIsoclinesBtn);
    isoclinesDelOneBtn_->clicked().connect(this,
                                           &HomeLeft::onDelOneIsoclinesBtn);
    isoclinesDelAllBtn_->clicked().connect(this,
                                           &HomeLeft::onDelAllIsoclinesBtn);

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
    if (gcfContainer_ != nullptr) {
        tabs_->removeTab(gcfContainer_);
        delete gcfContainer_;
        gcfContainer_ = nullptr;
    }
    if (curvesContainer_ != nullptr) {
        tabs_->removeTab(curvesContainer_);
        delete curvesContainer_;
        curvesContainer_ = nullptr;
    }
    if (isoclinesContainer_ != nullptr) {
        tabs_->removeTab(isoclinesContainer_);
        delete isoclinesContainer_;
        isoclinesContainer_ = nullptr;
    }
}

void HomeLeft::resetUI()
{
    g_globalLogger.debug("[HomeLeft] Starting UI reset...");
    evaluated_ = false;
    plotted_ = false;

    evaluatedCurve_ = false;
    nCurves_ = 0;

    xEquationInput_->setText(std::string());
    yEquationInput_->setText(std::string());
    gcfEquationInput_->setText(std::string());

    if (loggedIn_) {
        g_globalLogger.debug("[HomeLeft] Hiding settins...");
        hideSettings();
        g_globalLogger.debug("[HomeLeft] Showing settins...");
        showSettings();
    }
    g_globalLogger.debug("[HomeLeft] Sending reset signal...");
    resetSignal_.emit(1);
}

void HomeLeft::onRefreshPlotBtn()
{
    if (!evaluated_) {
        errorSignal_.emit("Cannot read results, evaluate a vector field first.");
    } else {
        g_globalLogger.debug("[HomeLeft] sending refreshPlot signal");
        if (viewComboBox_->currentIndex() == 0) {
            double proj;
            try {
                proj = std::stod(viewProjection_->text());
            } catch (...) {
                proj = PROJECTION_DEFAULT;
                viewProjection_->setText(std::to_string(PROJECTION_DEFAULT));
                g_globalLogger.warning("[HomeLeft] bad view settings, "
                                       "setting to default value");
            }
            refreshPlotSphereSignal_.emit(proj);
        } else {
            double minx, maxx, miny, maxy;
            try {
                minx = std::stod(viewMinX_->text());
            } catch (...) {
                minx = -1;
                viewMinX_->setText("-1");
                g_globalLogger.warning("[HomeLeft] bad view settings, "
                                       "setting to default value");
            }
            try {
                maxx = std::stod(viewMaxX_->text());
            } catch (...) {
                maxx = 1;
                viewMaxX_->setText("1");
                g_globalLogger.warning("[HomeLeft] bad view settings, "
                                       "setting to default value");
            }
            try {
                miny = std::stod(viewMinY_->text());
            } catch (...) {
                miny = -1;
                viewMinY_->setText("-1");
                g_globalLogger.warning("[HomeLeft] bad view settings, "
                                       "setting to default value");
            }
            try {
                maxy = std::stod(viewMaxY_->text());
            } catch (...) {
                maxy = 1;
                viewMaxY_->setText("1");
                g_globalLogger.warning("[HomeLeft] bad view settings, "
                                       "setting to default value");
            }
            refreshPlotPlaneSignal_.emit(viewComboBox_->currentIndex(), minx,
                                         maxx, miny, maxy);
        }
        plotted_ = true;
        tabs_->setCurrentWidget(viewContainer_);
    }
}

void HomeLeft::showOrbitsDialog(bool clickValid, double x, double y)
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

void HomeLeft::onOrbitsDialogChange()
{
    orbitsContinueBtn_->disable();
    orbitsForwardsBtn_->enable();
    orbitsBackwardsBtn_->enable();
}

void HomeLeft::onOrbitsForwardsBtn()
{
    if (!evaluated_ || orbitsXLineEdit_->text().empty() ||
        orbitsYLineEdit_->text().empty())
        return;

    orbitsStartSelected_ = true;

    orbitsContinueBtn_->enable();
    orbitsDeleteOneBtn_->enable();
    orbitsDeleteAllBtn_->enable();
    orbitsForwardsBtn_->disable();

    g_globalLogger.debug("[HomeLeft] orbit start (" +
                         orbitsXLineEdit_->text().toUTF8() + "," +
                         orbitsYLineEdit_->text().toUTF8() + ")");
    orbitIntegrateSignal_.emit(1, std::stod(orbitsXLineEdit_->text()),
                               std::stod(orbitsYLineEdit_->text()));
}

void HomeLeft::onOrbitsBackwardsBtn()
{
    if (!evaluated_ || orbitsXLineEdit_->text().empty() ||
        orbitsYLineEdit_->text().empty())
        return;

    orbitsStartSelected_ = true;

    orbitsContinueBtn_->enable();
    orbitsDeleteOneBtn_->enable();
    orbitsDeleteAllBtn_->enable();
    orbitsBackwardsBtn_->disable();

    g_globalLogger.debug("[HomeLeft] orbit start (" +
                         orbitsXLineEdit_->text().toUTF8() + "," +
                         orbitsYLineEdit_->text().toUTF8() + ")");
    orbitIntegrateSignal_.emit(-1, std::stod(orbitsXLineEdit_->text()),
                               std::stod(orbitsYLineEdit_->text()));
}

void HomeLeft::onOrbitsContinueBtn()
{
    if (orbitsStartSelected_)
        orbitIntegrateSignal_.emit(0, 0.0, 0.0);
}

void HomeLeft::onOrbitsDeleteOneBtn()
{
    orbitsStartSelected_ = false;

    orbitsContinueBtn_->disable();
    orbitsForwardsBtn_->enable();
    orbitsBackwardsBtn_->enable();
    // orbitsDeleteAllBtn_->disable();
    // orbitsDeleteOneBtn_->disable();

    g_globalLogger.debug("[HomeLeft] deleting last orbit");
    orbitDeleteSignal_.emit(1);
}

void HomeLeft::onOrbitsDeleteAllBtn()
{
    orbitsStartSelected_ = false;

    orbitsContinueBtn_->disable();
    orbitsForwardsBtn_->enable();
    orbitsBackwardsBtn_->enable();

    g_globalLogger.debug("[HomeLeft] deleting all orbits");
    orbitDeleteSignal_.emit(0);
}

void HomeLeft::onPlotGcfBtn()
{
    if (!evaluated_) {
        g_globalLogger.warning(
            "[HomeLeft] user tried to plot GCF for an un-evaluated VF");
        errorSignal_.emit("Introduce and evaluate a vector field with a common "
                     "factor first.");
    } else if (scriptHandler_->str_gcf_ == "0") {
        g_globalLogger.warning(
            "[HomeLeft] user tried to plot a nonexistent GCF");
        errorSignal_.emit(
            "The current vector field has no specified common factor.");
    } else if (!plotted_) {
        g_globalLogger.warning("[HomeLeft] user tried to plot GCF without "
                               "plotting vector field first.");
        errorSignal_.emit("Click the main Plot button first\n"
                     "in order to create the plot window.");
        return;
    } else {
        int npoints = gcfNPointsSpinBox_->value();
        if (npoints < GCF_NP_MIN || npoints > GCF_NP_MAX) {
            npoints = GCF_NP_DEFAULT;
            g_globalLogger.warning("[HomeLeft] gcf npoints out of bounds, "
                                   "setting to default value");
        }
        int prec = gcfPrecisionSpinBox_->value();
        if (prec < GCF_PREC_MIN || prec > GCF_PREC_MAX) {
            prec = GCF_PREC_DEFAULT;
            g_globalLogger.warning("[HomeLeft] gcf precision out of bounds, "
                                   "setting to default value");
        }
        g_globalLogger.debug("[HomeLeft] sent signal for GCF evaluation");
        gcfSignal_.emit(fileUploadName_, gcfAppearanceBtnGrp_->checkedId(),
                        npoints, prec);
    }
}

void HomeLeft::addParameterToList(std::string label, std::string value)
{
    addParameterSignal_.emit(label, value);
}

void HomeLeft::onPlotCurvesBtn()
{
    evaluatedCurve_ = false;

    // check if vf is evaluated
    if (!evaluated_) {
        errorSignal_.emit("Cannot plot curve yet, evaluate a vector field first.");
        return;
    }
    if (!plotted_) {
        errorSignal_.emit("Click the main Plot button first\n"
                     "in order to create the plot window.");
        return;
    }
    // check if a curve has been introduced
    std::string curve = curvesLineEdit_->text().toUTF8();
    if (curve.empty() || curve == "") {
        errorSignal_.emit(
            "The curve field must be filled with the equation of a curve.");
        return;
    }
    // set the curve equation
    scriptHandler_->str_curve_ = curve;

    // prepare file where we transform curve equation into table
    std::string fname;
    if (fileUploadName_.empty()) {
        fileUploadName_ =
            scriptHandler_->randomFileName(TMP_DIR, "_curve_prep.mpl");
    }
    scriptHandler_->prepareCurveTable(fileUploadName_);
    // execute file
    siginfo_t status = scriptHandler_->evaluateMapleScript(
        fileUploadName_ + "_curve_prep", stoi(scriptHandler_->time_limit_));
    // check for errors in execution
    if (status.si_status == 0) {
        g_globalLogger.debug("[HomeLeft] Maple curve tables script executed");
        curvesPlotBtn_->setEnabled(true);
        evaluatedCurve_ = true;
    } else {
        if (status.si_code == CLD_EXITED) {
            g_globalLogger.error("[HomeLeft] Maple error");
            evaluatedSignal_.emit(fileUploadName_);
        } else if (status.si_code == CLD_KILLED) {
            g_globalLogger.error("[HomeLeft] Maple process killed by system");
            errorSignal_.emit("Maple process killed by system.");
        } else if (status.si_code == -2) {
            g_globalLogger.error(
                "[HomeLeft] Maple computation ran out of time");
            errorSignal_.emit("Computation ran out of time");
        } else {
            g_globalLogger.error("[HomeLeft] unkwnown error in Maple process");
            errorSignal_.emit("Unknown error when creating Maple process.");
        }
        return;
    }
    // check input curve parameters
    int npoints = curvesNPointsSpinBox_->value();
    if (npoints < CURVES_NP_MIN || npoints > CURVES_NP_MAX) {
        npoints = CURVES_NP_DEFAULT;
        g_globalLogger.warning("[HomeLeft] curve npoints out of bounds, "
                               "setting to default value");
    }
    int prec = curvesPrecisionSpinBox_->value();
    if (prec < CURVES_PREC_MIN || prec > CURVES_PREC_MAX) {
        prec = CURVES_PREC_DEFAULT;
        g_globalLogger.warning("[HomeLeft] curve precision out of bounds, "
                               "setting to default value");
    }

    // start curve evaluation and plotting
    plotCurveSignal_.emit(fileUploadName_, curvesAppearanceBtnGrp_->checkedId(),
                          npoints, prec);
    // after this, wait for confirmation (HomeRight will send a curveConfirmed
    // signal)
}

void HomeLeft::curveConfirmed(bool computed)
{
    if (computed) {
        nCurves_++;
        if (!curvesDelAllBtn_->isEnabled())
            curvesDelAllBtn_->enable();
        if (!curvesDelOneBtn_->isEnabled())
            curvesDelOneBtn_->enable();
        g_globalLogger.debug(
            "[HomeLeft] evaluated and plotted curve, ncurves = " +
            std::to_string(nCurves_));
    } else {
        errorSignal_.emit(
            "Error while computing curve, check inputs and try again.");
    }
}

void HomeLeft::onDelOneCurvesBtn()
{
    if (nCurves_ <= 0)
        return;

    curveDeleteSignal_.emit(1);

    if (--nCurves_ == 0) {
        curvesDelOneBtn_->disable();
        curvesDelAllBtn_->disable();
    }

    g_globalLogger.debug("[HomeLeft] deleted last curve, ncurves = " +
                         std::to_string(nCurves_));
}

void HomeLeft::onDelAllCurvesBtn()
{
    if (nCurves_ <= 0)
        return;

    curveDeleteSignal_.emit(0);

    nCurves_ = 0;
    curvesDelOneBtn_->disable();
    curvesDelAllBtn_->disable();

    g_globalLogger.debug("[HomeLeft] deleted all curves, ncurves = " +
                         std::to_string(nCurves_));
}

void HomeLeft::onPlotIsoclinesBtn()
{
    evaluatedIsocline_ = false;

    // check if vf is evaluated
    if (!evaluated_) {
        errorSignal_.emit(
            "Cannot plot isocline yet, evaluate a vector field first.");
        return;
    }
    if (!plotted_) {
        errorSignal_.emit("Click the main Plot button first\n"
                     "in order to create the plot window.");
        return;
    }
    // check if a isocline has been introduced
    std::string isocline = isoclinesLineEdit_->text().toUTF8();
    if (isocline.empty() || isocline == "") {
        errorSignal_.emit("The slope field must be filled with a valid value.");
        return;
    }
    // set the isocline equation
    if (isocline == "0") {
        scriptHandler_->str_isocline_ = scriptHandler_->str_yeq_;
    } else if (isocline == "INF" || isocline == "inf") {
        scriptHandler_->str_isocline_ = scriptHandler_->str_xeq_;
    } else {
        bool done = false;
        // look for isocline string in param labels
        std::vector<std::string>::const_iterator it;
        for (it = scriptHandler_->paramLabels_.begin();
             it != scriptHandler_->paramLabels_.end(); it++) {
            if (isocline == *it) {
                scriptHandler_->str_isocline_ = "(" + scriptHandler_->str_yeq_ +
                                                ")-(" + isocline + ")*(" +
                                                scriptHandler_->str_xeq_ + ")";
                done = true;
            }
        }
        // check if value is correct
        if (!done) {
            double val;
            try {
                val = std::stod(isocline);
                scriptHandler_->str_isocline_ = "(" + scriptHandler_->str_yeq_ +
                                                ")-(" + isocline + ")*(" +
                                                scriptHandler_->str_xeq_ + ")";
            } catch (const std::invalid_argument &e) {
                g_globalLogger.error("[HomeLeft] invalid isocline slope.");
                errorSignal_.emit("Invalid value for isocline slope");
                return;
            } catch (const std::out_of_range &e) {
                g_globalLogger.error(
                    "[HomeLeft] value for isocline slope out of double range");
                errorSignal_.emit("[HomeLeft] the introduced value for the slope is "
                             "out\nof double precision range.");
                return;
            }
        }
    }

    // prepare file where we transform isocline equation into table
    std::string fname;
    if (fileUploadName_.empty()) {
        fileUploadName_ =
            scriptHandler_->randomFileName(TMP_DIR, "_isocline_prep.mpl");
    }
    scriptHandler_->prepareIsoclineTable(fileUploadName_);
    // execute file
    siginfo_t status = scriptHandler_->evaluateMapleScript(
        fileUploadName_ + "_isocline_prep", stoi(scriptHandler_->time_limit_));
    // check for errors in execution
    if (status.si_status == 0) {
        g_globalLogger.debug(
            "[HomeLeft] Maple isocline tables script executed");
        isoclinesPlotBtn_->setEnabled(true);
        evaluatedIsocline_ = true;
    } else {
        if (status.si_code == CLD_EXITED) {
            g_globalLogger.error("[HomeLeft] Maple error");
            evaluatedSignal_.emit(fileUploadName_);
        } else if (status.si_code == CLD_KILLED) {
            g_globalLogger.error("[HomeLeft] Maple process killed by system");
            errorSignal_.emit("Maple process killed by system.");
        } else if (status.si_code == -2) {
            g_globalLogger.error(
                "[HomeLeft] Maple computation ran out of time");
            errorSignal_.emit("Computation ran out of time");
        } else {
            g_globalLogger.error("[HomeLeft] unkwnown error in Maple process");
            errorSignal_.emit("Unknown error when creating Maple process.");
        }
        return;
    }
    // check input isocline parameters
    int npoints = isoclinesNPointsSpinBox_->value();
    if (npoints < CURVES_NP_MIN || npoints > CURVES_NP_MAX) {
        npoints = CURVES_NP_DEFAULT;
        g_globalLogger.warning("[HomeLeft] isocline npoints out of bounds, "
                               "setting to default value");
    }
    int prec = isoclinesPrecisionSpinBox_->value();
    if (prec < CURVES_PREC_MIN || prec > CURVES_PREC_MAX) {
        prec = CURVES_PREC_DEFAULT;
        g_globalLogger.warning("[HomeLeft] isocline precision out of bounds, "
                               "setting to default value");
    }

    // start isocline evaluation and plotting
    plotIsoclineSignal_.emit(fileUploadName_,
                             isoclinesAppearanceBtnGrp_->checkedId(), npoints,
                             prec);
}

void HomeLeft::isoclineConfirmed(bool computed)
{
    if (computed) {
        nIsoclines_++;
        if (!isoclinesDelAllBtn_->isEnabled())
            isoclinesDelAllBtn_->enable();
        if (!isoclinesDelOneBtn_->isEnabled())
            isoclinesDelOneBtn_->enable();
        g_globalLogger.debug(
            "[HomeLeft] evaluated and plotted isocline, nisoclines = " +
            std::to_string(nIsoclines_));
    } else {
        errorSignal_.emit(
            "Error while computing isocline, check inputs and try again.");
    }
}

void HomeLeft::onDelOneIsoclinesBtn()
{
    if (nIsoclines_ <= 0)
        return;

    isoclineDeleteSignal_.emit(1);

    if (--nIsoclines_ == 0) {
        isoclinesDelOneBtn_->disable();
        isoclinesDelAllBtn_->disable();
    }

    g_globalLogger.debug("[HomeLeft] deleted last isocline, nisoclines = " +
                         std::to_string(nIsoclines_));
}

void HomeLeft::onDelAllIsoclinesBtn()
{
    if (nIsoclines_ <= 0)
        return;

    isoclineDeleteSignal_.emit(0);

    nIsoclines_ = 0;
    isoclinesDelOneBtn_->disable();
    isoclinesDelAllBtn_->disable();

    g_globalLogger.debug("[HomeLeft] deleted all isoclines, nisoclines = " +
                         std::to_string(nIsoclines_));
}
