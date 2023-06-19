// Copyright (C) 2014-2023  CEA, EDF
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

#include "FeaturesPlugin_BooleanSmash.h"

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


static const double DEFAULT_FUZZY = 1.e-5;


//==================================================================================================
FeaturesPlugin_BooleanSmash::FeaturesPlugin_BooleanSmash()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_SMASH)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanSmash::initAttributes()
{
  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(USE_FUZZY_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(FUZZY_PARAM_ID(), ModelAPI_AttributeDouble::typeId());
  boolean(USE_FUZZY_ID())->setValue(false); // Do NOT use the fuzzy parameter by default.
  real(FUZZY_PARAM_ID())->setValue(DEFAULT_FUZZY);

  initVersion(BOP_VERSION_9_4(), selectionList(OBJECT_LIST_ID()), selectionList(TOOL_LIST_ID()));
}

//==================================================================================================
void FeaturesPlugin_BooleanSmash::execute()
{
  std::string anError;
  GeomAPI_ShapeHierarchy anObjectsHistory, aToolsHistory;
  ListOfShape aPlanes;

  // Getting objects and tools.
  if (!processAttribute(OBJECT_LIST_ID(), anObjectsHistory, aPlanes) ||
      !processAttribute(TOOL_LIST_ID(), aToolsHistory, aPlanes))
    return;

  int aResultIndex = 0;

  if (anObjectsHistory.empty() || aToolsHistory.empty()) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  // Collecting all shapes which will be smashed.
  ListOfShape aShapesToSmash = anObjectsHistory.objects();

  // List of original shapes for naming.
  ListOfShape anOriginalShapes;
  anOriginalShapes.insert(anOriginalShapes.end(), aShapesToSmash.begin(), aShapesToSmash.end());
  ListOfShape aTools = aToolsHistory.objects();
  anOriginalShapes.insert(anOriginalShapes.end(), aTools.begin(), aTools.end());

  // Collecting solids from compsolids which will not be modified in
  // boolean operation and will be added to result.
  ListOfShape aShapesToAdd;
  for (GeomAPI_ShapeHierarchy::iterator anIt = anObjectsHistory.begin();
       anIt != anObjectsHistory.end();
       ++anIt)
  {
    GeomShapePtr aParent = anObjectsHistory.parent(*anIt, false);
    if (aParent) {
      anOriginalShapes.push_back(aParent);

      ListOfShape aUsed, aNotUsed;
      anObjectsHistory.splitCompound(aParent, aUsed, aNotUsed);
      aShapesToAdd.insert(aShapesToAdd.end(), aNotUsed.begin(), aNotUsed.end());

      // add unused shapes of compounds/compsolids to the history,
      // to avoid treating them as unused later when constructing a compound containing
      // the result of Smash and all unused sub-shapes of multi-level compounds
      for (ListOfShape::iterator aNUIt = aNotUsed.begin(); aNUIt != aNotUsed.end(); ++aNUIt)
        anObjectsHistory.addObject(*aNUIt);
    }
  }

  // Getting fuzzy parameter.
  // Used as additional tolerance to eliminate tiny results.
  // Using -1 as fuzzy value in the GeomAlgoAPI means to ignore it during the boolean operation!
  bool aUseFuzzy = boolean(USE_FUZZY_ID())->value();
  double aFuzzy = (aUseFuzzy ? real(FUZZY_PARAM_ID())->value() : -1);

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  if (!aShapesToAdd.empty()) {
    // Cut objects with not used solids.
    std::shared_ptr<GeomAlgoAPI_Boolean> anObjectsCutAlgo(
      new GeomAlgoAPI_Boolean(aShapesToSmash,
                              aShapesToAdd,
                              GeomAlgoAPI_Tools::BOOL_CUT,
                              aFuzzy));

    if (GeomAlgoAPI_ShapeTools::area(anObjectsCutAlgo->shape()) > 1.e-27) {
      aShapesToSmash.clear();
      aShapesToSmash.push_back(anObjectsCutAlgo->shape());
      aMakeShapeList->appendAlgo(anObjectsCutAlgo);
    }

    // Cut tools with not used solids.
    std::shared_ptr<GeomAlgoAPI_Boolean> aToolsCutAlgo(
      new GeomAlgoAPI_Boolean(aTools,
                              aShapesToAdd,
                              GeomAlgoAPI_Tools::BOOL_CUT,
                              aFuzzy));

    if (GeomAlgoAPI_ShapeTools::area(aToolsCutAlgo->shape()) > 1.e-27) {
      aTools.clear();
      aTools.push_back(aToolsCutAlgo->shape());
      aMakeShapeList->appendAlgo(aToolsCutAlgo);
    }
  }

  // Cut objects with tools.
  std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(
    new GeomAlgoAPI_Boolean(aShapesToSmash,
                            aTools,
                            GeomAlgoAPI_Tools::BOOL_CUT,
                            aFuzzy));

  // Checking that the algorithm worked properly.
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  aMakeShapeList->appendAlgo(aBoolAlgo);

  // Put all (cut result, tools and not used solids) to PaveFiller.
  GeomShapePtr aShape = aBoolAlgo->shape();
  GeomAPI_ShapeIterator anIt(aShape);
  if (anIt.more() || aShape->shapeType() == GeomAPI_Shape::VERTEX) {
    aShapesToAdd.push_back(aShape);
  }
  aShapesToAdd.insert(aShapesToAdd.end(), aTools.begin(), aTools.end());

  if (aShapesToAdd.size() == 1) {
    aShape = aShapesToAdd.front();
  }
  else {
    std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
      new GeomAlgoAPI_PaveFiller(aShapesToAdd, true, aFuzzy));
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFillerAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aShape = aFillerAlgo->shape();
    aMakeShapeList->appendAlgo(aFillerAlgo);
  }

  // take into account a version of SMASH feature
  if (data()->version() == BOP_VERSION_9_4()) {
    // merge hierarchies of compounds containing objects and tools
    // and append the result of the FUSE operation
    aShape = keepUnusedSubsOfCompound(aShape, anObjectsHistory, aToolsHistory, aMakeShapeList);
  }

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);

  ModelAPI_Tools::loadModifiedShapes(aResultBody,
                                     anOriginalShapes,
                                     anOriginalShapes,
                                     aMakeShapeList,
                                     aShape);

  setResult(aResultBody, aResultIndex);
  aResultIndex++;

  ModelAPI_Tools::loadDeletedShapes(aResultBody,
                                    GeomShapePtr(),
                                    anOriginalShapes,
                                    aMakeShapeList,
                                    aShape);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
