
#ifndef NewGeom_SalomeViewer_H
#define NewGeom_SalomeViewer_H

#include "NewGeom.h"

#include <XGUI_SalomeViewer.h>

class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;

class NewGeom_OCCSelector;

class NewGeom_SalomeViewer : public XGUI_SalomeViewer
{
Q_OBJECT
 public:
  NewGeom_SalomeViewer(QObject* theParent);

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

  //! Perfroms the fit all for the active view
  virtual void fitAll();

  void setSelector(NewGeom_OCCSelector* theSel);

  NewGeom_OCCSelector* selector() const
  {
    return mySelector;
  }

 private slots:
  void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void onKeyPress(SUIT_ViewWindow*, QKeyEvent*);
  void onKeyRelease(SUIT_ViewWindow*, QKeyEvent*);

 private:
  NewGeom_OCCSelector* mySelector;

};

#endif
