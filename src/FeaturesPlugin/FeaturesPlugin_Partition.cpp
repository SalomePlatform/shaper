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

#include "FeaturesPlugin_Partition.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <iostream>
#include <list>
#include <sstream>


//=================================================================================================
FeaturesPlugin_Partition::FeaturesPlugin_Partition()
{
}

//=================================================================================================
void FeaturesPlugin_Partition::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  initVersion(THE_VERSION_1, selectionList(BASE_OBJECTS_ID()));
}

//=================================================================================================
void FeaturesPlugin_Partition::execute()
{
  ObjectHierarchy anObjects;
  ListOfShape aPlanes;

  // Getting objects.
  processAttribute(BASE_OBJECTS_ID(), anObjects, aPlanes);
  if(anObjects.IsEmpty()) {
    static const std::string aFeatureError = "Error: No objects for partition.";
    setError(aFeatureError);
    return;
  }

  ListOfShape aBaseObjects = anObjects.Objects();
  aBaseObjects.insert(aBaseObjects.end(), aPlanes.begin(), aPlanes.end());

  // resize planes to the bounding box of operated shapes
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  resizePlanes(anObjects.Objects(), aPlanes, aMakeShapeList);

  // cut unused solids of composolids from the objects of partition
  ListOfShape aTargetObjects, anUnusedSubs;
  std::string aError;
  if (!cutSubs(anObjects, aTargetObjects, anUnusedSubs, aMakeShapeList, aError)) {
    setError(aError);
    return;
  }
  aBaseObjects.insert(aBaseObjects.end(), anUnusedSubs.begin(), anUnusedSubs.end());

  // perform partition first time to split target solids by planes
  std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo(
      new GeomAlgoAPI_Partition(aTargetObjects, aPlanes));

  // Checking that the algorithm worked properly.
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPartitionAlgo, getKind(), aError)) {
    setError(aError);
    return;
  }

  aMakeShapeList->appendAlgo(aPartitionAlgo);
  GeomShapePtr aResultShape = aPartitionAlgo->shape();

  if (!anUnusedSubs.empty()) {
    // second pass of a partition to split shared faces of compsolids
    aTargetObjects.clear();
    aTargetObjects.push_back(aResultShape);
    aTargetObjects.insert(aTargetObjects.end(), anUnusedSubs.begin(), anUnusedSubs.end());

    aPartitionAlgo.reset(new GeomAlgoAPI_Partition(aTargetObjects, ListOfShape()));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPartitionAlgo, getKind(), aError)) {
      setError(aError);
      return;
    }

    aMakeShapeList->appendAlgo(aPartitionAlgo);
    aResultShape = aPartitionAlgo->shape();
  }

  int aResultIndex = 0;

  int aPartitionVersion = version();
  if (aPartitionVersion < THE_VERSION_1) {
    // default behaviors of Partition
    if(aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
      for(GeomAPI_ShapeIterator anIt(aResultShape); anIt.more(); anIt.next()) {
        storeResult(aBaseObjects, aPlanes, anIt.current(), aMakeShapeList, aResultIndex);
        ++aResultIndex;
      }
    } else {
      storeResult(aBaseObjects, aPlanes, aResultShape, aMakeShapeList, aResultIndex);
      ++aResultIndex;
    }
  }
  else {
    // merge hierarchies of compounds containing objects and tools
    GeomShapePtr aFirstShape = aResultShape;
    GeomAPI_ShapeIterator anIt;
    if (aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
      anIt = GeomAPI_ShapeIterator(aResultShape);
      aFirstShape = anIt.current();
      anIt.next();
    }

    GeomShapePtr aResultCompound =
      keepUnusedSubsOfCompound(aFirstShape, anObjects, ObjectHierarchy(), aMakeShapeList);

    if (anIt.more()) {
      if (aResultCompound->shapeType() != GeomAPI_Shape::COMPOUND) {
        // put the shape into compound
        ListOfShape aShapes;
        aShapes.push_back(aResultCompound);
        aResultCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
      }
      std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder);
      for (; anIt.more(); anIt.next())
        aBuilder->add(aResultCompound, anIt.current());
      aMakeShapeList->appendAlgo(aBuilder);
    }

    storeResult(aBaseObjects, aPlanes, aResultCompound, aMakeShapeList, aResultIndex);
    ++aResultIndex;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Partition::storeResult(
  ListOfShape& theObjects, ListOfShape& thePlanes,
  const GeomShapePtr theResultShape,
  const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
  const int theIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // if result is same as one of the base object, no modification was performed
  for(ListOfShape::const_iterator anObj = theObjects.cbegin(); anObj != theObjects.cend(); ++anObj)
  {
    if (anObj->get() && (*anObj)->isSame(theResultShape)) {
      aResultBody->store(theResultShape, false);
      setResult(aResultBody, theIndex);
      return;
    }
  }

  aResultBody->storeModified(theObjects, theResultShape, theMakeShape);

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = theMakeShape->mapOfSubShapes();
  theObjects.insert(theObjects.end(), thePlanes.begin(), thePlanes.end());
  for (ListOfShape::const_iterator anIt = theObjects.cbegin();
       anIt != theObjects.cend();
       ++anIt)
  {
    GeomShapePtr aShape = *anIt;
    aResultBody->loadModifiedShapes(theMakeShape, aShape, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(theMakeShape, aShape, GeomAPI_Shape::FACE);
    aResultBody->loadDeletedShapes(theMakeShape, aShape, GeomAPI_Shape::FACE);
  }

  setResult(aResultBody, theIndex);
}


//=================================================================================================

static bool cutSubs(ListOfShape& theSubsToCut,
                    const ListOfShape& theTools,
                    std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theMakeShapeList,
                    std::string& theError)
{
  if (theSubsToCut.empty() || theTools.empty())
    return true;

  // cut from current list of solids
  std::shared_ptr<GeomAlgoAPI_MakeShape> aCutAlgo(
      new GeomAlgoAPI_Boolean(theSubsToCut, theTools, GeomAlgoAPI_Tools::BOOL_CUT));
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCutAlgo, "", theError))
    return false;
  theMakeShapeList->appendAlgo(aCutAlgo);

  // update list of un-selected objects of the partition
  GeomAPI_Shape::ShapeType aType = theSubsToCut.front()->shapeType();
  theSubsToCut.clear();
  for (GeomAPI_ShapeExplorer anExp(aCutAlgo->shape(), aType); anExp.more(); anExp.next())
    theSubsToCut.push_back(anExp.current());
  return true;
}

bool FeaturesPlugin_Partition::cutSubs(
    FeaturesPlugin_Partition::ObjectHierarchy& theHierarchy,
    ListOfShape& theUsed,
    ListOfShape& theNotUsed,
    std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theMakeShapeList,
    std::string& theError)
{
  theUsed.clear();
  theNotUsed.clear();

  ObjectHierarchy::Iterator anIt = theHierarchy.Begin();

  // compose a set of tools for the CUT operation:
  // find the list of unused subs of the first argument or use itself
  ListOfShape aToolsForUsed, aToolsForUnused;
  GeomShapePtr aFirstArgument = theHierarchy.Parent(*anIt, false);
  if (aFirstArgument && aFirstArgument->shapeType() == GeomAPI_Shape::COMPSOLID) {
    theHierarchy.SplitCompound(aFirstArgument, theUsed, aToolsForUsed);
    theNotUsed = aToolsForUsed;
  }
  else {
    aFirstArgument = *anIt;
    theUsed.push_back(aFirstArgument);
  }
  aToolsForUnused.push_back(aFirstArgument);

  // cut subs
  bool isOk = true;
  for (++anIt; anIt != theHierarchy.End() && isOk; ++anIt) {
    ListOfShape aUsed, aNotUsed;

    GeomShapePtr aParent = theHierarchy.Parent(*anIt, false);
    if (aParent && aParent->shapeType() == GeomAPI_Shape::COMPSOLID) {
      aParent = theHierarchy.Parent(*anIt); // get parent once again to mark its subs as processed
      theHierarchy.SplitCompound(aParent, aUsed, aNotUsed);
    }
    else
      aUsed.push_back(*anIt);

    isOk = ::cutSubs(aUsed, aToolsForUsed, theMakeShapeList, theError)
        && ::cutSubs(aNotUsed, aToolsForUnused, theMakeShapeList, theError);
    if (isOk) {
      theUsed.insert(theUsed.end(), aUsed.begin(), aUsed.end());
      theNotUsed.insert(theNotUsed.end(), aNotUsed.begin(), aNotUsed.end());
    }
  }

  return isOk;
}
