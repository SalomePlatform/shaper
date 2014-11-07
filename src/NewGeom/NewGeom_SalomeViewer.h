
#ifndef NewGeom_SalomeViewer_H
#define NewGeom_SalomeViewer_H

#include "NewGeom.h"

#include <ModuleBase_IViewer.h>

class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;

class NewGeom_OCCSelector;

class NewGeom_SalomeViewer : public ModuleBase_IViewer
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

  //! Sets the view projection
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  virtual void setViewProjection(double theX, double theY, double theZ);

  void setSelector(NewGeom_OCCSelector* theSel);

  /// Add selection filter to the viewer
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove selection filter from the viewer
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove all selection filters from the viewer
  virtual void clearSelectionFilters();

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
