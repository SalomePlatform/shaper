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

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>

#include <GeomAPI_ShapeIterator.h>

//==================================================================================================
void FeaturesPlugin_Tools::loadModifiedShapes(ResultBodyPtr theResultBody,
                                              const ListOfShape& theBaseShapes,
                                              const ListOfShape& theTools,
                                              const GeomMakeShapePtr& theMakeShape,
                                              const GeomShapePtr theResultShape)
{
  theResultBody->storeModified(theBaseShapes, theResultShape, theMakeShape);

  ListOfShape aShapes = theBaseShapes;
  ListOfShape::const_iterator aToolIter = theTools.cbegin();
  for(; aToolIter != theTools.cend(); aToolIter++)
    aShapes.push_back(*aToolIter);

  for (ListOfShape::const_iterator anIter = aShapes.begin(); anIter != aShapes.end(); ++anIter)
  {
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::VERTEX);
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::EDGE);
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::FACE);
  }
}

//==================================================================================================
void FeaturesPlugin_Tools::loadModifiedShapes(ResultBodyPtr theResultBody,
                                              const GeomShapePtr& theBaseShape,
                                              const GeomMakeShapePtr& theMakeShape,
                                              const std::string theName)
{
  switch(theBaseShape->shapeType()) {
    case GeomAPI_Shape::COMPOUND: {
      for(GeomAPI_ShapeIterator anIt(theBaseShape); anIt.more(); anIt.next())
      {
        loadModifiedShapes(theResultBody,
                           anIt.current(),
                           theMakeShape,
                           theName);
      }
      break;
    }
    case GeomAPI_Shape::COMPSOLID:
    case GeomAPI_Shape::SOLID:
    case GeomAPI_Shape::SHELL: {
      theResultBody->loadModifiedShapes(theMakeShape,
                                        theBaseShape,
                                        GeomAPI_Shape::FACE,
                                        theName);
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::WIRE: {
      theResultBody->loadModifiedShapes(theMakeShape,
                                        theBaseShape,
                                        GeomAPI_Shape::EDGE,
                                        theName);
    }
    case GeomAPI_Shape::EDGE: {
      theResultBody->loadModifiedShapes(theMakeShape,
                                        theBaseShape,
                                        GeomAPI_Shape::VERTEX,
                                        theName);
    }
  }
}

//==================================================================================================
void FeaturesPlugin_Tools::loadDeletedShapes(ResultBodyPtr theResultBody,
  const GeomShapePtr theBaseShape,
  const ListOfShape& theTools,
  const GeomMakeShapePtr& theMakeShape,
  const GeomShapePtr theResultShapesCompound)
{
  ListOfShape aShapes = theTools;
  if (theBaseShape.get())
    aShapes.push_front(theBaseShape);

  for (ListOfShape::const_iterator anIter = aShapes.begin(); anIter != aShapes.end(); anIter++)
  {
    theResultBody->loadDeletedShapes(theMakeShape,
                                     *anIter,
                                     GeomAPI_Shape::VERTEX,
                                     theResultShapesCompound);
    theResultBody->loadDeletedShapes(theMakeShape,
                                     *anIter,
                                     GeomAPI_Shape::EDGE,
                                     theResultShapesCompound);
    theResultBody->loadDeletedShapes(theMakeShape,
                                     *anIter,
                                     GeomAPI_Shape::FACE,
                                     theResultShapesCompound);
  }
}

//==================================================================================================
void FeaturesPlugin_Tools::loadDeletedShapes(
  std::vector<ResultBaseAlgo>& theResultBaseAlgoList,
  const ListOfShape& theTools,
  const GeomShapePtr theResultShapesCompound)
{
  for (std::vector<ResultBaseAlgo>::iterator anIt = theResultBaseAlgoList.begin();
       anIt != theResultBaseAlgoList.end();
       ++anIt)
  {
    ResultBaseAlgo& aRCA = *anIt;
    loadDeletedShapes(aRCA.resultBody,
                      aRCA.baseShape,
                      theTools,
                      aRCA.makeShape,
                      theResultShapesCompound);
  }
}
