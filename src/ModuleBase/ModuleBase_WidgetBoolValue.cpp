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

#include <ModuleBase_WidgetBoolValue.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Data.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <QWidget>
#include <QLayout>
#include <QCheckBox>

ModuleBase_WidgetBoolValue::ModuleBase_WidgetBoolValue(QWidget* theParent,
                                                       const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aText = translate(theData->widgetLabel());
  QString aToolTip = translate(theData->widgetTooltip());
  bool isChecked = theData->getBooleanAttribute(ATTR_DEFAULT, false);

  myCheckBox = new QCheckBox(aText, this);
  myCheckBox->setToolTip(aToolTip);
  myCheckBox->setChecked(isChecked);

  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);
  aMainLayout->addWidget(myCheckBox);
  setLayout(aMainLayout);

  connect(myCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetBoolValue::~ModuleBase_WidgetBoolValue()
{
}

bool ModuleBase_WidgetBoolValue::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeBoolean> aBool = aData->boolean(attributeID());
  aBool->setValue(myCheckBox->isChecked());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetBoolValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeBoolean> aRef = aData->boolean(attributeID());

  bool isBlocked = myCheckBox->blockSignals(true);
  myCheckBox->setChecked(aRef->value());
  myCheckBox->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetBoolValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myCheckBox);
  return aList;
}

void ModuleBase_WidgetBoolValue::setHighlighted(bool)
{
  return;
}
