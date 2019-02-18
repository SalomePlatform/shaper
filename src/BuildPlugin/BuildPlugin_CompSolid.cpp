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

#include "BuildPlugin_CompSolid.h"

#include <GeomAlgoAPI_MakeVolume.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
BuildPlugin_CompSolid::BuildPlugin_CompSolid()
{
}

//=================================================================================================
void BuildPlugin_CompSolid::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_CompSolid::execute()
{
  // all the needed checkings are in validator, so, here just make and store result
  ListOfShape anOriginalShapes;
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if (!aShape.get())
      aShape = aSelection->context()->shape();
    anOriginalShapes.push_back(aShape);
  }
  std::shared_ptr<GeomAlgoAPI_MakeVolume> anAlgo(
    new GeomAlgoAPI_MakeVolume(anOriginalShapes, false));
  GeomShapePtr aVolumeRes = anAlgo->shape();

  // check and process result of volume maker
  GeomShapePtr aResShape = getSingleSubshape(aVolumeRes);
  storeResult(anOriginalShapes, aResShape, anAlgo);
}
