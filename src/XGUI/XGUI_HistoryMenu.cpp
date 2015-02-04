/*
 * XGUI_HistoryMenu.cpp
 *
 *  Created on: Feb 2, 2015
 *      Author: sbh
 */

#include <XGUI_HistoryMenu.h>

#include <ModelAPI_Session.h>

#include <QListWidget>
#include <QWidgetAction>
#include <QToolButton>

//! Extends given feature with previously created context menu.
//! \param theId - Id of the feature to add \a theMenu
//! \param theMenu - Enables or disables menu feature
XGUI_HistoryMenu::XGUI_HistoryMenu(QToolButton* theParent)
 : QMenu(theParent),
   myHistoryList(new QListWidget(this))
{
  theParent->setMenu(this);
  theParent->setPopupMode(QToolButton::MenuButtonPopup);

  QWidgetAction* aListAction = new QWidgetAction(this);
  aListAction->setDefaultWidget(myHistoryList);
  this->addAction(aListAction);

  myHistoryList->setMouseTracking(true); // track mouse hover
  myHistoryList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(myHistoryList, SIGNAL(itemEntered(QListWidgetItem *)),
          this,          SLOT(setStackSelectedTo(QListWidgetItem *)));
  connect(myHistoryList, SIGNAL(itemClicked(QListWidgetItem *)),
          this,          SLOT(onItemPressed(QListWidgetItem *)));
}

XGUI_HistoryMenu::~XGUI_HistoryMenu()
{
}

void XGUI_HistoryMenu::setHistory(const QList<QAction*>& theActions)
{
  myHistoryList->clear();
  foreach(QAction* anAct, theActions) {
    QListWidgetItem* anItem = new QListWidgetItem(anAct->icon(),
                                                  anAct->text(),
                                                  myHistoryList);
    anAct->deleteLater();
  }
}


void XGUI_HistoryMenu::setStackSelectedTo(QListWidgetItem * theItem)
{
  if (!theItem)
    return;

  QListWidgetItem* eachItem = NULL;
  bool isSelect = true;
  for(int aRow = 0; aRow < myHistoryList->count(); ++aRow) {
    eachItem = myHistoryList->item(aRow);
    myHistoryList->setItemSelected(eachItem, isSelect);
    // Deselect items below hovered
    if (eachItem == theItem) {
      isSelect = false;
    }
  }
}

void XGUI_HistoryMenu::onItemPressed(QListWidgetItem * theItem)
{
  int selectedSize = myHistoryList->row(theItem) + 1;
  emit actionsSelected(selectedSize);
  hide();
  myHistoryList->clear();
}
