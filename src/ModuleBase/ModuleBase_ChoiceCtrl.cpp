// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "ModuleBase_ChoiceCtrl.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_IconFactory.h"

#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QToolButton>

ModuleBase_ChoiceCtrl::ModuleBase_ChoiceCtrl(QWidget* theParent,
                                             const QStringList& theChoiceList,
                                             const QStringList& theIconsList,
                                             ControlType theType,
                                             Qt::Orientation theButtonsDir)
 : QWidget(theParent), myType(theType)
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aLayout);

  switch (myType) {
  case RadioButtons:
    {
      myButtons = new QButtonGroup(this);
      myGroupBox = new QGroupBox("", this);
      aLayout->addWidget(myGroupBox);

      QLayout* aBtnLayout = 0;
      switch (theButtonsDir) {
      case Qt::Horizontal:
        aBtnLayout = new QHBoxLayout(myGroupBox);
        break;
      case Qt::Vertical:
        aBtnLayout = new QVBoxLayout(myGroupBox);
        break;
      }
      ModuleBase_Tools::adjustMargins(aBtnLayout);

      if (theIconsList.length() == theChoiceList.length()) {
        int aId = 0;
        foreach(QString aBtnTxt, theChoiceList) {
          QToolButton* aBtn = new QToolButton(myGroupBox);
          aBtn->setFocusPolicy(Qt::StrongFocus);
          aBtn->setCheckable(true);
          aBtn->setToolTip(aBtnTxt);

          QPixmap aIcon = ModuleBase_IconFactory::loadPixmap(theIconsList.at(aId));
          aBtn->setIcon(aIcon);
          aBtn->setIconSize(aIcon.size());

          aBtnLayout->addWidget(aBtn);
          myButtons->addButton(aBtn, aId++);
        }
      } else {
        int aId = 0;
        foreach(QString aBtnTxt, theChoiceList) {
          QRadioButton* aBtn = new QRadioButton(aBtnTxt, myGroupBox);
          aBtnLayout->addWidget(aBtn);
          myButtons->addButton(aBtn, aId++);
        }
      }
      connect(myButtons, SIGNAL(buttonClicked(int)), this, SIGNAL(valueChanged(int)));
    }
    break;
  case ComboBox:
    myLabel = new QLabel("", this);
    aLayout->addWidget(myLabel);

    //std::string aToolstr = theData->widgetTooltip();
    //if (!aToolstr.empty()) {
    //  myLabel->setToolTip(QString::fromStdString(aToolstr));
    //}

    myCombo = new QComboBox(this);
    aLayout->addWidget(myCombo, 1);

    myCombo->addItems(theChoiceList);
    connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valueChanged(int)));
    break;
  }
}

void ModuleBase_ChoiceCtrl::setLabel(const QString& theText)
{
  switch (myType) {
  case RadioButtons:
    myGroupBox->setTitle(theText);
    break;
  case ComboBox:
    myLabel->setText(theText);
    break;
  }
}

void ModuleBase_ChoiceCtrl::setLabelIcon(const QString& theIcon)
{
  if (myType == ComboBox)
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(theIcon));
}

void ModuleBase_ChoiceCtrl::setValue(int theVal)
{
  switch (myType) {
  case RadioButtons:
    myButtons->button(theVal)->setChecked(true);
    break;
  case ComboBox:
    myCombo->setCurrentIndex(theVal);
    break;
  }
}

void ModuleBase_ChoiceCtrl::setValue(const QString& theVal)
{
  switch (myType) {
  case RadioButtons:
    foreach (QAbstractButton* aBtn, myButtons->buttons()) {
      aBtn->setChecked(aBtn->toolTip() == theVal);
    }
    break;
  case ComboBox:
    myCombo->setCurrentText(theVal);
    break;
  }
}


void ModuleBase_ChoiceCtrl::setTooltip(QString theTip)
{
  if (myType == ComboBox)
    myLabel->setToolTip(theTip);
}

int ModuleBase_ChoiceCtrl::value() const
{
  switch (myType) {
  case RadioButtons:
    return myButtons->checkedId();
  case ComboBox:
    return myCombo->currentIndex();
  }
  return -1;
}

bool ModuleBase_ChoiceCtrl::focusTo()
{
  if (myType == ComboBox)
    ModuleBase_Tools::setFocus(myCombo, "ModuleBase_WidgetChoice::focusTo()");
  else
    return false;
  return true;
}

QList<QWidget*> ModuleBase_ChoiceCtrl::getControls() const
{
  QList<QWidget*> aControls;
  if (myType == ComboBox)
    aControls.append(myCombo);
  return aControls;
}

void ModuleBase_ChoiceCtrl::setChoiceList(const QStringList& theChoiceList)
{
  if (myType == ComboBox) {
    myCombo->clear();
    myCombo->addItems(theChoiceList);
  }
}

QString ModuleBase_ChoiceCtrl::textValue() const
{
  switch (myType) {
  case RadioButtons:
    return myButtons->checkedButton()->toolTip();
  case ComboBox:
    return myCombo->currentText();
  }
  return "";
}
