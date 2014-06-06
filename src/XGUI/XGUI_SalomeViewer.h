#ifndef XGUI_SALOMEVIEWER_H
#define XGUI_SALOMEVIEWER_H

#include "XGUI.h"

#include <QObject>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

class QMouseEvent;
class QKeyEvent;

/**
* A Base object for definition of connector object to
* Salome Viewer. Reimplemented in NewGeom_SalomeViewer class
*/
class XGUI_EXPORT XGUI_SalomeViewer: public QObject
{
  Q_OBJECT
public:
  XGUI_SalomeViewer(QObject* theParent):QObject(theParent) {}

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

signals:
  void lastViewClosed();
  void tryCloseView();
  void deleteView();
  void viewCreated();
  void mousePress(QMouseEvent* theEvent);
  void mouseRelease(QMouseEvent* theEvent);
  void mouseDoubleClick(QMouseEvent* theEvent);
  void mouseMove(QMouseEvent* theEvent);
  void keyPress(QKeyEvent* theEvent);
  void keyRelease(QKeyEvent* theEvent);
  void activated();

  void selectionChanged();
};

#endif