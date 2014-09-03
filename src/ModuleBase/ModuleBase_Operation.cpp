/*
 * ModuleBase_Operation.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include "ModuleBase_Operation.h"

#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Validator.h>

#include <Events_Loop.h>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_Operation::ModuleBase_Operation(const QString& theId, QObject* theParent)
    : ModuleBase_IOperation(theId, theParent)
{
}

ModuleBase_Operation::~ModuleBase_Operation()
{
}

QString ModuleBase_Operation::id() const
{
  return getDescription()->operationId();
}

FeaturePtr ModuleBase_Operation::feature() const
{
  return myFeature;
}

bool ModuleBase_Operation::isNestedOperationsEnabled() const
{
  return true;
}

void ModuleBase_Operation::storeCustomValue()
{
  if (!myFeature) {
#ifdef _DEBUG
    qDebug() << "ModuleBase_Operation::storeCustom: " <<
    "trying to store value without opening a transaction.";
#endif
    return;
  }

  ModuleBase_ModelWidget* aCustom = dynamic_cast<ModuleBase_ModelWidget*>(sender());
  if (aCustom)
    aCustom->storeValue();
}

void ModuleBase_Operation::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
}

void ModuleBase_Operation::startOperation()
{
  if (!myIsEditing)
    createFeature();
  //emit callSlot();
  //commit();
}

void ModuleBase_Operation::stopOperation()
{
}

void ModuleBase_Operation::abortOperation()
{
}

void ModuleBase_Operation::commitOperation()
{
}

void ModuleBase_Operation::afterCommitOperation()
{
}

bool ModuleBase_Operation::canBeCommitted() const
{
  if (ModuleBase_IOperation::canBeCommitted()) {
    /*    FeaturePtr aFeature = feature();
     std::string aId = aFeature->getKind();

     PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
     ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
     std::list<ModelAPI_Validator*> aValidators;
     aFactory->validators(aId, aValidators);
     std::list<ModelAPI_Validator*>::const_iterator aIt;
     for (aIt = aValidators.cbegin(); aIt != aValidators.cend(); ++aIt) {
     const ModuleBase_FeatureValidator* aFValidator = 
     dynamic_cast<const ModuleBase_FeatureValidator*>(*aIt);
     if (aFValidator) {
     if (!aFValidator->isValid(aFeature))
     return false;
     }
     }*/
    return true;
  }
  return false;
}

void ModuleBase_Operation::flushUpdated()
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

void ModuleBase_Operation::flushCreated()
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
}

FeaturePtr ModuleBase_Operation::createFeature(const bool theFlushMessage)
{
  boost::shared_ptr<ModelAPI_Document> aDoc = document();
  myFeature = aDoc->addFeature(getDescription()->operationId().toStdString());
  if (myFeature) {  // TODO: generate an error if feature was not created
    myIsModified = true;
    // Model update should call "execute" of a feature.
    //myFeature->execute();
    // Init default values
    /*QList<ModuleBase_ModelWidget*> aWidgets = getDescription()->modelWidgets();
     QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
     for (; anIt != aLast; anIt++) {
     (*anIt)->storeValue(aFeature);
     }*/
  }

  if (theFlushMessage)
    flushCreated();
  return myFeature;
}

void ModuleBase_Operation::setFeature(FeaturePtr theFeature)
{
  myFeature = theFeature;
}

void ModuleBase_Operation::setEditingFeature(FeaturePtr theFeature)
{
  setFeature(theFeature);
  myIsEditing = true;
}

bool ModuleBase_Operation::hasObject(ObjectPtr theObj) const
{
  FeaturePtr aFeature = feature();
  if (aFeature) {
    if (aFeature == theObj)
      return true;
    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
      if ((*aIt) == theObj)
        return true;
    }
  }
  return false;
}
