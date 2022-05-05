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

#include "FeaturesPlugin_ExtrusionBoolean.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include <GeomAlgoAPI_ShapeTools.h>

//=================================================================================================
void FeaturesPlugin_ExtrusionBoolean::initAttributes()
{
  FeaturesPlugin_Extrusion::initAttributes();

  initBooleanAttributes();
}

//=================================================================================================
bool FeaturesPlugin_ExtrusionBoolean::makeGeneration(ListOfShape& theBaseShapes,
                                                     ListOfMakeShape& theMakeShapes)
{
  ListOfShape aBoundaryShapes;
  return makeExtrusions(theBaseShapes, aBoundaryShapes, theMakeShapes);
}

//=================================================================================================
void FeaturesPlugin_ExtrusionBoolean::storeGenerationHistory(ResultBodyPtr theResultBody,
                                        const GeomShapePtr theBaseShape,
                                        const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  FeaturesPlugin_Extrusion::storeGenerationHistory(theResultBody, theBaseShape, theMakeShape);
}

//=================================================================================================
void FeaturesPlugin_ExtrusionBoolean::getSizes(double& theToSize, double& theFromSize)
{
  if (string(CREATION_METHOD())->value() != CREATION_METHOD_THROUGH_ALL()) {
    FeaturesPlugin_Extrusion::getSizes(theToSize, theFromSize);
  } else {
    // Getting objects.
    ListOfShape anObjects;
    AttributeSelectionListPtr anObjectsSelList = myFeature->selectionList(OBJECTS_ID());
    for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
      AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
      GeomShapePtr anObject = anObjectAttr->value();
      if (!anObject.get()) {
        myFeature->setError("Error: Could not get object.");
        return;
      }
      anObjects.push_back(anObject);
    }

    // Getting prism bases.
    ListOfShape aBaseShapes;
    getBaseShapes(aBaseShapes);

    // Getting prism direction.
    std::shared_ptr<GeomAPI_Dir> aDir;
    getDirection(aDir);

    // Calculate sizes
    GeomAlgoAPI_ShapeTools::computeThroughAll(anObjects, aBaseShapes, aDir, theToSize, theFromSize);
  }
}
