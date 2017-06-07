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

#include "ModelHighAPI_Reference.h"

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
ModelHighAPI_Reference::ModelHighAPI_Reference()
{}

ModelHighAPI_Reference::ModelHighAPI_Reference(
    const std::shared_ptr<ModelAPI_Object> & theValue)
: myObject(theValue)
{}

ModelHighAPI_Reference::ModelHighAPI_Reference(
    const std::shared_ptr<ModelHighAPI_Interface> & theValue)
: myObject(std::shared_ptr<ModelAPI_Object>(theValue->defaultResult()))
{
  // the result is not constructed yet, forcibly do it
  if (!myObject) {
    theValue->execute(true);
    myObject = std::shared_ptr<ModelAPI_Object>(theValue->defaultResult());
  }
}

ModelHighAPI_Reference::~ModelHighAPI_Reference()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Reference::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute) const
{
  theAttribute->setValue(myObject);
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Reference::appendToList(
    const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute) const
{
  theAttribute->append(myObject);
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Feature> ModelHighAPI_Reference::feature() const
{
  return ModelAPI_Feature::feature(myObject);
}
