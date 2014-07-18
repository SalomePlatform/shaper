// File:        ModuleBase_WidgetFeature.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetFeature.h>

#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_WidgetValue.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

ModuleBase_WidgetFeature::ModuleBase_WidgetFeature(QWidget* theParent,
                                                   const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aKinds = QString::fromStdString(theData->getProperty(FEATURE_KEYSEQUENCE));
  myObjectKinds = aKinds.split(" ");

  myContainer = new QWidget(theParent);
  QHBoxLayout* aControlLay = new QHBoxLayout(myContainer);
  aControlLay->setContentsMargins(0, 0, 0, 0);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  myLabel = new QLabel(aLabelText, myContainer);
  aControlLay->addWidget(myLabel);

  myEditor = new QLineEdit(myContainer);
  QString anObjName = QString::fromStdString(attributeID());
  myEditor->setObjectName(anObjName);
  myEditor->setReadOnly(true);
  aControlLay->addWidget(myEditor);

  QString aTTip = QString::fromStdString(theData->widgetTooltip());
  myEditor->setToolTip(aTTip);

  aControlLay->addWidget(myEditor);
  aControlLay->setStretch(1, 1);
}

ModuleBase_WidgetFeature::~ModuleBase_WidgetFeature()
{
}

bool ModuleBase_WidgetFeature::setValue(ModuleBase_WidgetValue* theValue)
{
  bool isDone = false;

  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue = 
                         dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue)
      isDone = setObject(aFeatureValue->object());
  }
  return isDone;
}

bool ModuleBase_WidgetFeature::setObject(const ObjectPtr& theObject)
{
  // TODO
  //if (!myObjectKinds.contains(theObject->getKind().c_str()))
  //  return false;

  myObject = theObject;
  myEditor->setText(theObject ? theObject->data()->name().c_str() : "");
  emit valuesChanged();
  return true;
}

bool ModuleBase_WidgetFeature::storeValue(ObjectPtr theObject) const
{
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature)
    return false;
  boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  ModuleBase_WidgetFeature* that = (ModuleBase_WidgetFeature*) this;
  aRef->setObject(myObject);
  aFeature->execute();
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  return true;
}

bool ModuleBase_WidgetFeature::restoreValue(ObjectPtr theObject)
{
  boost::shared_ptr<ModelAPI_Data> aData = theObject->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aRef->object());
  if (aFeature) {
    myObject = aFeature;
    myEditor->setText(myObject ? myObject->data()->name().c_str() : "");
    return true;
  }
  return false;
}

QWidget* ModuleBase_WidgetFeature::getControl() const
{
  return myContainer;
}

QList<QWidget*> ModuleBase_WidgetFeature::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myLabel);
  aList.append(myEditor);
  return aList;
}
