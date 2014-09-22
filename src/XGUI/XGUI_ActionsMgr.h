/*
 * XGUI_ActionsMgr.h
 */

#ifndef XGUI_ACTIONSMGR_H_
#define XGUI_ACTIONSMGR_H_

#include "XGUI.h"

#include <QObject>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QKeySequence>

class XGUI_Command;
class XGUI_Workshop;
class XGUI_OperationMgr;
class ModuleBase_Operation;
class QAction;

class XGUI_EXPORT XGUI_ActionsMgr : public QObject
{
Q_OBJECT

 public:
  XGUI_ActionsMgr(XGUI_Workshop* theWorkshop);
  virtual ~XGUI_ActionsMgr();

  //! Add a command in the manager.
  //! Please note that nested commands in the Salome mode (No XGUI_Command, pure QActions)
  //! won't be extracted and should be added manually using the addNestedCommands method.
  void addCommand(QAction* theCmd);
  //! Sets relation between the command (with given Id) and it's nested actions.
  void addNestedCommands(const QString& theId, const QStringList& theCommands);

  QStringList nestedCommands(const QString& theId) const;

  bool isNested(const QString& theId) const;

  QKeySequence registerShortcut(const QString& theKeySequence);

 public slots:
  //! Update workbench actions according to OperationMgr state:
  //! No active operations: all actions but nested are available
  //! There is active operation: current operation + it's nested
  //! are enabled, all the rest is disabled. All active commands is checked.
  void update();
  //! Sets all commands checked if it's operation is active.
  void updateCheckState();

 protected:
  //! Sets all actions to isEnabled state.
  void setAllEnabled(bool isEnabled);
  //! Sets to isEnabled state all siblings of the given operation and it's parents recursively
  void setNestedStackEnabled(ModuleBase_Operation* theOperation);
  //! Sets all nested actions to isEnabled state for the command with given ID.
  //! If ID is empty - all nested actions will be affected.
  void setNestedCommandsEnabled(bool isEnabled, const QString& theParent = QString());
  //! Sets the action with theId to theChecked state.
  void setActionChecked(const QString& theId, const bool theChecked);
  //! Sets the action with theId to theEnabled state.
  void setActionEnabled(const QString& theId, const bool theEnabled);

 private:
  QMap<QString, QAction*> myActions;
  QMap<QString, QStringList> myNestedActions;
  QList<QKeySequence> myShortcuts;

  XGUI_Workshop* myWorkshop;
  XGUI_OperationMgr* myOperationMgr;
};

#endif /* XGUI_ACTIONSMGR_H_ */

