// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <math.h>


static void getPointAndLine(const ConstraintPtr& theConstraint, const StoragePtr& theStorage,
                            EntityWrapperPtr& thePoint, EntityWrapperPtr& theLine)
{
  for (int i = 0; i < 2; ++i) {
    AttributePtr anAttr = theConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(i));
    EntityWrapperPtr anEntity = theStorage->entity(anAttr);
    if (anEntity->type() == ENTITY_POINT)
      thePoint = anEntity;
    else if (anEntity->type() == ENTITY_LINE)
      theLine = anEntity;
  }
}

static void adjustOddPoint(const EntityWrapperPtr& theDistPoint,
                           const EntityWrapperPtr& theDistLine,
                           GCSPointPtr& theOddPoint)
{
  if (!theOddPoint)
    return;

  std::shared_ptr<GeomAPI_Lin2d> aLine = PlaneGCSSolver_Tools::line(theDistLine);
  std::shared_ptr<GeomAPI_Pnt2d> aPoint = PlaneGCSSolver_Tools::point(theDistPoint);

  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());

  double aDot = aPtLineVec->dot(aLineVec);
  std::shared_ptr<GeomAPI_XY> aProjectedPnt =
    aLine->location()->xy()->added(aLineVec->multiplied(aDot));

  *(theOddPoint->x) = aProjectedPnt->x();
  *(theOddPoint->y) = aProjectedPnt->y();
}

static FeaturePtr getFeature(AttributeRefAttrPtr theRefAttr)
{
  ObjectPtr anObj;
  if (theRefAttr->isObject())
    anObj = theRefAttr->object();
  else
    anObj = theRefAttr->attr()->owner();
  return ModelAPI_Feature::feature(anObj);
}

static void calculateDistanceDirection(const ConstraintPtr& theConstraint,
                                       const StoragePtr& theStorage,
                                       double& theDirX, double& theDirY)
{
  std::shared_ptr<GeomDataAPI_Dir> aDistDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      theConstraint->attribute(SketchPlugin_ConstraintDistance::DIRECTION_ID()));
  if (aDistDir && aDistDir->isInitialized()) {
    theDirX = aDistDir->x();
    theDirY = aDistDir->y();
    if (fabs(theDirX) > tolerance || fabs(theDirY) > tolerance)
      return;
  }

  AttributeRefAttrPtr aRefAttrA = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributeRefAttrPtr aRefAttrB = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());

  EntityWrapperPtr aEntityA = theStorage->entity(aRefAttrA);
  EntityWrapperPtr aEntityB = theStorage->entity(aRefAttrB);

  GCSPointPtr aPoint;
  if (aEntityA->type() != ENTITY_LINE && aEntityB->type() != ENTITY_LINE) {
    aPoint = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(aEntityA)->point();
    theDirX = 1.0;
    theDirY = 0.0;

    EdgeWrapperPtr anEdgeA = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(
        theStorage->entity(getFeature(aRefAttrA)));
    EdgeWrapperPtr anEdgeB = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(
        theStorage->entity(getFeature(aRefAttrB)));

    if (anEdgeA && anEdgeB) {
      GCS::DeriVector2 aDirA = anEdgeA->entity()->CalculateNormal(*aPoint);
      GCS::DeriVector2 aDirB = anEdgeB->entity()->CalculateNormal(*aPoint);
      double x = -aDirA.x + aDirB.x;
      double y = -aDirA.y + aDirB.y;
      double norm = sqrt(x*x + y*y);
      if (norm > tolerance) {
        theDirX = x / norm;
        theDirY = y / norm;
      }
    }
  }
}

static void moveEntity(const ConstraintPtr& theConstraint,
                       const StoragePtr& theStorage,
                       const double theDX, const double theDY)
{
  static const double THE_SHIFT = 1.e-4;

  AttributeRefAttrPtr aRefAttrA = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributeRefAttrPtr aRefAttrB = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());

  EntityWrapperPtr aEntityA = theStorage->entity(aRefAttrA);
  EntityWrapperPtr aEntityB = theStorage->entity(aRefAttrB);

  PointWrapperPtr aPointA = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(aEntityA);
  PointWrapperPtr aPointB = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(aEntityB);

  if (aPointA) {
    *aPointA->point()->x -= THE_SHIFT * theDX;
    *aPointA->point()->y -= THE_SHIFT * theDY;
  }
  else if (aPointB) {
    *aPointB->point()->x += THE_SHIFT * theDX;
    *aPointB->point()->y += THE_SHIFT * theDY;
  }
}



void SketchSolver_ConstraintDistance::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  ScalarWrapperPtr aValue = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(theValue);
  bool isCoincidence = fabs(aValue->value()) < tolerance;

  if (theAttributes[1]) {
    if (myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID())
      myType = CONSTRAINT_HORIZONTAL_DISTANCE;
    else if (myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
      myType = CONSTRAINT_VERTICAL_DISTANCE;
    else
      myType = isCoincidence ? CONSTRAINT_PT_PT_COINCIDENT : CONSTRAINT_PT_PT_DISTANCE;
  } else if (theAttributes[2] && theAttributes[2]->type() == ENTITY_LINE)
    myType = isCoincidence ? CONSTRAINT_PT_ON_CURVE : CONSTRAINT_PT_LINE_DISTANCE;
  else
    theAttributes.clear();

  if (myType == CONSTRAINT_HORIZONTAL_DISTANCE || myType == CONSTRAINT_VERTICAL_DISTANCE)
    mySignValue = aValue->value() < 0.0 ? -1.0 : 1.0;

  myPrevValue = 0.0;

  if (isCoincidence)
    myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateCoincidence::GROUP());
  else
    myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateFeature::GROUP());
}

void SketchSolver_ConstraintDistance::adjustConstraint()
{
  if (getType() == CONSTRAINT_PT_LINE_DISTANCE) {
    bool isSigned = myBaseConstraint->boolean(SketchPlugin_ConstraintDistance::SIGNED())->value();
    if (myIsSigned == isSigned) {
      // adjust auxiliary point for sign-keeping
      if (isSigned) {
        EntityWrapperPtr aDistPoint, aDistLine;
        getPointAndLine(myBaseConstraint, myStorage, aDistPoint, aDistLine);
        adjustOddPoint(aDistPoint, aDistLine, myOddPoint);
      }
    }
    else {
      // Adjust point-line distance by setting/removing additional constraints
      if (isSigned)
        addConstraintsToKeepSign();
      else
        removeConstraintsKeepingSign();
    }
    myIsSigned = isSigned;
  }
}

void SketchSolver_ConstraintDistance::update()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint);
  myPrevValue = aConstraint->value();

  bool isDistanceAlognDir =
    myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID() ||
    myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID();

  AttributeDoublePtr aCurValue = myBaseConstraint->real(SketchPlugin_Constraint::VALUE());
  bool isZeroSwitch = fabs(myPrevValue) < tolerance && fabs(aCurValue->value()) > tolerance;
  bool isNonZeroSwitch = fabs(myPrevValue) > tolerance && fabs(aCurValue->value()) < tolerance;

  if (!isDistanceAlognDir && (isZeroSwitch || isNonZeroSwitch)) {
    // the value is changed from non-zero to zero or vice versa
    remove();
    process();

    // move entities to avoid conflicting constraints
    if (isZeroSwitch) {
      double aDirX, aDirY;
      // calculate the direction basing on the distanced objects
      calculateDistanceDirection(myBaseConstraint, myStorage, aDirX, aDirY);
      moveEntity(myBaseConstraint, myStorage, aDirX, aDirY);

      if (myOddPoint) {
        removeConstraintsKeepingSign();
        addConstraintsToKeepSign();
      }
    }
  }
  else {
    SketchSolver_Constraint::update();
    if (isDistanceAlognDir && mySignValue * aConstraint->value() < 0.0) {
      if (isZeroSwitch)
        aConstraint->setValue(-aConstraint->value());
      else
        mySignValue *= -1.0;
    }
  }
}

bool SketchSolver_ConstraintDistance::remove()
{
  removeConstraintsKeepingSign();
  return SketchSolver_Constraint::remove();
}

void SketchSolver_ConstraintDistance::addConstraintsToKeepSign()
{
  std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
      std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

  ConstraintWrapperPtr aConstraint = aStorage->constraint(myBaseConstraint);
  std::list<GCSConstraintPtr> aGCSConstraints = aConstraint->constraints();

  // calculate projection of the point on the line and find a sign of a distance
  EntityWrapperPtr aDistPoint, aDistLine;
  getPointAndLine(myBaseConstraint, myStorage, aDistPoint, aDistLine);

  std::shared_ptr<GeomAPI_Lin2d> aLine = PlaneGCSSolver_Tools::line(aDistLine);
  std::shared_ptr<GeomAPI_Pnt2d> aPoint = PlaneGCSSolver_Tools::point(aDistPoint);

  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());
  if (aLineVec->cross(aPtLineVec) >= 0.)
    mySignValue = PI/2.0;
  else
    mySignValue = - PI/2.0;

  // create auxiliary point on the line and set auxiliary constraints
  myOddPoint = GCSPointPtr(new GCS::Point);
  myOddPoint->x = aStorage->createParameter();
  myOddPoint->y = aStorage->createParameter();
  adjustOddPoint(aDistPoint, aDistLine, myOddPoint);

  PointWrapperPtr aPointWr = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(aDistPoint);
  EdgeWrapperPtr anEdgeWr = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(aDistLine);
  std::shared_ptr<GCS::Line> aGCSLine = std::dynamic_pointer_cast<GCS::Line>(anEdgeWr->entity());
  // point-on-line
  GCSConstraintPtr aNewConstraint(new GCS::ConstraintPointOnLine(*myOddPoint, *aGCSLine));
  aGCSConstraints.push_back(aNewConstraint);
  // angle which keep orientation
  aNewConstraint = GCSConstraintPtr(new GCS::ConstraintL2LAngle(
      aGCSLine->p1, aGCSLine->p2, *myOddPoint, *(aPointWr->point()), &mySignValue));
  aGCSConstraints.push_back(aNewConstraint);

  aConstraint->setConstraints(aGCSConstraints);

  aStorage->removeConstraint(myBaseConstraint);
  aStorage->addConstraint(myBaseConstraint, aConstraint);
}

void SketchSolver_ConstraintDistance::removeConstraintsKeepingSign()
{
  if (!myOddPoint)
    return; // no sign kept => nothing to remove

  std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
      std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

  ConstraintWrapperPtr aConstraint = aStorage->constraint(myBaseConstraint);
  std::list<GCSConstraintPtr> aGCSConstraints = aConstraint->constraints();

  aStorage->removeConstraint(myBaseConstraint);

  // remove parameters related to auxiliary point
  GCS::SET_pD aParams;
  aParams.insert(myOddPoint->x);
  aParams.insert(myOddPoint->y);
  aStorage->removeParameters(aParams);

  // remove constraints keeping sign of point-line distance,
  // not more than 2 additional constraints is possible
  if (!aGCSConstraints.empty())
    aGCSConstraints.pop_back();
  if (!aGCSConstraints.empty())
    aGCSConstraints.pop_back();
  aConstraint->setConstraints(aGCSConstraints);
  aStorage->addConstraint(myBaseConstraint, aConstraint);

  myIsSigned = false;
}

void SketchSolver_ConstraintDistance::notify(const FeaturePtr& theFeature,
                                             PlaneGCSSolver_Update*)
{
  if (getType() == CONSTRAINT_PT_LINE_DISTANCE && myIsSigned &&
      theFeature->getKind() == SketchPlugin_Sketch::ID()) {
    // the sketch plane was updated, recalculate auxiliary constraints
    removeConstraintsKeepingSign();
    addConstraintsToKeepSign();
    myIsSigned = true; // reset it, due to changing by removeConstraintsKeepingSign()
  }
}
