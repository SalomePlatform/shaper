/*
 * XGUI_HistoryMenu.h
 *
 *  Created on: Feb 2, 2015
 *      Author: sbh
 */

#ifndef XGUI_HISTORYMENU_H_
#define XGUI_HISTORYMENU_H_

#include <XGUI.h>
#include <QMenu>

#include <ModuleBase_ActionInfo.h>

class QListWidget;
class QToolButton;
class QListWidgetItem;

class XGUI_EXPORT XGUI_HistoryMenu : public QMenu
{
  Q_OBJECT
 public:
  explicit XGUI_HistoryMenu(QToolButton* theParent);
  explicit XGUI_HistoryMenu(QAction* theParent);
  virtual ~XGUI_HistoryMenu();

 signals:
  void actionSelected(int);

 public slots:
  void setHistory(const QList<ActionInfo>&);

 protected:
  virtual void leaveEvent(QEvent *);

 protected slots:
  void setStackSelectedTo(QListWidgetItem *);
  void onItemPressed(QListWidgetItem *);
  void initMenu();


 private:
  QListWidget* myHistoryList;
};

#endif /* XGUI_OPERATIONSSTACKPOPUP_H_ */
