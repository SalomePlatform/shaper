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

#include "FeaturesPlugin_BooleanCommon.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

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

  initVersion(BOP_VERSION_9_4(), selectionList(OBJECT_LIST_ID()), selectionList(TOOL_LIST_ID()));
}

//==================================================================================================
void FeaturesPlugin_BooleanCommon::execute()
{
  ListOfShape aPlanes;
  GeomAPI_ShapeHierarchy anObjects, aTools;

  bool isSimpleMode = false;

  AttributeStringPtr aCreationMethodAttr = string(CREATION_METHOD());
  if (aCreationMethodAttr.get()
      && aCreationMethodAttr->value() == CREATION_METHOD_SIMPLE()) {
    isSimpleMode = true;
  }

  // Getting objects.
  if (!processAttribute(OBJECT_LIST_ID(), anObjects, aPlanes))
    return;
  // Planes are not supported as objects of COMMON operation
  aPlanes.clear();

  // Getting tools.
  if (!isSimpleMode &&
      !processAttribute(TOOL_LIST_ID(), aTools, aPlanes))
    return;

  if (anObjects.empty() || (!isSimpleMode && aTools.empty() && aPlanes.empty())) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  // version of COMMON feature
  const std::string aCommonVersion = data()->version();

  int aResultIndex = 0;
  GeomShapePtr aResultCompound;

  std::string anError;
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;

  if (isSimpleMode) {
    GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
    GeomShapePtr aShape = *anObjectsIt;
    for (++anObjectsIt; anObjectsIt != anObjects.end(); ++anObjectsIt) {
      std::shared_ptr<GeomAlgoAPI_Boolean> aCommonAlgo(
        new GeomAlgoAPI_Boolean(aShape,
                                *anObjectsIt,
                                GeomAlgoAPI_Tools::BOOL_COMMON));

      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCommonAlgo, getKind(), anError)) {
        setError(anError);
        return;
      }

      aShape = aCommonAlgo->shape();
      aMakeShapeList->appendAlgo(aCommonAlgo);
    }

    if (aCommonVersion == BOP_VERSION_9_4()) {
      // merge hierarchies of compounds containing objects and tools
      // and append the result of the FUSE operation
      aShape = keepUnusedSubsOfCompound(aShape, anObjects, aTools, aMakeShapeList);
    }

    GeomAPI_ShapeIterator aShapeIt(aShape);
    if (aShapeIt.more() || aShape->shapeType() == GeomAPI_Shape::VERTEX) {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

      ListOfShape anObjectList = anObjects.objects();
      ListOfShape aToolsList;
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               anObjectList,
                                               aToolsList,
                                               aMakeShapeList,
                                               aShape);
      GeomShapePtr aBaseShape = anObjectList.front();
      anObjectList.pop_front();
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      aToolsList = anObjectList;
      FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = aBaseShape;
      aRBA.makeShape = aMakeShapeList;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back(aShape);
    }
  } else {
    if (aCommonVersion == BOP_VERSION_9_4()) {
      // merge hierarchies of compounds containing objects and tools
      aResultCompound =
          keepUnusedSubsOfCompound(GeomShapePtr(), anObjects, aTools, aMakeShapeList);
    }

    bool isOk = true;
    for (GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
         anObjectsIt != anObjects.end() && isOk;
         ++anObjectsIt)
    {
      GeomShapePtr anObject = *anObjectsIt;
      GeomShapePtr aParent = anObjects.parent(anObject);

      if (aParent) {
        GeomAPI_Shape::ShapeType aShapeType = aParent->shapeType();
        if (aShapeType == GeomAPI_Shape::COMPOUND) {
          // Compound handling
          isOk = processCompound(GeomAlgoAPI_Tools::BOOL_COMMON,
                                 anObjects, aParent, aTools.objects(),
                                 aResultIndex, aResultBaseAlgoList, aResultShapesList,
                                 aResultCompound);
        }
        else if (aShapeType == GeomAPI_Shape::COMPSOLID) {
          // Compsolid handling
          isOk = processCompsolid(GeomAlgoAPI_Tools::BOOL_COMMON,
                                  anObjects, aParent, aTools.objects(), ListOfShape(),
                                  aResultIndex, aResultBaseAlgoList, aResultShapesList,
                                  aResultCompound);
        }
      } else {
        // process object as is
        isOk = processObject(GeomAlgoAPI_Tools::BOOL_COMMON,
                             anObject, aTools.objects(), aPlanes,
                             aResultIndex, aResultBaseAlgoList, aResultShapesList,
                             aResultCompound);
      }
    }

    storeResult(anObjects.objects(), aTools.objects(), aResultCompound, aResultIndex,
                aMakeShapeList, aResultBaseAlgoList);
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  if (!aResultCompound)
    aResultCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList,
                                          aTools.objects(),
                                          aResultCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
