// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_BooleanCommon.h"

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Tools.h>


//==================================================================================================
FeaturesPlugin_BooleanCommon::FeaturesPlugin_BooleanCommon()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_COMMON)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanCommon::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//==================================================================================================
void FeaturesPlugin_BooleanCommon::execute()
{
  ListOfShape anObjects, aTools, aPlanes;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompoundObjects;

  bool isSimpleMode = false;

  AttributeStringPtr aCreationMethodAttr = string(CREATION_METHOD());
  if (aCreationMethodAttr.get()
      && aCreationMethodAttr->value() == CREATION_METHOD_SIMPLE()) {
    isSimpleMode = true;
  }

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
    if (!isSimpleMode
        && aResCompSolidPtr.get())
    {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      GeomAPI_Shape::ShapeType aShapeType = aResCompSolidPtr->shape()->shapeType();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>& aMap =
        aShapeType == GeomAPI_Shape::COMPSOLID ? aCompSolidsObjects : aCompoundObjects;

      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
        anIt = aMap.begin();
      for (; anIt != aMap.end(); anIt++) {
        if (anIt->first->isEqual(aContextShape)) {
          aMap[anIt->first].push_back(anObject);
          break;
        }
      }
      if (anIt == aMap.end()) {
        aMap[aContextShape].push_back(anObject);
      }

    }
    else {
      anObjects.push_back(anObject);
    }
  }

  // Getting tools.
  if (!isSimpleMode) {
    AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
    for (int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
      AttributeSelectionPtr aToolAttr = aToolsSelList->value(aToolsIndex);
      GeomShapePtr aTool = aToolAttr->value();
      if (!aTool.get()) {
        // It could be a construction plane.
        ResultPtr aContext = aToolAttr->context();
        aPlanes.push_back(aToolAttr->context()->shape());
      } else {
        aTools.push_back(aTool);
      }
    }
  }

  if ((anObjects.empty() && aCompSolidsObjects.empty() && aCompoundObjects.empty())
      || (!isSimpleMode && aTools.empty() && aPlanes.empty())) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  int aResultIndex = 0;
  std::string anError;
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;

  if (isSimpleMode)
  {
    ListOfShape::iterator anObjectsIt = anObjects.begin();
    GeomShapePtr aShape = *anObjectsIt;
    for (++anObjectsIt; anObjectsIt != anObjects.end(); ++anObjectsIt) {
      std::shared_ptr<GeomAlgoAPI_Boolean> aCommonAlgo(
        new GeomAlgoAPI_Boolean(aShape,
                                *anObjectsIt,
                                GeomAlgoAPI_Boolean::BOOL_COMMON));

      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCommonAlgo, getKind(), anError)) {
        setError(anError);
        return;
      }

      aShape = aCommonAlgo->shape();
      aMakeShapeList->appendAlgo(aCommonAlgo);
    }

    GeomAPI_ShapeIterator aShapeIt(aShape);
    if (aShapeIt.more() || aShape->shapeType() == GeomAPI_Shape::VERTEX) {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

      ListOfShape anEmptyTools;
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               anObjects,
                                               anEmptyTools,
                                               aMakeShapeList,
                                               aShape);
      GeomShapePtr aBaseShape = anObjects.front();
      anObjects.pop_front();
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      aTools = anObjects;
      FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = aBaseShape;
      aRBA.makeShape = aMakeShapeList;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back(aShape);
    }
  } else {
    for (ListOfShape::iterator anObjectsIt = anObjects.begin();
         anObjectsIt != anObjects.end();
         ++anObjectsIt)
    {
      std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
      ListOfShape aListWithObject;
      aListWithObject.push_back(anObject);
      std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
      std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
      GeomShapePtr aResShape;

      std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints =
        GeomAlgoAPI_ShapeTools::getBoundingBox(aListWithObject, 1.0);

      // Resize planes.
      ListOfShape aToolsWithPlanes = aTools;
      for (ListOfShape::const_iterator anIt = aPlanes.cbegin();
           anIt != aPlanes.cend();
           ++anIt) {
        GeomShapePtr aPlane = *anIt;
        GeomShapePtr aTool = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aPlane, aBoundingPoints);
        std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMkShCustom(
          new GeomAlgoAPI_MakeShapeCustom);
        aMkShCustom->addModified(aPlane, aTool);
        aMakeShapeList->appendAlgo(aMkShCustom);
        aToolsWithPlanes.push_back(aTool);
      }

      aBoolAlgo.reset(new GeomAlgoAPI_Boolean(aListWithObject,
        aToolsWithPlanes,
        GeomAlgoAPI_Boolean::BOOL_COMMON));
      aResShape = aBoolAlgo->shape();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
        setError(anError);
        return;
      }

      aMakeShapeList->appendAlgo(aBoolAlgo);

      GeomAPI_ShapeIterator aShapeIt(aResShape);
      if (aShapeIt.more() || aResShape->shapeType() == GeomAPI_Shape::VERTEX) {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody =
          document()->createBody(data(), aResultIndex);
        ListOfShape anObjectList;
        anObjectList.push_back(anObject);
        FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                                 anObjectList,
                                                 aTools,
                                                 aMakeShapeList,
                                                 aResShape);
        setResult(aResultBody, aResultIndex);
        aResultIndex++;

        FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
        aRBA.resultBody = aResultBody;
        aRBA.baseShape = anObject;
        aRBA.makeShape = aMakeShapeList;
        aResultBaseAlgoList.push_back(aRBA);
        aResultShapesList.push_back(aResShape);
      }
    }

    // Compsolids handling
    for (std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
      anIt = aCompSolidsObjects.begin();
      anIt != aCompSolidsObjects.end();
      ++anIt)
    {
      std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
      ListOfShape& aUsedInOperationSolids = anIt->second;

      // Collecting solids from compsolids which will not be modified in boolean operation.
      ListOfShape aNotUsedSolids;
      for (GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID);
        anExp.more();
        anExp.next())
      {
        std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
        ListOfShape::iterator aUsedIt = aUsedInOperationSolids.begin();
        for (; aUsedIt != aUsedInOperationSolids.end(); aUsedIt++) {
          if (aSolidInCompSolid->isEqual(*aUsedIt)) {
            break;
          }
        }
        if (aUsedIt == aUsedInOperationSolids.end()) {
          aNotUsedSolids.push_back(aSolidInCompSolid);
        }
      }

      std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
      std::shared_ptr<GeomAlgoAPI_Boolean> aCommonAlgo(
        new GeomAlgoAPI_Boolean(aUsedInOperationSolids,
          aTools,
          GeomAlgoAPI_Boolean::BOOL_COMMON));

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCommonAlgo, getKind(), anError)) {
        setError(anError);
        return;
      }

      aMakeShapeList->appendAlgo(aCommonAlgo);
      GeomShapePtr aResultShape = aCommonAlgo->shape();

      // Add result to not used solids from compsolid.
      if (!aNotUsedSolids.empty()) {
        ListOfShape aShapesToAdd = aNotUsedSolids;
        aShapesToAdd.push_back(aCommonAlgo->shape());
        std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
          new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
        if (!aFillerAlgo->isDone()) {
          std::string aFeatureError = "Error: PaveFiller algorithm failed.";
          setError(aFeatureError);
          return;
        }

        aMakeShapeList->appendAlgo(aFillerAlgo);
        aResultShape = aFillerAlgo->shape();
      }

      GeomAPI_ShapeIterator aShapeIt(aResultShape);
      if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX)
      {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody =
          document()->createBody(data(), aResultIndex);

        ListOfShape aCompSolidList;
        aCompSolidList.push_back(aCompSolid);
        FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                                 aCompSolidList,
                                                 aTools,
                                                 aMakeShapeList,
                                                 aResultShape);
        setResult(aResultBody, aResultIndex);
        aResultIndex++;

        FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
        aRBA.resultBody = aResultBody;
        aRBA.baseShape = aCompSolid;
        aRBA.makeShape = aMakeShapeList;
        aResultBaseAlgoList.push_back(aRBA);
        aResultShapesList.push_back(aResultShape);
      }
    }

    // Compounds handling
    for (std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
      anIt = aCompoundObjects.begin();
      anIt != aCompoundObjects.end();
      ++anIt)
    {
      std::shared_ptr<GeomAPI_Shape> aCompound = anIt->first;
      ListOfShape& aUsedInOperationShapes = anIt->second;

      // Collecting shapes from compound which will not be modified in boolean operation.
      ListOfShape aNotUsedShapes;
      for (GeomAPI_ShapeIterator aCompIt(aCompound);
        aCompIt.more();
        aCompIt.next())
      {
        std::shared_ptr<GeomAPI_Shape> aShapeInCompound = aCompIt.current();
        ListOfShape::iterator aUsedIt = aUsedInOperationShapes.begin();
        for (; aUsedIt != aUsedInOperationShapes.end(); aUsedIt++) {
          if (aShapeInCompound->isEqual(*aUsedIt)) {
            break;
          }
        }
        if (aUsedIt == aUsedInOperationShapes.end()) {
          aNotUsedShapes.push_back(aShapeInCompound);
        }
      }

      std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
      std::shared_ptr<GeomAlgoAPI_Boolean> aCommonAlgo(
        new GeomAlgoAPI_Boolean(aUsedInOperationShapes,
          aTools,
          GeomAlgoAPI_Boolean::BOOL_COMMON));

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCommonAlgo, getKind(), anError)) {
        setError(anError);
        return;
      }

      aMakeShapeList->appendAlgo(aCommonAlgo);
      GeomShapePtr aResultShape = aCommonAlgo->shape();

      // Add result to not used shape from compound.
      if (!aNotUsedShapes.empty()) {
        ListOfShape aShapesForResult = aNotUsedShapes;
        if (aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
          for (GeomAPI_ShapeIterator aResultIt(aResultShape); aResultIt.more(); aResultIt.next()) {
            aShapesForResult.push_back(aResultIt.current());
          }
        }
        else {
          aShapesForResult.push_back(aResultShape);
        }

        if (aShapesForResult.size() == 1) {
          aResultShape = aShapesForResult.front();
        }
        else {
          aResultShape = GeomAlgoAPI_CompoundBuilder::compound(aShapesForResult);
        }
      }

      GeomAPI_ShapeIterator aShapeIt(aResultShape);
      if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX) {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody =
          document()->createBody(data(), aResultIndex);

        ListOfShape aCompoundList;
        aCompoundList.push_back(aCompound);
        FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                                 aCompoundList,
                                                 aTools,
                                                 aMakeShapeList,
                                                 aResultShape);
        setResult(aResultBody, aResultIndex);
        aResultIndex++;

        FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
        aRBA.resultBody = aResultBody;
        aRBA.baseShape = aCompound;
        aRBA.makeShape = aMakeShapeList;
        aResultBaseAlgoList.push_back(aRBA);
        aResultShapesList.push_back(aResultShape);
      }
    }

  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList, aTools, aResultShapesCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
