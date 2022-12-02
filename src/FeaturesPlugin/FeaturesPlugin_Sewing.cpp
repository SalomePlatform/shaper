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

#include <FeaturesPlugin_Sewing.h>

#include <GeomAlgoAPI_Sewing.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>



//=================================================================================================
FeaturesPlugin_Sewing::FeaturesPlugin_Sewing()
{
}

//=================================================================================================
void FeaturesPlugin_Sewing::initAttributes()
{
  data()->addAttribute(OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(ALLOW_NON_MANIFOLD_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(TOLERANCE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ALWAYS_CREATE_RESULT_ID(), ModelAPI_AttributeBoolean::typeId());
}

//=================================================================================================
void FeaturesPlugin_Sewing::execute()
{
  // Collect all base shapes
  ListOfShape aShapes;
  getOriginalShapes(OBJECTS_LIST_ID(), aShapes);

  // Get all other feature arguments
  bool isAllowNonManifold = boolean(FeaturesPlugin_Sewing::ALLOW_NON_MANIFOLD_ID())->value();
  bool isAlwaysCreateResult = boolean(FeaturesPlugin_Sewing::ALWAYS_CREATE_RESULT_ID())->value();
  double aTolerance = real(FeaturesPlugin_Sewing::TOLERANCE_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Sewing> aSewingAlgo(new GeomAlgoAPI_Sewing(aShapes, isAllowNonManifold, aTolerance));

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSewingAlgo, getKind(), anError))
  {
    setError(anError);
    return;
  }
    
  // Store result.
  GeomShapePtr aResult = aSewingAlgo->shape();

  if (!isSewn(aShapes, aResult) && !isAlwaysCreateResult)
  {
    static const std::string anError = "Error: No faces were sewn.";
    setError(anError);
    return;
  }

  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  aResultBody->storeModified(aShapes, aResult, aSewingAlgo);

  setResult(aResultBody, anIndex);
}

//=================================================================================================
void FeaturesPlugin_Sewing::getOriginalShapes(const std::string& theAttributeName,
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
bool FeaturesPlugin_Sewing::isSewn(const ListOfShape& theInputs,
                                   const GeomShapePtr theResult)
{
  // Consider the list of input shapes the same as the result, if
  //  * both arguments have the same number of shells
  //  * the total number of faces in these shells did NOT change.
  int nbInputShells = 0, nbInputFaces = 0;
  for (ListOfShape::const_iterator anIt = theInputs.cbegin();
       anIt != theInputs.cend();
       ++anIt)
  {
    GeomShapePtr aShape = *anIt;
    if (aShape.get() && aShape->isShell())
    {
      nbInputShells++;
      nbInputFaces += aShape->subShapes(GeomAPI_Shape::FACE, true).size();
    }
  }

  int nbResultShells = 0, nbResultFaces = 0;
  if (theResult->isCompound())
  {
    ListOfShape shells = theResult->subShapes(GeomAPI_Shape::SHELL, true);
    nbResultShells = shells.size();
    for (ListOfShape::const_iterator anIt = shells.cbegin();
        anIt != shells.cend();
        ++anIt)
    {
      GeomShapePtr aShape = *anIt;
      if (aShape.get() && aShape->isShell())
      {
        nbInputFaces += aShape->subShapes(GeomAPI_Shape::FACE, true).size();
      }
    }
  }
  else if (theResult->isShell())
  {
    nbResultShells = 1;
    nbResultFaces = theResult->subShapes(GeomAPI_Shape::FACE, true).size();
  }

  return (nbResultShells > nbInputShells || (nbResultShells == nbInputShells && nbResultFaces > nbInputFaces));
}
