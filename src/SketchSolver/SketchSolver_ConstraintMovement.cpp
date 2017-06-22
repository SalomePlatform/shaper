// Copyright (C) 2017  CEA/DEN, EDF R&D
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

#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Pnt2d.h>


SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(FeaturePtr theFeature)
  : SketchSolver_ConstraintFixed(ConstraintPtr()),
    myMovedFeature(theFeature),
    mySimpleMove(true)
{
}

SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(AttributePtr thePoint)
  : SketchSolver_ConstraintFixed(ConstraintPtr()),
    myDraggedPoint(thePoint),
    mySimpleMove(true)
{
  myMovedFeature = ModelAPI_Feature::feature(thePoint->owner());
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
  if (theMovedFeature->getKind() == SketchPlugin_Circle::ID())
    isSimple = (theDraggedPoint.get() != 0);
  else if (theMovedFeature->getKind() == SketchPlugin_Arc::ID()) {
    isSimple = (theDraggedPoint.get() != 0 &&
                theDraggedPoint->id() == SketchPlugin_Arc::CENTER_ID());
  }
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

  EntityWrapperPtr anEntity =
      myDraggedPoint ? myStorage->entity(myDraggedPoint) : myStorage->entity(myMovedFeature);
  if (!anEntity) {
    myStorage->update(myMovedFeature, true);
    anEntity =
        myDraggedPoint ? myStorage->entity(myDraggedPoint) : myStorage->entity(myMovedFeature);
    if (!anEntity)
      return aConstraint;
  }

  mySimpleMove = isSimpleMove(myMovedFeature, myDraggedPoint);

  if (mySimpleMove)
    aConstraint = fixFeature(anEntity);
  else {
    if (myDraggedPoint) // start or end point of arc has been moved
      aConstraint = fixArcExtremity(anEntity);
    else // arc or circle has been moved
      aConstraint = fixPointOnCircle(anEntity);
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

  PointWrapperPtr aPoint =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theArcExtremity);

  double* aParams[nbParams] = { aPoint->point()->x, aPoint->point()->y,
                                anArc->center.x, anArc->center.y };

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
  GCS::Point aPointOnCircle;
  aPointOnCircle.x = &myFixedValues[0];
  aPointOnCircle.y = &myFixedValues[1];

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
  double aDelta[2] = { theDestinationPoint->x() - myStartPoint->x(),
                       theDestinationPoint->y() - myStartPoint->y() };

  int aMaxSize = mySimpleMove ? (int)myFixedValues.size() : 2;
  for (int i = 0; i < aMaxSize; ++i)
    myFixedValues[i] += aDelta[i % 2];
}
