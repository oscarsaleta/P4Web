#include <Wt/WPaintedWidget>

#include <Wt/WPaintDevice>

class PlotRegion : public Wt::WPaintedWidget
{
public:
    PlotRegion(Wt::WContainerWidget *parent = 0, int width = 200, int height = 200);
    ~PlotRegion();
    void setSize(int width, int height);
    
protected:
    void paintEvent(Wt::WPaintDevice *);

private:
    int width_;
    int height_;
};