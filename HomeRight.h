#include <Wt/WStackedWidget>

class HomeRight : public Wt::WStackedWidget
{
public:
    HomeRight(Wt::WContainerWidget *parent = 0);
    ~HomeRight();

private:
    Wt::WGroupBox *outputBox_;
    Wt::WTextArea *outputTextArea_;
    
};