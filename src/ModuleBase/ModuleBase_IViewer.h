// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_IViewer_H
#define ModuleBase_IViewer_H

#include "ModuleBase.h"
#include <QObject>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

class QMouseEvent;
class QKeyEvent;
class QContextMenuEvent;
class ModuleBase_IViewWindow;

/**
 * \ingroup GUI
 * A Base object for definition of connector object to
 * Salome Viewer. Reimplemented in NewGeom_SalomeViewer class
 */
class MODULEBASE_EXPORT ModuleBase_IViewer : public QObject
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  ModuleBase_IViewer(QObject* theParent)
      : QObject(theParent)
  {
  }

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const = 0;

  //! Retrurns V3d_Vioewer from current viewer
  virtual Handle(V3d_Viewer) v3dViewer() const = 0;

  //! Returns Vsd_View object from currently active view window
  virtual Handle(V3d_View) activeView() const = 0;

  //! Enable or disable selection in the viewer
  //! \param isEnabled is enable or disable flag
  virtual void enableSelection(bool isEnabled) = 0;

  //! Returns true if selection is enabled
  virtual bool isSelectionEnabled() const = 0;

  //! Enable or disable multiselection in the viewer
  //! \param isEnable is enable or disable flag
  virtual void enableMultiselection(bool isEnable) = 0;

  //! Returns true if multiselection is enabled
  virtual bool isMultiSelectionEnabled() const = 0;

  //! Perfroms the fit all for the active view
  virtual void fitAll() = 0;

  //! Sets the view projection
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  virtual void setViewProjection(double theX, double theY, double theZ) = 0;

  /// Add selection filter to the viewer
  /// \param theFilter a selection filter
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Remove selection filter from the viewer
  /// \param theFilter a selection filter
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Remove all selection filters from the viewer
  virtual void clearSelectionFilters() = 0;

  /// Update current viewer
  virtual void update() = 0;

  /// Method returns True if the viewer can process editing objects 
  /// by mouse drugging. If this is impossible thet it has to return False.
  virtual bool canDragByMouse() const { return true; }

signals:
  /// Signal emited when last view window is closed
  void lastViewClosed();

  /// Signal emited before view window is closed
  void tryCloseView(ModuleBase_IViewWindow* theWnd);

  /// Signal emited on delete view window
  void deleteView(ModuleBase_IViewWindow* theWnd);

  /// Signal emited on creation of view window
  void viewCreated(ModuleBase_IViewWindow* theWnd);

  /// Signal emited on key release
  void activated(ModuleBase_IViewWindow* theWnd);

  /// Signal emited on mouse press
  void mousePress(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Signal emited on mouse release
  void mouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Signal emited on mouse double click
  void mouseDoubleClick(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Signal emited on mouse move
  void mouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// Signal emited on key press
  void keyPress(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent);

  /// Signal emited on key release
  void keyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent);

  /// Signal emited on selection changed
  void selectionChanged();

  /// Signal emited on selection changed
  void contextMenuRequested(QContextMenuEvent*);
};

#endif
