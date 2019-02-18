// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
