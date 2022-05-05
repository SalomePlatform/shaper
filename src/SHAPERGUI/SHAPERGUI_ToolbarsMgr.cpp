// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "SHAPERGUI_ToolbarsMgr.h"
#include "SHAPERGUI.h"

#include <CAM_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <QtxActionToolMgr.h>

#include <QLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>


#define SEPARATOR "------"
#define LIST_WIDTH 180

class SHAPERGUI_CommandIdItem : public QListWidgetItem
{
public:
  SHAPERGUI_CommandIdItem(QListWidget* theParent, int theId, SHAPERGUI* theModule)
    : QListWidgetItem(theParent), myModule(theModule), myId(theId) {}


  virtual QVariant data(int theRole) const {
    QAction* aAction = 0;
    if (myId != -1)
      aAction = myModule->action(myId);

    switch (theRole) {
    case Qt::DisplayRole:
      if (aAction)
        return aAction->text();
      else
        return SEPARATOR;
    case Qt::DecorationRole:
      if (aAction)
        return aAction->icon();
      else
        return QIcon();
    }
    return QListWidgetItem::data(theRole);
  }


  int id() const { return myId; }

private:
  SHAPERGUI* myModule;
  int myId;
};


//************************************************************************************
//************************************************************************************
//************************************************************************************
SHAPERGUI_ToolbarsDlg::SHAPERGUI_ToolbarsDlg(SHAPERGUI* theModule)
  : QDialog(theModule->application()->desktop()),
  myModule(theModule),
  myResult(theModule->shaperToolbars()),
  myIsReset(false)
{
  myFreeCommands = theModule->getFreeCommands();

  setWindowTitle(tr("Toolbars"));
  QVBoxLayout* aMailLayout = new QVBoxLayout(this);

  // Controls part of the dialog
  QWidget* aControlsWgt = new QWidget(this);
  QHBoxLayout* aContolsLay = new QHBoxLayout(aControlsWgt);
  aContolsLay->setContentsMargins(0, 0, 0, 0);
  aMailLayout->addWidget(aControlsWgt);

  // Right controls
  QWidget* aListWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aListLayout = new QVBoxLayout(aListWgt);
  aListLayout->setContentsMargins(0, 0, 0, 0);
  aContolsLay->addWidget(aListWgt);

  aListLayout->addWidget(new QLabel(tr("Toolbars:"), aListWgt));

  myToolbarsList = new QListWidget(aListWgt);
  connect(myToolbarsList, SIGNAL(doubleClicked(const QModelIndex&)),
    SLOT(onDoubleClick(const QModelIndex&)));
  aListLayout->addWidget(myToolbarsList);

  QWidget* aFreeItemsWgt = new QWidget(aListWgt);
  QHBoxLayout* aFreeLayout = new QHBoxLayout(aFreeItemsWgt);
  aFreeLayout->setContentsMargins(0, 0, 0, 0);
  aListLayout->addWidget(aFreeItemsWgt);

  aFreeLayout->addWidget(new QLabel(tr("Number of commands out of toolbars:"), aFreeItemsWgt));
  myFreeNbLbl = new QLabel(aFreeItemsWgt);
  aFreeLayout->addWidget(myFreeNbLbl);

  // Left controls
  QWidget* aButtonsWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aBtnLayout = new QVBoxLayout(aButtonsWgt);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);
  aContolsLay->addWidget(aButtonsWgt);

  QPushButton* aAddBtn = new QPushButton(tr("Add..."), aButtonsWgt);
  aAddBtn->setToolTip(tr("Add a new empty toolbar to the toolbars list"));
  connect(aAddBtn, SIGNAL(clicked(bool)), SLOT(onAdd()));
  aBtnLayout->addWidget(aAddBtn);

  QPushButton* aEditBtn = new QPushButton(tr("Edit..."), aButtonsWgt);
  aEditBtn->setToolTip(tr("Edit currently selected toolbar"));
  connect(aEditBtn, SIGNAL(clicked(bool)), SLOT(onEdit()));
  aBtnLayout->addWidget(aEditBtn);

  QPushButton* aDeleteBtn = new QPushButton(tr("Delete"), aButtonsWgt);
  aDeleteBtn->setToolTip(tr("Delete currently selected toolbar"));
  connect(aDeleteBtn, SIGNAL(clicked(bool)), SLOT(onDelete()));
  aBtnLayout->addWidget(aDeleteBtn);
  aBtnLayout->addStretch(1);

  QPushButton* aResetBtn = new QPushButton(tr("Reset"), aButtonsWgt);
  aResetBtn->setToolTip(tr("Restore default toolbars structure"));
  connect(aResetBtn, SIGNAL(clicked(bool)), SLOT(onReset()));
  aBtnLayout->addWidget(aResetBtn);
  aBtnLayout->addSpacing(19);

  // Buttons part of the dialog
  QDialogButtonBox* aButtons =
    new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal, this);
  aMailLayout->addWidget(aButtons);
  connect(aButtons, SIGNAL(accepted()), SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), SLOT(reject()));
  connect(aButtons, SIGNAL(helpRequested()), SLOT(onHelp()));

  updateToolbarsList();
  updateNumber();
}

void SHAPERGUI_ToolbarsDlg::onAdd()
{
  QString aNewToolbar =
    QInputDialog::getText(this, tr("Create toolbar"), tr("Name of a new toolbar"));
  if (!(aNewToolbar.isNull() || aNewToolbar.isEmpty())) {
    if (!myResult.contains(aNewToolbar)) {
      myResult[aNewToolbar] = QIntList();
      updateToolbarsList();
      myIsReset = false;
    }
    else {
      QString aMsg = tr("A tool bar with name %1 already exists").arg(aNewToolbar);
      QMessageBox::warning(this, tr("Create toolbar"), aMsg);
    }
  }
}

void SHAPERGUI_ToolbarsDlg::onEdit()
{
  QList<QListWidgetItem*> aSelected = myToolbarsList->selectedItems();
  if (aSelected.size() == 1) {
    QString aToolbarName = aSelected.first()->text();
    int aPos = aToolbarName.lastIndexOf(" (");
    aToolbarName = aToolbarName.left(aPos);

    SHAPERGUI_ToolbarItemsDlg aDlg(this, myModule,
      aToolbarName, myFreeCommands, myResult[aToolbarName]);
    if (aDlg.exec() == QDialog::Accepted) {
      myFreeCommands = aDlg.freeItems();
      myResult[aToolbarName] = aDlg.toolbarItems();
      updateNumber();
      updateToolbarsList();
      myIsReset = false;
    }
  }
}

void SHAPERGUI_ToolbarsDlg::onDelete()
{
  QList<QListWidgetItem*> aSelected = myToolbarsList->selectedItems();
  if (aSelected.size() == 1) {
    QString aToolbarName = aSelected.first()->text();
    int aPos = aToolbarName.lastIndexOf(" (");
    aToolbarName = aToolbarName.left(aPos);

    QString aMsg = tr("Toolbar %1 will be deleted. Continue?").arg(aToolbarName);
    if (QMessageBox::question(this, tr("Delete toolbar"), aMsg) == QMessageBox::Yes) {
      myFreeCommands.append(myResult[aToolbarName]);
      // remove separators from free items
      myFreeCommands.removeAll(-1);
      myResult.remove(aToolbarName);
      updateToolbarsList();
      updateNumber();
      myIsReset = false;
    }
  }
}

void SHAPERGUI_ToolbarsDlg::updateToolbarsList()
{
  myToolbarsList->clear();
  QStringList aItems;
  QMap<QString, QIntList>::const_iterator aIt;
  for (aIt = myResult.cbegin(); aIt != myResult.cend(); aIt++) {
    aItems.append(aIt.key() + tr(" (%1 commands)").arg(aIt.value().size() - aIt.value().count(-1)));
  }
  myToolbarsList->addItems(aItems);
}


void SHAPERGUI_ToolbarsDlg::onDoubleClick(const QModelIndex& theIdx)
{
  if (theIdx.isValid())
    onEdit();
}

void SHAPERGUI_ToolbarsDlg::updateNumber()
{
  myFreeNbLbl->setText(QString::number(myFreeCommands.size()));
}

void SHAPERGUI_ToolbarsDlg::onReset()
{
  myResult = myModule->defaultToolbars();
  updateNumber();
  updateToolbarsList();
  myIsReset = true;
}

void SHAPERGUI_ToolbarsDlg::onHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if (app)
    app->onHelpContextModule("SHAPER", "Introduction.html", "toolbars-dialog-box");
}


//************************************************************************************
//************************************************************************************
//************************************************************************************
SHAPERGUI_ToolbarItemsDlg::SHAPERGUI_ToolbarItemsDlg(QWidget* theParent,
  SHAPERGUI* theModule,
  const QString& theToolbar,
  const QIntList& theFreeItems,
  const QIntList& theItemsList)
  : QDialog(theParent),
  myModule(theModule)
{
  setWindowTitle(tr("Edit toolbar"));

  QVBoxLayout* aMailLayout = new QVBoxLayout(this);

  // Name of toolbar
  QWidget* aNameWgt = new QWidget(this);
  QHBoxLayout* aNameLay = new QHBoxLayout(aNameWgt);
  aNameLay->setContentsMargins(0, 0, 0, 0);
  aMailLayout->addWidget(aNameWgt);

  aNameLay->addWidget(new QLabel(tr("Toolbar name:"), aNameWgt));
  QLabel* aNameLbl = new QLabel(theToolbar, aNameWgt);
  QFont aFont = aNameLbl->font();
  aFont.setBold(true);
  aNameLbl->setFont(aFont);
  aNameLay->addWidget(aNameLbl);
  aNameLay->addStretch(1);

  // Lists widget
  QWidget* aControlsWgt = new QWidget(this);
  QHBoxLayout* aCtrlLayout = new QHBoxLayout(aControlsWgt);
  aCtrlLayout->setContentsMargins(0, 0, 0, 0);
  aMailLayout->addWidget(aControlsWgt);

  // Left list
  QWidget* aCommandsWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aCommandsLay = new QVBoxLayout(aCommandsWgt);
  aCommandsLay->setContentsMargins(0, 0, 0, 0);
  aCtrlLayout->addWidget(aCommandsWgt);

  aCommandsLay->addWidget(new QLabel(tr("Out of toolbars:"), aCommandsWgt));
  myCommandsList = new QListWidget(aCommandsWgt);
  myCommandsList->setSortingEnabled(false);

  myCommandsList->addItem(new SHAPERGUI_CommandIdItem(myCommandsList, -1, myModule));
  foreach(int aId, theFreeItems) {
    myCommandsList->addItem(new SHAPERGUI_CommandIdItem(myCommandsList, aId, myModule));
  }
  myCommandsList->setMaximumWidth(LIST_WIDTH);
  aCommandsLay->addWidget(myCommandsList);

  // Middle buttons
  QWidget* aButtonsWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aBtnLayout = new QVBoxLayout(aButtonsWgt);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);
  aCtrlLayout->addWidget(aButtonsWgt);

  aBtnLayout->addStretch(1);
  QPushButton* aAddButton = new QPushButton(QIcon(":pictures/arrow-right.png"), "", aButtonsWgt);
  connect(aAddButton, SIGNAL(clicked(bool)), SLOT(onAddItem()));
  aBtnLayout->addWidget(aAddButton);

  aBtnLayout->addSpacing(20);

  QPushButton* aDelButton = new QPushButton(QIcon(":pictures/arrow-left.png"), "", aButtonsWgt);
  connect(aDelButton, SIGNAL(clicked(bool)), SLOT(onDelItem()));
  aBtnLayout->addWidget(aDelButton);
  aBtnLayout->addStretch(1);

  // Right list
  QWidget* aItemsWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aItemsLay = new QVBoxLayout(aItemsWgt);
  aItemsLay->setContentsMargins(0, 0, 0, 0);
  aCtrlLayout->addWidget(aItemsWgt);

  aItemsLay->addWidget(new QLabel(tr("In the toolbar:"), aItemsWgt));
  myItemsList = new QListWidget(aItemsWgt);
  myItemsList->setSortingEnabled(false);
  foreach(int aId, theItemsList) {
    myItemsList->addItem(new SHAPERGUI_CommandIdItem(myItemsList, aId, myModule));
  }
  myItemsList->setMaximumWidth(LIST_WIDTH);
  myItemsList->viewport()->installEventFilter(this);
  aItemsLay->addWidget(myItemsList);

  // Buttons of right list
  QWidget* aBtnWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aBtnLay = new QVBoxLayout(aBtnWgt);
  aBtnLay->setContentsMargins(0, 0, 0, 0);
  aCtrlLayout->addWidget(aBtnWgt);

  aBtnLay->addStretch(1);
  QPushButton* aUpButton = new QPushButton(QIcon(":pictures/arrow-up.png"), "", aBtnWgt);
  connect(aUpButton, SIGNAL(clicked(bool)), SLOT(onUp()));
  aBtnLay->addWidget(aUpButton);

  aBtnLay->addSpacing(20);

  QPushButton* aDownButton = new QPushButton(QIcon(":pictures/arrow-down.png"), "", aBtnWgt);
  connect(aDownButton, SIGNAL(clicked(bool)), SLOT(onDown()));
  aBtnLay->addWidget(aDownButton);
  aBtnLay->addStretch(1);

  // Buttons part of the dialog
  QDialogButtonBox* aButtons =
    new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal, this);
  aMailLayout->addWidget(aButtons);
  connect(aButtons, SIGNAL(accepted()), SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), SLOT(reject()));
  connect(aButtons, SIGNAL(helpRequested()), SLOT(onHelp()));
}

void SHAPERGUI_ToolbarItemsDlg::onAddItem()
{
  QList<QListWidgetItem*> aCurrentList = myCommandsList->selectedItems();
  if (aCurrentList.size() > 0) {
    SHAPERGUI_CommandIdItem* aItem = dynamic_cast<SHAPERGUI_CommandIdItem*>(aCurrentList.first());
    int aId = aItem->id();
    if (aId != -1) {
      myCommandsList->removeItemWidget(aItem);
      delete aItem;
    }
    QModelIndex aIdx = myItemsList->currentIndex();
    aItem = new SHAPERGUI_CommandIdItem(0, aId, myModule);
    if (aIdx.isValid()) {
      int aRow = aIdx.row();
      myItemsList->insertItem(aRow, aItem);
    }
    else {
      myItemsList->addItem(aItem);
    }
  }
}

void SHAPERGUI_ToolbarItemsDlg::onDelItem()
{
  QList<QListWidgetItem*> aCurrentList = myItemsList->selectedItems();
  if (aCurrentList.size() > 0) {
    SHAPERGUI_CommandIdItem* aItem = dynamic_cast<SHAPERGUI_CommandIdItem*>(aCurrentList.first());
    int aId = aItem->id();
    myItemsList->removeItemWidget(aItem);
    delete aItem;
    if (aId != -1) {
      aItem = new SHAPERGUI_CommandIdItem(myCommandsList, aId, myModule);
      myCommandsList->addItem(aItem);
    }
  }
}


bool SHAPERGUI_ToolbarItemsDlg::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theEvent->type() == QEvent::MouseButtonRelease) {
    QMouseEvent* aMouseEvent = (QMouseEvent*)theEvent;
    QModelIndex aIdx = myItemsList->indexAt(aMouseEvent->pos());
    if (!aIdx.isValid() || aMouseEvent->button() == Qt::RightButton) {
      myItemsList->setCurrentIndex(QModelIndex());
    }
  }
  return QDialog::eventFilter(theObj, theEvent);
}

void SHAPERGUI_ToolbarItemsDlg::onUp()
{
  QModelIndex aCurrentIdx = myItemsList->currentIndex();
  if (aCurrentIdx.isValid()) {
    int aRow = aCurrentIdx.row();
    if (aRow > 0) {
      QListWidgetItem* aItem = myItemsList->takeItem(aRow);
      aRow--;
      myItemsList->insertItem(aRow, aItem);
      myItemsList->setCurrentRow(aRow);
    }
  }
}

void SHAPERGUI_ToolbarItemsDlg::onDown()
{
  QModelIndex aCurrentIdx = myItemsList->currentIndex();
  if (aCurrentIdx.isValid()) {
    int aNb = myItemsList->count();
    int aRow = aCurrentIdx.row();
    if (aRow < (aNb - 1)) {
      QListWidgetItem* aItem = myItemsList->takeItem(aRow);
      aRow++;
      myItemsList->insertItem(aRow, aItem);
      myItemsList->setCurrentRow(aRow);
    }
  }
}

QIntList SHAPERGUI_ToolbarItemsDlg::freeItems() const
{
  return getItems(myCommandsList, 1);
}

QIntList SHAPERGUI_ToolbarItemsDlg::toolbarItems() const
{
  return getItems(myItemsList, 0);
}

QIntList SHAPERGUI_ToolbarItemsDlg::getItems(QListWidget* theWidget, int theStart) const
{
  QIntList aList;
  SHAPERGUI_CommandIdItem* aItem = 0;
  int aNb = theWidget->count();
  for (int i = theStart; i < aNb; i++) {
    aItem = (SHAPERGUI_CommandIdItem*)theWidget->item(i);
    aList.append(aItem->id());
  }
  return aList;
}

void SHAPERGUI_ToolbarItemsDlg::onHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if (app)
    app->onHelpContextModule("SHAPER", "Introduction.html", "edit-toolbar-dialog-box");
}
