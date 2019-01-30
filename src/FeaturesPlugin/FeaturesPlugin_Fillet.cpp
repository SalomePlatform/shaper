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
#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Fillet.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Tools.h>

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

// If theShape is a compound of a single sub-shape, return this sub-shape
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
    GeomShapePtr aParent;
    if (aContext.get()) {
      ResultBodyPtr aCtxOwner = ModelAPI_Tools::bodyOwner(aContext);
      aParent = aCtxOwner ? aCtxOwner->shape() : aContext->shape();
    } else { // get it from a feature
      FeaturePtr aFeature = anObjectAttr->contextFeature();
      if (aFeature.get()) {
        aParent = aFeature->firstResult()->shape();
      }
    }
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
  std::string anError;

  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape anOriginalShapesList, aResultShapesList;

  GeomAPI_DataMapOfShapeMapOfShapes::iterator anIt = aSolidsAndSubs.begin();
  for (; anIt != aSolidsAndSubs.end(); ++anIt) {
    GeomShapePtr aSolid = anIt.first();
    ListOfShape aFilletEdgesAndVertices = anIt.second();

    ListOfShape aFilletEdges = selectEdges(aFilletEdgesAndVertices);
    if (isFixedRadius)
      aFilletBuilder.reset(new GeomAlgoAPI_Fillet(aSolid, aFilletEdges, aRadius1));
    else
      aFilletBuilder.reset(new GeomAlgoAPI_Fillet(aSolid, aFilletEdges, aRadius1, aRadius2));

    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFilletBuilder, getKind(), anError)) {
      setError(anError);
      return;
    }

    GeomShapePtr aResult = unwrapCompound(aFilletBuilder->shape());
    std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

    ListOfShape aBaseShapes;
    aBaseShapes.push_back(aSolid);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                             aFilletBuilder, aResult, "Fillet");

    setResult(aResultBody, aResultIndex);
    aResultIndex++;

    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = aSolid;
    aRBA.makeShape = aFilletBuilder;
    aResultBaseAlgoList.push_back(aRBA);
    aResultShapesList.push_back(aResult);
    anOriginalShapesList.push_back(aSolid);
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList,
      anOriginalShapesList, aResultShapesCompound);

  removeResults(aResultIndex);
}

void FeaturesPlugin_Fillet::loadNamingDS(
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
    const std::shared_ptr<GeomAPI_Shape> theResultShape,
    const std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape)
{
  //load result
  if(theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape, false);
    return;
  }

  theResultBody->storeModified(theBaseShape, theResultShape);

  const std::string aFilletFaceName = "Fillet_Face";

  // Store modified faces
  theResultBody->loadModifiedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::FACE);

  // Store new faces generated from edges and vertices
  theResultBody->loadGeneratedShapes(theMakeShape,
                                     theBaseShape,
                                     GeomAPI_Shape::EDGE,
                                     aFilletFaceName);
  theResultBody->loadGeneratedShapes(theMakeShape,
                                     theBaseShape,
                                     GeomAPI_Shape::VERTEX,
                                     aFilletFaceName);

  // Deleted shapes
  theResultBody->loadDeletedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::EDGE);
  theResultBody->loadDeletedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::FACE);
}
