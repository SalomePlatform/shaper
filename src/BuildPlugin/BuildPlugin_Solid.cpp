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

#include "BuildPlugin_Solid.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_MakeVolume.h>

//=================================================================================================
BuildPlugin_Solid::BuildPlugin_Solid()
{
}

//=================================================================================================
void BuildPlugin_Solid::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Solid::execute()
{
  // all the needed checkings are in validator, so, here just make and store result
  ListOfShape anOriginalFaces;
  ListOfShape anOriginalSolids;
  getOriginalShapesAndContexts(BASE_OBJECTS_ID(), anOriginalFaces, anOriginalSolids);

  std::shared_ptr<GeomAlgoAPI_MakeVolume> anAlgo(
    new GeomAlgoAPI_MakeVolume(anOriginalFaces, false));
  // check and process result of volume maker
  GeomShapePtr aResShape = getSingleSubshape(anAlgo->shape());
  storeResult(anAlgo, anOriginalFaces, anOriginalSolids, aResShape);
}

GeomShapePtr BuildPlugin_Solid::getSingleSubshape(const GeomShapePtr& theCompound)
{
  if (theCompound->shapeType() == GeomAPI_Shape::COMPOUND) {
    GeomAPI_ShapeIterator anIt(theCompound);
    GeomShapePtr aFoundSub;
    for (; anIt.more() && !aFoundSub; anIt.next()) {
      return anIt.current();
    }
  }
  return theCompound;
}
