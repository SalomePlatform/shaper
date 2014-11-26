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
 * A Base object for definition of connector object to
 * Salome Viewer. Reimplemented in NewGeom_SalomeViewer class
 */
class MODULEBASE_EXPORT ModuleBase_IViewer : public QObject
{
Q_OBJECT
 public:
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
  virtual void enableSelection(bool isEnabled) = 0;

  //! Returns true if selection is enabled
  virtual bool isSelectionEnabled() const = 0;

  //! Enable or disable multiselection in the viewer
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
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Remove selection filter from the viewer
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Remove all selection filters from the viewer
  virtual void clearSelectionFilters() = 0;

signals:
  void lastViewClosed();
  void tryCloseView(ModuleBase_IViewWindow* theWnd);
  void deleteView(ModuleBase_IViewWindow* theWnd);
  void viewCreated(ModuleBase_IViewWindow* theWnd);
  void activated(ModuleBase_IViewWindow* theWnd);

  void mousePress(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);
  void mouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);
  void mouseDoubleClick(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);
  void mouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  void keyPress(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent);
  void keyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent);

  void selectionChanged();
  void contextMenuRequested(QContextMenuEvent*);
};

#endif
