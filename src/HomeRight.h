#include <Wt/WContainerWidget>

#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WTabWidget>
#include <Wt/WTextArea>
#include <Wt/WToolBar>

class HomeRight : public Wt::WContainerWidget
{
public:
    HomeRight(Wt::WContainerWidget *parent = 0);
    ~HomeRight();

    void readResults(int,std::string);

private:
    Wt::WTabWidget *tabWidget_;

    Wt::WContainerWidget *outputContainer_;
    Wt::WTextArea *outputTextArea_;
    Wt::WString outputTextAreaContent_;
    Wt::WToolBar *outputButtonsToolbar_;
    Wt::WPushButton *fullResButton_;
    Wt::WPushButton *finResButton_;
    Wt::WPushButton *infResButton_;
    Wt::WPushButton *clearButton_;

    std::string fileName_;
    std::string fullResults_;
    std::string finResults_;
    std::string infResults_;

    Wt::WContainerWidget *plotContainer_;

    int flag_;

    void fullResults();
    void showFinResults();
    void showInfResults();
    
};