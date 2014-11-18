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

#include <QHBoxLayout>
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
  myMainWidget = new QWidget(theParent);
  QHBoxLayout* aMainLay = new QHBoxLayout(myMainWidget);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QString aTitle = QString::fromStdString(theData->widgetLabel());
  QLabel* aTitleLabel = new QLabel(aTitle, myMainWidget);
  aMainLay->addWidget(aTitleLabel);
  myLineEdit = new QLineEdit(myMainWidget);
  aMainLay->addWidget(myLineEdit);
  myLineEdit->setMinimumHeight(20);
  myMainWidget->setLayout(aMainLay);

  connect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged()));
}

ModuleBase_WidgetLineEdit::~ModuleBase_WidgetLineEdit()
{
}

bool ModuleBase_WidgetLineEdit::storeValue() const
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

bool ModuleBase_WidgetLineEdit::restoreValue()
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

QWidget* ModuleBase_WidgetLineEdit::getControl() const
{
  return myMainWidget;
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
