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

#include "FeaturesPlugin_BooleanFill.h"
#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>
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
void FeaturesPlugin_BooleanFill::execute()
{
  std::string anError;
  ObjectHierarchy anObjects, aTools;
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

  if (anObjects.IsEmpty() || (aTools.IsEmpty() && aPlanes.empty())) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;

  // For solids cut each object with all tools.
  bool isOk = true;
  for (ObjectHierarchy::Iterator anObjectsIt = anObjects.Begin();
       anObjectsIt != anObjects.End() && isOk;
       ++anObjectsIt) {
    GeomShapePtr anObject = *anObjectsIt;
    GeomShapePtr aParent = anObjects.Parent(anObject, false);

    if (aParent && aParent->shapeType() == GeomAPI_Shape::COMPSOLID) {
      // get parent once again to mark it and the subs as processed
      aParent = anObjects.Parent(anObject);
      // compsolid handling
      isOk = processCompsolid(GeomAlgoAPI_Tools::BOOL_PARTITION,
                              anObjects, aParent, aTools.Objects(), aPlanes,
                              aResultIndex, aResultBaseAlgoList, aResultShapesList);
    } else {
      // process object as is
      isOk = processObject(GeomAlgoAPI_Tools::BOOL_PARTITION,
                           anObject, aTools.Objects(), aPlanes,
                           aResultIndex, aResultBaseAlgoList, aResultShapesList);
    }
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList,
                                          aTools.Objects(),
                                          aResultShapesCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
