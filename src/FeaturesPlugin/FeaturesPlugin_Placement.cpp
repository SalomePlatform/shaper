// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Placement.h"

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeHierarchy.h>
#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_Transform.h>
#include <GeomAlgoAPI_Tools.h>

#include <FeaturesPlugin_Tools.h>

static const std::string PLACEMENT_VERSION_1("v9.5");

FeaturesPlugin_Placement::FeaturesPlugin_Placement()
{
}

void FeaturesPlugin_Placement::initAttributes()
{
  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(START_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(END_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(REVERSE_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(CENTERING_ID(), ModelAPI_AttributeBoolean::typeId());

  if (!aSelection->isInitialized()) {
    // new feature, not read from file
    data()->setVersion(PLACEMENT_VERSION_1);
  }
}

void FeaturesPlugin_Placement::execute()
{
  bool isKeepSubShapes = data()->version() == PLACEMENT_VERSION_1;

  // Getting objects.
  GeomAPI_ShapeHierarchy anObjects;
  std::list<ResultPtr> aParts;
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECTS_LIST_ID());
  if (!FeaturesPlugin_Tools::shapesFromSelectionList(
       anObjectsSelList, isKeepSubShapes, anObjects, aParts))
    return;

  // Verify the start shape
  AttributeSelectionPtr anObjRef = selection(START_SHAPE_ID());
  if(!anObjRef) {
    return;
  }
  std::shared_ptr<GeomAPI_Shape> aStartShape = anObjRef->value();
  if(!aStartShape) {
    static const std::string aSelectionError = "Error: The start shape selection is bad.";
    setError(aSelectionError);
    return;
  }


  std::shared_ptr<GeomAPI_Shape> aStartShapeContext;
  ResultPtr aContextRes = anObjRef->context();
  if (aContextRes.get()) {
    aStartShapeContext = aContextRes->shape();
  }
  if(!aStartShapeContext.get()) {
    static const std::string aContextError = "Error: The start shape selection context is bad.";
    setError(aContextError);
    return;
  }

  // Verify the end shape
  anObjRef = selection(END_SHAPE_ID());
  std::shared_ptr<GeomAPI_Shape> anEndShape = anObjRef->value();
  if(!anEndShape) {
    static const std::string aSelectionError = "Error: The end shape selection is bad.";
    setError(aSelectionError);
    return;
  }

  std::shared_ptr<GeomAPI_Shape> anEndShapeContext;
  aContextRes = anObjRef->context();
  if(aContextRes.get()) {
    anEndShapeContext = aContextRes->shape();
  }
  if(!anEndShapeContext.get()) {
    static const std::string aContextError = "Error: The end shape selection context is bad.";
    setError(aContextError);
    return;
  }

  // Verify planarity of faces and linearity of edges
  std::shared_ptr<GeomAPI_Shape> aShapes[2] = {aStartShape, anEndShape};
  for (int i = 0; i < 2; i++) {
    if (!isShapeValid(aShapes[i])) {
      return;
    }
  }

  // Flags of the Placement
  bool isReverse = boolean(REVERSE_ID())->value();
  bool isCentering = boolean(CENTERING_ID())->value();

  // Getting transformation.
  GeomAlgoAPI_Placement aPlacementAlgo(
    aStartShapeContext, anEndShapeContext, aStartShape, anEndShape, isReverse, isCentering, true);
  if(!aPlacementAlgo.isDone()) {
    static const std::string aFeatureError = "Error: Placement algorithm failed.";
    setError(aFeatureError);
    return;
  }
  std::shared_ptr<GeomAPI_Trsf> aTrsf = aPlacementAlgo.transformation();

  int aResultIndex = 0;
  std::string anError;
  for (std::list<ResultPtr>::iterator aPRes = aParts.begin(); aPRes != aParts.end(); ++aPRes) {
    // Applying transformation to each part.
    ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPRes);
    ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
    aResultPart->setTrsf(aContextRes, aTrsf);
    setResult(aResultPart, aResultIndex++);
  }

  // Collect transformations for each object.
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);

  for(GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
      anObjectsIt != anObjects.end(); anObjectsIt++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    std::shared_ptr<GeomAlgoAPI_Transform> aTransformAlgo(
        new GeomAlgoAPI_Transform(aBaseShape, aTrsf));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aTransformAlgo, getKind(), anError)) {
      setError(anError);
      break;
    }

    anObjects.markModified(aBaseShape, aTransformAlgo->shape());
    aMakeShapeList->appendAlgo(aTransformAlgo);
  }

  // Build results of the operation.
  const ListOfShape& anOriginalShapes = anObjects.objects();
  ListOfShape aTopLevel;
  anObjects.topLevelObjects(aTopLevel);
  for (ListOfShape::iterator anIt = aTopLevel.begin(); anIt != aTopLevel.end(); ++anIt) {
    //LoadNamingDS
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, anOriginalShapes, ListOfShape(),
                                             aMakeShapeList, *anIt, "Placed");
    setResult(aResultBody, aResultIndex++);
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//==================================================================================================
bool FeaturesPlugin_Placement::isShapeValid(GeomShapePtr theShape)
{
  if (theShape->isCompound()) {
    GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::SHAPE;
    for (GeomAPI_ShapeIterator anIt(theShape); anIt.more(); anIt.next()) {
      GeomShapePtr aCurrentShape = anIt.current();
      if (aShapeType == GeomAPI_Shape::SHAPE) {
        aShapeType = aCurrentShape->shapeType();
      }
      else if (aShapeType != aCurrentShape->shapeType()) {
        static const std::string aLinearityError =
          "Error: Selected compound contains shapes with different types.";
        setError(aLinearityError);
        return false;
      }

      if (!isShapeValid(aCurrentShape)) {
        return false;
      }
    }
  }
  else if (theShape->isFace()) {
    std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(theShape));
    if (!aFace->isPlanar()) {
      static const std::string aPlanarityError = "Error: One of selected faces is not planar.";
      setError(aPlanarityError);
      return false;
    }
  }
  else if (theShape->isEdge()) {
    std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theShape));
    if (!anEdge->isLine()) {
      static const std::string aLinearityError = "Error: One of selected edges is not linear.";
      setError(aLinearityError);
      return false;
    }
  }

  return true;
}
