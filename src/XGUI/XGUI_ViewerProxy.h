#ifndef XGUI_VIEWERPROXY_H
#define XGUI_VIEWERPROXY_H

#include "XGUI.h"
#include "XGUI_SalomeViewer.h"

class XGUI_Workshop;
class XGUI_ViewWindow;

/**
* Proxy class which repersents or XGUI_Viewer or Salome Viewer
* dependently on current launching environment.
* It is reccomennded to use this class in operation for accessing to viewer 
* functionality instead of direct access to a viewer
*/
class XGUI_EXPORT XGUI_ViewerProxy: public XGUI_SalomeViewer
{
  Q_OBJECT
public:
  XGUI_ViewerProxy(XGUI_Workshop* theParent);

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const;

  //! Retrurns V3d_Vioewer from current viewer
  virtual Handle(V3d_Viewer) v3dViewer() const;

  //! Returns Vsd_View object from currently active view window
  virtual Handle(V3d_View) activeView() const;

  //! Enable or disable selection in the viewer
  virtual void enableSelection(bool isEnabled);

  //! Returns true if selection is enabled
  virtual bool isSelectionEnabled() const;

  //! Enable or disable multiselection in the viewer
  virtual void enableMultiselection(bool isEnable);

  //! Returns true if multiselection is enabled
  virtual bool isMultiSelectionEnabled() const;

  //! Sets the view projection
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  void setViewProjection(double theX, double theY, double theZ);

  //! Sets the view fitted all
  void fitAll();

  /// Connects to a viewer according to current environment
  void connectToViewer();

private slots:
  void onMousePress(XGUI_ViewWindow*, QMouseEvent*);
  void onMouseRelease(XGUI_ViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(XGUI_ViewWindow*, QMouseEvent*);
  void onMouseMove(XGUI_ViewWindow*, QMouseEvent*);
  void onKeyPress(XGUI_ViewWindow*, QKeyEvent*);
  void onKeyRelease(XGUI_ViewWindow*, QKeyEvent*);

private:
  XGUI_Workshop* myWorkshop;
};

#endif
