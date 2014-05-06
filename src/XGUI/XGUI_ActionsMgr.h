/*
 * XGUI_ActionsMgr.h
 */

#ifndef XGUI_ACTIONSMGR_H_
#define XGUI_ACTIONSMGR_H_

#include "XGUI.h"

#include <QObject>
#include <QMap>
#include <QStringList>

class XGUI_Command;
class XGUI_Workshop;
class QAction;

class XGUI_EXPORT XGUI_ActionsMgr: public QObject
{
  Q_OBJECT

public:
  XGUI_ActionsMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_ActionsMgr();


  void addCommand(XGUI_Command* theCmd);

  /// Register a command in SALOME mode
  /// \param theId - string ID of the command
  /// \param theCmd - command object
  void addCommand(QString theId, QAction* theCmd);

  void saveCommandsState();
  void restoreCommandState();

  /// Set the action is checked
  /// \param theId - string ID of the command
  /// \praram theChecked - the new checked state
  void setActionChecked(const QString& theId, const bool theChecked);

  void updateAction(const QString&);
  void setNestedActionsEnabled(bool);

public slots:
  void setActionsDisabled(bool isEnabled);

private:
  QStringList myNestedActions;
  QMap<QString, QAction*> myActions;
  QMap<QString, bool> myActionsState;

  XGUI_Workshop* myWorkshop;
};

#endif /* XGUI_ACTIONSMGR_H_ */
