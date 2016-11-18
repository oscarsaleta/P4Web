#include <Wt/WPaintedWidget>

#include <Wt/WPainter>
#include <Wt/WPaintDevice>

class PlotRegion : public Wt::WPaintedWidget
{
public:
    PlotRegion(Wt::WContainerWidget *parent = 0, int width = 200, int height = 200);
    ~PlotRegion();

protected:
    void paintEvent(Wt::WPaintDevice *);

private:
    int width_;
    int height_;
};