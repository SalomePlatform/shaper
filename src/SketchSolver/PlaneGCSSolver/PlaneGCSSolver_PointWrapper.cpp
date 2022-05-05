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

#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Tools.h>

#include <GeomDataAPI_Point2D.h>

PlaneGCSSolver_PointWrapper::PlaneGCSSolver_PointWrapper(const GCSPointPtr thePoint)
  : myPoint(thePoint)
{
}

bool PlaneGCSSolver_PointWrapper::update(AttributePtr theAttribute)
{
  bool isUpdated = false;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D) {
    isUpdated = PlaneGCSSolver_Tools::updateValue(aPoint2D->x(), *(myPoint->x)) || isUpdated;
    isUpdated = PlaneGCSSolver_Tools::updateValue(aPoint2D->y(), *(myPoint->y)) || isUpdated;
  }
  return isUpdated;
}
