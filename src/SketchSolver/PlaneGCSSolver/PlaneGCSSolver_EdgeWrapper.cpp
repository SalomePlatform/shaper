// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <cmath>

static bool isLine(const GCSCurvePtr& theEntity)
{
  return std::dynamic_pointer_cast<GCS::Line>(theEntity).get();
}

static bool isCircle(const GCSCurvePtr& theEntity)
{
  return std::dynamic_pointer_cast<GCS::Circle>(theEntity).get();
}

static bool isArc(const GCSCurvePtr& theEntity)
{
  return std::dynamic_pointer_cast<GCS::Arc>(theEntity).get();
}

static bool isEllipse(const GCSCurvePtr& theEntity)
{
  return std::dynamic_pointer_cast<GCS::Ellipse>(theEntity).get();
}

static bool isEllipticArc(const GCSCurvePtr& theEntity)
{
  return std::dynamic_pointer_cast<GCS::ArcOfEllipse>(theEntity).get();
}


PlaneGCSSolver_EdgeWrapper::PlaneGCSSolver_EdgeWrapper(const GCSCurvePtr theEntity)
  : myEntity(theEntity)
{
  if (isLine(myEntity))
    myType = ENTITY_LINE;
  else if (isArc(myEntity))
    myType = ENTITY_ARC;
  else if (isCircle(myEntity))
    myType = ENTITY_CIRCLE;
  else if (isEllipticArc(myEntity))
    myType = ENTITY_ELLIPTIC_ARC;
  else if (isEllipse(myEntity))
    myType = ENTITY_ELLIPSE;
}

static double squareDistance(const GCS::Point& theP1, const GCS::Point& theP2)
{
  double dx = *theP1.x - *theP2.x;
  double dy = *theP1.y - *theP2.y;
  return dx*dx + dy*dy;
}

bool PlaneGCSSolver_EdgeWrapper::isDegenerated() const
{
  static const double THE_SQ_TOL = tolerance * 1e-2;
  static const double THE_ANGLE_TOL = 1.e-5;
  static const double THE_MAX_RADIUS = 1e8;
  static const double THE_SQ_MAX_RADIUS = THE_MAX_RADIUS * THE_MAX_RADIUS;

  if (myType == ENTITY_LINE) {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(myEntity);
    return squareDistance(aLine->p1, aLine->p2) < THE_SQ_TOL;
  }
  else if (myType == ENTITY_CIRCLE) {
    std::shared_ptr<GCS::Circle> aCircle = std::dynamic_pointer_cast<GCS::Circle>(myEntity);
    return *aCircle->rad < tolerance || *aCircle->rad > THE_MAX_RADIUS;
  }
  else if (myType == ENTITY_ARC) {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(myEntity);
    double anAngleDiff = fabs(*anArc->startAngle - *anArc->endAngle);
    double aSqRadius = squareDistance(anArc->center, anArc->start);
    return aSqRadius < THE_SQ_TOL || aSqRadius > THE_SQ_MAX_RADIUS || // <- arc radius
           anAngleDiff < THE_ANGLE_TOL || fabs(anAngleDiff - 2*PI) < THE_ANGLE_TOL; // <- arc angle
  }
  else if (myType == ENTITY_ELLIPSE) {
    std::shared_ptr<GCS::Ellipse> anEllipse = std::dynamic_pointer_cast<GCS::Ellipse>(myEntity);
    return *anEllipse->radmin < tolerance || anEllipse->getRadMaj() > THE_MAX_RADIUS;
  }
  else if (myType == ENTITY_ELLIPTIC_ARC) {
    std::shared_ptr<GCS::ArcOfEllipse> anArc =
        std::dynamic_pointer_cast<GCS::ArcOfEllipse>(myEntity);
    double anAngleDiff = fabs(*anArc->startAngle - *anArc->endAngle);
    return *anArc->radmin < THE_SQ_TOL || anArc->getRadMaj() > THE_SQ_MAX_RADIUS || // <- arc radius
           anAngleDiff < THE_ANGLE_TOL || fabs(anAngleDiff - 2*PI) < THE_ANGLE_TOL; // <- arc angle
  }
  return false;
}
