#ifndef XGUI_MAINWINDOW_H
#define XGUI_MAINWINDOW_H

#include "XGUI.h"
#include <QMainWindow>

class XGUI_MainMenu;
class XGUI_Viewer;
class XGUI_ActionsMgr;
class XGUI_ViewWindow;
class QMdiArea;
class QMdiSubWindow;
class PyConsole_EnhConsole;
class QCloseEvent;

/**\class XGUI_MainWindow
 * \ingroup GUI
 * \brief Main window of the application (Desktop).
 * It contains: Object Browser, 3d Viewer, Python console, property panel, main menu
 */
class XGUI_EXPORT XGUI_MainWindow : public QMainWindow
{
Q_OBJECT

 public:
  XGUI_MainWindow(QWidget* parent = 0);
  virtual ~XGUI_MainWindow();

  //! Returns main menu object
  XGUI_MainMenu* menuObject() const
  {
    return myMenuBar;
  }

  //! Returns MDI area
  QMdiArea* mdiArea() const;

  //! Returns 3d viewer
  XGUI_Viewer* viewer() const
  {
    return myViewer;
  }

 public slots:
  void showPythonConsole();
  void hidePythonConsole();
  //! Python console can be a dock widget 
  void dockPythonConsole();
  //! or can be a tab in the main menu.
  void undockPythonConsole();

  void createSubWindow();

 private slots:
  void cascadeWindows();
  void onViewCreated(XGUI_ViewWindow* theWindow);
  void onDeleteView(XGUI_ViewWindow* theWindow);
  void activateView();
  void onViewActivated(QMdiSubWindow* theSubWnd);

signals:
  void exitKeySequence();

 protected:
  void closeEvent(QCloseEvent* event);
  void createMainMenu();

 private:
  XGUI_MainMenu* myMenuBar;
  XGUI_Viewer* myViewer;

  PyConsole_EnhConsole* myPythonConsole;
};

class XGUI_EXPORT CloseEventWatcher : public QObject
{
Q_OBJECT

 public:
  CloseEventWatcher(QObject* theParent);

signals:
  void widgetClosed();

 protected:
  bool eventFilter(QObject *obj, QEvent *);

};

#endif
