/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
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

#ifndef WIN_SPHERE_H
#define WIN_SPHERE_H

/*!
 * @brief This file implements the class WWinSphere
 * @file win_sphere.h
 *
 * The WWinSphere class holds a WVFStudy object and also
 * contains all the information and methods needed for plotting
 * the contents of the study (the output from Maple and also
 * the separatrices computed numerically) in a plane or sphere
 * projection.
 */

#include "custom.h"
#include "file_tab.h"

#include <Wt/WContainerWidget>
#include <Wt/WPainter>
#include <Wt/WPaintDevice>
#include <Wt/WPointF>
//#include <Wt/WString>
#include <Wt/WPaintedWidget>


//#define SELECTINGPOINTSTEPS         5
//#define SELECTINGPOINTSPEED         150

/**
 * Sphere class, which performs the plotting work
 * @class WWinSphere
 *
 * This class derives from Wt::WPaintedWidget, so it is in fact
 * a widget in which we can paint.
 *
 * It also contains an object of the class WVFStudy, because
 * this is the object that parses Maple results and therefore
 * has all the information about what has to be plotted.
 *
 * In P4, the QVFStudy (WVFStudy here) object was originally a global
 * object called VFResults. This does not work for us, because we need
 * several studies and different plots to be executed at the same time
 * by different users of P4Web. This means that a single global object
 * causes problems (each new vector field study modifies that object
 * and renders other sessions unusable).
 *
 * This is why we made that every WWinSphere has its own WVFStudy,
 * this way there are no conflicts.
 */
class WWinSphere : public Wt::WPaintedWidget
{
    /*static int numSpheres;          /**< spheres are constructed as a linked list
                                        and when a new sphere is created, this counter
                                        increases */
    //static WWinSphere **SphereList; ///< linked list of spheres

public:
    /**
     * Constructor method for a spherical plot
     * @param *parent       container widget which created the sphere
     * @param width         width of the painting area
     * @param height        height of the painting area
     * @param basename      name of the file that contains Maple output for the current vector field
     * @param projection    projection for the sphere
     */
    WWinSphere( Wt::WContainerWidget *parent=0, int width=255, int height=255, std::string basename="", double projection=-1.0 );
    /**
     * Constructor method for a planar (or chart) plot
     * @param *parent   container widget which created the sphere
     * @param width     width of the painting area
     * @param height    height of the painting area
     * @param basename  name of the file that contains Maple output for the current vector field
     * @param type      type of view
     * @param minx      minimum x for plot
     * @param maxx      maximum x for plot
     * @param miny      minimum y for plot
     * @param maxy      maximum y for plot
     */
    WWinSphere( Wt::WContainerWidget *parent=0, int width=255, int height=255, std::string basename="", int type=1, double minx=-1, double maxx=1, double miny=-1, double maxy=1);
    /**
     * Destructor method
     */
    ~WWinSphere();

    int width_;                 ///< width of the plotting area
    int height_;                ///< height of the plotting area
    WVFStudy *study_;           ///< WVFStudy object which will parse results from Maple
    std::string basename_;      ///< filename where Maple output is stored
    int typeOfView_;            ///< type of view for *study_
    int projection_;
    double viewMinX_;
    double viewMaxX_;
    double viewMinY_;
    double viewMaxY_;
    
    /**
     * X Coordinate change: from world (double) to window (int) coordinates
     * @param  x coordinate to transform
     * @return   tranformed coordinate
     */
    int coWinX( double x );     // coordinate changes: from world to windows coordinates
    /**
     * Y Coordinate change: from world (double) to window (int) coordinates
     * @param  y coordinate to transform
     * @return   transformed coordinate
     */
    int coWinY( double y );
    /**
     * X Coordinate change: from window (int) to world (double) coordinates
     * @param  x coordinate to transform
     * @return   tranformed coordinate
     */
    double coWorldX( int x );   // from windows to world coordinates
    /**
     * Y Coordinate change: from (int) to world (double) coordinates
     * @param  y coordinate to transform
     * @return   transformed coordinate
     */
    double coWorldY( int y );
    /**
     * Horizontal distance tranformation: from world (double) to window (int)
     * @param  deltax distance to transform
     * @return        tranformed distance
     */
    int coWinH( double deltax );
    /**
     * Vertical distance transformation: from world (double) to window (int) 
     * @param  deltay distance to transform
     * @return        transformed distance
     */
    int coWinV( double deltay );

    double x0;                  ///< world-coordinates of upper-left corner
    double y0;                  ///< world-coordinates of upper-left corner
    double x1;                  ///< world-coordinates of upper-right corner
    double y1;                  ///< world-coordinates of upper-right corner
    double dx;                  ///< x1-x0
    double dy;                  ///< y1-y0

    int paintedXMin;            /**< to know the update rectangle after painting
                                we keep to smallest rectangle enclosing
                                all painted objects. */
    int paintedXMax;            /**< to know the update rectangle after painting
                                we keep to smallest rectangle enclosing
                                all painted objects. */            
    int paintedYMin;            /**< to know the update rectangle after painting
                                we keep to smallest rectangle enclosing
                                all painted objects. */            
    int paintedYMax;            /**< to know the update rectangle after painting
                                we keep to smallest rectangle enclosing
                                all painted objects. */

    Wt::WString chartString_;   ///< string that identifies which chart we're in
    Wt::WString plotCaption_;   ///< string that shows type of view and cursor coordinates

    int spherebgcolor;          ///< background color
    WWinSphere * next;          ///< next WWinSphere (linked list)
    //int SelectingX, SelectingY, SelectingPointStep, SelectingPointRadius;
    //QTimer * SelectingTimer;

    /**
     * Get chart coordinates for a point
     * @param  chart Which chart (defined in custom.h)
     * @param  x1    Coordinate 1 of point
     * @param  y1    Coordinate 2 of point
     * @param  pos   Array where coordinates are stored
     * @return       Always @c true
     */
    bool getChartPos( int chart, double x1, double y1, double * pos );

    /**
     * Plot a saddle
     * @param p saddle struct
     */
    void plotPoint( saddle * p );
    /**
     * Plot a node
     * @param p node struct
     */
    void plotPoint( node * p );
    /**
     * Plot a semi-elementary singularity
     * @param p semi-elementary singularity struct
     */
    void plotPoint( semi_elementary * p );
    /**
     * Plot a weak focus
     * @param p weak focus struct
     */
    void plotPoint( weak_focus * p );
    /**
     * Plot a strong focus
     * @param p strong focus struct
     */
    void plotPoint( strong_focus * p );
    /**
     * Plot a degenerate singularity
     * @param p degenerate singularity struct
     */
    void plotPoint( degenerate * p );
    /**
     * Plot separatrices around a semi-elementary singular point
     * @param p semi-elementary singularity struct
     */
    void plotPointSeparatrices( semi_elementary * p );
    /**
     * Plot separatrices around a saddle
     * @param p saddle struct
     */
    void plotPointSeparatrices( saddle * p );
    /**
     * Plot separatrices around a degenerate singular point
     * @param p degenerate singularity struct
     */
    void plotPointSeparatrices( degenerate * p );
    /**
     * Plot all singularities
     */
    void plotPoints( void );
    /**
     * Plot all separatrices
     */
    void plotSeparatrices( void );
    
    //void plotGcf( void );
    /**
     * Plot Poincaré sphere (circle at infinity)
     */
    void plotPoincareSphere( void );
    /**
     * Plot Poincaré-Lyapunov sphere (circle at infinity)
     */
    void plotPoincareLyapunovSphere( void );
    /**
     * Plot line at infinity
     */
    void plotLineAtInfinity( void );

    //void MarkSelection( int x1, int y1, int x2, int y2, int selectiontype );

    /**
     * Produce a linked list of lines with the shape of an ellipse
     * @param  cx     x coordinate of center
     * @param  cy     y coordinate of center
     * @param  a      major semi axis
     * @param  b      minor semi axis
     * @param  dotted is the ellipse dotted or a line
     * @param  resa   @p a converted to window length
     * @param  resb   @p b converted to window length
     * @return        linked list of lines that form the ellipse
     */
    P4POLYLINES *produceEllipse( double cx, double cy, double a, double b, bool dotted, double resa, double resb );

    /**
     * Draw a point with a given color
     * @param x     point is (x,y)
     * @param y     point is (x,y)
     * @param color color (defined in color.h)
     */
    void drawPoint( double x, double y, int color );
    /**
     * Draw a line given by two points with a given color
     * @param x1    point 1 is (x1,y1)
     * @param y1    point 1 is (x1,y1)
     * @param x2    point 2 is (x2,y2)
     * @param y2    point 2 is (x2,y2)
     * @param color color (defined in color.h)
     */
    void drawLine( double x1, double y1, double x2, double y2, int color );


    /**
     * Setup everything before starting to plot
     *
     * This function triggers a parse of the Maple results file,
     * which fills the WVFStudy object member of this class. It
     * also sets up the WVFStudy pointer-to-member functions to
     * point at the correct functions for the selected configuration
     * of the study (correct view, sphere, etc). Then, it deletes
     * any remaining circles or lines at infinity and creates a
     * new one depending on the configuration.
     *
     * @return @c true if setup was successful, @c false if results could
     * not be read
     */
    bool setupPlot( void );
    //void updatePointSelection( void );


    void integrateOrbit( int dir );

    orbits_points * integrate_orbit( double pcoord[3],double step,int dir,int color, int points_to_int,struct orbits_points **orbit );

    void drawOrbit( double * pcoord, struct orbits_points * points, int color );

    bool startOrbit( double x, double y, bool R );

    void drawOrbits();

    void deleteLastOrbit();






    /**
     * React to a mouse hover event to set a string
     *
     * This function takes the coordinates of the mouse cursor,
     * transforms them to the current view (poincaré sphere, p-l,
     * plane, or one of the charts) and sets plotCaption_ to reflect
     * these coordinates.
     * 
     * @param e WMouseEvent, contains the coordinates of the mouse cursor
     */
    void mouseMovementEvent( Wt::WMouseEvent e );

    /**
     * React to a mouse click event to emit the coordinates
     *
     * This function takes the coordinates of the mouse cursor
     * where the click has been performed, and sends them to the
     * parent widget (HomeRight)
     * 
     * @param e WMouseEvent, contains the coordinates of the mouse cursor
     */
    void mouseClickEvent( Wt::WMouseEvent e );

    Wt::Signal<Wt::WString>& hoverSignal() { return hoverSignal_; }
    Wt::Signal<bool,double,double>& clickedSignal() { return clickedSignal_; }

    /**
     * Method that sends a signal to print some message in the output
     * text area from #HomeRight
     */
    Wt::Signal<std::string>& errorSignal() { return errorSignal_; }

    Wt::WPainter *staticPainter;       /**< pointer to a painter linked to a paint device
                                            created in a paint event. This makes possible
                                            to distribute painting to different functions
                                            and compiling units (even from outside the
                                            object) */

    bool plotDone_;
    

protected:
    /**
     * Paint event for this painted widget
     * @param p paint device (passed automatically by the generator of the event)
     *
     * When an update is called upon the painted widget (this), a paint event is
     * generated and this function implements the behavior.
     */
    void paintEvent( Wt::WPaintDevice * p );

private:

    Wt::Signal<Wt::WString> hoverSignal_;
    Wt::Signal<bool,double,double> clickedSignal_;
    /**
     * Signal emitted when there's an error while reading results from Maple
     */
    Wt::Signal<std::string> errorSignal_;

    
    Wt::WContainerWidget * parentWnd;   /**< parent widget (stored from @c parent, argument
                                            passed to constructor) */
    bool ReverseYaxis;                  /**< when calculating coordinates: this determines
                                            orientation of horizontal axis.  Normally false,
                                            only true when printing. */
    
    P4POLYLINES * CircleAtInfinity; ///< linked list of lines that form the Poincaré circle
    P4POLYLINES * PLCircle;         /**< linked list of lines that form the Poincaré-
                                        Lyapunov circle */

    void setChartString(int p, int q, bool isu1v1chart, bool negchart);

    bool plotPrepared_;
    bool firstTimePlot_;


};


#endif /* WIN_SPHERE_H */
