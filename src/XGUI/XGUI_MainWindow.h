#ifndef XGUI_MAINWINDOW_H
#define XGUI_MAINWINDOW_H

#include "XGUI.h"
#include <QMainWindow>

class XGUI_MainMenu;
class XGUI_Viewer;
class XGUI_ObjectsBrowser;
class QMdiArea;
class PyConsole_EnhConsole;

/**\class XGUI_MainWindow
 * \ingroup GUI
 * \brief Main window of the application (Desktop).
 * It contains: Object Browser, 3d Viewer, Python console, property panel, main menu
 */
class XGUI_EXPORT XGUI_MainWindow: public QMainWindow
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

  //! Returns Object browser
  XGUI_ObjectsBrowser* objectBrowser() const
  {
    return myObjectBrowser;
  }

  //! Returns MDI area
  QMdiArea* mdiArea() const;

  //! Returns 3d viewer
  XGUI_Viewer* viewer() const
  {
    return myViewer;
  }

  // Creates Dock widgets: Object broewser and Property panel
  void createDockWidgets();
  void setPropertyPannelTitle(const QString& theTitle);

public slots:
  void showPythonConsole();
  void hidePythonConsole();
  void showPropertyPanel();
  void hidePropertyPanel();
  void showObjectBrowser();
  void hideObjectBrowser();

private:
  QDockWidget* createObjectBrowser();
  QDockWidget* createPropertyPanel();

  XGUI_MainMenu* myMenuBar;
  XGUI_ObjectsBrowser* myObjectBrowser;
  QDockWidget* myPropertyPanelDock;

  XGUI_Viewer* myViewer;

  PyConsole_EnhConsole* myPythonConsole;
};

#endif
