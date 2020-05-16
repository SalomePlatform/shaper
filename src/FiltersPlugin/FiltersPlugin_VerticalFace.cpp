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

#include "FiltersPlugin_VerticalFace.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Solid.h>
#include <GeomAPI_Shell.h>
#include <GeomAPI_Cylinder.h>

#include <cmath>

bool FiltersPlugin_VerticalFace::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::FACE;
}

bool FiltersPlugin_VerticalFace::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                      const ModelAPI_FiltersArgs& /*theArgs*/) const
{
  static const double THE_TOLERANCE = 1.e-7;

  bool isVertical = false;
  if (!theShape->isFace())
    return isVertical;

  GeomFacePtr aFace(new GeomAPI_Face(theShape));

  GeomPlanePtr aPlane = aFace->getPlane();
  if (aPlane) {
    GeomDirPtr aDir = aPlane->direction();
    isVertical = fabs(aDir->z()) <= THE_TOLERANCE;
  }
  else {
    GeomCylinderPtr aCylinder = aFace->getCylinder();
    if (aCylinder) {
      GeomDirPtr aDir = aCylinder->axis();
      isVertical = fabs(fabs(aDir->z()) - 1.0) <= THE_TOLERANCE;
    }
  }
  return isVertical;
}
