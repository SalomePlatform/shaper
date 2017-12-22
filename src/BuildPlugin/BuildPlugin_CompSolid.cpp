// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#include "BuildPlugin_CompSolid.h"

#include <GeomAlgoAPI_MakeShape.h>
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
  ListOfShape anOriginalShapes;
  std::shared_ptr<GeomAlgoAPI_MakeShape> aVolumeMaker;
  if (!build(anOriginalShapes, aVolumeMaker))
    return;

  GeomShapePtr aVolumeRes = aVolumeMaker->shape();

  // check and process result of volume maker
  GeomShapePtr aResShape = getSingleSubshape(aVolumeRes, GeomAPI_Shape::COMPSOLID);
  if (!aResShape) // try to build a solid
    aResShape = getSingleSubshape(aVolumeRes, GeomAPI_Shape::SOLID);

  int anIndex = 0;
  if (aResShape) {
    storeResult(anOriginalShapes, aResShape, aVolumeMaker);
    ++anIndex;
  }
  removeResults(anIndex);
}
