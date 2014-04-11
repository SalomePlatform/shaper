#ifndef XGUI_MainMenu_H
#define XGUI_MainMenu_H

#include <QObject>
#include <QList>

class XGUI_Command;
class XGUI_MainWindow;
class XGUI_Workbench;
class XGUI_MenuGroupPanel;

class QTabWidget;
class QLabel;
class QAction;
class QDockWidget;
class QEvent;

/**\class XGUI_MainMenu
 * \ingroup GUI
 * \brief Class for creation of main menu (set of workbenches)
 */
class XGUI_MainMenu: public QObject
{
Q_OBJECT
public:
  XGUI_MainMenu(XGUI_MainWindow *parent);
  virtual ~XGUI_MainMenu();

  //! Creates and adds a new workbench (menu group) with the given name and returns it.
  XGUI_Workbench* addWorkbench(const QString& theId, const QString& theText = "");

  //! Returns workbench (menu group) by the given name.
  XGUI_Workbench* findWorkbench(const QString& theId);

  //! Returns General page (predefined workbench)
  XGUI_Workbench* generalPage() const { return myGeneralPage; }

  //! Rerturns last created workbench in dock widget container
  QDockWidget* getLastDockWindow() const { return myMenuTabs.last(); }

  //! Returns already created command by its ID
  XGUI_Command* feature(const QString& theId) const;

  //! Returns list of created commands
  QList<XGUI_Command*> features() const;

protected:
  virtual bool eventFilter(QObject *theWatched, QEvent *theEvent);

private:
  XGUI_MainWindow* myDesktop;
  QList<QDockWidget*> myMenuTabs;
  XGUI_Workbench* myGeneralPage;
};

#endif
