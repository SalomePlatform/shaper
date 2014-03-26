
#ifndef XGUI_ViewPort_H
#define XGUI_ViewPort_H

#include <QWidget>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

class XGUI_ViewWindow;

class XGUI_ViewPort : public QWidget
{
    Q_OBJECT
public:
    XGUI_ViewPort(XGUI_ViewWindow* theParent, 
                  const Handle(V3d_Viewer)& theViewer, 
                  V3d_TypeOfView theType = V3d_ORTHOGRAPHIC);
    ~XGUI_ViewPort();

    virtual QPaintEngine* paintEngine() const { return 0; }

    QImage dumpView();

    Handle(V3d_View) getView() const { return activeView(); }

signals:
  //void                  vpChangeBackground( const Qtx::BackgroundData& );
    void                  vpClosed();
    void                  vpMapped();


protected:
    virtual void          paintEvent( QPaintEvent* );
    virtual void          resizeEvent( QResizeEvent* );


private:
    Handle(V3d_View)      activeView() const { return myActiveView; }

    bool mapView( const Handle(V3d_View)& theView);
    bool setWindow( const Handle(V3d_View)& theView);
    bool mapped( const Handle(V3d_View)& theView) const;
    void updateBackground();
    void attachWindow( const Handle(V3d_View)& theView, const Handle(Aspect_Window)& theWnd);


    Handle(V3d_View) myOrthoView;
    Handle(V3d_View) myPerspView;
    Handle(V3d_View) myActiveView;

    Handle(Aspect_Window) myWindow;
  
    bool myPaintersRedrawing;
};


#endif