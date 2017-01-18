#ifndef HOMELEFT_H
#define HOMELEFT_H

/*!
 * @brief Left side of UI
 * @file HomeLeft.h
 * @author Oscar Saleta
 */

#include "MyAuthWidget.h"
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
    HomeLeft(Wt::WContainerWidget *parent = 0, MyAuthWidget *authWidget = 0);
    /** 
     * Destructor method for HomeLeft
     */
    ~HomeLeft();

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
    MyAuthWidget *authWidget_;

    Wt::WGroupBox *fileUploadBox_;
    Wt::WFileUpload *fileUploadWidget_;
    std::string fileUploadName_;

    Wt::WGroupBox *equationsBox_;
    Wt::WText *xLabel_;
    Wt::WLineEdit *xEquationInput_;
    Wt::WText *yLabel_;
    Wt::WLineEdit *yEquationInput_;
    Wt::WPushButton *evalButton_;
    Wt::WPushButton *plotButton_;

    Wt::WAnchor *saveButton_;
    std::string saveFileName_;
    Wt::WFileResource *saveFileResource_;

    void setupUI();
    void setupConnectors();

    void fileUploaded();
    void fileTooLarge();

    std::string openTempStream(std::string, std::string, std::ofstream&);
    void prepareMapleFile();
    void fillMapleScript(std::string, std::ofstream&);
    void evaluate();

    void saveFile();
    void prepareSaveFile();

    void onPlot();

    Wt::Signal<std::string> evaluatedSignal_;
    Wt::Signal<std::string> errorSignal_;
    Wt::Signal<std::string> onPlotSignal_;


};

#endif // HOMELEFT_H