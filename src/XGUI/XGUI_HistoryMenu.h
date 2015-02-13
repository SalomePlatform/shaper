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

/**
 * \class XGUI_HistoryMenu
 * \ingroup GUI
 * \brief Extends regular QToolButton of QAction with HistoryMenu (stack of actions)
 */
class XGUI_EXPORT XGUI_HistoryMenu : public QMenu
{
  Q_OBJECT
 public:
  /// Creates history menu for button
  explicit XGUI_HistoryMenu(QToolButton* theParent);
  /// Creates history menu for action
  explicit XGUI_HistoryMenu(QAction* theParent);
  virtual ~XGUI_HistoryMenu();

 signals:
  /// Signal. Emited then n-th action is selected in stack
  void actionSelected(int);

 public slots:
  /// Set list of actions
  void setHistory(const QList<ActionInfo>&);

 protected:
  /// Unselects all items when cursor leaves the list
  virtual void leaveEvent(QEvent *);

 protected slots:
  /// Selects all items in stack to the given item including it
  void setStackSelectedTo(QListWidgetItem *);
  /// Emits actionSelected() signal.
  void onItemPressed(QListWidgetItem *);
  /// Initializes internal controls of the Menu
  void initMenu();

 private:
  QListWidget* myHistoryList; ///< List to contain actions
};

#endif /* XGUI_OPERATIONSSTACKPOPUP_H_ */
