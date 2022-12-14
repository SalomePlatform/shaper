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

#include "FeaturesPlugin_GlueFaces.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_GlueFaces.h>
#include <GeomAlgoAPI_Tools.h>

#include <sstream>


//==================================================================================================
FeaturesPlugin_GlueFaces::FeaturesPlugin_GlueFaces()
{
}

//==================================================================================================
void FeaturesPlugin_GlueFaces::initAttributes()
{
  data()->addAttribute(OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(TOLERANCE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(KEEP_NON_SOLIDS_ID(), ModelAPI_AttributeBoolean::typeId());
}

//==================================================================================================
void FeaturesPlugin_GlueFaces::execute()
{
  // Collect all selected shapes into a single list of shapes
  ListOfShape aShapes;
  getOriginalShapes(OBJECTS_LIST_ID(), aShapes);

  // Get all other feature arguments
  double aTolerance = real(FeaturesPlugin_GlueFaces::TOLERANCE_ID())->value();
  bool isKeepNonSolids = boolean(FeaturesPlugin_GlueFaces::KEEP_NON_SOLIDS_ID())->value();

  std::shared_ptr<GeomAlgoAPI_GlueFaces> aGluingAlgo(new GeomAlgoAPI_GlueFaces(aShapes, aTolerance, isKeepNonSolids));

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aGluingAlgo, getKind(), anError))
  {
    setError(anError);
    eraseResults();
    return;
  }

  // Store result.
  GeomShapePtr aResult = aGluingAlgo->shape();

  if (!isGlued(aShapes, aResult))
  {
    static const std::string anError2 = "Error: No faces were glued.";
    setError(anError2);
    data()->execState(ModelAPI_ExecState::ModelAPI_StateDone);
    eraseResults();
    setResultFromInput(aShapes);
    return;
  }

  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  aResultBody->storeModified(aShapes, aResult, aGluingAlgo);

  // Ensure the result body is named after this feature
  std::wstring aBaseName = feature(aResultBody)->data()->name();
  std::wostringstream aNameStr;
  aNameStr << aBaseName << "_" << (anIndex+1);
  std::wstring aName = aNameStr.str();
  aResultBody->data()->setName(aName);

  setResult(aResultBody, anIndex);
}

//=================================================================================================
void FeaturesPlugin_GlueFaces::getOriginalShapes(const std::string& theAttributeName,
                                                 ListOfShape&       theShapes)
{
  // Collect all selections into a single list of shapes
  AttributeSelectionListPtr aSelectionList = selectionList(theAttributeName);
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex)
  {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    GeomShapePtr aContext = aSelection->context()->shape();
    if (!aShape.get())
    {
      aShape = aContext;
    }
    theShapes.push_back(aShape);
  }
}

//=================================================================================================
bool FeaturesPlugin_GlueFaces::isGlued(const ListOfShape& theInputs,
                                       const GeomShapePtr theResult)
{
  if (!theResult.get())
    return false;

  // Consider the list of input shapes the same as the result, if
  //  * the total number of faces did NOT change.
  int nbInputFaces = 0, nbInputEdges = 0;
  for (ListOfShape::const_iterator anIt = theInputs.cbegin();
       anIt != theInputs.cend();
       ++anIt)
  {
    GeomShapePtr aShape = *anIt;
    if (aShape.get())
    {
      nbInputFaces += aShape->subShapes(GeomAPI_Shape::FACE, true).size();
      nbInputEdges += aShape->subShapes(GeomAPI_Shape::EDGE, true).size();
    }
  }

  int nbResultFaces = 0, nbResultEdges = 0;
  nbResultFaces = theResult->subShapes(GeomAPI_Shape::FACE, true).size();
  nbResultEdges = theResult->subShapes(GeomAPI_Shape::EDGE, true).size();
  return(0 < nbResultFaces && ((nbResultFaces < nbInputFaces) || (nbResultFaces == nbInputFaces && nbResultEdges < nbInputEdges)));
}

//=================================================================================================
void FeaturesPlugin_GlueFaces::setResultFromInput(const ListOfShape& theInputs)
{
  GeomShapePtr aResult;

  // Make sure the result will be a compound of the input shapes, if not already
  if (theInputs.size() == 1 && theInputs.front().get() && theInputs.front()->isCompound())
  {
    aResult = theInputs.front();
  }
  else
  {
    aResult = GeomAlgoAPI_CompoundBuilder::compound(theInputs);
  }

  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  aResultBody->store(aResult);

  setResult(aResultBody, anIndex);
}
