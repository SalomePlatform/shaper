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


class SHAPERGUI_ToolbarItemsDlg : public QDialog
{
  Q_OBJECT
public:
  SHAPERGUI_ToolbarItemsDlg(QWidget* theParent,
    SHAPERGUI* theModule,
    const QString& theToolbar,
    const QIntList& theFreeItems,
    const QIntList& theItemsList);

private slots:
  void onAddItem();
  void onDelItem();

private:
  SHAPERGUI* myModule;
  QIntList myFreeItems;
  QIntList myToolItems;

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
  SHAPERGUI_ToolbarsDlg(SHAPERGUI* theModule,
    const QIntList& theActionsList,
    const QMap<QString, QIntList>& theToolbars);

  QMap<QString, QIntList> result() const { return myToolbars; }

private slots:
  void onAdd();

  void onEdit();

  void onDelete();

private:
  void updateToolbarsList();

  QIntList getFreeCommands() const;

private:
  SHAPERGUI* myModule;
  QIntList myActionsList;
  QMap<QString, QIntList> myToolbars;

  QListWidget* myToolbarsList;
};


#endif