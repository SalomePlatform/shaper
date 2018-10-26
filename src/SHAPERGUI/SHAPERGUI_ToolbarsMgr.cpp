// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SHAPERGUI_ToolbarsMgr.h"
#include "SHAPERGUI.h"

#include <CAM_Application.h>
#include <SUIT_Desktop.h>

#include <QLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>


class SHAPERGUI_CommandIdItem : public QListWidgetItem
{
public:
  SHAPERGUI_CommandIdItem(QListWidget* theParent, int theId, SHAPERGUI* theModule)
    : QListWidgetItem(theParent), myId(theId), myModule(theModule) {}


  virtual QVariant data(int theRole) const {
    if (theRole == Qt::DisplayRole) {
      if (myId == -1)
        return "------";
      QAction* aAction = myModule->action(myId);
      if (aAction)
        return aAction->text();
    }
    return QListWidgetItem::data(theRole);
  }


private:
  SHAPERGUI* myModule;
  int myId;
};


//************************************************************************************
//************************************************************************************
//************************************************************************************
SHAPERGUI_ToolbarsDlg::SHAPERGUI_ToolbarsDlg(SHAPERGUI* theModule,
  const QIntList& theActionsList,
  const QMap<QString, QIntList>& theToolbars)
  : QDialog(theModule->application()->desktop()),
  myModule(theModule),
  myActionsList(theActionsList),
  myToolbars(theToolbars)
{
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
  myToolbarsList->addItems(theToolbars.keys());
  aListLayout->addWidget(myToolbarsList);

  // Left controls
  QWidget* aButtonsWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aBtnLayout = new QVBoxLayout(aButtonsWgt);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);
  aContolsLay->addWidget(aButtonsWgt);

  QPushButton* aAddBtn = new QPushButton(tr("Add..."), aButtonsWgt);
  connect(aAddBtn, SIGNAL(clicked(bool)), SLOT(onAdd()));
  aBtnLayout->addWidget(aAddBtn);

  QPushButton* aEditBtn = new QPushButton(tr("Edit..."), aButtonsWgt);
  connect(aEditBtn, SIGNAL(clicked(bool)), SLOT(onEdit()));
  aBtnLayout->addWidget(aEditBtn);

  QPushButton* aDeleteBtn = new QPushButton(tr("Delete"), aButtonsWgt);
  connect(aDeleteBtn, SIGNAL(clicked(bool)), SLOT(onDelete()));
  aBtnLayout->addWidget(aDeleteBtn);
  aBtnLayout->addStretch(1);

  // Buttons part of the dialog
  QDialogButtonBox* aButtons =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  aMailLayout->addWidget(aButtons);
  connect(aButtons, SIGNAL(accepted()), SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), SLOT(reject()));
}

void SHAPERGUI_ToolbarsDlg::onAdd()
{
  QString aNewToolbar =
    QInputDialog::getText(this, tr("Create toolbar"), tr("Name of a new toolbar"));
  if (!(aNewToolbar.isNull() || aNewToolbar.isEmpty())) {
    if (!myToolbars.contains(aNewToolbar)) {
      myToolbars[aNewToolbar] = QIntList();
      updateToolbarsList();
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
    QIntList aFreeItems = getFreeCommands();
    SHAPERGUI_ToolbarItemsDlg aDlg(this, myModule,
      aToolbarName, aFreeItems, myToolbars[aToolbarName]);
    aDlg.exec();
  }
}

void SHAPERGUI_ToolbarsDlg::onDelete()
{
  QList<QListWidgetItem*> aSelected = myToolbarsList->selectedItems();
  if (aSelected.size() == 1) {
    QString aToolbarName = aSelected.first()->text();
    QString aMsg = tr("Toolbar %1 will be deleted. Continue?").arg(aToolbarName);
    if (QMessageBox::question(this, tr("Delete toolbar"), aMsg) == QMessageBox::Yes) {
      myToolbars.remove(aToolbarName);
      updateToolbarsList();
    }
  }
}

void SHAPERGUI_ToolbarsDlg::updateToolbarsList()
{
  myToolbarsList->clear();
  myToolbarsList->addItems(myToolbars.keys());
}

QIntList SHAPERGUI_ToolbarsDlg::getFreeCommands() const
{
  QIntList aFreeCommands;
  QMap<QString, QIntList>::const_iterator aIt;
  foreach(int aCmd, myActionsList) {
    bool aIsFree = true;
    for (aIt = myToolbars.cbegin(); aIt != myToolbars.cend(); aIt++) {
      if (aIt.value().contains(aCmd)) {
        aIsFree = false;
        break;
      }
    }
    if (aIsFree)
      aFreeCommands.append(aCmd);
  }
  return aFreeCommands;
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
  myModule(theModule),
  myFreeItems(theFreeItems),
  myToolItems(theItemsList)
{
  setWindowTitle(tr("Edit toolbar items"));

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
  foreach(int aId, myFreeItems) {
    myCommandsList->addItem(new SHAPERGUI_CommandIdItem(myCommandsList, aId, myModule));
  }
  myCommandsList->setMaximumWidth(150);
  aCommandsLay->addWidget(myCommandsList);

  // Middle buttons
  QWidget* aButtonsWgt = new QWidget(aControlsWgt);
  QVBoxLayout* aBtnLayout = new QVBoxLayout(aButtonsWgt);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);
  aCtrlLayout->addWidget(aButtonsWgt);

  aBtnLayout->addStretch(1);
  QPushButton* aAddButton = new QPushButton("--->", aButtonsWgt);
  connect(aAddButton, SIGNAL(clicked(bool)), SLOT(onAddItem()));
  aBtnLayout->addWidget(aAddButton);

  aBtnLayout->addSpacing(20);

  QPushButton* aDelButton = new QPushButton("<---", aButtonsWgt);
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
  foreach(int aId, myToolItems) {
    myItemsList->addItem(new SHAPERGUI_CommandIdItem(myItemsList, aId, myModule));
  }
  myItemsList->setMaximumWidth(150);
  aItemsLay->addWidget(myItemsList);

  // Buttons part of the dialog
  QDialogButtonBox* aButtons =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  aMailLayout->addWidget(aButtons);
  connect(aButtons, SIGNAL(accepted()), SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), SLOT(reject()));
}

void SHAPERGUI_ToolbarItemsDlg::onAddItem()
{

}

void SHAPERGUI_ToolbarItemsDlg::onDelItem()
{

}