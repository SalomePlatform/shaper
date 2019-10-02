// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Chamfer.h"
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
#include <GeomAlgoAPI_Chamfer.h>
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


FeaturesPlugin_Chamfer::FeaturesPlugin_Chamfer()
{
}

void FeaturesPlugin_Chamfer::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Chamfer::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::OBJECT_LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::D1_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::D2_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::D_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Chamfer::ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
}


void FeaturesPlugin_Chamfer::execute()
{
   AttributeStringPtr aCreationMethod = string(CREATION_METHOD());
   if (!aCreationMethod)
     return;
 
   std::list<std::pair<GeomShapePtr, ListOfShape> > aSolidsAndSubs; 
   std::map<GeomShapePtr, GeomShapePtr> aMapEdgeFace;
 
   // getting objects and sort them according to parent solids
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
 
     // searching this parent is already in the list aSolidsAndSubs
     std::list<std::pair<GeomShapePtr, ListOfShape> >::iterator aSearch = aSolidsAndSubs.begin();
     ListOfShape* aFound;
     for(; aSearch != aSolidsAndSubs.end(); aSearch++) {
       if (aSearch->first->isSame(aParent)) {
         aFound = &(aSearch->second);
         break;
       }
     }
     if (aSearch == aSolidsAndSubs.end()) { // not found, so, add a new one
       aSolidsAndSubs.push_back(std::pair<GeomShapePtr, ListOfShape>(aParent, ListOfShape()));
       aFound = &(aSolidsAndSubs.back().second);
     }

    ListOfShape anEdgesAndVertices;
    collectSubs(anObject, anEdgesAndVertices, GeomAPI_Shape::EDGE);
    collectSubs(anObject, anEdgesAndVertices, GeomAPI_Shape::VERTEX);
    for (ListOfShape::iterator aEIt = anEdgesAndVertices.begin();
         aEIt != anEdgesAndVertices.end(); ++aEIt)
      aFound->push_back(*aEIt);
     
     if (anObject->isFace()) {
       for (ListOfShape::iterator aEIt = anEdgesAndVertices.begin();
          aEIt != anEdgesAndVertices.end(); ++aEIt) {
         if ((*aEIt)->isEdge()) {
           aMapEdgeFace[(*aEIt)] = anObject;
         }
       }
     }
   }
 
   //bool isOption1 = true;
   double aD1 = 0.0, aD2 = 0.0, aD = 0.0, anAngle = 0.0;
   if (aCreationMethod->value() == CREATION_METHOD_DISTANCE_DISTANCE()) {
     aD1 = real(FeaturesPlugin_Chamfer::D1_ID())->value();
     aD2 = real(FeaturesPlugin_Chamfer::D2_ID())->value();
   } else {
     aD = real(FeaturesPlugin_Chamfer::D_ID())->value();
     anAngle = real(FeaturesPlugin_Chamfer::ANGLE_ID())->value();
   }
 
   // Perform chamfer operation
   GeomAlgoAPI_MakeShapeList aMakeShapeList;
   std::shared_ptr<GeomAlgoAPI_Chamfer> aChamferBuilder;
   int aResultIndex = 0;
   std::string anError;
   
 
   std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
   ListOfShape anOriginalShapesList, aResultShapesList;
 
   std::list<std::pair<GeomShapePtr, ListOfShape> >::iterator anIt = aSolidsAndSubs.begin();
   for (; anIt != aSolidsAndSubs.end(); ++anIt) {
     GeomShapePtr aSolid = anIt->first;
     ListOfShape aFilletEdgesAndVertices = anIt->second;
 
     ListOfShape aFilletEdges = selectEdges(aFilletEdgesAndVertices);
     if (aCreationMethod->value() == CREATION_METHOD_DISTANCE_DISTANCE()) {
       aChamferBuilder.reset(new GeomAlgoAPI_Chamfer(
         aSolid, aFilletEdges, aMapEdgeFace, true, aD1, aD2));
     } else {
       aChamferBuilder.reset(new GeomAlgoAPI_Chamfer(
         aSolid, aFilletEdges, aMapEdgeFace, false, aD, anAngle));
     }
     
     if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aChamferBuilder, getKind(), anError)) {
       setError(anError);
       return;
     }
 
    GeomShapePtr aResult = unwrapCompound(aChamferBuilder->shape());
    std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

    ListOfShape aBaseShapes;
    aBaseShapes.push_back(aSolid);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                             aChamferBuilder, aResult, "Chamfer");

    setResult(aResultBody, aResultIndex);
    aResultIndex++;

    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = aSolid;
    aRBA.makeShape = aChamferBuilder;
    aResultBaseAlgoList.push_back(aRBA);
    aResultShapesList.push_back(aResult);
    anOriginalShapesList.push_back(aSolid);

    const std::string aFilletFaceName = "Chamfer";
    ListOfShape::iterator aSelectedBase = aFilletEdges.begin();
    for(; aSelectedBase != aFilletEdges.end(); aSelectedBase++) {
      GeomShapePtr aBase = *aSelectedBase;
      // Store new faces generated from edges and vertices
      aResultBody->loadGeneratedShapes(
        aChamferBuilder, aBase, GeomAPI_Shape::EDGE, aFilletFaceName, true);
    }
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList,
      anOriginalShapesList, aResultShapesCompound);

  removeResults(aResultIndex);
}
