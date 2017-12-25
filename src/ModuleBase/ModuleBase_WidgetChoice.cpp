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

#include "ModuleBase_WidgetChoice.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_IconFactory.h"
#include "ModuleBase_ChoiceCtrl.h"

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_Data.h>
#include <Config_WidgetAPI.h>
#include <Config_PropManager.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QToolButton>

ModuleBase_WidgetChoice::ModuleBase_WidgetChoice(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)//, myCombo(0), myButtons(0)
{
  QString aLabelText = translate(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  std::string aTypes = theData->getProperty("string_list");
  QStringList aList;

  foreach(QString aType, QString(aTypes.c_str()).split(' ')) {
    aList.append(translate(aType.toStdString()));
  }
  if (aTypes.empty()) {
    myStringListAttribute = theData->getProperty("string_list_attribute");
    if (!myStringListAttribute.empty())
      aList.clear();
  }
  if (theData->getBooleanAttribute("use_in_title", false))
    myButtonTitles = aList;

  bool aHasDefaultValue;
  int aDefaultVal = QString::fromStdString(getDefaultValue()).toInt(&aHasDefaultValue);

  // Widget type can be combobox or radiobuttons
  std::string aWgtType = theData->getProperty("widget_type");
  std::string aIcons = theData->getProperty("icons_list");
  QStringList aIconList = QString(aIcons.c_str()).split(' ');

  std::string aWgtDir = theData->getProperty("buttons_dir");

  QHBoxLayout* aLayout = new QHBoxLayout(this);
  myChoiceCtrl =  new ModuleBase_ChoiceCtrl(this, aList, aIconList,
    (aWgtType == "radiobuttons")? ModuleBase_ChoiceCtrl::RadioButtons : ModuleBase_ChoiceCtrl::ComboBox,
    (aWgtDir == "horizontal")? Qt::Horizontal : Qt::Vertical);
  myChoiceCtrl->setLabel(aLabelText);

  if (!aLabelIcon.isEmpty())
    myChoiceCtrl->setLabelIcon(aLabelIcon);

  connect(myChoiceCtrl, SIGNAL(valueChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

  int aCheckedId = aHasDefaultValue ? aDefaultVal : 0;
  myChoiceCtrl->setValue(aCheckedId);
  aLayout->addWidget(myChoiceCtrl);
}

ModuleBase_WidgetChoice::~ModuleBase_WidgetChoice()
{
}

bool ModuleBase_WidgetChoice::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aIntAttr = aData->integer(attributeID());

  aIntAttr->setValue(myChoiceCtrl->value());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetChoice::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aIntAttr = aData->integer(attributeID());

  if (aIntAttr->value() != -1) {
    bool isBlocked = myChoiceCtrl->blockSignals(true);
    if (!myStringListAttribute.empty()) {
      AttributeStringArrayPtr aStrAttr = aData->stringArray(myStringListAttribute);
      QStringList aChoiceList;
      if (aStrAttr) {
        for (int i = 0; i < aStrAttr->size(); i++) {
          aChoiceList << aStrAttr->value(i).c_str();
        }
        myChoiceCtrl->setChoiceList(aChoiceList);
      }
    }
    if (aIntAttr->isInitialized())
      myChoiceCtrl->setValue(aIntAttr->value());
    else {
      bool aHasDefaultValue;
      int aDefaultVal = QString::fromStdString(getDefaultValue()).toInt(&aHasDefaultValue);
      myChoiceCtrl->setValue(aHasDefaultValue ? aDefaultVal : 0);
    }
    myChoiceCtrl->blockSignals(isBlocked);
    emit itemSelected(this, aIntAttr->value());
  }
  return true;
}

bool ModuleBase_WidgetChoice::focusTo()
{
  return myChoiceCtrl->focusTo();
}

QList<QWidget*> ModuleBase_WidgetChoice::getControls() const
{
  return myChoiceCtrl->getControls();
}

QString ModuleBase_WidgetChoice::getPropertyPanelTitle(int theIndex)
{
  QString aTitle;
  if (myButtonTitles.length() > theIndex)
    aTitle = myButtonTitles[theIndex];
  return aTitle;
}

void ModuleBase_WidgetChoice::onCurrentIndexChanged(int theIndex)
{
  emit valuesChanged();
  // Don't transfer focus
  // emit focusOutWidget(this);

  emit itemSelected(this, theIndex);
}
