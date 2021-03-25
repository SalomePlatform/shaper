// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointArrayWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Pnt2d.h>

#include <cmath>

static GCS::Point createGCSPoint(double* x, double* y)
{
  GCS::Point aPoint;
  aPoint.x = x;
  aPoint.y = y;
  return aPoint;
}


SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(FeaturePtr theFeature)
  : SketchSolver_ConstraintFixed(ConstraintPtr()),
    myMovedFeature(theFeature),
    mySimpleMove(true)
{
}

SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(AttributePtr theAttribute,
                                                                 const int thePointIndex)
  : SketchSolver_ConstraintFixed(ConstraintPtr()),
    myDraggedAttribute(theAttribute),
    myDraggedPointIndex(thePointIndex),
    mySimpleMove(true)
{
  myMovedFeature = ModelAPI_Feature::feature(theAttribute->owner());
}

void SketchSolver_ConstraintMovement::blockEvents(bool isBlocked)
{
  if (myMovedFeature)
    myMovedFeature->data()->blockSendAttributeUpdated(isBlocked);
}

void SketchSolver_ConstraintMovement::process()
{
  cleanErrorMsg();
  if (!myMovedFeature || !myStorage) {
    // Not enough parameters are initialized
    return;
  }

  mySolverConstraint = initMovement();
  if (!myErrorMsg.empty() || !mySolverConstraint) {
    // Nothing to move, clear the feature to avoid changing its group
    // after removing the Movement constraint.
    myMovedFeature = FeaturePtr();
    return;
  }
  myStorage->addMovementConstraint(mySolverConstraint);
}


static bool isSimpleMove(FeaturePtr theMovedFeature, AttributePtr theDraggedPoint)
{
  bool isSimple = true;
#ifdef CHANGE_RADIUS_WHILE_MOVE
  if (theMovedFeature->getKind() == SketchPlugin_Circle::ID() ||
      theMovedFeature->getKind() == SketchPlugin_Ellipse::ID())
    isSimple = (theDraggedPoint.get() != 0);
  else if (theMovedFeature->getKind() == SketchPlugin_Arc::ID() ||
           theMovedFeature->getKind() == SketchPlugin_EllipticArc::ID()) {
    isSimple = (theDraggedPoint.get() != 0 &&
               (theDraggedPoint->id() == SketchPlugin_Arc::CENTER_ID() ||
                theDraggedPoint->id() == SketchPlugin_EllipticArc::CENTER_ID()));
  }
#endif
  return isSimple;
}

ConstraintWrapperPtr SketchSolver_ConstraintMovement::initMovement()
{
  ConstraintWrapperPtr aConstraint;

  // if the feature is copy, do not move it
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(myMovedFeature);
  if (!aSketchFeature || aSketchFeature->isCopy()) {
    myStorage->setNeedToResolve(true);
    return aConstraint;
  }

  EntityWrapperPtr anEntity = myDraggedAttribute ? myStorage->entity(myDraggedAttribute)
                                                 : myStorage->entity(myMovedFeature);
  if (!anEntity) {
    myStorage->update(myMovedFeature, true);
    anEntity = myDraggedAttribute ? myStorage->entity(myDraggedAttribute)
                                  : myStorage->entity(myMovedFeature);
    if (!anEntity)
      return aConstraint;
  }

  mySimpleMove = isSimpleMove(myMovedFeature, myDraggedAttribute);

  if (mySimpleMove) {
    if (anEntity->type() == ENTITY_POINT_ARRAY) {
      anEntity = std::dynamic_pointer_cast<PlaneGCSSolver_PointArrayWrapper>(anEntity)
                 ->value(myDraggedPointIndex);
    }
    aConstraint = fixFeature(anEntity);
  }
  else {
    if (myDraggedAttribute) // start or end point of arc has been moved
      aConstraint = fixArcExtremity(anEntity);
    else if (anEntity->type() == ENTITY_CIRCLE || anEntity->type() == ENTITY_ARC) {
      // arc or circle has been moved
      aConstraint = fixPointOnCircle(anEntity);
    }
    else if (anEntity->type() == ENTITY_ELLIPSE || anEntity->type() == ENTITY_ELLIPTIC_ARC) {
      // ellipse or elliptic arc has been moved
      aConstraint = fixPointOnEllipse(anEntity);
    }
  }

  return aConstraint;
}

ConstraintWrapperPtr SketchSolver_ConstraintMovement::fixArcExtremity(
    const EntityWrapperPtr& theArcExtremity)
{
  static const int nbParams = 4;
  myFixedValues.reserve(nbParams); // moved point and center of arc

  EdgeWrapperPtr aCircularEntity = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(
      myStorage->entity(myMovedFeature));
  std::shared_ptr<GCS::Arc> anArc =
      std::dynamic_pointer_cast<GCS::Arc>(aCircularEntity->entity());
  std::shared_ptr<GCS::ArcOfEllipse> anEllArc =
      std::dynamic_pointer_cast<GCS::ArcOfEllipse>(aCircularEntity->entity());

  PointWrapperPtr aPoint =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theArcExtremity);

  double* aParams[nbParams] = { aPoint->point()->x, aPoint->point()->y, 0, 0 };
  if (anArc) {
    aParams[2] = anArc->center.x;
    aParams[3] = anArc->center.y;
  }
  else if (anEllArc) {
    aParams[2] = anEllArc->center.x;
    aParams[3] = anEllArc->center.y;
  }

  std::list<GCSConstraintPtr> aConstraints;
  for (int i = 0; i < nbParams; ++i) {
    myFixedValues.push_back(*aParams[i]);
    GCSConstraintPtr aNewConstraint(new GCS::ConstraintEqual(&myFixedValues[i], aParams[i]));
    aNewConstraint->rescale(0.01);
    aConstraints.push_back(aNewConstraint);
  }

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));
}

ConstraintWrapperPtr SketchSolver_ConstraintMovement::fixPointOnCircle(
    const EntityWrapperPtr& theCircular)
{
  static const double scale = 0.01;
  static const int nbParams = 4;
  myFixedValues.reserve(nbParams); // moved point and center of arc/circle

  EdgeWrapperPtr aCircularEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theCircular);
  std::shared_ptr<GCS::Circle> aCircular =
      std::dynamic_pointer_cast<GCS::Circle>(aCircularEntity->entity());

  // initialize fixed values
  myFixedValues.push_back(*aCircular->center.x + *aCircular->rad);
  myFixedValues.push_back(*aCircular->center.y);
  myFixedValues.push_back(*aCircular->center.x);
  myFixedValues.push_back(*aCircular->center.y);

  // create a moved point
  GCS::Point aPointOnCircle = createGCSPoint(&myFixedValues[0], &myFixedValues[1]);

  std::list<GCSConstraintPtr> aConstraints;
  // point-on-circle
  GCSConstraintPtr aNewConstraint(
      new GCS::ConstraintP2PDistance(aPointOnCircle, aCircular->center, aCircular->rad));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);
  // fixed center (x)
  aNewConstraint = GCSConstraintPtr(
      new GCS::ConstraintEqual(&myFixedValues[2], aCircular->center.x));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);
  // fixed center (y)
  aNewConstraint = GCSConstraintPtr(
      new GCS::ConstraintEqual(&myFixedValues[3], aCircular->center.y));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));
}

ConstraintWrapperPtr SketchSolver_ConstraintMovement::fixPointOnEllipse(
    const EntityWrapperPtr& theConic)
{
  static const double scale = 0.01;
  static const int nbParams = 6;
  myFixedValues.reserve(nbParams); // moved point; center and focus of ellipse

  EdgeWrapperPtr anEdge = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theConic);
  std::shared_ptr<GCS::Ellipse> aConic = std::dynamic_pointer_cast<GCS::Ellipse>(anEdge->entity());

  // major axis direction
  double dx = *aConic->focus1.x - *aConic->center.x;
  double dy = *aConic->focus1.y - *aConic->center.y;
  double norm = sqrt(dx * dx + dy* dy);
  if (norm < tolerance) {
    dx = 1.0;
    dy = 0.0;
  }
  else {
    dx /= norm;
    dy /= norm;
  }

  double aMajorRad = aConic->getRadMaj();

  // initialize fixed values
  myFixedValues.push_back(*aConic->center.x + dx * aMajorRad);
  myFixedValues.push_back(*aConic->center.y + dy * aMajorRad);
  myFixedValues.push_back(*aConic->center.x);
  myFixedValues.push_back(*aConic->center.y);
  myFixedValues.push_back(*aConic->focus1.x);
  myFixedValues.push_back(*aConic->focus1.y);

  // create a moved point
  GCS::Point aPointOnEllipse = createGCSPoint(&myFixedValues[0], &myFixedValues[1]);

  std::list<GCSConstraintPtr> aConstraints;
  // point-on-circle
  GCSConstraintPtr aNewConstraint(
    new GCS::ConstraintPointOnEllipse(aPointOnEllipse, *aConic));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);
  // fixed center (x)
  aNewConstraint = GCSConstraintPtr(
    new GCS::ConstraintEqual(&myFixedValues[2], aConic->center.x));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);
  // fixed center (y)
  aNewConstraint = GCSConstraintPtr(
    new GCS::ConstraintEqual(&myFixedValues[3], aConic->center.y));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);
  // focus on the major axis
  GCS::Point aStartPoint = createGCSPoint(&myFixedValues[2], &myFixedValues[3]);
  GCS::Point aEndPoint   = createGCSPoint(&myFixedValues[4], &myFixedValues[5]);
  aNewConstraint = GCSConstraintPtr(
    new GCS::ConstraintPointOnLine(aConic->focus1, aStartPoint, aEndPoint));
  aNewConstraint->rescale(scale);
  aConstraints.push_back(aNewConstraint);

  return ConstraintWrapperPtr(
    new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));
}

void SketchSolver_ConstraintMovement::startPoint(
    const std::shared_ptr<GeomAPI_Pnt2d>& theStartPoint)
{
  myStartPoint = theStartPoint;
  if (!mySimpleMove) {
    myFixedValues[0] = myStartPoint->x();
    myFixedValues[1] = myStartPoint->y();
  }
}

void SketchSolver_ConstraintMovement::moveTo(
    const std::shared_ptr<GeomAPI_Pnt2d>& theDestinationPoint)
{
  if (!myMovedFeature)
    return; // nothing to move

  double aDelta[2] = { theDestinationPoint->x() - myStartPoint->x(),
                       theDestinationPoint->y() - myStartPoint->y() };

#ifdef CHANGE_RADIUS_WHILE_MOVE
  int aMaxSize = mySimpleMove ? (int)myFixedValues.size() : 2;
#else
  int aMaxSize =
      myMovedFeature->getKind() == SketchPlugin_Line::ID() && !myDraggedAttribute ? 4 : 2;
#endif
  for (int i = 0; i < aMaxSize; ++i)
    myFixedValues[i] += aDelta[i % 2];
}
