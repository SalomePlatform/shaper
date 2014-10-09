/*
 * ModuleBase_WidgetMultiSelector.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>

#include <Config_WidgetAPI.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QObject>
#include <QString>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>

ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  myMainWidget = new QWidget(theParent);
  QVBoxLayout* aMainLay = new QVBoxLayout(myMainWidget);
  ModuleBase_Tools::adjustMargins(aMainLay);
  QString aTitle = QString::fromStdString(theData->widgetLabel());
  QLabel* aTitleLabel = new QLabel(aTitle, myMainWidget);
  aMainLay->addWidget(aTitleLabel);
  myListControl = new QTextEdit(myMainWidget);
  myListControl->setReadOnly(true);
  aMainLay->addWidget(myListControl);
  myListControl->setMinimumHeight(20);
  myMainWidget->setLayout(aMainLay);

  connect(myListControl, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged()));
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
}

bool ModuleBase_WidgetMultiSelector::storeValue() const
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());
  QString aWidgetValue = myListControl->toPlainText();
  aStringAttr->setValue(aWidgetValue.toStdString());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetMultiSelector::restoreValue()
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(attributeID());

  bool isBlocked = myListControl->blockSignals(true);
  myListControl->setText(QString::fromStdString(aStringAttr->value()));
  myListControl->blockSignals(isBlocked);

  return true;
}

QWidget* ModuleBase_WidgetMultiSelector::getControl() const
{
  return myMainWidget;
}

QList<QWidget*> ModuleBase_WidgetMultiSelector::getControls() const
{
  QList<QWidget*> result;
  result << myListControl;
  return result;
}
