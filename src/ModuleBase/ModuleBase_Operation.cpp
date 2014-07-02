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
#include <Model_Events.h>

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
  if(!myFeature){
    #ifdef _DEBUG
    qDebug() << "ModuleBase_Operation::storeCustom: " <<
        "trying to store value without opening a transaction.";
    #endif
    return;
  }

  ModuleBase_ModelWidget* aCustom = dynamic_cast<ModuleBase_ModelWidget*>(sender());
  if (aCustom)
    aCustom->storeValue(myFeature);
}

void ModuleBase_Operation::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
}

void ModuleBase_Operation::startOperation()
{
  if (!myIsEditing)
    setFeature(createFeature());
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
  if (myFeature) myFeature->execute();
}

void ModuleBase_Operation::afterCommitOperation()
{
}

void ModuleBase_Operation::flushUpdated()
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
}

void ModuleBase_Operation::flushCreated()
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_CREATED));
}

FeaturePtr ModuleBase_Operation::createFeature(const bool theFlushMessage)
{
  boost::shared_ptr<ModelAPI_Document> aDoc = document();
  FeaturePtr aFeature = aDoc->addFeature(getDescription()->operationId().toStdString());
  if (aFeature) { // TODO: generate an error if feature was not created
    myIsModified = true;
    aFeature->execute();
    // Init default values
    /*QList<ModuleBase_ModelWidget*> aWidgets = getDescription()->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
    for (; anIt != aLast; anIt++) {
      (*anIt)->storeValue(aFeature);
    }*/
  }

  if (theFlushMessage)
    flushCreated();
  return aFeature;
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
