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

#include "FeaturesPlugin_Placement.h"

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_BodyBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_Transform.h>
#include <GeomAlgoAPI_Tools.h>

#include <FeaturesPlugin_Tools.h>

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
}

void FeaturesPlugin_Placement::execute()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECTS_LIST_ID());
  if(anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

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

  // Applying transformation to each object.
  int aResultIndex = 0;
  std::string anError;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
      anObjectsIt++, aContext++) {

    // for part results just set transformation
    if (aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group()) {
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      ResultPartPtr aResultPart = document()->copyPart(anOrigin, data(), aResultIndex);
      aResultPart->setTrsf(aContextRes, aTrsf);
      setResult(aResultPart, aResultIndex);
    } else {
      std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
      std::shared_ptr<GeomAlgoAPI_Transform> aTransformAlgo(new GeomAlgoAPI_Transform(aBaseShape,
                                                                                      aTrsf));

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aTransformAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      //LoadNamingDS
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

      ListOfShape aShapes;
      aShapes.push_back(aBaseShape);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aShapes, ListOfShape(),
                                               aTransformAlgo, aTransformAlgo->shape(), "Placed");
      setResult(aResultBody, aResultIndex);
    }
    aResultIndex++;
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
