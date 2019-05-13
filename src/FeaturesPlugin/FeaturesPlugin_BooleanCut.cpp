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

#include "FeaturesPlugin_BooleanCut.h"

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

static const int THE_CUT_VERSION_1 = 20190506;

//==================================================================================================
FeaturesPlugin_BooleanCut::FeaturesPlugin_BooleanCut()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_CUT)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanCut::initAttributes()
{
  FeaturesPlugin_Boolean::initAttributes();
  initVersion(THE_CUT_VERSION_1);
}

//==================================================================================================
void FeaturesPlugin_BooleanCut::execute()
{
  ObjectHierarchy anObjects, aTools;
  ListOfShape aPlanes;

  // Getting objects and tools
  if (!processAttribute(OBJECT_LIST_ID(), anObjects, aPlanes) ||
      !processAttribute(TOOL_LIST_ID(), aTools, aPlanes))
    return;

  int aResultIndex = 0;

  if(anObjects.IsEmpty() || aTools.IsEmpty()) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  // version of CUT feature
  int aCutVersion = version();

  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;
  std::string anError;

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());

  GeomShapePtr aResultCompound;
  if (aCutVersion == THE_CUT_VERSION_1) {
    // merge hierarchies of compounds containing objects and tools
    aResultCompound =
        keepUnusedSubsOfCompound(GeomShapePtr(), anObjects, aTools, aMakeShapeList);
  }

  // For solids cut each object with all tools.
  bool isOk = true;
  for (ObjectHierarchy::Iterator anObjectsIt = anObjects.Begin();
       anObjectsIt != anObjects.End() && isOk;
       ++anObjectsIt) {
    GeomShapePtr anObject = *anObjectsIt;
    GeomShapePtr aParent = anObjects.Parent(anObject);

    if (aParent) {
      GeomAPI_Shape::ShapeType aShapeType = aParent->shapeType();
      if (aShapeType == GeomAPI_Shape::COMPOUND) {
        // Compound handling
        isOk = processCompound(GeomAlgoAPI_Tools::BOOL_CUT,
                               anObjects, aParent, aTools.Objects(),
                               aResultIndex, aResultBaseAlgoList, aResultShapesList,
                               aResultCompound);
      }
      else if (aShapeType == GeomAPI_Shape::COMPSOLID) {
        // Compsolid handling
        isOk = processCompsolid(GeomAlgoAPI_Tools::BOOL_CUT,
                                anObjects, aParent, aTools.Objects(), ListOfShape(),
                                aResultIndex, aResultBaseAlgoList, aResultShapesList,
                                aResultCompound);
      }
    } else {
      // process object as is
      isOk = processObject(GeomAlgoAPI_Tools::BOOL_CUT,
                           anObject, aTools.Objects(), aPlanes,
                           aResultIndex, aResultBaseAlgoList, aResultShapesList,
                           aResultCompound);
    }
  }

  GeomAPI_ShapeIterator aShapeIt(aResultCompound);
  if (aShapeIt.more()) {
    std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

    ListOfShape anObjectList = anObjects.Objects();
    ListOfShape aToolsList = aTools.Objects();
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                             anObjectList,
                                             aToolsList,
                                             aMakeShapeList,
                                             aResultCompound);
    setResult(aResultBody, aResultIndex++);

    // merge algorithms
    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = anObjectList.front();
    for (std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>::iterator
         aRBAIt = aResultBaseAlgoList.begin();
         aRBAIt != aResultBaseAlgoList.end(); ++aRBAIt) {
      aMakeShapeList->appendAlgo(aRBAIt->makeShape);
    }
    aRBA.makeShape = aMakeShapeList;
    aResultBaseAlgoList.clear();
    aResultBaseAlgoList.push_back(aRBA);
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  if (!aResultCompound)
    aResultCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList,
                                          aTools.Objects(),
                                          aResultCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
