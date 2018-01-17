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

#include "ModuleBase_WidgetNameEdit.h"

#include <ModelAPI_Result.h>

#include <QLineEdit>

ModuleBase_WidgetNameEdit::ModuleBase_WidgetNameEdit(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& thePlaceHolder )
 : ModuleBase_WidgetLineEdit(theParent, theData, thePlaceHolder)
{
  // Disconnect the signal in order to avoid Apply button activation automatically
  // it will activate apply because of value isApplyEnabledByActiveWidget in
  // XGUI_ErrorMgr::updateActions
  disconnect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
}


bool ModuleBase_WidgetNameEdit::storeValueCustom()
{
  if(!myFeature)
    return false;

  QString aValue = myLineEdit->text();
  std::string aName = aValue.toStdString();
  myFeature->data()->setName(aName);
  ResultPtr aRes = myFeature->firstResult();
  if (aRes.get())
    aRes->data()->setName(aName);
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetNameEdit::restoreValueCustom()
{
  if(!myFeature)
    return false;

  bool isBlocked = myLineEdit->blockSignals(true);
  myLineEdit->setText(QString::fromStdString(myFeature->data()->name()));
  myLineEdit->blockSignals(isBlocked);

  return true;
}