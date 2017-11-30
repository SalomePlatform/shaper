// Copyright (C) 2017  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Fillet.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Fillet.h>
#include <GeomAlgoAPI_MakeShapeList.h>

#include <GeomAPI_DataMapOfShapeMapOfShapes.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>


// Obtain all sub-shapes from the shape and append them to the list
static void collectSubs(const GeomShapePtr& theShape,
                              ListOfShape& theSubs,
                        const GeomAPI_Shape::ShapeType theShapeType)
{
  GeomAPI_ShapeExplorer anExp(theShape, theShapeType);
  for (; anExp.more(); anExp.next()) {
    GeomShapePtr aShape = anExp.current();
    // Store all shapes with FORWARD orientation to avoid duplication of shared edges/vertices
    aShape->setOrientation(GeomAPI_Shape::FORWARD);
    theSubs.push_back(aShape);
  }
}

// Extract edges from the list
static ListOfShape selectEdges(const ListOfShape& theShapes)
{
  ListOfShape anEdges;
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt)
    if ((*anIt)->isEdge())
      anEdges.push_back(*anIt);
  return anEdges;
}

// If theShape is a compound of single shape, return it
static GeomShapePtr unwrapCompound(const GeomShapePtr& theShape)
{
  GeomShapePtr aShape = theShape;
  if(aShape->shapeType() == GeomAPI_Shape::COMPOUND) {
    int aSubResultsNb = 0;
    GeomAPI_ShapeIterator anIt(aShape);
    for(; anIt.more(); anIt.next())
      ++aSubResultsNb;

    if(aSubResultsNb == 1) {
      anIt.init(aShape);
      aShape = anIt.current();
    }
  }
  return aShape;
}


FeaturesPlugin_Fillet::FeaturesPlugin_Fillet()
{
}

void FeaturesPlugin_Fillet::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(START_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(END_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), END_RADIUS_ID());
}


void FeaturesPlugin_Fillet::execute()
{
  AttributeStringPtr aCreationMethod = string(CREATION_METHOD());
  if (!aCreationMethod)
    return;

  GeomAPI_DataMapOfShapeMapOfShapes aSolidsAndSubs;

  // getting objects and sort them accroding to parent solids
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); ++anObjectsIndex) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if (!anObject)
      return;

    ResultPtr aContext = anObjectAttr->context();
    ResultCompSolidPtr aCtxOwner = ModelAPI_Tools::compSolidOwner(aContext);
    GeomShapePtr aParent = aCtxOwner ? aCtxOwner->shape() : aContext->shape();
    if (!aParent)
      return;

    ListOfShape anEdgesAndVertices;
    collectSubs(anObject, anEdgesAndVertices, GeomAPI_Shape::EDGE);
    collectSubs(anObject, anEdgesAndVertices, GeomAPI_Shape::VERTEX);
    for (ListOfShape::iterator aEIt = anEdgesAndVertices.begin();
         aEIt != anEdgesAndVertices.end(); ++aEIt)
      aSolidsAndSubs.add(aParent, *aEIt);
  }

  bool isFixedRadius = true;
  double aRadius1 = 0.0, aRadius2 = 0.0;
  if (aCreationMethod->value() == CREATION_METHOD_SINGLE_RADIUS())
    aRadius1 = real(RADIUS_ID())->value();
  else {
    aRadius1 = real(START_RADIUS_ID())->value();
    aRadius2 = real(END_RADIUS_ID())->value();
    isFixedRadius = false;
  }

  // Perform fillet operation
  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  std::shared_ptr<GeomAlgoAPI_Fillet> aFilletBuilder;
  int aResultIndex = 0;

  GeomAPI_DataMapOfShapeMapOfShapes::iterator anIt = aSolidsAndSubs.begin();
  for (; anIt != aSolidsAndSubs.end(); ++anIt) {
    GeomShapePtr aSolid = anIt.first();
    ListOfShape aFilletEdgesAndVertices = anIt.second();

    ListOfShape aFilletEdges = selectEdges(aFilletEdgesAndVertices);
    if (isFixedRadius)
      aFilletBuilder.reset(new GeomAlgoAPI_Fillet(aSolid, aFilletEdges, aRadius1));
    else
      aFilletBuilder.reset(new GeomAlgoAPI_Fillet(aSolid, aFilletEdges, aRadius1, aRadius2));
    if (isFailed(aFilletBuilder))
      return;

    GeomShapePtr aResult = unwrapCompound(aFilletBuilder->shape());
    std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

    loadNamingDS(aResultBody, aSolid, aFilletEdgesAndVertices, aResult, aFilletBuilder);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }
  removeResults(aResultIndex);
}

bool FeaturesPlugin_Fillet::isFailed(
    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm)
{
  if (!theAlgorithm->isDone()) {
    static const std::string aFeatureError = "Error: fillet algorithm failed.";
    setError(aFeatureError);
    return true;
  }
  if (theAlgorithm->shape()->isNull()) {
    static const std::string aShapeError = "Error: Resulting shape of fillet is Null.";
    setError(aShapeError);
    return true;
  }
  if (!theAlgorithm->isValid()) {
    std::string aFeatureError = "Error: Resulting shape of fillet is not valid.";
    setError(aFeatureError);
    return true;
  }
  return false;
}

void FeaturesPlugin_Fillet::loadNamingDS(
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
    const ListOfShape& theFilletShapes,
    const std::shared_ptr<GeomAPI_Shape> theResultShape,
    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape)
{
  //load result
  if(theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape, false);
    return;
  }

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfShapes = theMakeShape->mapOfSubShapes();

  const int aDeletedTag = 1;
  const int aModifyTag = 2;
  const int aGeneratedTag = 3;
  /// sub solids will be placed at labels 4, 5, etc. if result is compound of solids
  const int aSubsolidsTag = 4;

  theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);
  theResultBody->storeGenerated(theBaseShape, theResultShape);

  const std::string aModFaceName = "Modified_Face";
  const std::string aFilletFaceName = "Fillet_Face";

  // Store modified faces
  theResultBody->loadAndOrientModifiedShapes(theMakeShape.get(), theBaseShape, GeomAPI_Shape::FACE,
      aModifyTag, aModFaceName, *aMapOfShapes);

  // Store new faces generated from edges and vertices
  theResultBody->loadAndOrientGeneratedShapes(theMakeShape.get(), theBaseShape, GeomAPI_Shape::EDGE,
      aGeneratedTag, aFilletFaceName, *aMapOfShapes);
  theResultBody->loadAndOrientGeneratedShapes(theMakeShape.get(), theBaseShape, GeomAPI_Shape::VERTEX,
      aGeneratedTag, aFilletFaceName, *aMapOfShapes);

  // Deleted shapes
  theResultBody->loadDeletedShapes(theMakeShape.get(), theBaseShape,
                                   GeomAPI_Shape::EDGE, aDeletedTag);
  theResultBody->loadDeletedShapes(theMakeShape.get(), theBaseShape,
                                   GeomAPI_Shape::FACE, aDeletedTag);
}
