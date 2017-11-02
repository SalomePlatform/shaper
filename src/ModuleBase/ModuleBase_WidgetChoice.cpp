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

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <Config_WidgetAPI.h>
#include <Config_PropManager.h>

#include <QDir>
#include <QFile>
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QTextStream>
#include <QToolButton>

void getValues(const std::string& thePath, const std::string& theFileName,
               QStringList& theValues)
{
  QString aFileName = thePath.c_str();
  aFileName += QDir::separator();
  aFileName += theFileName.c_str();

  QFile aFile(aFileName);
  if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QTextStream aStream(&aFile);
  while (!aStream.atEnd()) {
    QString aLine = aStream.readLine();
    if (!aLine.isEmpty())
      theValues.append(aLine);
  }
}

ModuleBase_WidgetChoice::ModuleBase_WidgetChoice(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myCombo(0), myButtons(0)
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = translate(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  std::string aTypes = theData->getProperty("string_list");
  QStringList aList;

  foreach(QString aType, QString(aTypes.c_str()).split(' ')) {
    aList.append(translate(aType.toStdString()));
  }
  if (aTypes.empty()) {
    aList.clear();
    std::string aFileName = theData->getProperty("file_name");
    if (!aFileName.empty()) {
      std::string aPath = Config_PropManager::string("Plugins", "combo_box_elements_path");
      getValues(aPath, aFileName, aList);
    }
  }

  if (theData->getBooleanAttribute("use_in_title", false))
    myButtonTitles = aList;

  bool aHasDefaultValue;
  int aDefaultVal = QString::fromStdString(getDefaultValue()).toInt(&aHasDefaultValue);
  // Widget type can be combobox or radiobuttons
  std::string aWgtType = theData->getProperty("widget_type");
  if ((aWgtType.length() > 0) && (aWgtType == "radiobuttons")) {
    myButtons = new QButtonGroup(this);
    QGroupBox* aGroupBox = new QGroupBox(aLabelText, this);
    aLayout->addWidget(aGroupBox);


    QLayout* aBtnLayout = 0;
    std::string aWgtDir = theData->getProperty("buttons_dir");
    if (aWgtDir == "horizontal")
      aBtnLayout = new QHBoxLayout(aGroupBox);
    else
      aBtnLayout = new QVBoxLayout(aGroupBox);
    ModuleBase_Tools::adjustMargins(aBtnLayout);

    std::string aIcons = theData->getProperty("icons_list");
    QStringList aIconList = QString(aIcons.c_str()).split(' ');
    if (aIconList.length() == aList.length()) {
      int aId = 0;
      foreach(QString aBtnTxt, aList) {
        QToolButton* aBtn = new QToolButton(aGroupBox);
        aBtn->setFocusPolicy(Qt::StrongFocus);
        aBtn->setCheckable(true);
        aBtn->setToolTip(aBtnTxt);

        QPixmap aIcon = ModuleBase_IconFactory::loadPixmap(aIconList.at(aId));
        aBtn->setIcon(aIcon);
        aBtn->setIconSize(aIcon.size());

        aBtnLayout->addWidget(aBtn);
        myButtons->addButton(aBtn, aId++);
      }

    } else {
      int aId = 0;
      foreach(QString aBtnTxt, aList) {
        QRadioButton* aBtn = new QRadioButton(aBtnTxt, aGroupBox);
        aBtnLayout->addWidget(aBtn);
        myButtons->addButton(aBtn, aId++);
      }
    }
    int aCheckedId = aHasDefaultValue ? aDefaultVal : 0;
    myButtons->button(aDefaultVal)->setChecked(true);
    connect(myButtons, SIGNAL(buttonClicked(int)), this, SLOT(onCurrentIndexChanged(int)));
  } else {
    myLabel = new QLabel(aLabelText, this);
    if (!aLabelIcon.isEmpty())
      myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
    aLayout->addWidget(myLabel);

    std::string aToolstr = theData->widgetTooltip();
    if (!aToolstr.empty()) {
      myLabel->setToolTip(QString::fromStdString(aToolstr));
    }

    myCombo = new QComboBox(this);
    aLayout->addWidget(myCombo, 1);

    myCombo->addItems(aList);

    if (aHasDefaultValue && aDefaultVal < aList.size())
      myCombo->setCurrentIndex(aDefaultVal);

    connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  }
}

ModuleBase_WidgetChoice::~ModuleBase_WidgetChoice()
{
}

bool ModuleBase_WidgetChoice::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aIntAttr = aData->integer(attributeID());

  if (myCombo)
    aIntAttr->setValue(myCombo->currentIndex());
  else
    aIntAttr->setValue(myButtons->checkedId());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetChoice::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aIntAttr = aData->integer(attributeID());

  if (aIntAttr->value() != -1) {
    if (myCombo) {
      bool isBlocked = myCombo->blockSignals(true);
      myCombo->setCurrentIndex(aIntAttr->value());
      myCombo->blockSignals(isBlocked);
    } else {
      bool isBlocked = myButtons->blockSignals(true);
      if (aIntAttr->isInitialized())
        myButtons->button(aIntAttr->value())->setChecked(true);
      else {
        bool aHasDefaultValue;
        int aDefaultVal = QString::fromStdString(getDefaultValue()).toInt(&aHasDefaultValue);
        myButtons->button(aHasDefaultValue ? aDefaultVal : 0)->setChecked(true);
      }
      myButtons->blockSignals(isBlocked);
      emit itemSelected(this, aIntAttr->value());
    }
  }
  return true;
}

bool ModuleBase_WidgetChoice::focusTo()
{
  if (myCombo)
    ModuleBase_Tools::setFocus(myCombo, "ModuleBase_WidgetChoice::focusTo()");
  else
    return false;
  return true;
}

QList<QWidget*> ModuleBase_WidgetChoice::getControls() const
{
  QList<QWidget*> aControls;
  if (myCombo)
    aControls.append(myCombo);
  //else {
  //  //foreach(QAbstractButton* aBtn, myButtons->buttons())
  //  //if (myButtons->checkedId() != -1)
  //  //  aControls.append(myButtons->button(myButtons->checkedId()));
  //}
  return aControls;
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
