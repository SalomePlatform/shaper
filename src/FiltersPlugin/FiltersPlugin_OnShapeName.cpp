// Copyright (C) 2014-2024  CEA, EDF
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

#include "FiltersPlugin_OnShapeName.h"
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Tools.h>

#include <Locale_Convert.h>
#include <regex>


bool FiltersPlugin_OnShapeName::isSupported(GeomAPI_Shape::ShapeType /*theType*/) const
{
  return true;
}

//=======================================================================
bool FiltersPlugin_OnShapeName::isOk(const GeomShapePtr& theShape, const ResultPtr& theResult,
                                     const ModelAPI_FiltersArgs& theArgs) const
{
  if (!theResult)
    return false;

  AttributePtr anAttr = theArgs.argument("pattern");
  AttributeStringPtr aValue = std::dynamic_pointer_cast<ModelAPI_AttributeString>(anAttr);
  if (!aValue || !anAttr->isInitialized())
    return false;

  std::regex aPattern(aValue->value());

  ResultBodyPtr aBodyRes = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);

  for(; aBodyRes.get(); aBodyRes = ModelAPI_Tools::bodyOwner(aBodyRes))
  {
    const DataPtr data = aBodyRes->data();
    if (data.get()) 
    {
      const std::string aResName = Locale::Convert::toString(data->name());
      if(std::regex_search(aResName, aPattern))
      {
        return true;
      }
    }
  }

  return false;
}

//=======================================================================
std::string FiltersPlugin_OnShapeName::xmlRepresentation() const
{
  return xmlFromFile("filter-OnShapeName.xml");
}

//=======================================================================
void FiltersPlugin_OnShapeName::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("pattern", ModelAPI_AttributeString::typeId());
}
