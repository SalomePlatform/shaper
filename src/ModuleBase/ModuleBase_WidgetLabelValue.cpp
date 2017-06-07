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

#include "ModuleBase_WidgetLabelValue.h"

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>
#include <ModuleBase_LabelValue.h>

#include <ModelAPI_AttributeDouble.h>

#include <QLabel>
#include <QVBoxLayout>

ModuleBase_WidgetLabelValue::ModuleBase_WidgetLabelValue(QWidget* theParent,
                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(0);

  QString aText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  QString aToolTip = QString::fromStdString(theData->widgetTooltip());

  myLabel = new ModuleBase_LabelValue(theParent, aText, aToolTip, aLabelIcon);
  bool isOk;
  double aDefVal = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
  if (isOk) {
    myLabel->setValue(aDefVal);
  }

  aLayout->addWidget(myLabel);
}

ModuleBase_WidgetLabelValue::~ModuleBase_WidgetLabelValue()
{
}

QList<QWidget*> ModuleBase_WidgetLabelValue::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  return aControls;
}

bool ModuleBase_WidgetLabelValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr anAttribute = aData->real(attributeID());
  double aValue = 0;
  if (anAttribute.get() && anAttribute->isInitialized())
    aValue = anAttribute->value();
  myLabel->setValue(aValue);
  return true;
}

bool ModuleBase_WidgetLabelValue::storeValueCustom()
{
  return true;
}
