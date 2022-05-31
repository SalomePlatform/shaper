// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef XGUI_VIEWERPROXY_H
#define XGUI_VIEWERPROXY_H

#include "XGUI.h"
#include <ModuleBase_IViewer.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

#include <AIS_Trihedron.hxx>
#include <AIS_ListOfInteractive.hxx>

#ifdef HAVE_SALOME
#include <OCCViewer_ViewModel.h>
#else
  #include <AppElements_Viewer.h>
  #include <AppElements_ViewWindow.h>
#endif



class XGUI_Workshop;
/**
 * \ingroup GUI
 * Proxy class which repersents or AppElements_Viewer or Salome Viewer
 * dependently on current launching environment.
 * It is reccomennded to use this class in operation for accessing to viewer 
 * functionality instead of direct access to a viewer
 */
class XGUI_EXPORT XGUI_ViewerProxy : public ModuleBase_IViewer
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  XGUI_ViewerProxy(XGUI_Workshop* theParent);

  /// Connects some signals to the viewer from the module connector
  void connectViewProxy();

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const;

  //! Trihedron 3d object shown in the viewer
  virtual Handle(AIS_Trihedron) trihedron() const;

  //! Retrurns V3d_Vioewer from current viewer
  virtual Handle(V3d_Viewer) v3dViewer() const;

  //! Returns Vsd_View object from currently active view window
  virtual Handle(V3d_View) activeView() const;

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

  //! Sets the view projection
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  /// \param theTwist the twist angle in radians
  virtual void setViewProjection( double theX, double theY, double theZ,
                                  double theTwist );

  //! Sets the view fitted all
  virtual void fitAll();

  //! Erases all presentations from the viewer
  virtual void eraseAll();

  /// Connects to a viewer according to current environment
  void connectToViewer();

  /// Add selection filter to the viewer
  virtual void addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove selection filter from the viewer
  virtual void removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Returns true if the selection filter is set to the viewer
  /// \param theFilter a selection filter
  virtual bool hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter);

  /// Remove all selection filters from the viewer
  virtual void clearSelectionFilters();

  /// Update current viewer
  virtual void update();

  /// Method returns True if the viewer can process editing objects
  /// by mouse drugging. If this is impossible thet it has to return False.
  virtual bool canDragByMouse() const;

  virtual void updateHighlight();

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

  //! Set color scale parameters according to user preferences and window size
  void setupColorScale();

  // Fit all along Z (perpendicular to display)
  //virtual void Zfitall();

#ifdef HAVE_SALOME
  virtual void setFitter(OCCViewer_Fitter* theFitter);
  virtual OCCViewer_Fitter* fitter() const;
#else
  virtual void setFitter(AppElements_Fitter* theFitter);
  virtual AppElements_Fitter* fitter() const;
#endif

signals:
  /// Emits by mouse entering the view port
  void enterViewPort();

  /// Emits by mouse leaving of the view port
  void leaveViewPort();

protected:
  /// processes the application signals to catch the mouse leaving state of the main window
  /// \param theObject
  /// \param theEvent
  bool eventFilter(QObject *theObject, QEvent *theEvent);

private slots:
  void onViewCreated(ModuleBase_IViewWindow* theWnd);

#ifndef HAVE_SALOME
  void onTryCloseView(AppElements_ViewWindow*);
  void onDeleteView(AppElements_ViewWindow*);
  void onViewCreated(AppElements_ViewWindow*);
  void onActivated(AppElements_ViewWindow*);

  void onMousePress(AppElements_ViewWindow*, QMouseEvent*);
  void onMouseRelease(AppElements_ViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(AppElements_ViewWindow*, QMouseEvent*);
  void onMouseMove(AppElements_ViewWindow*, QMouseEvent*);

  void onKeyPress(AppElements_ViewWindow*, QKeyEvent*);
  void onKeyRelease(AppElements_ViewWindow*, QKeyEvent*);

  void onViewTransformed(AppElements_ViewWindow::OperationType);
#else
  void onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*);
  void onKeyPress(ModuleBase_IViewWindow*, QKeyEvent*);
  void onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*);
#endif

 private:
   void displayHighlight(FeaturePtr theFeature, const TopoDS_Shape& theIgnoreShape);
   bool eraseHighlight();
   void createPresentations(const ResultPtr& theRes, const TopoDS_Shape& theIgnoreShape);


  XGUI_Workshop* myWorkshop;
  ResultPtr myResult;
  AIS_ListOfInteractive myHighlights;

};

#endif
