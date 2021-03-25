// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <SamplePanelPlugin_Panel.h>
#include <SamplePanelPlugin_Feature.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

SamplePanelPlugin_Panel::SamplePanelPlugin_Panel(QWidget* theParent)
: QWidget(theParent), myDefaultValue(0)
{
  QGridLayout* aLayout = new QGridLayout(this);
  aLayout->addWidget(new QLabel(tr("Values:")), 0, 0);

  myComboBox = new QComboBox(this);
  myComboBox->addItem("Value_1");
  myComboBox->addItem("Value_2");
  myComboBox->addItem("Value_3");

  connect(myComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  aLayout->addWidget(myComboBox, 0, 1);
}

void SamplePanelPlugin_Panel::setFeature(const FeaturePtr& theFeature)
{
  myFeature = theFeature;

  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  /// the attribute should be filled with some value, because Apply button of the Property
  /// panel is enabled only if all attributes of the feature are initialized
  /// It is possible to make it in the initializeAttribute method of the feature after
  /// attribute creation
  if (!aValueAttribute->isInitialized()) {
    aValueAttribute->setValue(myDefaultValue);
    /// to update error state of the feature: previous value was that this attribute is not
    /// initialized yet. Apply of Property panel was disabled.
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }

  int aValueId = aValueAttribute->value();
  myComboBox->setCurrentIndex(aValueId);
}

void SamplePanelPlugin_Panel::onCurrentIndexChanged(int theIndex)
{
  if (!myFeature.get())
    return;

  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  aValueAttribute->setValue(theIndex);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}
