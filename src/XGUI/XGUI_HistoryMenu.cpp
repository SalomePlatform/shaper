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
#include <QAction>

//! Extends given feature with previously created context menu.
//! \param theId - Id of the feature to add \a theMenu
//! \param theMenu - Enables or disables menu feature
XGUI_HistoryMenu::XGUI_HistoryMenu(QAction* theParent)
 : QMenu(NULL),
   myHistoryList(NULL)
{
  theParent->setMenu(this);
  initMenu();

  connect(theParent, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

//! Extends given feature with previously created context menu.
//! \param theId - Id of the feature to add \a theMenu
//! \param theMenu - Enables or disables menu feature
XGUI_HistoryMenu::XGUI_HistoryMenu(QToolButton* theParent)
 : QMenu(theParent),
   myHistoryList(NULL)
{
  theParent->setMenu(this);
  theParent->setPopupMode(QToolButton::MenuButtonPopup);

  initMenu();
}

void XGUI_HistoryMenu::initMenu()
{
  myHistoryList = new QListWidget(this);
  QWidgetAction* aListAction = new QWidgetAction(this);
  aListAction->setDefaultWidget(myHistoryList);
  this->addAction(aListAction);
  myHistoryList->setMouseTracking(true);  // track mouse hover
  myHistoryList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(myHistoryList, SIGNAL(itemEntered(QListWidgetItem *)), this,
          SLOT(setStackSelectedTo(QListWidgetItem *)));
  connect(myHistoryList, SIGNAL(itemClicked(QListWidgetItem *)), this,
          SLOT(onItemPressed(QListWidgetItem *)));
}

XGUI_HistoryMenu::~XGUI_HistoryMenu()
{
}

void XGUI_HistoryMenu::setHistory(const QList<ActionInfo>& theActions)
{
  myHistoryList->clear();
  foreach(ActionInfo anAct, theActions) {
    QListWidgetItem* anItem = new QListWidgetItem(anAct.icon, anAct.text, myHistoryList);
  }
}

void XGUI_HistoryMenu::leaveEvent(QEvent* theEvent)
{
  setStackSelectedTo(NULL);
  QMenu::leaveEvent(theEvent);
}

void XGUI_HistoryMenu::setStackSelectedTo(QListWidgetItem * theItem)
{
  QListWidgetItem* eachItem = NULL;
  bool isSelect = theItem != NULL;
  for(int aRow = 0; aRow < myHistoryList->count(); ++aRow) {
    eachItem = myHistoryList->item(aRow);
    myHistoryList->setItemSelected(eachItem, isSelect);
    // Deselect items below hovered
    if (eachItem == theItem) {
      isSelect = false;
    }
  }
  // to avoid blinking caused by QMenu paint event (paints on top of the list)
  myHistoryList->repaint();
}

void XGUI_HistoryMenu::onItemPressed(QListWidgetItem * theItem)
{
  int selectedSize = myHistoryList->row(theItem) + 1;
  emit actionSelected(selectedSize);
  hide();
}
