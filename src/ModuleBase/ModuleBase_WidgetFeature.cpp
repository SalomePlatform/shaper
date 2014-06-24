// File:        ModuleBase_WidgetFeature.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetFeature.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <Model_Events.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <QWidget>

ModuleBase_WidgetFeature::ModuleBase_WidgetFeature(QWidget* theParent,
                                                   const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aKinds = QString::fromStdString(theData->getProperty(FEATURE_KEYSEQUENCE));
  myFeatureKinds = aKinds.split(" ");
}

ModuleBase_WidgetFeature::~ModuleBase_WidgetFeature()
{
}

bool ModuleBase_WidgetFeature::setFeature(const FeaturePtr& theFeature)
{
  if (!theFeature && myFeatureKinds.contains(theFeature->getKind().c_str()))
    return false;

  //bool isBlocked = this->blockSignals(true);
  myFeature = theFeature;
  //this->blockSignals(isBlocked);
  emit valuesChanged();
  return true;
}

bool ModuleBase_WidgetFeature::storeValue(FeaturePtr theFeature) const
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  ModuleBase_WidgetFeature* that = (ModuleBase_WidgetFeature*) this;
  //bool isBlocked = that->blockSignals(true);
  aRef->setFeature(myFeature);
  theFeature->execute();
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  //that->blockSignals(isBlocked);

  return true;
}

bool ModuleBase_WidgetFeature::restoreValue(FeaturePtr theFeature)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(attributeID()));

  //bool isBlocked = this->blockSignals(true);
  myFeature = aRef->feature();
  //this->blockSignals(isBlocked);
  return true;
}

QWidget* ModuleBase_WidgetFeature::getControl() const
{
  return 0;
}

QList<QWidget*> ModuleBase_WidgetFeature::getControls() const
{
  QList<QWidget*> aControls;
  return aControls;
}
