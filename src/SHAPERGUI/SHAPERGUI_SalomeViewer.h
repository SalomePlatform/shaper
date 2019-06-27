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

#ifndef SHAPERGUI_SalomeViewer_H
#define SHAPERGUI_SalomeViewer_H

#include "SHAPER_SHAPERGUI.h"

#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>

#include <OCCViewer_ViewWindow.h>

#include <V3d_View.hxx>
#include <AIS_Trihedron.hxx>

class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;

class SHAPERGUI_OCCSelector;
class OCCViewer_Viewer;
class SUIT_ViewManager;

/**
* \ingroup Salome
* A class for providing access of SHAPERGUI functionality to 
* SALOME view window functionality
*/
class SHAPERGUI_SalomeView: public ModuleBase_IViewWindow
{
public:
  /// Constructor
  /// \param theViewer a reference to a viewer
  SHAPERGUI_SalomeView(OCCViewer_Viewer* theViewer);

  virtual Handle(V3d_View) v3dView() const;

  /// Returns the view window view port
  virtual QWidget* viewPort() const;

  /// Set the current viewer
  /// \param theViewer a viewer instance
  void setViewer(OCCViewer_Viewer* theViewer) { myViewer = theViewer; }

  /// Returns current viewer
  OCCViewer_Viewer* viewer() const { return myViewer; }

  /// Sets the current view
  void setCurrentView(SUIT_ViewWindow* theView) { myCurrentView = theView; }

private:
  /// A reference to a viewer
  OCCViewer_Viewer* myViewer;
  SUIT_ViewWindow* myCurrentView;
};


/**
* \ingroup Salome
* A class for providing access of SHAPERGUI functionality to 
* SALOME viewer functionality
*/
class SHAPERGUI_SalomeViewer : public ModuleBase_IViewer
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  SHAPERGUI_SalomeViewer(QObject* theParent);

  ~SHAPERGUI_SalomeViewer();

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const;

  //! Retrurns V3d_Vioewer from current viewer
  virtual Handle(V3d_Viewer) v3dViewer() const;

  //! Trihedron 3d object shown in the viewer
  virtual Handle(AIS_Trihedron) trihedron() const;

  //! Returns Vsd_View object from currently active view window
  virtual Handle(V3d_View) activeView() const;

  //! Returns viewer view port
  virtual QWidget* activeViewPort() const;

  //! Enable or disable selection in the viewer
  virtual void enableSelection(bool isEnabled);

  //! Returns true if selection is enabled
  virtual bool isSelectionEnabled() const;

  //! Enable or disable multiselection in the viewer
  virtual void enableMultiselection(bool isEnable);

  //! Returns true if multiselection is enabled
  virtual bool isMultiSelectionEnabled() const;

  //! Enable or disable draw mode in the viewer
  virtual bool enableDrawMode(bool isEnabled);

  //! For some signals it disconnects the window from usual signal and connect it to the module ones
  void reconnectActions(SUIT_ViewWindow* theWindow, const bool theUseSHAPERSlot);

  //! Perfroms the fit all for the active view
  virtual void fitAll();

  //! Erases all presentations from the viewer
  virtual void eraseAll();

  //! Sets the view projection
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  /// \param theTwist the twist angle in radians
  virtual void setViewProjection( double theX, double theY, double theZ,
                                  double theTwist );

  /// Set selector
  /// \param theSel a selector instance
  void setSelector(SHAPERGUI_OCCSelector* theSel);

  /// Add selection filter to the viewer
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove selection filter from the viewer
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Returns true if the selection filter is set to the viewer
  /// \param theFilter a selection filter
  virtual bool hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove all selection filters from the viewer
  virtual void clearSelectionFilters();

  /// Returns current selector
  SHAPERGUI_OCCSelector* selector() const
  {
    return mySelector;
  }

  /// Update current viewer
  virtual void update();

  /// Method returns True if the viewer can process editing objects
  /// by mouse drugging. If this is impossible thet it has to return False.
  virtual bool canDragByMouse() const;

  /// Activate or deactivate viewer
  /// \param toActivate - activation flag
  void activateViewer(bool toActivate);


  // Methods for color scale management

  //! Returns True if ColorScale is visible
  virtual bool isColorScaleVisible() const;

  //! Show/Hide ColorScale object
  virtual void setColorScaleShown(bool on);

  //! Set position of color scale
  // \param theX is X position relative to current view width
  // \param theY is Y position relative to current view heigth
  virtual void setColorScalePosition(double theX, double theY);

  //! Set size of color scale
  // \param theW is width relative to current view width
  // \param theh is height relative to current view heigth
  virtual void setColorScaleSize(double theW, double theH);

  //! Set range of color scale
  // \param theMin is a minimal value
  // \param theMax is a maximal value
  virtual void setColorScaleRange(double theMin, double theMax);

  //! Set number of intervals of color scale
  // \param theNb is number of intervals
  virtual void setColorScaleIntervals(int theNb);

  //! Set text heigth of color scale
  // \param theH is number of intervals
  virtual void setColorScaleTextHeigth(int theH);

  //! Set color of text of color scale
  // \param theH is number of intervals
  virtual void setColorScaleTextColor(const QColor& theColor);

  //! Set title of color scale
  // \param theText is a title
  virtual void setColorScaleTitle(const QString& theText);

  // Fit all along Z (perpendicular to display)
  //virtual void Zfitall();

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
  void onActivated(SUIT_ViewManager*);

  void onSelectionChanged();
  void onViewTransformed(OCCViewer_ViewWindow::OperationType);

  /// Emit signal about trihedron visiblity change because SALOME sets
  /// the trihedron visible by this signal.
  /// It is necessary to activate the viewer trihedron in the current selection mode
  void onViewPortMapped();

 private:
  SHAPERGUI_OCCSelector* mySelector;
  SHAPERGUI_SalomeView* myView;
  bool myIsSelectionChanged;
};


#endif
