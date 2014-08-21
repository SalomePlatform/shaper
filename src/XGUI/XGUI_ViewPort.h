#ifndef XGUI_ViewPort_H
#define XGUI_ViewPort_H

#include "XGUI.h"

#include <Qtx.h>

#include <QWidget>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <gp_Pnt.hxx>

class XGUI_ViewWindow;

class XGUI_EXPORT XGUI_ViewPort : public QWidget
{
Q_OBJECT
 public:
  XGUI_ViewPort(XGUI_ViewWindow* theParent, const Handle(V3d_Viewer)& theViewer,
                V3d_TypeOfView theType = V3d_ORTHOGRAPHIC);
  virtual ~XGUI_ViewPort();

  virtual QPaintEngine* paintEngine() const
  {
    return 0;
  }

  QImage dumpView(QRect theRect = QRect(), bool toUpdate = true);

  Handle(V3d_View) getView() const
  {
    return activeView();
  }

  void startRotation(int x, int y, int theRotationPointType, const gp_Pnt& theSelectedPoint);
  void rotate(int, int, int, const gp_Pnt&);
  void endRotation();

  // TRANSFORMATIONS
  void pan(int dx, int dy);
  void setCenter(int x, int y);
  void fitRect(const QRect& rect);
  void startZoomAtPoint(int x, int y);
  void zoom(int x0, int y0, int x, int y);
  void fitAll(bool theKeepScale = false, bool theWithZ = true, bool theUpd = true);

  void setAdvancedZoomingEnabled(const bool theState)
  {
    myIsAdvancedZoomingEnabled = theState;
  }
  bool isAdvancedZoomingEnabled() const
  {
    return myIsAdvancedZoomingEnabled;
  }

  Qtx::BackgroundData background() const
  {
    return myBackground;
  }

  void setBackground(const Qtx::BackgroundData& bgData);

  void syncronizeWith(const XGUI_ViewPort* ref);

signals:
  void vpChangeBackground(const Qtx::BackgroundData&);
  void vpClosed();
  void vpMapped();
  void vpTransformed();
  void vpUpdated();
  void resized();

 protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);

 private:
  Handle(V3d_View) activeView() const
  {
    return myActiveView;
  }

  bool mapView(const Handle(V3d_View)& theView);
  bool setWindow(const Handle(V3d_View)& theView);
  bool mapped(const Handle(V3d_View)& theView) const;
  void updateBackground();
  void attachWindow(const Handle(V3d_View)& theView, const Handle(Aspect_Window)& theWnd);

  Handle(V3d_View) myOrthoView;Handle(V3d_View) myPerspView;Handle(V3d_View) myActiveView;

  Handle(Aspect_Window) myWindow;

  bool myPaintersRedrawing;
  bool myIsAdvancedZoomingEnabled;

  double myScale;

  Qtx::BackgroundData myBackground;
  int myBgImgHeight;
  int myBgImgWidth;
};

#endif
