// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "FiltersPlugin_OnPlaneSide.h"

#include <ModelAPI_AttributeSelection.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_XYZ.h>


bool FiltersPlugin_OnPlaneSide::isSupported(GeomAPI_Shape::ShapeType /*theType*/) const
{
  return true;
}

bool FiltersPlugin_OnPlaneSide::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                     const ModelAPI_FiltersArgs& theArgs) const
{
  static const double THE_TOLERANCE = 1.e-7;

  AttributePtr aAttr = theArgs.argument("OnPlaneSide");
  AttributeSelectionPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aAttr);
  if (!aList.get())
    return false;
  GeomShapePtr aPlanarShape = aList->value();
  if (!aPlanarShape) {
    ResultPtr aContext = aList->context();
    if (!aContext)
      return false;
    aPlanarShape = aContext->shape();
  }

  GeomPlanePtr aPlane = aPlanarShape->face()->getPlane();
  GeomPointPtr aPlaneLoc = aPlane->location();
  GeomDirPtr aPlaneNorm = aPlane->direction();

  GeomPointPtr aShapeCenter = theShape->middlePoint();
  GeomXYZPtr aVec = aShapeCenter->xyz()->decreased(aPlaneLoc->xyz());

  return aVec->dot(aPlaneNorm->xyz()) > THE_TOLERANCE;
}

std::string FiltersPlugin_OnPlaneSide::xmlRepresentation() const
{
  return xmlFromFile("filter-OnPlaneSide.xml");
}

void FiltersPlugin_OnPlaneSide::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("OnPlaneSide", ModelAPI_AttributeSelection::typeId());
}
