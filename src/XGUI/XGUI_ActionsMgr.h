/*
 * XGUI_ActionsMgr.h
 */

#ifndef XGUI_ACTIONSMGR_H_
#define XGUI_ACTIONSMGR_H_

#include <QObject>
#include <QMap>
#include <QStringList>

class XGUI_Command;
class QAction;

class XGUI_ActionsMgr: public QObject
{
  Q_OBJECT

public:
  XGUI_ActionsMgr(QObject* theParent);
  virtual ~XGUI_ActionsMgr();


  void addCommand(XGUI_Command* theCmd);
  void saveCommandsState();
  void restoreCommandState();

  void setNestedActionsEnabled(bool);

public slots:
  void setActionsDisabled(bool isEnabled);

private:
  QStringList myNestedActions;
  QMap<QString, QAction*> myActions;
  QMap<QString, bool> myActionsState;
};

#endif /* XGUI_ACTIONSMGR_H_ */
