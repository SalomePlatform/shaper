#ifndef XGUI_MainMenu_H
#define XGUI_MainMenu_H

#include "XGUI.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QWidget>

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
class XGUI_EXPORT XGUI_MainMenu: public QWidget
{
  Q_OBJECT

  //! Size of menu item
  //TODO(sbh, vsv): Move to the settings
  enum ItemSize {
    Small = 20,
    Medium = 25,
    Large = 30
  };

public:
  XGUI_MainMenu(XGUI_MainWindow *parent);
  virtual ~XGUI_MainMenu();

  //! Creates and adds a new workbench (menu group) with the given name and returns it.
  XGUI_Workbench* addWorkbench(const QString& theId, const QString& theText = "");

  //! Returns workbench (menu group) by the given name.
  XGUI_Workbench* findWorkbench(const QString& theId)  const;

  //! Returns General page (predefined workbench)
  XGUI_MenuGroupPanel* generalPage() const { return myGeneralPage; }

  //! Rerturns last created workbench in dock widget container
  //QDockWidget* getLastDockWindow() const;

  //! Returns already created command by its ID
  XGUI_Command* feature(const QString& theId) const;

  //! Returns list of created commands
  QList<XGUI_Command*> features() const;

  virtual bool eventFilter(QObject *theWatched, QEvent *theEvent);

  //! Displays given console as a tab in the workbench
  void insertConsole(QWidget*);
  //! Removes already created tab with python console
  void removeConsole();

  //! Defines size of menu item.
  //! In the future this value should be extracted from the preferences.
  int menuItemSize() const;
  //! Defines number of menu item rows.
  //! In the future this value should be extracted from the preferences.
  int menuItemRowsCount() const;
  //! Defines height of the main menu. (Number of rows * row height)
  int menuHeight() const;

private:
  XGUI_MainWindow* myDesktop;
  QTabWidget* myMenuTabs;
  XGUI_MenuGroupPanel* myGeneralPage;

  QMap<XGUI_Command*, bool> myCommandState;
};

#endif
