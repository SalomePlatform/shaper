// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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
#ifdef HAVE_SALOME
class OCCViewer_Fitter;
#else
class AppElements_Fitter;
#endif

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

  /// Set flag which indicates that viewer is used for 2d operations
  /// \param is2d a new 2d mode state
  void set2dMode(bool is2d) {
    myIs2dMode = is2d;
  }

  /// Returns current state of 2d mode flag
  bool is2dMode() const {
    return myIs2dMode;
  }

  // Methods for color scale management

  //! Returns True if ColorScale is visible
  virtual bool isColorScaleVisible() const = 0;

  //! Show/Hide ColorScale object
  virtual void setColorScaleShown(bool on) = 0;

  //! Set position of color scale
  // \param theX is X position relative to current view width
  // \param theY is Y position relative to current view heigth
  virtual void setColorScalePosition(double theX, double theY) = 0;

  //! Set size of color scale
  // \param theW is width relative to current view width
  // \param theh is height relative to current view heigth
  virtual void setColorScaleSize(double theW, double theH) = 0;

  //! Set range of color scale
  // \param theMin is a minimal value
  // \param theMax is a maximal value
  virtual void setColorScaleRange(double theMin, double theMax) = 0;

  //! Set number of intervals of color scale
  // \param theNb is number of intervals
  virtual void setColorScaleIntervals(int theNb) = 0;

  //! Set text heigth of color scale
  // \param theH is number of intervals
  virtual void setColorScaleTextHeigth(int theH) = 0;

  //! Set color of text of color scale
  // \param theH is number of intervals
  virtual void setColorScaleTextColor(const QColor& theColor) = 0;

  //! Set title of color scale
  // \param theText is a title
  virtual void setColorScaleTitle(const QString& theText) = 0;

  //! Type for mapping from text to color to display the names lines in different colors
  typedef std::list<std::pair<std::wstring, std::vector<int> > > TextColor;

  //! Sets the text displayed in right-top corner of the 3D view
  //! \param theText the text to display, or empty string to erase presentation;
  //!        the first item is the font name and text color
  //! \param theSize size of the text font
  virtual void setText(const TextColor& theText, const int theSize) = 0;


  /// Switches On additional highlight for objects preselection
  void showSelectionHighlight() {
    myShowHighlight = true;
  }

  /// Switches Off additional highlight for objects preselection
  void hideSelectionHighlight() {
    myShowHighlight = false;
  }

#ifdef HAVE_SALOME
  virtual void setFitter(OCCViewer_Fitter* theFitter) = 0;
  virtual OCCViewer_Fitter* fitter() const = 0;
#else
  virtual void setFitter(AppElements_Fitter* theFitter) = 0;
  virtual AppElements_Fitter* fitter() const = 0;
#endif

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

    bool myIs2dMode;

    bool myShowHighlight;
};

#endif
