// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModuleBase_IViewer_H
#define ModuleBase_IViewer_H

#include "ModuleBase.h"
#include <QObject>
#include <QMap>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_Trihedron.hxx>

class QMouseEvent;
class QKeyEvent;
class QContextMenuEvent;
class ModuleBase_IViewWindow;

/**
 * \ingroup GUI
 * A Base object for definition of connector object to
 * Salome Viewer. Reimplemented in SHAPERGUI_SalomeViewer class
 */
class MODULEBASE_EXPORT ModuleBase_IViewer : public QObject
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  ModuleBase_IViewer(QObject* theParent);

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const = 0;

  //! Trihedron 3d object shown in the viewer
  virtual Handle(AIS_Trihedron) trihedron() const = 0;

  //! Retrurns V3d_Vioewer from current viewer
  virtual Handle(V3d_Viewer) v3dViewer() const = 0;

  //! Returns Vsd_View object from currently active view window
  virtual Handle(V3d_View) activeView() const = 0;

  //! Returns currently active view port widget
  virtual QWidget* activeViewPort() const = 0;

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

  //! Enable or disable draw mode in the viewer
  virtual bool enableDrawMode(bool isEnabled) = 0;

  //! Perfroms the fit all for the active view
  virtual void fitAll() = 0;

  //! Erases all presentations from the viewer
  virtual void eraseAll() = 0;

  //! Sets the view projection
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  /// \param theTwist the twist angle in radians
  virtual void setViewProjection( double theX, double theY, double theZ,
                                  double theTwist ) = 0;

  /// Add selection filter to the viewer
  /// \param theFilter a selection filter
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Remove selection filter from the viewer
  /// \param theFilter a selection filter
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Returns true if the selection filter is set to the viewer
  /// \param theFilter a selection filter
  virtual bool hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter) = 0;

  /// Update current viewer
  virtual void update() = 0;

  /// Returns a scale factor of the given view
  /// \param theView a view object
  const double Scale(const Handle(V3d_View)& theView)
  {
    if (!myWindowScale.contains(theView))
      myWindowScale.insert(theView, theView->Camera()->Scale());
    return myWindowScale[theView];
  }

  /// Remember a scale factor for the view object
  /// \param theView a view object
  /// \param theVal a scale factor
  void SetScale(const Handle(V3d_View)& theView, const double theVal)
  { myWindowScale[theView] = theVal; }

  /// Method returns True if the viewer can process editing objects
  /// by mouse drugging. If this is impossible thet it has to return False.
  virtual bool canDragByMouse() const { return true; }

  /// Fit all along Z (perpendicular to display)
  //virtual void Zfitall() = 0;

  /// Show highlight for pre-highlighted sub-shape
  virtual void updateHighlight() {}

  static Handle(Prs3d_Drawer) DefaultHighlightDrawer;

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

  /// Signal emitted on transformation of view point in view window
  /// \param theTransformation type of transformation (see AppElements_ViewWindow::OperationType)
  void viewTransformed(int theTransformation);

  /// Signal emited on selection changed
  void trihedronVisibilityChanged(bool theState);

  protected:
    /// A map for storing a scale factors dependent on view object
    QMap<Handle(V3d_View), double> myWindowScale;

};

#endif
