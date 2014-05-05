/*
 * ModuleBase_IOperation.cpp
 *
 *  Created on: May 5, 2014
 *      Author: nds
 */

#include "ModuleBase_IOperation.h"
#include "ModuleBase_OperationDescription.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_PluginManager.h>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_IOperation::ModuleBase_IOperation(const QString& theId, QObject* theParent)
 : QObject(theParent)
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

bool ModuleBase_IOperation::isGranted() const
{
  return false;
}

boost::shared_ptr<ModelAPI_Document> ModuleBase_IOperation::document() const
{
  return ModelAPI_PluginManager::get()->rootDocument();
}

void ModuleBase_IOperation::start()
{
  document()->startOperation();

  startOperation();
  emit started();
}

void ModuleBase_IOperation::resume()
{
}

void ModuleBase_IOperation::abort()
{
  abortOperation();
  emit aborted();

  stopOperation();

  document()->abortOperation();
  emit stopped();
}

void ModuleBase_IOperation::commit()
{
  commitOperation();
  emit committed();

  stopOperation();

  document()->finishOperation();
  emit stopped();
}

void ModuleBase_IOperation::setRunning(bool theState)
{
  if (!theState) {
    abort();
  }
}
