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

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <cmath>

PlaneGCSSolver_EdgeWrapper::PlaneGCSSolver_EdgeWrapper(const GCSCurvePtr theEntity)
  : myEntity(theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(myEntity);
  if (aLine) myType = ENTITY_LINE;
  else {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(myEntity);
    if (anArc) myType = ENTITY_ARC;
    else {
      std::shared_ptr<GCS::Circle> aCircle = std::dynamic_pointer_cast<GCS::Circle>(myEntity);
      if (aCircle) myType = ENTITY_CIRCLE;
    }
  }
}

static double squareDistance(const GCS::Point& theP1, const GCS::Point& theP2)
{
  double dx = *theP1.x - *theP2.x;
  double dy = *theP1.y - *theP2.y;
  return dx*dx + dy*dy;
}

bool PlaneGCSSolver_EdgeWrapper::isDegenerated() const
{
  static const double aSqTol = tolerance * 1e-2;
  static const double aMaxRadius = 1e8;
  if (myType == ENTITY_LINE) {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(myEntity);
    return squareDistance(aLine->p1, aLine->p2) < aSqTol;
  }
  else if (myType == ENTITY_CIRCLE) {
    std::shared_ptr<GCS::Circle> aCircle = std::dynamic_pointer_cast<GCS::Circle>(myEntity);
    return *aCircle->rad < tolerance || *aCircle->rad > aMaxRadius;
  }
  else if (myType == ENTITY_ARC) {
    static const double anAngleTol = 1.e-5;
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(myEntity);
    double anAngleDiff = fabs(*anArc->startAngle - *anArc->endAngle);
    double aSqRadius = squareDistance(anArc->center, anArc->start);
    return aSqRadius < aSqTol || aSqRadius > aMaxRadius * aMaxRadius || // <- arc radius
           anAngleDiff < anAngleTol || fabs(anAngleDiff - 2*PI) < anAngleTol; // <- arc angle
  }
  return false;
}
