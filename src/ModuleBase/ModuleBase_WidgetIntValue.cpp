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

#include <ModuleBase_WidgetIntValue.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_IconFactory.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeString.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QEvent>
#include <QTimer>

#include <math.h>

#ifndef INT_MAX
#define INT_MAX   2147483647
#endif

#ifdef _DEBUG
#include <iostream>
#endif

ModuleBase_WidgetIntValue::ModuleBase_WidgetIntValue(QWidget* theParent,
                                                     const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QFormLayout* aControlLay = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aControlLay);

  QString aLabelText = translate(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(aLabelIcon));

  mySpinBox = new ModuleBase_ParamSpinBox(this);
  mySpinBox->setDecimals(0);
  QString anObjName = QString::fromStdString(attributeID());
  mySpinBox->setObjectName(anObjName);

  bool isOk = false;
  std::string aProp = theData->getProperty(DOUBLE_WDG_MIN);
  int aMinVal = QString::fromStdString(aProp).toInt(&isOk);
  if (isOk) {
    mySpinBox->setMinimum(aMinVal);
  } else {
    mySpinBox->setMinimum(-INT_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_MAX);
  int aMaxVal = QString::fromStdString(aProp).toInt(&isOk);
  if (isOk) {
    mySpinBox->setMaximum(aMaxVal);
  } else {
    mySpinBox->setMaximum(INT_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_STEP);
  int aStepVal = QString::fromStdString(aProp).toInt(&isOk);
  if (isOk) {
    mySpinBox->setSingleStep(aStepVal);
  }

  int aDefVal = QString::fromStdString(getDefaultValue()).toInt(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  }

  QString aTTip = translate(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);
  myLabel->setToolTip(aTTip);

  aControlLay->addRow(myLabel, mySpinBox);
  connect(mySpinBox, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
}

ModuleBase_WidgetIntValue::~ModuleBase_WidgetIntValue()
{
}

void ModuleBase_WidgetIntValue::activateCustom()
{
  ModuleBase_ModelWidget::activateCustom();
  QStringList aParameters;
  ModuleBase_Tools::getParameters(aParameters);
  mySpinBox->setCompletionList(aParameters);
}

bool ModuleBase_WidgetIntValue::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || mySpinBox->hasVariable()) {
    aDone = false;
  } else {
    bool isOk;
    int aDefValue = QString::fromStdString(getDefaultValue()).toInt(&isOk);
    // reset the value just if there is a default value definition in the XML definition
    // if the value can not be found by the default value, do nothing
    if (isOk) {
      ModuleBase_Tools::setSpinValue(mySpinBox, aDefValue);
      storeValue();
      aDone = true;
    }
  }
  return aDone;
}

bool ModuleBase_WidgetIntValue::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeIntegerPtr anAttribute = aData->integer(attributeID());
  if (mySpinBox->hasVariable()) {
    // Here is a text of a real value or an expression.
    QString aText = mySpinBox->text();
    if (aText.contains('=')) {
      if (!myParameter.get()) {
        myParameter = ModuleBase_Tools::createParameter(aText);
        if (!myParameter.get()) {
          anAttribute->setExpressionError("Parameter cannot be created");
          anAttribute->setExpressionInvalid(true);
          updateObject(myFeature);
          return false;
        } else if (anAttribute->expressionInvalid()) {
          anAttribute->setExpressionError("");
          anAttribute->setExpressionInvalid(false);
        }
      } else {
        ModuleBase_Tools::editParameter(myParameter, aText);
      }
      aText = aText.split('=').at(0);
    } else if (myParameter.get()) {
      // Nullyfy the parameter reference without deletion of the created
      myParameter = FeaturePtr();
    }
    anAttribute->setText(aText.toStdString());
  } else {
    // it is important to set the empty text value to the attribute before set the value
    // because setValue tries to calculate the attribute value according to the
    // attribute current text
    if (anAttribute->expressionInvalid()) {
      anAttribute->setExpressionError("");
      anAttribute->setExpressionInvalid(false);
    }
    anAttribute->setText("");
    anAttribute->setValue(mySpinBox->value());
  }
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetIntValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeIntegerPtr anAttribute = aData->integer(attributeID());
  std::string aTextRepr = anAttribute->text();
  if (!aTextRepr.empty()) {
    QString aText = QString::fromStdString(aTextRepr);
    //if (aText.endsWith('=')) {
    //  if (!myParameter.get()) {
    //    QString aName = aText.left(aText.indexOf('=')).trimmed();
    //    myParameter = ModuleBase_Tools::findParameter(aName);
    //  }
    //  /// If myParameter is empty then it was not created because of an error
    //  if (!myParameter.get())
    //    return false;

    //  AttributeStringPtr aExprAttr = myParameter->string("expression");
    //  aText += aExprAttr->value().c_str();
    //}
    ModuleBase_Tools::setSpinText(mySpinBox, aText);
  } else {
    ModuleBase_Tools::setSpinValue(mySpinBox,
      anAttribute->isInitialized() ? anAttribute->value() : 0);
    if (anAttribute->isInitialized() && anAttribute->expressionInvalid()) {
      anAttribute->setExpressionError("");
      anAttribute->setExpressionInvalid(false);
    }
  }
  return true;
}

void ModuleBase_WidgetIntValue::selectContent()
{
  mySpinBox->selectAll();
}

QList<QWidget*> ModuleBase_WidgetIntValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(mySpinBox);
  return aList;
}

bool ModuleBase_WidgetIntValue::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    mySpinBox->selectAll();
  }
  return isModified;
}
