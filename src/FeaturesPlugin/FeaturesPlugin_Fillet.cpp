// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Fillet.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Fillet.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_ShapeExplorer.h>

// Extract edges from the list
static ListOfShape extractEdges(const ListOfShape& theShapes)
{
  ListOfShape anEdges;
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt)
    for (GeomAPI_ShapeExplorer anExp(*anIt, GeomAPI_Shape::EDGE); anExp.more(); anExp.next())
      anEdges.push_back(anExp.current());
  return anEdges;
}


FeaturesPlugin_Fillet::FeaturesPlugin_Fillet()
{
}

void FeaturesPlugin_Fillet::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
  AttributePtr aSelectionList =
      data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(START_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(END_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), END_RADIUS_ID());

  initVersion(aSelectionList);
}

AttributePtr FeaturesPlugin_Fillet::objectsAttribute()
{
  return attribute(OBJECT_LIST_ID());
}

const std::string& FeaturesPlugin_Fillet::modifiedShapePrefix() const
{
  static const std::string& THE_PREFIX("Fillet");
  return THE_PREFIX;
}

GeomMakeShapePtr FeaturesPlugin_Fillet::performOperation(const GeomShapePtr& theSolid,
                                                         const ListOfShape& theEdges)
{
  AttributeStringPtr aCreationMethod = string(CREATION_METHOD());
  if (!aCreationMethod)
    return GeomMakeShapePtr();

  bool isFixedRadius = aCreationMethod->value() == CREATION_METHOD_SINGLE_RADIUS();
  double aRadius1 = 0.0, aRadius2 = 0.0;
  if (isFixedRadius)
    aRadius1 = real(RADIUS_ID())->value();
  else {
    aRadius1 = real(START_RADIUS_ID())->value();
    aRadius2 = real(END_RADIUS_ID())->value();
  }

  // Perform fillet operation
  std::shared_ptr<GeomAlgoAPI_Fillet> aFilletBuilder;
  std::string anError;

  ListOfShape aFilletEdges = extractEdges(theEdges);
  if (isFixedRadius)
    aFilletBuilder.reset(new GeomAlgoAPI_Fillet(theSolid, aFilletEdges, aRadius1));
  else
    aFilletBuilder.reset(new GeomAlgoAPI_Fillet(theSolid, aFilletEdges, aRadius1, aRadius2));

  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFilletBuilder, getKind(), anError)) {
    setError(anError);
    return GeomMakeShapePtr();
  }
  return aFilletBuilder;
}
