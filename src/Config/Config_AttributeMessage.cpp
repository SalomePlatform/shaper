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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "Config_AttributeMessage.h"

Config_AttributeMessage::Config_AttributeMessage(const Events_ID theId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myAttributeId = std::string(); // Attribute unique id
  myFeatureId = std::string(); // Feature unique id
  myIsObligatory = true;
  myIsConcealment = false;
}

Config_AttributeMessage::~Config_AttributeMessage()
{

}

const std::string& Config_AttributeMessage::featureId() const
{
  return myFeatureId;
}

void Config_AttributeMessage::setFeatureId(const std::string& theId)
{
  myFeatureId = theId;
}

const std::string& Config_AttributeMessage::attributeId() const
{
  return myAttributeId;
}

void Config_AttributeMessage::setAttributeId(const std::string& theId)
{
  myAttributeId = theId;
}

bool Config_AttributeMessage::isConcealment() const
{
  return myIsConcealment;
}

void Config_AttributeMessage::setConcealment(bool theConcealment)
{
  this->myIsConcealment = theConcealment;
}

bool Config_AttributeMessage::isObligatory() const
{
  return myIsObligatory;
}

void Config_AttributeMessage::setObligatory(bool theObligatory)
{
  this->myIsObligatory = theObligatory;
}

const std::list<std::pair<std::string, std::string> >& Config_AttributeMessage::getCases() const
{
  return myCases;
}

void Config_AttributeMessage::setCases(const std::list<std::pair<std::string,
                                       std::string> >& theCases)
{
  myCases = theCases;
}

bool Config_AttributeMessage::isMainArgument() const
{
  return myIsMainArgument;
}

void Config_AttributeMessage::setMainArgument(bool isMainArg)
{
  myIsMainArgument = isMainArg;
}
