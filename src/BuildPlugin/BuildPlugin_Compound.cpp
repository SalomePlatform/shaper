// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Compound.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_CompoundBuilder.h>


//=================================================================================================
BuildPlugin_Compound::BuildPlugin_Compound()
{
}

//=================================================================================================
void BuildPlugin_Compound::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Compound::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if(aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Collect base shapes.
  ListOfShape anOriginalShapes;
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if (!aShape.get()) {
      if (!aSelection->context().get()) {
        setError("Invalid selection");
        return;
      }
      aShape = aSelection->context()->shape();
    }
    anOriginalShapes.push_back(aShape);
  }

  // Build compound.
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(anOriginalShapes);
  // Copy shape.
  GeomMakeShapePtr aCopyAlgo(new GeomAlgoAPI_Copy(aCompound));
  GeomShapePtr aCopyCompound = aCopyAlgo->shape();

  int anIndexToRemove = 0;
  if (aCopyCompound) {
    ResultBodyPtr aResultBody = document()->createBody(data(), anIndexToRemove++);
    aResultBody->storeModified(anOriginalShapes, aCopyCompound, aCopyAlgo);
    aResultBody->loadModifiedShapes(aCopyAlgo, aCompound, GeomAPI_Shape::VERTEX);
    aResultBody->loadModifiedShapes(aCopyAlgo, aCompound, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(aCopyAlgo, aCompound, GeomAPI_Shape::FACE);
    setResult(aResultBody);
  }
  removeResults(anIndexToRemove);
}
