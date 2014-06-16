/*
 * ModuleBase_OperationDescription.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include <ModuleBase_OperationDescription.h>
#include <QString>

ModuleBase_OperationDescription::ModuleBase_OperationDescription(const QString& theId)
: myOperationId(theId)
{
}

ModuleBase_OperationDescription::~ModuleBase_OperationDescription()
{
}

const QString& ModuleBase_OperationDescription::operationId() const
{
  return myOperationId;
}

const QString& ModuleBase_OperationDescription::description() const
{
  return myDescription;
}

void ModuleBase_OperationDescription::setDescription(const QString& theDescription)
{
  myDescription = theDescription;
}

void ModuleBase_OperationDescription::setModelWidgets(const std::string& theXmlRepresentation,
                                                      const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  myWidgets.clear();
  QList<ModuleBase_ModelWidget*>::const_iterator anIt = theWidgets.begin(), aLast = theWidgets.end();
  for (; anIt != aLast; anIt++)
    myWidgets.push_back(*anIt);
  myXmlRepresentation = theXmlRepresentation;
}

const QList<ModuleBase_ModelWidget*>& ModuleBase_OperationDescription::modelWidgets() const
{
  return myWidgets;
}

bool ModuleBase_OperationDescription::hasXmlRepresentation() const
{
  return !myWidgets.empty() || !myXmlRepresentation.empty();
}
