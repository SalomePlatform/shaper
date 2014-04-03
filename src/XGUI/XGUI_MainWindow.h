#ifndef XGUI_MAINWINDOW_H
#define XGUI_MAINWINDOW_H

#include <QMainWindow>

class XGUI_MainMenu;
class XGUI_Viewer;
class QTreeWidget;
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

  QTreeWidget* objectBrowser() const
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

  void showPythonConsole();
  void hidePythonConsole();

private:
  //!! For test purposes only
  void fillObjectBrowser();
  void addPropertyPanel();

  XGUI_MainMenu* myMenuBar;
  QTreeWidget* myObjectBrowser;

  XGUI_Viewer* myViewer;

  PyConsole_EnhConsole* myPythonConsole;
};

#endif
