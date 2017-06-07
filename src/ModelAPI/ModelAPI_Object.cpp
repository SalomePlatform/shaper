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

#include "ModelAPI_Object.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_Events.h"
#include <Events_Loop.h>

bool ModelAPI_Object::isInHistory()
{
  if (myData->isValid()) {
    return myData->isInHistory();
  }
  return true; // default value
}

void ModelAPI_Object::setInHistory(
  const std::shared_ptr<ModelAPI_Object> theObject, const bool theFlag)
{
  if (isInHistory() != theFlag) {
    if (myData->isValid()) {
      myData->setIsInHistory(theFlag);
    }
    myDoc->updateHistory(theObject);
  }
}

std::shared_ptr<ModelAPI_Data> ModelAPI_Object::data() const
{
  return myData;
}

bool ModelAPI_Object::isSame(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  return theObject.get() == this;
}

std::shared_ptr<ModelAPI_Document> ModelAPI_Object::document() const
{
  return myDoc;
}

void ModelAPI_Object::attributeChanged(const std::string& theID)
{
}

ModelAPI_Object::ModelAPI_Object()
{
}

ModelAPI_Object::~ModelAPI_Object()
{
}

void ModelAPI_Object::setData(std::shared_ptr<ModelAPI_Data> theData)
{
  myData = theData;
}

void ModelAPI_Object::setDoc(std::shared_ptr<ModelAPI_Document> theDoc)
{
  myDoc = theDoc;
}

void ModelAPI_Object::erase()
{
  if (myData->isValid() && myData != myData->invalidPtr())
    myData->erase();
  setData(myData->invalidPtr());
}

bool ModelAPI_Object::isDisplayed()
{
  return myData->isDisplayed();
}

void ModelAPI_Object::setDisplayed(const bool theDisplay)
{
  myData->setDisplayed(theDisplay);
}
