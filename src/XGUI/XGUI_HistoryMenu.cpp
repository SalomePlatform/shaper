// Copyright (C) 2014-2024  CEA, EDF
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

#include <XGUI_HistoryMenu.h>

#include <ModelAPI_Session.h>

#include <QListWidget>
#include <QWidgetAction>
#include <QToolButton>
#include <QAction>

XGUI_HistoryMenu::XGUI_HistoryMenu(QAction* theParent)
 : QMenu(NULL),
   myHistoryList(NULL)
{
  theParent->setMenu(this);
  initMenu();

  connect(theParent, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

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
#ifdef _DEBUG
    QListWidgetItem* anItem =
#endif
      new QListWidgetItem(anAct.icon, anAct.text, myHistoryList);
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

void hideUpToMenuBar( QMenu* theMenu )
{
  theMenu->hide();
  foreach( QWidget* aWidget, theMenu->menuAction()->associatedWidgets() )
  {
    QMenu* aMenu = qobject_cast<QMenu*>( aWidget );
    if( aMenu )
    {
      aMenu->hide();
      hideUpToMenuBar( aMenu );
    }
  }
}

void XGUI_HistoryMenu::onItemPressed(QListWidgetItem * theItem)
{
  int selectedSize = myHistoryList->row(theItem) + 1;
  emit actionSelected(selectedSize);
  hideUpToMenuBar( this );
}
