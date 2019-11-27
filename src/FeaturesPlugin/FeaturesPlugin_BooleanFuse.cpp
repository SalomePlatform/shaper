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

#include "FeaturesPlugin_BooleanFuse.h"

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_UnifySameDomain.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

//==================================================================================================
FeaturesPlugin_BooleanFuse::FeaturesPlugin_BooleanFuse()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_FUSE)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanFuse::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(REMOVE_INTERSECTION_EDGES_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), OBJECT_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());

  initVersion(THE_VERSION_1, selectionList(OBJECT_LIST_ID()), selectionList(TOOL_LIST_ID()));
}

//==================================================================================================
void FeaturesPlugin_BooleanFuse::execute()
{
  std::string anError;
  ObjectHierarchy anObjectsHierarchy, aToolsHierarchy;
  ListOfShape aPlanes;

  bool isSimpleCreation = false;

  AttributeStringPtr aCreationMethodAttr = string(CREATION_METHOD());
  if (aCreationMethodAttr.get()
      && aCreationMethodAttr->value() == CREATION_METHOD_SIMPLE())
  {
    isSimpleCreation = true;
  }

  // Getting objects.
  if (!processAttribute(OBJECT_LIST_ID(), anObjectsHierarchy, aPlanes))
    return;

  // Getting tools.
  if (!isSimpleCreation &&
      !processAttribute(TOOL_LIST_ID(), aToolsHierarchy, aPlanes))
    return;

  ListOfShape anObjects, aTools, anEdgesAndFaces;
  // all objects except edges and faces
  anObjectsHierarchy.ObjectsByType(anEdgesAndFaces, anObjects,
                                   GeomAPI_Shape::FACE, GeomAPI_Shape::EDGE);
  aToolsHierarchy.ObjectsByType(anEdgesAndFaces, aTools,
                                GeomAPI_Shape::FACE, GeomAPI_Shape::EDGE);

  if ((anObjects.size() + aTools.size() + anEdgesAndFaces.size()) < 2) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  // version of FUSE feature
  int aFuseVersion = version();

  // Collecting all solids which will be fused.
  ListOfShape aSolidsToFuse;
  aSolidsToFuse.insert(aSolidsToFuse.end(), anObjects.begin(), anObjects.end());
  aSolidsToFuse.insert(aSolidsToFuse.end(), aTools.begin(), aTools.end());

  // Collecting solids from compsolids which will not be modified
  // in boolean operation and will be added to result.
  ListOfShape aShapesToAdd;
  for (ObjectHierarchy::Iterator anObjectsIt = anObjectsHierarchy.Begin();
       !isSimpleCreation && anObjectsIt != anObjectsHierarchy.End();
       ++anObjectsIt) {
    GeomShapePtr anObject = *anObjectsIt;
    GeomShapePtr aParent = anObjectsHierarchy.Parent(anObject, false);

    if (aParent && aParent->shapeType() == GeomAPI_Shape::COMPSOLID) {
      // mark all subs of this parent as precessed to avoid handling twice
      aParent = anObjectsHierarchy.Parent(anObject);

      ListOfShape aUsed, aNotUsed;
      anObjectsHierarchy.SplitCompound(aParent, aUsed, aNotUsed);
      aShapesToAdd.insert(aShapesToAdd.end(), aNotUsed.begin(), aNotUsed.end());
    }
  }

  ListOfShape anOriginalShapes = aSolidsToFuse;
  anOriginalShapes.insert(anOriginalShapes.end(), aShapesToAdd.begin(), aShapesToAdd.end());

  // Cut edges and faces(if we have any) with solids.
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  GeomShapePtr aCuttedEdgesAndFaces;
  if (!anEdgesAndFaces.empty()) {
    std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(anEdgesAndFaces,
      anOriginalShapes, GeomAlgoAPI_Tools::BOOL_CUT));
    if (aCutAlgo->isDone()) {
      aCuttedEdgesAndFaces = aCutAlgo->shape();
      aMakeShapeList->appendAlgo(aCutAlgo);
    }
  }
  anOriginalShapes.insert(anOriginalShapes.end(), anEdgesAndFaces.begin(),
                          anEdgesAndFaces.end());

  // If we have compsolids then cut with not used solids all others.
  if (!aShapesToAdd.empty()) {
    aSolidsToFuse.clear();
    for (ListOfShape::iterator
         anIt = anOriginalShapes.begin(); anIt != anOriginalShapes.end(); anIt++) {
      ListOfShape aOneObjectList;
      aOneObjectList.push_back(*anIt);
      std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(
        new GeomAlgoAPI_Boolean(aOneObjectList, aShapesToAdd, GeomAlgoAPI_Tools::BOOL_CUT));

      if (GeomAlgoAPI_ShapeTools::volume(aCutAlgo->shape()) > 1.e-27) {
        aSolidsToFuse.push_back(aCutAlgo->shape());
        aMakeShapeList->appendAlgo(aCutAlgo);
      }
    }
  }

  if (!aSolidsToFuse.empty()) {
    anObjects.clear();
    anObjects.push_back(aSolidsToFuse.back());
    aSolidsToFuse.pop_back();
    aTools = aSolidsToFuse;
  }

  // Fuse all objects and all tools.
  GeomShapePtr aShape;
  if (anObjects.size() == 1 && aTools.empty()) {
    aShape = anObjects.front();
  } else if (anObjects.empty() && aTools.size() == 1) {
    aShape = aTools.front();
  } else if ((anObjects.size() + aTools.size()) > 1) {
    std::shared_ptr<GeomAlgoAPI_Boolean> aFuseAlgo(new GeomAlgoAPI_Boolean(anObjects,
      aTools,
      GeomAlgoAPI_Tools::BOOL_FUSE));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFuseAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aShape = aFuseAlgo->shape();
    aMakeShapeList->appendAlgo(aFuseAlgo);
  }

  // Combine result with not used solids from compsolid and edges and faces (if we have any).
  if (aCuttedEdgesAndFaces.get() && !aCuttedEdgesAndFaces->isNull()) {
    aShapesToAdd.push_back(aCuttedEdgesAndFaces);
  } else {
    aShapesToAdd.insert(aShapesToAdd.end(), anEdgesAndFaces.begin(), anEdgesAndFaces.end());
  }
  if (!aShapesToAdd.empty()) {
    if (aShape.get()) {
      aShapesToAdd.push_back(aShape);
    }
    std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
      new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFillerAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aShape = aFillerAlgo->shape();
    aMakeShapeList->appendAlgo(aFillerAlgo);
  }

  bool isRemoveEdges = false;
  AttributeBooleanPtr removeEdgesAttr = boolean(REMOVE_INTERSECTION_EDGES_ID());
  if (removeEdgesAttr.get()) {
    isRemoveEdges = removeEdgesAttr->value();
  }

  if (isRemoveEdges) {
    std::shared_ptr<GeomAlgoAPI_UnifySameDomain> aUnifyAlgo(
      new GeomAlgoAPI_UnifySameDomain(aShape));

    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aUnifyAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aShape = aUnifyAlgo->shape();
    aMakeShapeList->appendAlgo(aUnifyAlgo);
  }

  if (aFuseVersion == THE_VERSION_1) {
    // merge hierarchies of compounds containing objects and tools
    // and append the result of the FUSE operation
    aShape = keepUnusedSubsOfCompound(aShape, anObjectsHierarchy, aToolsHierarchy, aMakeShapeList);
  }

  int aResultIndex = 0;

  ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

  ListOfShape anEmptyTools;
  FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                           anOriginalShapes,
                                           anEmptyTools,
                                           aMakeShapeList,
                                           aShape);
  setResult(aResultBody, aResultIndex);
  aResultIndex++;

  FeaturesPlugin_Tools::loadDeletedShapes(aResultBody,
                                          GeomShapePtr(),
                                          anOriginalShapes,
                                          aMakeShapeList,
                                          aShape);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
