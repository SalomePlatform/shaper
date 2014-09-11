/*
 * ModuleBase_IOperation.cpp
 *
 *  Created on: May 5, 2014
 *      Author: nds
 */

#include "ModuleBase_IOperation.h"
#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_IOperation::ModuleBase_IOperation(const QString& theId, QObject* theParent)
    : QObject(theParent),
      myIsEditing(false),
      myIsModified(false)
{
  myDescription = new ModuleBase_OperationDescription(theId);
}

ModuleBase_IOperation::~ModuleBase_IOperation()
{
  delete myDescription;
}

ModuleBase_OperationDescription* ModuleBase_IOperation::getDescription() const
{
  return myDescription;
}

bool ModuleBase_IOperation::canBeCommitted() const
{
  return true;
}

/*void ModuleBase_IOperation::setModelWidgets(const std::string& theXmlRepresentation,
 QList<ModuleBase_ModelWidget*> theWidgets)
 {
 QList<ModuleBase_ModelWidget*>::const_iterator anIt = theWidgets.begin(), aLast = theWidgets.end();
 for (; anIt != aLast; anIt++) {
 QObject::connect(*anIt, SIGNAL(valuesChanged()),  this, SLOT(storeCustomValue()));
 }
 getDescription()->setModelWidgets(theXmlRepresentation, theWidgets);
 }*/

boost::shared_ptr<ModelAPI_Document> ModuleBase_IOperation::document() const
{
  return ModelAPI_Session::get()->moduleDocument();
}

void ModuleBase_IOperation::start()
{
  ModelAPI_Session::get()->startOperation();

  startOperation();
  emit started();
}

void ModuleBase_IOperation::resume()
{
  emit resumed();
}

void ModuleBase_IOperation::abort()
{
  abortOperation();
  emit aborted();

  stopOperation();

  ModelAPI_Session::get()->abortOperation();
  emit stopped();
}

bool ModuleBase_IOperation::commit()
{
  if (canBeCommitted()) {
    commitOperation();
    emit committed();

    stopOperation();

    ModelAPI_Session::get()->finishOperation();
    emit stopped();

    afterCommitOperation();
    return true;
  }
  return false;
}

void ModuleBase_IOperation::setRunning(bool theState)
{
  if (!theState) {
    abort();
  }
}
