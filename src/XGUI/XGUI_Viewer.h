#ifndef XGUI_Viewer_H
#define XGUI_Viewer_H

#include "XGUI.h"
#include "XGUI_Constants.h"

#include <QObject>
#include <QMap>
#include <QList>

#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

class XGUI_MainWindow;
class QMdiSubWindow;
class XGUI_ViewWindow;
class QMouseEvent;
class QKeyEvent;

class AIS_ListOfInteractive;

/**\class XGUI_Viewer
 * \ingroup GUI
 * \brief Represents a 3d viewer. The viewer manages 3d scene and a set of view windows
 * when each of view window is a one point of view on this scene.
 */
class XGUI_EXPORT XGUI_Viewer: public QObject
{
Q_OBJECT
public:
  static QString backgroundData(QStringList&, QIntList&, QIntList&);

  XGUI_Viewer(XGUI_MainWindow* theParent, bool DisplayTrihedron = true);
  ~XGUI_Viewer();

  //! Creates a new view window
  QMdiSubWindow* createView(V3d_TypeOfView theType = V3d_ORTHOGRAPHIC);

  //! Return pointer on a main window - parent of the Viewer
  XGUI_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  //! Returns OCCT object which manages 3d scene
  Handle(V3d_Viewer) v3dViewer() const
  {
    return myV3dViewer;
  }

  //! Returns OCCT object which manages displaying and selection in 3d scene
  Handle(AIS_InteractiveContext) AISContext() const
  {
    return myAISContext;
  }

  //! Activate local selection
  //! \param theAIS the list of objects
  //! \param theMode the selection mode
  //! \param isUpdateViewer the state wether the viewer should be updated immediatelly
  void setLocalSelection(const AIS_ListOfInteractive& theAISObjects, const int theMode,
                         const bool isUpdateViewer);
  //! Deactivate local selection
  //! \param isUpdateViewer the state wether the viewer should be updated immediatelly
  void setGlobalSelection(const bool isUpdateViewer);

  /// Return objects selected in 3D viewer
  /// \param theList - list to be filled with selected objects
  void  getSelectedObjects(AIS_ListOfInteractive& theList);

  /// Return shapes selected in 3D viewer
  /// \param theList - list to be filled with selected shapes
  void getSelectedShapes(NCollection_List<TopoDS_Shape>& theList);

  /// Selects objects in 3D viewer. Other selected objects are left as selected
  /// \param theList - list objects to be selected
  void  setObjectsSelected(const AIS_ListOfInteractive& theList);

  /// Select the object in 3D viewer.
  /// \param theIO - list objects to be selected
  void setSelected(const Handle(AIS_InteractiveObject)& theIO) { myAISContext->SetSelected(theIO); }

  //! Trihedron 3d object shown in the viewer
  Handle(AIS_Trihedron) trihedron() const
  {
    return myTrihedron;
  }

  //! On/Off visibility of the trihedron object
  void toggleTrihedron();

  //! Returns true if trihedron is visible
  bool isTrihedronVisible() const;

  //! Returns true if trihedron is visible
  void setTrihedronShown(bool on);

  //! Returns trihedron size
  double trihedronSize() const;

  //! Sets trihedron size
  void setTrihedronSize(const double sz, bool isRelative);

  bool trihedronRelative() const
  {
    return myIsRelative;
  }
  //! Update trihedron
  void updateTrihedron();

  //! Compute trihedron size dependent on 3d scene size
  bool computeTrihedronSize(double& theNewSize, double& theSize);

  static void setHotButton(XGUI::InteractionStyle theInteractionStyle, XGUI::HotOperation theOper,
                           Qt::KeyboardModifiers theState, Qt::MouseButtons theButton);
  static void getHotButton(XGUI::InteractionStyle theInteractionStyle, XGUI::HotOperation theOper,
                           Qt::KeyboardModifiers& theState, Qt::MouseButtons& theButton);

  typedef QMap<XGUI::HotOperation, Qt::KeyboardModifiers> StatesMap;
  typedef QMap<XGUI::HotOperation, Qt::MouseButtons> ButtonsMap;

  typedef QMap<XGUI::InteractionStyle, StatesMap> InteractionStyle2StatesMap;
  typedef QMap<XGUI::InteractionStyle, ButtonsMap> InteractionStyle2ButtonsMap;

  static InteractionStyle2StatesMap myStateMap;
  static InteractionStyle2ButtonsMap myButtonMap;

signals:
  void lastViewClosed();
  void tryCloseView(XGUI_ViewWindow* theWindow);
  void deleteView(XGUI_ViewWindow* theWindow);
  void viewCreated(XGUI_ViewWindow* theWindow);
  void mousePress(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);
  void mouseRelease(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);
  void mouseDoubleClick(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);
  void mouseMove(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);
  void keyPress(XGUI_ViewWindow* theWindow, QKeyEvent* theEvent);
  void keyRelease(XGUI_ViewWindow* theWindow, QKeyEvent* theEvent);
  void activated(XGUI_ViewWindow* theWindow);
  void selectionChanged();

public slots:
  void onWindowMinimized(QMdiSubWindow*);
  void onWindowActivated(QMdiSubWindow*);

private slots:
  void onViewClosed(QMdiSubWindow*);
  void onMouseMove(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);
  void onMouseReleased(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);

private:
  void addView(QMdiSubWindow* theView);

  /*! Removes the View from internal Views list.*/
  void removeView(QMdiSubWindow* theView);

private:
  XGUI_MainWindow* myMainWindow;

  Handle(V3d_Viewer) myV3dViewer;Handle(AIS_Trihedron) myTrihedron;Handle(AIS_InteractiveContext) myAISContext;

  XGUI::InteractionStyle myInteractionStyle;

  bool myPreselectionEnabled;
  bool mySelectionEnabled;
  bool myMultiSelectionEnabled;
  bool myIsRelative;

  double myTrihedronSize;
  
  QList<QMdiSubWindow*> myViews;

  QMdiSubWindow* myActiveView;
};

#endif
