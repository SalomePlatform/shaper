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

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_WidgetDoubleValue.h>
#include <ModuleBase_IconFactory.h>

#include <QFormLayout>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QString>

#include <cfloat>

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif
#ifdef _DEBUG
#include <iostream>
#endif

//#define DEBUG_COMPLETE_WITH_PARAMETERS

ModuleBase_WidgetDoubleValue::ModuleBase_WidgetDoubleValue(QWidget* theParent,
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

  bool aAcceptVariables = theData->getBooleanAttribute(DOUBLE_WDG_ACCEPT_EXPRESSIONS, true);

  mySpinBox = new ModuleBase_ParamSpinBox(this);
  mySpinBox->setAcceptVariables(aAcceptVariables);
  QString anObjName = QString::fromStdString(attributeID());
  mySpinBox->setObjectName(anObjName);

  bool isOk = false;
  std::string aProp = theData->getProperty(DOUBLE_WDG_MIN);
  double aMinVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setMinimum(aMinVal);
  } else {
    mySpinBox->setMinimum(-DBL_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_MAX);
  double aMaxVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setMaximum(aMaxVal);
  } else {
    mySpinBox->setMaximum(DBL_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_STEP);
  double aStepVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    double aMinStep = pow(10, -1. * (double) mySpinBox->decimals());
    if(aStepVal < aMinStep){
      aStepVal = aMinStep;
    }
    mySpinBox->setSingleStep(aStepVal);
  }

  double aDefVal = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  }

  QString aTTip = translate(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);
  myLabel->setToolTip(aTTip);

  aControlLay->addRow(myLabel, mySpinBox);
  // we should listen textChanged signal as valueChanged do not send when text is modified
  connect(mySpinBox, SIGNAL(textChanged(const QString&)), this, SIGNAL(valuesModified()));
  mySpinBox->setValueEnabled(isValueEnabled());
}

ModuleBase_WidgetDoubleValue::~ModuleBase_WidgetDoubleValue()
{
}

void ModuleBase_WidgetDoubleValue::activateCustom()
{
  ModuleBase_ModelWidget::activateCustom();
#ifdef DEBUG_COMPLETE_WITH_PARAMETERS
  QStringList aParameters;
  ModuleBase_Tools::getParameters(aParameters);
  mySpinBox->setCompletionList(aParameters);
#endif
}

bool ModuleBase_WidgetDoubleValue::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || mySpinBox->hasVariable()) {
    aDone = false;
  } else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
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

bool ModuleBase_WidgetDoubleValue::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (mySpinBox->hasVariable()) {
    // Here is a text of a real value or an expression.
    std::string aText = mySpinBox->text().toStdString();
    aReal->setText(aText);
  } else {
    // it is important to set the empty text value to the attribute before set the value
    // because setValue tries to calculate the attribute value according to the
    // attribute current text
    aReal->setText("");
    aReal->setValue(mySpinBox->value());
  }
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetDoubleValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aRef = aData->real(attributeID());
  std::string aTextRepr = aRef->text();
  if (!aTextRepr.empty()) {
    ModuleBase_Tools::setSpinText(mySpinBox, QString::fromStdString(aTextRepr));
  } else {
    ModuleBase_Tools::setSpinValue(mySpinBox, aRef->isInitialized() ? aRef->value() : 0);
  }
  return true;
}

void ModuleBase_WidgetDoubleValue::selectContent()
{
  mySpinBox->selectAll();
}

QList<QWidget*> ModuleBase_WidgetDoubleValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(mySpinBox);
  return aList;
}

bool ModuleBase_WidgetDoubleValue::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    mySpinBox->selectAll();
  }
  return isModified;
}
