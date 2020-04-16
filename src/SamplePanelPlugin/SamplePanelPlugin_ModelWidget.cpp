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

#include <SamplePanelPlugin_ModelWidget.h>
#include <SamplePanelPlugin_Feature.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Events.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

SamplePanelPlugin_ModelWidget::SamplePanelPlugin_ModelWidget(QWidget* theParent,
                                                const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myDefaultValue(0)
{
  QVBoxLayout* aLay = new QVBoxLayout(this);
  aLay->setContentsMargins(0, 0, 0, 0);

  myMainWidget = new QWidget(this);
  aLay->addWidget(myMainWidget);

  QGridLayout* aLayout = new QGridLayout(myMainWidget);
  aLayout->addWidget(new QLabel(tr("Values:")), 0, 0);

  myComboBox = new QComboBox(myMainWidget);
  myComboBox->addItem("Value_1");
  myComboBox->addItem("Value_2");
  myComboBox->addItem("Value_3");

  connect(myComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valuesChanged()));
  aLayout->addWidget(myComboBox, 0, 1);
}

QList<QWidget*> SamplePanelPlugin_ModelWidget::getControls() const
{
  QList<QWidget*> aControls;
  // this control will accept focus and will be highlighted in the Property Panel
  aControls.push_back(myComboBox);
  return aControls;
}

bool SamplePanelPlugin_ModelWidget::storeValueCustom()
{
  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  aValueAttribute->setValue(myComboBox->currentIndex());
  updateObject(myFeature);
  return true;
}

bool SamplePanelPlugin_ModelWidget::restoreValueCustom()
{
  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);

  bool isBlocked = myComboBox->blockSignals(true);
  myComboBox->setCurrentIndex(aValueAttribute->value());
  myComboBox->blockSignals(isBlocked);

  return true;
}

