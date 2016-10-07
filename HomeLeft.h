#ifndef __HOMELEFT_H_INCLUDE__
#define __HOMELEFT_H_INCLUDE__

#include <Wt/WContainerWidget>

class HomeLeft : public Wt::WContainerWidget
{
public:
    HomeLeft(Wt::WContainerWidget *parent = 0);
    ~HomeLeft();
    
private:
    Wt::WGroupBox *fileUploadBox_;
    Wt::WFileUpload *fileUploadWidget_;
    Wt::WPushButton *fileUploadButton_;
    std::string fileUploadName_;

    Wt::WGroupBox *equationsBox_;
    Wt::WText *xLabel_;
    Wt::WLineEdit *xEquationInput_;
    Wt::WText *yLabel_;
    Wt::WLineEdit *yEquationInput_;
    Wt::WPushButton *evalButton_;


    void fileUploaded();
    void fileTooLarge();

};

#endif // __HOMELEFT_H_INCLUDE__