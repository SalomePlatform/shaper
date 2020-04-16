// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SHAPERGUI_ToolbarsMgr_H
#define SHAPERGUI_ToolbarsMgr_H

#include "SHAPER_SHAPERGUI.h"

#include <Qtx.h>

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QMap>

class QListWidget;
class SHAPERGUI;
class QLabel;

/**
* \ingroup Salome
* A dialog box for editing of toolbar items
*/
class SHAPERGUI_ToolbarItemsDlg : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent for the dialog
  /// \param theModule a module with toolbars
  /// \param theToolbar a name of the toolbar for editing
  /// \param theFreeItems a list of commands out of toolbars
  /// \param theItemsList a list of command in the toolbar
  SHAPERGUI_ToolbarItemsDlg(QWidget* theParent,
    SHAPERGUI* theModule,
    const QString& theToolbar,
    const QIntList& theFreeItems,
    const QIntList& theItemsList);

  /// Returns list of free commands
  QIntList freeItems() const;

  /// Returns list of commands in the toolbar
  QIntList toolbarItems() const;

protected:
  /// An redifinition of a virtual function
  /// \param theObj an object
  /// \param theEvent an event
  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);

private slots:
  /// A slot for button to add an item to toolbar commands
  void onAddItem();

  /// A slot for button to remove an item from toolbar commands
  void onDelItem();

  /// A slot to move a current item up in list of toolbar command
  void onUp();

  /// A slot to move a current item down in list of toolbar command
  void onDown();

  /// A slot on help call
  void onHelp();

private:
  QIntList getItems(QListWidget* theWidget, int theStart) const;

  SHAPERGUI* myModule;

  QListWidget* myCommandsList;
  QListWidget* myItemsList;
};


/**
* \ingroup Salome
* A dialog box for toolbars management
*/
class SHAPERGUI_ToolbarsDlg : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theModule a SHAPER module
  SHAPERGUI_ToolbarsDlg(SHAPERGUI* theModule);

  /// Returns result of editing
  QMap<QString, QIntList> result() const { return myResult; }

  /// Returns a flag of resetted toolbars structure
  bool isReset() const {
    return myIsReset;
  }

private slots:
  /// A slot to add a new toolbar
  void onAdd();

  /// A slot to edit a current toolbar
  void onEdit();

  /// A slot to delete a current toolbar
  void onDelete();

  /// A slot to reset toolbars
  void onReset();

  /// A slot on help call
  void onHelp();

  /// A slot called on double click on item in list
  void onDoubleClick(const QModelIndex& theIdx);

private:
  /// Update content of toolbars list
  void updateToolbarsList();

  /// Update number of free items
  void updateNumber();

private:
  SHAPERGUI* myModule;
  QMap<QString, QIntList> myResult;
  QIntList myFreeCommands;

  QLabel* myFreeNbLbl;
  QListWidget* myToolbarsList;

  bool myIsReset;
};


#endif