// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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

/*void ModuleBase_OperationDescription::setModelWidgets(const std::string& theXmlRepresentation,
 const QList<ModuleBase_ModelWidget*>& theWidgets)
 {
 myWidgets.clear();
 QList<ModuleBase_ModelWidget*>::const_iterator anIt = theWidgets.begin(), aLast = theWidgets.end();
 for (; anIt != aLast; anIt++)
 myWidgets.push_back(*anIt);
 myXmlRepresentation = theXmlRepresentation;
 }*/

/*const QList<ModuleBase_ModelWidget*>& ModuleBase_OperationDescription::modelWidgets() const
 {
 return myWidgets;
 }*/

const QString& ModuleBase_OperationDescription::xmlRepresentation() const
{
  return myXmlRepresentation;
}

void ModuleBase_OperationDescription::setXmlRepresentation(const QString& theXMLRepresentation)
{
  myXmlRepresentation = theXMLRepresentation;
}

bool ModuleBase_OperationDescription::hasXmlRepresentation() const
{
  return /*!myWidgets.empty() || */!myXmlRepresentation.isEmpty();
}
