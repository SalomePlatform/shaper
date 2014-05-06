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

const QString& ModuleBase_OperationDescription::xmlRepresentation() const
{
  return myXmlRepr;
}

void ModuleBase_OperationDescription::setXmlRepresentation(const QString& xmlRepr)
{
  myXmlRepr = xmlRepr;
}

const QString& ModuleBase_OperationDescription::description() const
{
  return myDescription;
}

void ModuleBase_OperationDescription::setDescription(const QString& theDescription)
{
  myDescription = theDescription;
}

