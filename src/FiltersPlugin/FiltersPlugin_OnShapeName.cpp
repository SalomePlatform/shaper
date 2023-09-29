// Copyright (C) 2014-2023  CEA, EDF
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
// #include <Model_Data.h>
// #include <TDF_Label.hxx>
// #include <TNaming_Builder.hxx>
// #include <TNaming_NamedShape.hxx>
// #include <TDataStd_Name.hxx>
// #include <TNaming_Tool.hxx>
// #include <TopoDS_Shape.hxx>

#include <Locale_Convert.h>
#include <boost/algorithm/string.hpp>

bool FiltersPlugin_OnShapeName::isSupported(GeomAPI_Shape::ShapeType /*theType*/) const
{
  return true;
}

static bool getShapesName(const GeomShapePtr& theShape,
                          ResultBodyPtr theResultBody,
                          std::string& theShapeName)
{
  int nbSubRes = theResultBody->numberOfSubs();
  if(theResultBody->shape()->isSame(theShape) || (nbSubRes == 0 && theResultBody->shape()->isSubShape(theShape)))
  {
    DataPtr data = theResultBody->data();
    if (data.get()) 
    {
      theShapeName = Locale::Convert::toString(data->name());
      return true;
    }
  }

  bool aResult(false);
  ResultBodyPtr subResult;
  for (int i = 0; i < nbSubRes; i++)
  {
    subResult = theResultBody->subResult(i);
    if(getShapesName(theShape, subResult, theShapeName))
    {
      aResult = true;
      break;
    }
  }

  return aResult;
}

bool FiltersPlugin_OnShapeName::isOk(const GeomShapePtr& theShape, const ResultPtr& theResult,
                                     const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr anAttr = theArgs.argument("pattern");
  AttributeStringPtr aValue = std::dynamic_pointer_cast<ModelAPI_AttributeString>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized())
    return false;
  std::string aPattern= aValue->value();


  ResultBodyPtr aBodyRes = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  std::string aShapeName;
  if(!getShapesName(theShape, aBodyRes, aShapeName))
  {
    return false;
  }

  //remove * from both sides of pattern
  if(aPattern.back() == '*' && aPattern.front() == '*')
  {
    aPattern.pop_back();
    aPattern.erase(aPattern.begin());
  }

  bool isFound = false;
  if(aPattern.front() == '*')
  {
    aPattern.erase(aPattern.begin());
    isFound = boost::algorithm::ends_with(aShapeName, aPattern);
  }
  else if(aPattern.back() == '*')
  {
    aPattern.pop_back();
    isFound = boost::algorithm::starts_with(aShapeName, aPattern);
  }
  else
  {
    isFound = (aShapeName.find(aPattern) != std::string::npos);
  }

  return isFound;
}

std::string FiltersPlugin_OnShapeName::xmlRepresentation() const
{
  return xmlFromFile("filter-OnShapeName.xml");
}

void FiltersPlugin_OnShapeName::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("pattern", ModelAPI_AttributeString::typeId());
}