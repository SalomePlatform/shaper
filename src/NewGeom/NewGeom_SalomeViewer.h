// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#ifndef NewGeom_SalomeViewer_H
#define NewGeom_SalomeViewer_H

#include "NewGeom.h"

#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>

#include <V3d_View.hxx>

class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;

class NewGeom_OCCSelector;
class OCCViewer_Viewer;
class SUIT_ViewManager;

/**
* \ingroup Salome
* A class for providing access of NewGeom functionality to 
* SALOME view window functionality
*/
class NewGeom_SalomeView: public ModuleBase_IViewWindow
{
public:
  /// Constructor
  /// \param theViewer a reference to a viewer
  NewGeom_SalomeView(OCCViewer_Viewer* theViewer) { myViewer = theViewer; }

  virtual Handle(V3d_View) v3dView() const;

  /// Set the current viewer
  /// \param theViewer a viewer instance
  void setViewer(OCCViewer_Viewer* theViewer) { myViewer = theViewer; }

  /// Returns current viewer
  OCCViewer_Viewer* viewer() const { return myViewer; }

private:
  /// A reference to a viewer
  OCCViewer_Viewer* myViewer;
};


/**
* \ingroup Salome
* A class for providing access of NewGeom functionality to 
* SALOME viewer functionality
*/
class NewGeom_SalomeViewer : public ModuleBase_IViewer
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  NewGeom_SalomeViewer(QObject* theParent);

  ~NewGeom_SalomeViewer();

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

  /// Set selector
  /// \param theSel a selector instance
  void setSelector(NewGeom_OCCSelector* theSel);

  /// Add selection filter to the viewer
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove selection filter from the viewer
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove all selection filters from the viewer
  virtual void clearSelectionFilters();

  /// Returns current selector
  NewGeom_OCCSelector* selector() const
  {
    return mySelector;
  }

  /// Update current viewer
  virtual void update();


 private slots:
  void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void onKeyPress(SUIT_ViewWindow*, QKeyEvent*);
  void onKeyRelease(SUIT_ViewWindow*, QKeyEvent*);

  void onTryCloseView(SUIT_ViewWindow*);
  void onDeleteView(SUIT_ViewWindow*);
  void onViewCreated(SUIT_ViewWindow*);
  void onActivated(SUIT_ViewWindow*);

  void onSelectionChanged();

 private:
  NewGeom_OCCSelector* mySelector;
  NewGeom_SalomeView* myView;
  bool myIsSelectionChanged;
};


#endif
