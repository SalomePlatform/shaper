#ifndef XGUI_MAINWINDOW_H
#define XGUI_MAINWINDOW_H

#include <QMainWindow>

class XGUI_MainMenu;
class XGUI_Viewer;
class XGUI_ObjectsBrowser;
class QMdiArea;
class PyConsole_EnhConsole;

class XGUI_MainWindow: public QMainWindow
{
Q_OBJECT

public:
  XGUI_MainWindow(QWidget* parent = 0);
  virtual ~XGUI_MainWindow();

  XGUI_MainMenu* menuObject() const
  {
    return myMenuBar;
  }

  XGUI_ObjectsBrowser* objectBrowser() const
  {
    return myObjectBrowser;
  }

  void showObjectBrowser();
  void hideObjectBrowser();

  QMdiArea* mdiArea() const;

  XGUI_Viewer* viewer() const
  {
    return myViewer;
  }

public slots:
  void showPythonConsole();
  void hidePythonConsole();
  void showPropertyPanel();
  void hidePropertyPanel();

private:
  void createDockWidgets();
  QDockWidget* createObjectBrowser();
  QDockWidget* createPropertyPanel();

  XGUI_MainMenu* myMenuBar;
  XGUI_ObjectsBrowser* myObjectBrowser;
  QDockWidget* myPropertyPanelDock;

  XGUI_Viewer* myViewer;

  PyConsole_EnhConsole* myPythonConsole;
};

#endif
