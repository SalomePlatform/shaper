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
#include <ModelAPI_PluginManager.h>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_IOperation::ModuleBase_IOperation(const QString& theId, QObject* theParent)
 : QObject(theParent), myIsEditing(false), myIsModified(false)
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

bool ModuleBase_IOperation::isGranted(ModuleBase_IOperation* /*theOperation*/) const
{
  return false;
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
  emit resumed();
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
  
  afterCommitOperation();
}

void ModuleBase_IOperation::setRunning(bool theState)
{
  if (!theState) {
    abort();
  }
}
