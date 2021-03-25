// Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

#include <PlaneGCSSolver_PointArrayWrapper.h>
#include <PlaneGCSSolver_Tools.h>

#include <GeomDataAPI_Point2DArray.h>

#include <GeomAPI_Pnt2d.h>

PlaneGCSSolver_PointArrayWrapper::PlaneGCSSolver_PointArrayWrapper(
    const std::vector<PointWrapperPtr>& thePoints)
  : myPoints(thePoints)
{
}

bool PlaneGCSSolver_PointArrayWrapper::update(AttributePtr theAttribute)
{
  bool isUpdated = false;
  std::shared_ptr<GeomDataAPI_Point2DArray> aPointArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(theAttribute);
  if (aPointArray && aPointArray->size() == (int)myPoints.size()) {
    std::vector<PointWrapperPtr>::iterator aPIt = myPoints.begin();
    for (int anIndex = 0; aPIt != myPoints.end(); ++aPIt, ++anIndex) {
      GeomPnt2dPtr aPnt = aPointArray->pnt(anIndex);

      const GCSPointPtr& aGCSPoint = (*aPIt)->point();
      isUpdated = PlaneGCSSolver_Tools::updateValue(aPnt->x(), *(aGCSPoint->x)) || isUpdated;
      isUpdated = PlaneGCSSolver_Tools::updateValue(aPnt->y(), *(aGCSPoint->y)) || isUpdated;
    }
  }
  return isUpdated;
}
