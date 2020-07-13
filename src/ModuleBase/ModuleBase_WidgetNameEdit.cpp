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

#include "ModuleBase_WidgetNameEdit.h"

#include <ModelAPI_Result.h>

#include <QLineEdit>


bool ModuleBase_WidgetNameEdit::storeValueCustom()
{
  if(!myFeature)
    return false;

  QString aValue = myLineEdit->text();
  std::wstring aName = aValue.toStdWString();
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
  myLineEdit->setText(QString::fromStdWString(myFeature->data()->name()));
  myLineEdit->blockSignals(isBlocked);

  return true;
}