// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetLineEdit.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetLineEdit.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>

#include <Config_WidgetAPI.h>

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QString>

#include <memory>
#include <string>

ModuleBase_WidgetLineEdit::ModuleBase_WidgetLineEdit(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  QFormLayout* aMainLay = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  QLabel* aLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    aLabel->setPixmap(QPixmap(aLabelIcon));

  myLineEdit = new QLineEdit(this);
  myLineEdit->setMinimumHeight(20);
  aMainLay->addRow(aLabel, myLineEdit);
  this->setLayout(aMainLay);

  connect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged()));
}

ModuleBase_WidgetLineEdit::~ModuleBase_WidgetLineEdit()
{
}

bool ModuleBase_WidgetLineEdit::storeValueCustom() const
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myLineEdit->text();
  aStringAttr->setValue(aWidgetValue.toStdString());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetLineEdit::restoreValueCustom()
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myLineEdit->blockSignals(true);
  myLineEdit->setText(QString::fromStdString(aStringAttr->value()));
  myLineEdit->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetLineEdit::getControls() const
{
  QList<QWidget*> result;
  result << myLineEdit;
  return result;
}

void ModuleBase_WidgetLineEdit::onTextChanged()
{
  storeValue();
}
