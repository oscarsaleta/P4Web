#ifndef HOMELEFT_H
#define HOMELEFT_H

/*!
 * @brief Left side of UI
 * @file HomeLeft.h
 * @author Oscar Saleta
 */
#include <Wt/WContainerWidget>


#include <Wt/WSignal>

/** 
 * This class holds the UI from the left side of the website
 * 
 * @class HomeLeft
 * 
 * This class derives from a WContainerWidget, and it serves as a container
 * for several elements, including an upload widget, two input forms for
 * entering the vector field (x' and y'), and three buttons (evaluate,
 * plot and save).
 *
 * When a button is pressed, a signal is sent and #HomeRight receives it
 * and acts accordingly. The connection between HomeLeft and HomeRight is
 * made in #MainUI.
 */
class HomeLeft : public Wt::WContainerWidget
{
public:
    /** 
     * Constructor method for HomeLeft
     * @param parent Parent container widget
     */
    HomeLeft(Wt::WContainerWidget *parent = 0);
    /** 
     * Destructor method for HomeLeft
     */
    ~HomeLeft();

    /**
     * Show additional settings for a logged in user
     */
    void showSettings();
    /**
     * Hide additional settings (when a user logs out)
     */
    void hideSettings();

    /**
     * Method that sends a signal when a vector field is evaluated by Maple
     */
    Wt::Signal<std::string>& evaluatedSignal();
    /**
     * Method that sends a signal to print some message in the output text area from #HomeRight
     */
    Wt::Signal<std::string>& errorSignal();
    /**
     * Method that sends a signal when the plot button is pressed in order to display a plot
     */
    Wt::Signal<std::string>& onPlotSignal(); 

private:
    /* PUBLIC UI (no need to log in) */
    Wt::WGroupBox       *fileUploadBox_;
    Wt::WFileUpload     *fileUploadWidget_;
    std::string         fileUploadName_;

    Wt::WGroupBox       *equationsBox_;
    Wt::WLineEdit       *xEquationInput_;
    Wt::WLineEdit       *yEquationInput_;
    Wt::WPushButton     *evalButton_;
    Wt::WPushButton     *plotButton_;
    Wt::WPushButton     *clearButton_;

    Wt::WAnchor         *saveButton_;
    std::string         saveFileName_;
    Wt::WFileResource   *saveFileResource_;

    /* PRIVATE UI (log in needed) */
    Wt::WGroupBox       *settingsBox_;
    
    Wt::WButtonGroup    *calculationsBtnGroup_;
    enum Calculations   { Algebraic = 0, Numeric = 1 };

    Wt::WButtonGroup    *separatricesBtnGroup_;
    enum Separatrices   { Yes = 0, No = 1 };

    Wt::WSpinBox        *accuracySpinBox_;
    Wt::WSpinBox        *precisionSpinBox_;
    Wt::WDoubleSpinBox  *epsilonSpinBox_;
    Wt::WSpinBox        *levAppSpinBox_;
    Wt::WSpinBox        *numericLevelSpinBox_;
    Wt::WSpinBox        *maxLevelSpinBox_;
    Wt::WSpinBox        *maxWeakLevelSpinBox_;
    Wt::WSpinBox        *PLWeightPSpinBox_;
    Wt::WSpinBox        *PLWeightQSpinBox_;

    /* SIGNALS */
    Wt::Signal<std::string> evaluatedSignal_;
    Wt::Signal<std::string> errorSignal_;
    Wt::Signal<std::string> onPlotSignal_;

    /* FUNCTIONS */
    // sets up public UI
    void setupUI();
    // sets up connectors for buttons, forms, etc
    void setupConnectors();
    // what to do when file is uploaded
    void fileUploaded();
    // what to do when uploaded file is too large
    void fileTooLarge();
    // read uploaded file
    void parseInputFile();
    // open writable file with random name in tmp folder
    std::string openTempStream(std::string, std::string, std::ofstream&);
    // prepare to write the maple script
    void prepareMapleFile();
    // write the options inside the maple script
    void fillMapleScript(std::string, std::ofstream&);
    // run maple on the script
    void evaluate();
    // write a tmp save file in server for download
    void prepareSaveFile();
    // what to do when plot button is pressed
    void onPlot();

    /* MAPLE FILE PARAMETERS */
    bool loggedIn_;
    Wt::WString str_bindir;
    Wt::WString str_p4m;
    Wt::WString str_tmpdir;
    Wt::WString str_lypexe;
    Wt::WString str_sepexe;
    Wt::WString str_exeprefix;
    Wt::WString str_platform;
    Wt::WString str_sumtablepath;
    Wt::WString str_removecmd;
    Wt::WString str_simplify;
    Wt::WString str_simplifycmd;
    Wt::WString str_critpoints;
    Wt::WString str_saveall;
    Wt::WString str_vectable;
    Wt::WString str_fintab;
    Wt::WString str_finres;
    Wt::WString str_inftab;
    Wt::WString str_infres;
    Wt::WString str_userf;
    Wt::WString str_gcf;
    Wt::WString str_numeric;
    Wt::WString str_epsilon;
    Wt::WString str_testsep;
    Wt::WString str_precision;
    Wt::WString str_precision0;
    Wt::WString str_taylor;
    Wt::WString str_numericlevel;
    Wt::WString str_maxlevel;
    Wt::WString str_weaklevel;
    Wt::WString str_userp;
    Wt::WString str_userq;

};

#endif // HOMELEFT_H