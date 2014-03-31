#ifndef XGUI_Viewer_H
#define XGUI_Viewer_H

#include "XGUI_Constants.h"

#include <QObject>
#include <QMap>

#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>

class XGUI_MainWindow;
class QMdiSubWindow;

class XGUI_Viewer: public QObject
{
Q_OBJECT
public:
  static QString backgroundData(QStringList&, QIntList&, QIntList&);

  XGUI_Viewer(XGUI_MainWindow* theParent, bool DisplayTrihedron = true);
  ~XGUI_Viewer();

  QMdiSubWindow* createView(V3d_TypeOfView theType = V3d_ORTHOGRAPHIC);

  XGUI_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  Handle(V3d_Viewer) v3dViewer() const
  {
    return myV3dViewer;
  }

  Handle(AIS_InteractiveContext) AISContext() const
  {
    return myAISContext;
  }

  Handle(AIS_Trihedron) trihedron() const
  {
    return myTrihedron;
  }

  void toggleTrihedron();
  bool isTrihedronVisible() const;
  void setTrihedronShown(const bool on);
  double trihedronSize() const;
  void setTrihedronSize(const double sz, bool isRelative);
  bool trihedronRelative() const
  {
    return myIsRelative;
  }
  void updateTrihedron();
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

private slots:
  void onViewClosed();
  void onViewMapped();

private:
  XGUI_MainWindow* myMainWindow;

  Handle(V3d_Viewer) myV3dViewer;Handle(AIS_Trihedron) myTrihedron;Handle(AIS_InteractiveContext) myAISContext;

  XGUI::InteractionStyle myInteractionStyle;

  bool myPreselectionEnabled;
  bool mySelectionEnabled;
  bool myMultiSelectionEnabled;
  bool myIsRelative;

  double myTrihedronSize;
};

#endif
