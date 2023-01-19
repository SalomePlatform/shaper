// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_BooleanFill.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <algorithm>
#include <map>


//=================================================================================================
FeaturesPlugin_BooleanFill::FeaturesPlugin_BooleanFill()
  : FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_FILL)
{
}

//=================================================================================================
void FeaturesPlugin_BooleanFill::initAttributes()
{
  FeaturesPlugin_Boolean::initAttributes();
  initVersion(BOP_VERSION_9_4(), selectionList(OBJECT_LIST_ID()), selectionList(TOOL_LIST_ID()));
}

//=================================================================================================
void FeaturesPlugin_BooleanFill::execute()
{
  std::string anError;
  GeomAPI_ShapeHierarchy anObjects, aTools;
  ListOfShape aPlanes;

  // Getting objects.
  if (!processAttribute(OBJECT_LIST_ID(), anObjects, aPlanes))
    return;
  // Planes are not supported as objects of FILL operation
  aPlanes.clear();

  // Getting tools.
  if (!processAttribute(TOOL_LIST_ID(), aTools, aPlanes))
    return;

  int aResultIndex = 0;

  if (anObjects.empty() || (aTools.empty() && aPlanes.empty())) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  std::vector<ModelAPI_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;

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

  // For solids cut each object with all tools.
  bool isOk = true;
  for (GeomAPI_ShapeHierarchy::iterator anObjectsIt = anObjects.begin();
       anObjectsIt != anObjects.end() && isOk;
       ++anObjectsIt) {
    GeomShapePtr anObject = *anObjectsIt;
    GeomShapePtr aParent = anObjects.parent(anObject, false);

    if (aParent && aParent->shapeType() == GeomAPI_Shape::COMPSOLID) {
      // get parent once again to mark it and the subs as processed
      aParent = anObjects.parent(anObject);
      // compsolid handling
      isOk = processCompsolid(GeomAlgoAPI_Tools::BOOL_PARTITION,
                              anObjects, aParent, aTools.objects(), aPlanes,
                              aFuzzy,
                              aResultIndex, aResultBaseAlgoList, aResultShapesList,
                              aResultCompound);
    } else {
      // process object as is
      isOk = processObject(GeomAlgoAPI_Tools::BOOL_PARTITION,
                           anObject, aTools.objects(), aPlanes,
                           aFuzzy,
                           aResultIndex, aResultBaseAlgoList, aResultShapesList,
                           aResultCompound);
    }
  }

  storeResult(anObjects.objects(), aTools.objects(), aResultCompound, aResultIndex,
              aMakeShapeList, aResultBaseAlgoList);

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  if (!aResultCompound)
    aResultCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  ModelAPI_Tools::loadDeletedShapes(aResultBaseAlgoList,
                                    aTools.objects(),
                                    aResultCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
