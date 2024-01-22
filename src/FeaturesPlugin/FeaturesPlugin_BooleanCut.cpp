// Copyright (C) 2014-2024  CEA, EDF
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

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
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


static const ListOfShape ExplodeCompounds(const ListOfShape &aList)
{
  ListOfShape subShapes;
  for (auto shp = aList.cbegin(); shp != aList.cend(); ++shp)
  {
    if ((*shp).get() && (*shp)->isCompound()) {
      // Use all sub shapes of the compound
      for (GeomAPI_ShapeIterator anExp(*shp); anExp.more(); anExp.next()) {
        GeomShapePtr aCurrent = anExp.current();
        subShapes.push_back(aCurrent);
      }
    }
    else
      subShapes.push_back(*shp);
  }

  return subShapes;
}

//==================================================================================================
FeaturesPlugin_BooleanCut::FeaturesPlugin_BooleanCut()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_CUT)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanCut::initAttributes()
{
  FeaturesPlugin_Boolean::initAttributes();
  initVersion(BOP_VERSION_9_4(), selectionList(OBJECT_LIST_ID()), selectionList(TOOL_LIST_ID()));
}

//==================================================================================================
void FeaturesPlugin_BooleanCut::execute()
{
  GeomAPI_ShapeHierarchy anObjects, aTools;
  ListOfShape aPlanes;

  // Getting objects and tools
  if (!processAttribute(OBJECT_LIST_ID(), anObjects, aPlanes) ||
      !processAttribute(TOOL_LIST_ID(), aTools, aPlanes))
    return;

  int aResultIndex = 0;

  if(anObjects.empty() || aTools.empty()) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  std::vector<ModelAPI_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;
  std::string anError;

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());

  GeomShapePtr aResultCompound;
  if (data()->version() == BOP_VERSION_9_4()) {
    // merge hierarchies of compounds containing objects and tools
    aResultCompound =
        keepUnusedSubsOfCompound(GeomShapePtr(), anObjects, aTools, aMakeShapeList);
  }

  // Getting fuzzy parameter.
  // Used as additional tolerance to eliminate tiny results.
  // Using -1 as fuzzy value in the GeomAlgoAPI means to ignore it during the boolean operation!
  bool aUseFuzzy = boolean(USE_FUZZY_ID())->value();
  double aFuzzy = (aUseFuzzy ? real(FUZZY_PARAM_ID())->value() : -1);

  // When selecting a compound tool object, use its exploded subshapes as tool object instead.
  const ListOfShape aToolList = ExplodeCompounds(aTools.objects());

  // For solids cut each object with all tools.
  bool isOk = true;
  for (GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
       anObjectsIt != anObjects.end() && isOk;
       ++anObjectsIt) {
    GeomShapePtr anObject = *anObjectsIt;
    GeomShapePtr aParent = anObjects.parent(anObject);

    if (aParent) {
      GeomAPI_Shape::ShapeType aShapeType = aParent->shapeType();
      if (aShapeType == GeomAPI_Shape::COMPOUND) {
        // Compound handling
        isOk = processCompound(GeomAlgoAPI_Tools::BOOL_CUT,
                               anObjects, aParent, aToolList,
                               aFuzzy,
                               aResultIndex, aResultBaseAlgoList, aResultShapesList,
                               aResultCompound);
      }
      else if (aShapeType == GeomAPI_Shape::COMPSOLID) {
        // Compsolid handling
        isOk = processCompsolid(GeomAlgoAPI_Tools::BOOL_CUT,
                                anObjects, aParent, aToolList, ListOfShape(),
                                aFuzzy,
                                aResultIndex, aResultBaseAlgoList, aResultShapesList,
                                aResultCompound);
      }
    } else {
      // process object as is
      isOk = processObject(GeomAlgoAPI_Tools::BOOL_CUT,
                           anObject, aToolList, aPlanes,
                           aFuzzy,
                           aResultIndex, aResultBaseAlgoList, aResultShapesList,
                           aResultCompound);
    }
  }

  storeResult(anObjects.objects(), aToolList, aResultCompound, aResultIndex,
              aMakeShapeList, aResultBaseAlgoList);

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  if (!aResultCompound)
    aResultCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  ModelAPI_Tools::loadDeletedShapes(aResultBaseAlgoList,
                                    aToolList,
                                    aResultCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
