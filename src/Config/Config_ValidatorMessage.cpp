// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Config_ValidatorMessage.h>

Config_ValidatorMessage::Config_ValidatorMessage(const Events_ID theId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myValidatorId = "";
  myFeatureId = "";
  myAttributeId = "";
}

Config_ValidatorMessage::~Config_ValidatorMessage()
{
}

const std::string& Config_ValidatorMessage::validatorId() const
{
  return myValidatorId;
}

const std::string& Config_ValidatorMessage::featureId() const
{
  return myFeatureId;
}

const std::string& Config_ValidatorMessage::attributeId() const
{
  return myAttributeId;
}

const std::list<std::string>& Config_ValidatorMessage::parameters() const
{
  return myVaidatorParameters;
}

bool Config_ValidatorMessage::isValid() const
{
  return !myValidatorId.empty();
}

void Config_ValidatorMessage::setValidatorId(const std::string& validatorId)
{
  myValidatorId = validatorId;
}

void Config_ValidatorMessage::setValidatorParameters(const std::list<std::string>& parameters)
{
  myVaidatorParameters = parameters;
}

void Config_ValidatorMessage::setFeatureId(const std::string& theId)
{
  myFeatureId = theId;
}

void Config_ValidatorMessage::setAttributeId(const std::string& theId)
{
  myAttributeId = theId;
}
