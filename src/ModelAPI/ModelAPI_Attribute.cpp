// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <ModelAPI_Attribute.h>

ModelAPI_Attribute::~ModelAPI_Attribute()
{
}

  /// Sets the owner of this attribute
void ModelAPI_Attribute::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  myObject = theObject;
}

  /// Returns the owner of this attribute
const std::shared_ptr<ModelAPI_Object>& ModelAPI_Attribute::owner() const
{
  return myObject;
}

  /// Returns true if attribute was  initialized by some value
bool ModelAPI_Attribute::isInitialized()
{
  return myIsInitialized;
}

void ModelAPI_Attribute::setInitialized()
{
  if (!mySetInitializedBlocked)
    myIsInitialized = true;
}

bool ModelAPI_Attribute::blockSetInitialized(const bool theBlock)
{
  bool aBlocked = mySetInitializedBlocked;

  mySetInitializedBlocked = theBlock;

  return aBlocked;
}

void ModelAPI_Attribute::setIsArgument(const bool theFlag)
{
  myIsArgument = theFlag;
}

bool ModelAPI_Attribute::isArgument()
{
  return myIsArgument;
}

bool ModelAPI_Attribute::setImmutable(const bool theFlag)
{
  bool aResult = myIsImmutable;
  myIsImmutable = theFlag;
  return aResult;
}

bool ModelAPI_Attribute::isImmutable()
{
  return myIsImmutable;
}

const std::string& ModelAPI_Attribute::id() const
{
  return myID;
}

ModelAPI_Attribute::ModelAPI_Attribute()
{
  mySetInitializedBlocked = false;
  myIsInitialized = false;
  myIsArgument = true;
  myIsImmutable = false;
}

  /// Sets the ID of the attribute in Data (called from Data)
void ModelAPI_Attribute::setID(const std::string theID)
{
  myID = theID;
}

void ModelAPI_Attribute::reinit() {}

void ModelAPI_Attribute::reset()
{
  myIsInitialized = false;
}
