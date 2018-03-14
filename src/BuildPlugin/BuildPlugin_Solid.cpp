// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Solid.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_MakeVolume.h>

//=================================================================================================
BuildPlugin_Solid::BuildPlugin_Solid()
{
}

//=================================================================================================
void BuildPlugin_Solid::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Solid::execute()
{
  // all the needed checkings are in validator, so, here just make and store result
  ListOfShape anOriginalShapes;
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if (!aShape.get())
      aShape = aSelection->context()->shape();
    anOriginalShapes.push_back(aShape);
  }
  std::shared_ptr<GeomAlgoAPI_MakeVolume> anAlgo(
    new GeomAlgoAPI_MakeVolume(anOriginalShapes, false));
  // check and process result of volume maker
  GeomShapePtr aResShape = getSingleSubshape(anAlgo->shape());
  storeResult(anOriginalShapes, aResShape, anAlgo);
}

void BuildPlugin_Solid::storeResult(const ListOfShape& theOriginalShapes,
                                    const GeomShapePtr& theResultShape,
                                    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm)
{
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->store(theResultShape);

  // Store faces
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubs = theAlgorithm->mapOfSubShapes();
  int aModifiedTag = 1;
  for(ListOfShape::const_iterator anIt = theOriginalShapes.cbegin();
      anIt != theOriginalShapes.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;
    aResultBody->loadAndOrientModifiedShapes(theAlgorithm.get(), aShape, GeomAPI_Shape::FACE,
        aModifiedTag, "Modified_Face", *aMapOfSubs.get(), false, true, true);
  }
  setResult(aResultBody);
}

GeomShapePtr BuildPlugin_Solid::getSingleSubshape(const GeomShapePtr& theCompound)
{
  if (theCompound->shapeType() == GeomAPI_Shape::COMPOUND) {
    GeomAPI_ShapeIterator anIt(theCompound);
    GeomShapePtr aFoundSub;
    for (; anIt.more() && !aFoundSub; anIt.next()) {
      return anIt.current();
    }
  }
  return theCompound;
}
