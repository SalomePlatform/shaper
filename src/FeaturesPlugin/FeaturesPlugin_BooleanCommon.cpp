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

#include "FeaturesPlugin_BooleanCommon.h"

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
    if (aResCompSolidPtr.get())
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

  if ((anObjects.empty() && aCompSolidsObjects.empty())
      || (!isSimpleMode && aTools.empty() && aPlanes.empty())) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  int aResultIndex = 0;
  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  GeomAPI_DataMapOfShapeShape aMapOfShapes;

  if (isSimpleMode)
  {
    ListOfShape::iterator anObjectsIt = anObjects.begin();
    GeomShapePtr aShape = *anObjectsIt;
    for (++anObjectsIt; anObjectsIt != anObjects.end(); ++anObjectsIt) {
      std::shared_ptr<GeomAlgoAPI_Boolean> aCommonAlgo(
        new GeomAlgoAPI_Boolean(aShape,
                                *anObjectsIt,
                                GeomAlgoAPI_Boolean::BOOL_COMMON));

      if (!aCommonAlgo->isDone()) {
        std::string aFeatureError = "Error: An algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if (aCommonAlgo->shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if (!aCommonAlgo->isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }

      aShape = aCommonAlgo->shape();
      aMakeShapeList.appendAlgo(aCommonAlgo);
      aMapOfShapes.merge(aCommonAlgo->mapOfSubShapes());
    }

    GeomAPI_ShapeIterator aShapeIt(aShape);
    if (aShapeIt.more() || aShape->shapeType() == GeomAPI_Shape::VERTEX) {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

      loadNamingDS(aResultBody, anObjects.front(), anObjects, aShape, aMakeShapeList, aMapOfShapes);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
    }
  } else {
    for (ListOfShape::iterator anObjectsIt = anObjects.begin();
         anObjectsIt != anObjects.end();
         ++anObjectsIt)
    {
      std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
      ListOfShape aListWithObject;
      aListWithObject.push_back(anObject);
      GeomAlgoAPI_MakeShapeList aMakeShapeList;
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
        aMakeShapeList.appendAlgo(aMkShCustom);
        aToolsWithPlanes.push_back(aTool);
      }

      aBoolAlgo.reset(new GeomAlgoAPI_Boolean(aListWithObject,
        aToolsWithPlanes,
        GeomAlgoAPI_Boolean::BOOL_COMMON));
      aResShape = aBoolAlgo->shape();

      // Checking that the algorithm worked properly.
      if (!aBoolAlgo->isDone()) {
        static const std::string aFeatureError = "Error: Boolean algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if (aResShape->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if (!aBoolAlgo->isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }

      aMakeShapeList.appendAlgo(aBoolAlgo);

      GeomAPI_ShapeIterator aShapeIt(aResShape);
      if (aShapeIt.more() || aResShape->shapeType() == GeomAPI_Shape::VERTEX) {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody =
          document()->createBody(data(), aResultIndex);

        loadNamingDS(aResultBody, anObject, aTools, aResShape,
                     aMakeShapeList, *(aBoolAlgo->mapOfSubShapes()));
        setResult(aResultBody, aResultIndex);
        aResultIndex++;
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
      if (!aCommonAlgo->isDone()) {
        static const std::string aFeatureError = "Error: Boolean algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if (aCommonAlgo->shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if (!aCommonAlgo->isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }

      aMakeShapeList->appendAlgo(aCommonAlgo);
      GeomAPI_DataMapOfShapeShape aMapOfShapes;
      aMapOfShapes.merge(aCommonAlgo->mapOfSubShapes());
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
        aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());
        aResultShape = aFillerAlgo->shape();
      }

      GeomAPI_ShapeIterator aShapeIt(aResultShape);
      if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX)
      {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody =
          document()->createBody(data(), aResultIndex);

        loadNamingDS(aResultBody,
          aCompSolid,
          aTools,
          aResultShape,
          *aMakeShapeList,
          aMapOfShapes);
        setResult(aResultBody, aResultIndex);
        aResultIndex++;
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
      if (!aCommonAlgo->isDone()) {
        static const std::string aFeatureError = "Error: Boolean algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if (aCommonAlgo->shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if (!aCommonAlgo->isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }

      aMakeShapeList->appendAlgo(aCommonAlgo);
      GeomAPI_DataMapOfShapeShape aMapOfShapes;
      aMapOfShapes.merge(aCommonAlgo->mapOfSubShapes());
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

        loadNamingDS(aResultBody,
          aCompound,
          aTools,
          aResultShape,
          *aMakeShapeList,
          aMapOfShapes);
        setResult(aResultBody, aResultIndex);
        aResultIndex++;
      }
    }

  }

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//==================================================================================================
void FeaturesPlugin_BooleanCommon::loadNamingDS(ResultBodyPtr theResultBody,
                                                const GeomShapePtr theBaseShape,
                                                const ListOfShape& theTools,
                                                const GeomShapePtr theResultShape,
                                                GeomAlgoAPI_MakeShape& theMakeShape,
                                                GeomAPI_DataMapOfShapeShape& theMapOfShapes)
{
  //load result
  if (theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape, false);
  } else {
    const int aModifyVTag = 1;
    const int aModifyETag = 2;
    const int aModifyFTag = 3;
    const int aDeletedTag = 4;
    /// sub solids will be placed at labels 5, 6, etc. if result is compound of solids
    const int aSubsolidsTag = 5;

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    const std::string aModVName = "Modified_Vertex";
    const std::string aModEName = "Modified_Edge";
    const std::string aModFName = "Modified_Face";

    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::VERTEX,
                                               aModifyVTag, aModVName, theMapOfShapes, false,
                                               false, true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::EDGE,
                                               aModifyETag, aModEName, theMapOfShapes, false,
                                               false, true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModifyFTag, aModFName, theMapOfShapes, false,
                                               false, true);

    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::VERTEX, aDeletedTag);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::EDGE, aDeletedTag);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::FACE, aDeletedTag);

    for (ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++)
    {
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX,
                                                 aModifyVTag, aModVName, theMapOfShapes, false,
                                                 false, true);

      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE,
                                                 aModifyETag, aModEName, theMapOfShapes, false,
                                                 false, true);

      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE,
                                                 aModifyFTag, aModFName, theMapOfShapes, false,
                                                 false, true);

      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX, aDeletedTag);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE, aDeletedTag);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
    }
  }
}
