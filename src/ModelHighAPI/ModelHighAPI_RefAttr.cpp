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

#include "ModelHighAPI_RefAttr.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
ModelHighAPI_RefAttr::ModelHighAPI_RefAttr()
: myVariantType(VT_ATTRIBUTE)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelAPI_Attribute> & theValue)
: myVariantType(VT_ATTRIBUTE)
, myAttribute(theValue)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelAPI_Object> & theValue)
: myVariantType(VT_OBJECT)
, myObject(theValue)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelHighAPI_Interface> & theValue)
: myVariantType(VT_OBJECT)
, myObject(std::shared_ptr<ModelAPI_Object>(theValue->defaultResult()))
{
}

ModelHighAPI_RefAttr::~ModelHighAPI_RefAttr()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_RefAttr::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute) const
{
  switch(myVariantType) {
    case VT_ATTRIBUTE: theAttribute->setAttr(myAttribute); return;
    case VT_OBJECT: theAttribute->setObject(myObject); return;
  }
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_RefAttr::appendToList(
    const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute) const
{
  switch(myVariantType) {
    case VT_ATTRIBUTE: theAttribute->append(myAttribute); return;
    case VT_OBJECT: theAttribute->append(myObject); return;
  }
}

//--------------------------------------------------------------------------------------
bool ModelHighAPI_RefAttr::isEmpty() const
{
  return !(myAttribute && myObject);
}
