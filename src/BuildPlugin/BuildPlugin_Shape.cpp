// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "BuildPlugin_Shape.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_PlanarEdges.h>


//=================================================================================================
void BuildPlugin_Shape::storeResult(const GeomMakeShapePtr& theAlgorithm,
                                    const ListOfShape& theOriginalShapes,
                                    const ListOfShape& theOriginalSolids,
                                    const GeomShapePtr& theResultShape,
                                    const int theResultIndex)
{
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
  aResultBody->storeModified(theOriginalSolids, theResultShape, theAlgorithm);

  for (ListOfShape::const_iterator anIt = theOriginalShapes.cbegin();
       anIt != theOriginalShapes.cend();
       ++anIt)
  {
    GeomShapePtr aShape = *anIt;
    aResultBody->loadModifiedShapes(theAlgorithm, aShape, GeomAPI_Shape::VERTEX);
    aResultBody->loadModifiedShapes(theAlgorithm, aShape, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(theAlgorithm, aShape, GeomAPI_Shape::FACE);
  }
  setResult(aResultBody, theResultIndex);
}

//=================================================================================================
void BuildPlugin_Shape::getOriginalShapesAndContexts(const std::string& theSelectionListID,
                                                     ListOfShape& theShapes,
                                                     ListOfShape& theContexts)
{
  std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aContexts;

  AttributeSelectionListPtr aSelectionList = selectionList(theSelectionListID);
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    GeomShapePtr aContext = aSelection->context()->shape();
    if (!aShape.get())
      aShape = aContext;
    theShapes.push_back(aShape);

    // do not collect sketch faces, because they are stored as compounds
    // and then are treated as modified by the algorithm
    if (!std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContext).get())
      aContexts.insert(aContext);
  }

  std::set<GeomShapePtr, GeomAPI_Shape::Comparator>::const_iterator anIt = aContexts.begin();
  for (; anIt != aContexts.end(); ++anIt)
    theContexts.push_back(*anIt);
}
