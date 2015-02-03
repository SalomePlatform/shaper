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

class QListWidget;
class QToolButton;
class QListWidgetItem;

class XGUI_EXPORT XGUI_HistoryMenu : public QMenu
{
  Q_OBJECT
 public:
  explicit XGUI_HistoryMenu(QToolButton* theParent);
  virtual ~XGUI_HistoryMenu();

 signals:
  void actionsSelected(int);

 public slots:
  void setHistory(const QList<QAction*>&);

 protected slots:
  void setStackSelectedTo(QListWidgetItem *);
  void onItemPressed(QListWidgetItem *);

 private:
  QListWidget* myHistoryList;
};

#endif /* XGUI_OPERATIONSSTACKPOPUP_H_ */
