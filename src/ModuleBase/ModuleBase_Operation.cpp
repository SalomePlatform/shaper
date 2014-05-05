/*
 * ModuleBase_Operation.cpp
 *
 *  Created on: May 5, 2014
 *      Author: nds
 */

#include "ModuleBase_Operation.h"

#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_WidgetCustom.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

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

boost::shared_ptr<ModelAPI_Feature> ModuleBase_Operation::feature() const
{
  return myFeature;
}

void ModuleBase_Operation::storeReal(double theValue)
{
  if(!myFeature){
    #ifdef _DEBUG
    qDebug() << "ModuleBase_Operation::storeReal: " <<
        "trying to store value without opening a transaction.";
    #endif
    return;
  }
  QString anId = sender()->objectName();
  boost::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  boost::shared_ptr<ModelAPI_AttributeDouble> aReal = aData->real(anId.toStdString());
  aReal->setValue(theValue);
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

  ModuleBase_WidgetCustom* aCustom = dynamic_cast<ModuleBase_WidgetCustom*>(sender());
  if (aCustom)
    aCustom->store(myFeature);
}

void ModuleBase_Operation::startOperation()
{
  boost::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_PluginManager::get()->rootDocument();
  myFeature = aDoc->addFeature(getDescription()->operationId().toStdString());
  if (myFeature) // TODO: generate an error if feature was not created
    myFeature->execute();
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
