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
  ListOfShape anOriginalShapes;
  std::shared_ptr<GeomAlgoAPI_MakeShape> aVolumeMaker;
  if (!build(anOriginalShapes, aVolumeMaker))
    return;

  // check and process result of volume maker
  GeomShapePtr aResShape = getSingleSubshape(aVolumeMaker->shape(), GeomAPI_Shape::SOLID);
  int anIndex = 0;
  if (aResShape) {
    storeResult(anOriginalShapes, aResShape, aVolumeMaker);
    ++anIndex;
  }
  removeResults(anIndex);
}

//=================================================================================================
bool BuildPlugin_Solid::build(ListOfShape& theOriginalShapes,
                              std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm)
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if (!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return false;
  }
  if (aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return false;
  }

  // Collect base shapes.
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if (!aShape.get())
      aShape = aSelection->context()->shape();
    theOriginalShapes.push_back(aShape);
  }

  theAlgorithm =
      std::shared_ptr<GeomAlgoAPI_MakeVolume>(new GeomAlgoAPI_MakeVolume(theOriginalShapes));
  return !isAlgorithmFailed(theAlgorithm);
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

GeomShapePtr BuildPlugin_Solid::getSingleSubshape(const GeomShapePtr& theCompound,
                                                  const GeomAPI_Shape::ShapeType theShapeType)
{
  if (theCompound->shapeType() == theShapeType)
    return theCompound;
  else if (theCompound->shapeType() == GeomAPI_Shape::COMPOUND) {
    GeomAPI_ShapeIterator anIt(theCompound);
    GeomShapePtr aFoundSub;
    for (; anIt.more() && !aFoundSub; anIt.next()) {
      aFoundSub = anIt.current();
      if (aFoundSub->shapeType() != theShapeType)
        return GeomShapePtr(); // not alone sub-shape
    }
    if (anIt.more()) {
      std::string anError = "Error: unable to build a ";
      switch (theShapeType) {
      case GeomAPI_Shape::SOLID: anError += "solid"; break;
      case GeomAPI_Shape::COMPSOLID: anError += "compsolid"; break;
      default: anError += "subshape"; break;
      }
      setError(anError);
    } else
      return aFoundSub;
  }
  // not a solid
  return GeomShapePtr();
}

bool BuildPlugin_Solid::isAlgorithmFailed(
    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm)
{
  if (!theAlgorithm->isDone()) {
    static const std::string aFeatureError = "Error: MakeVolume algorithm failed.";
    setError(aFeatureError);
    return true;
  }
  if (theAlgorithm->shape()->isNull()) {
    static const std::string aShapeError = "Error: Resulting shape of MakeVolume is Null.";
    setError(aShapeError);
    return true;
  }
  if (!theAlgorithm->isValid()) {
    std::string aFeatureError = "Error: Resulting shape of MakeVolume is not valid.";
    setError(aFeatureError);
    return true;
  }
  return false;
}
