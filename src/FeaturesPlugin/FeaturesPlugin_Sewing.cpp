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

//#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_Sewing.h>
#include <GeomAlgoAPI_Tools.h>

//#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Shape.h>
//#include <GeomAPI_ShapeExplorer.h>
//#include <GeomAPI_ShapeIterator.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
//#include <ModelAPI_ResultConstruction.h>

//---------------------------------------------------------
// #define USE_DEBUG
// #define DEBUG
// static const char *dbg_class = "FeaturesPlugin_Sewing";
// #include "MBDebug.h"
// #include "MBModel.h"
// #include "MBGeom.h"
//---------------------------------------------------------



//=================================================================================================
FeaturesPlugin_Sewing::FeaturesPlugin_Sewing()
{
//  DBG_FUN();
}

//=================================================================================================
void FeaturesPlugin_Sewing::initAttributes()
{
//  DBG_FUN();

  data()->addAttribute(OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(ALLOW_NON_MANIFOLD_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(TOLERANCE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ALWAYS_CREATE_RESULT_ID(), ModelAPI_AttributeBoolean::typeId());
}

//=================================================================================================
void FeaturesPlugin_Sewing::execute()
{
//  DBG_FUN();

  // Collect all base shapes
  ListOfShape aShapes;
  getOriginalShapes(OBJECTS_LIST_ID(), aShapes);
//  SHOW(aShapes);

  // Get all other feature arguments
  bool isAllowNonManifold = boolean(FeaturesPlugin_Sewing::ALLOW_NON_MANIFOLD_ID())->value();
  bool isAlwaysCreateResult = boolean(FeaturesPlugin_Sewing::ALWAYS_CREATE_RESULT_ID())->value();
  double aTolerance = real(FeaturesPlugin_Sewing::TOLERANCE_ID())->value();
  // SHOW(isAllowNonManifold);
  // SHOW(isAlwaysCreateResult);
  // SHOW(aTolerance);

  std::shared_ptr<GeomAlgoAPI_Sewing> aSewingAlgo(new GeomAlgoAPI_Sewing(aShapes, isAllowNonManifold, aTolerance));

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSewingAlgo, getKind(), anError))
  {
    // SHOW(anError);
    setError(anError);
    return;
  }
    
  // Store result.
  GeomShapePtr aResult = aSewingAlgo->shape();
  // SHOW(aResult);

  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  // SHOW(aResultBody);
  aResultBody->storeModified(aShapes, aResult, aSewingAlgo);
  // SHOW(aResultBody);

  if (!isSewn(aShapes, aResult) && !isAlwaysCreateResult)
  {
    static const std::string anError = "Error: No faces were sewn.";
    setError(anError);
    return;
  }

  setResult(aResultBody, anIndex);
}

//=================================================================================================
void FeaturesPlugin_Sewing::getOriginalShapes(const std::string& theAttributeName,
                                              ListOfShape&       theShapes)
{
  // DBG_FUN();
  // ARG(theAttributeName);

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
  // DBG_FUN();
  // ARG(theInputs);
  // ARG(theResult);

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
  // SHOW(nbInputShells);
  // SHOW(nbInputFaces);

  int nbResultShells = 0, nbResultFaces = 0;
  if (theResult->isCompound())
  {
    // MSGEL("...result is COMPOUND");
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
    // MSGEL("...result is single SHELL");
    nbResultShells = 1;
    nbResultFaces = theResult->subShapes(GeomAPI_Shape::FACE, true).size();
  }
  // SHOW(nbResultShells);
  // SHOW(nbResultFaces);

  return (nbResultShells >= nbInputShells && nbResultFaces > nbInputFaces);
}
