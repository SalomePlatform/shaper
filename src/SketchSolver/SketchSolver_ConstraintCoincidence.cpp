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

#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>
#include <PlaneGCSSolver_PointArrayWrapper.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeInteger.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

static void getCoincidentFeatureExtremities(const ConstraintPtr& theConstraint,
                                            const StoragePtr& theStorage,
                                            EntityWrapperPtr theExtremities[2])
{
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
    AttributeRefAttrPtr aRefAttr = theConstraint->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
    if (!aRefAttr || !aRefAttr->isObject())
      continue;

    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
    if (!aFeature)
      continue;

    if (aFeature->getKind() == SketchPlugin_Line::ID()) {
      theExtremities[0] = theStorage->entity(aFeature->attribute(SketchPlugin_Line::START_ID()));
      theExtremities[1] = theStorage->entity(aFeature->attribute(SketchPlugin_Line::END_ID()));
    } else if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      theExtremities[0] = theStorage->entity(aFeature->attribute(SketchPlugin_Arc::START_ID()));
      theExtremities[1] = theStorage->entity(aFeature->attribute(SketchPlugin_Arc::END_ID()));
    } else if (aFeature->getKind() == SketchPlugin_EllipticArc::ID()) {
      theExtremities[0] = theStorage->entity(
          aFeature->attribute(SketchPlugin_EllipticArc::START_POINT_ID()));
      theExtremities[1] = theStorage->entity(
          aFeature->attribute(SketchPlugin_EllipticArc::END_POINT_ID()));
    }
  }
}

static void getPointOwnerAndParent(const AttributeRefAttrPtr theRefAttr,
                                   AttributePoint2DPtr& thePoint,
                                   FeaturePtr& theOwner,
                                   FeaturePtr& theParent)
{
  AttributePtr anAttr = theRefAttr->attr();
  if (theRefAttr->isObject()) {
    FeaturePtr anOwner = ModelAPI_Feature::feature(theRefAttr->object());
    if (anOwner && anOwner->getKind() == SketchPlugin_Point::ID())
      anAttr = anOwner->attribute(SketchPlugin_Point::COORD_ID());
    else
      return;
  }
  thePoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
  if (thePoint) {
    theOwner = ModelAPI_Feature::feature(thePoint->owner());
    if (theOwner) {
      AttributeReferencePtr aParentRef =
          theOwner->reference(SketchPlugin_SketchEntity::PARENT_ID());
      theParent = aParentRef ? ModelAPI_Feature::feature(aParentRef->value()) : FeaturePtr();
    }
  }
}

static void ellipseDiameters(FeaturePtr theEllipse,
                             std::pair<std::string, std::string>& theMajorAxis,
                             std::pair<std::string, std::string>& theMinorAxis)
{
  if (theEllipse->getKind() == SketchPlugin_Ellipse::ID()) {
    theMajorAxis.first = SketchPlugin_Ellipse::MAJOR_AXIS_START_ID();
    theMajorAxis.second = SketchPlugin_Ellipse::MAJOR_AXIS_END_ID();
    theMinorAxis.first = SketchPlugin_Ellipse::MINOR_AXIS_START_ID();
    theMinorAxis.second = SketchPlugin_Ellipse::MINOR_AXIS_END_ID();
  } else if (theEllipse->getKind() == SketchPlugin_EllipticArc::ID()) {
    theMajorAxis.first = SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID();
    theMajorAxis.second = SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID();
    theMinorAxis.first = SketchPlugin_EllipticArc::MINOR_AXIS_START_ID();
    theMinorAxis.second = SketchPlugin_EllipticArc::MINOR_AXIS_END_ID();
  }
}

static void findDiameterOnEllipse(FeaturePtr theConstruction,
                                  FeaturePtr theEllipse,
                                  AttributePtr& theStart,
                                  AttributePtr& theEnd)
{
  AttributePtr anEllipseAttr;
  const std::set<AttributePtr>& aRefs = theConstruction->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
       aRefIt != aRefs.end(); ++aRefIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
    if (anOwner && anOwner->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID()) {
      AttributeRefAttrPtr aRefAttr;
      if ((*aRefIt)->id() == SketchPlugin_Constraint::ENTITY_A())
        aRefAttr = anOwner->refattr(SketchPlugin_Constraint::ENTITY_B());
      else
        aRefAttr = anOwner->refattr(SketchPlugin_Constraint::ENTITY_A());
      anEllipseAttr = aRefAttr->attr();
      break;
    }
  }
  if (!anEllipseAttr)
    return;

  std::pair<std::string, std::string> aMajorAxis, aMinorAxis;
  ellipseDiameters(theEllipse, aMajorAxis, aMinorAxis);
  if (anEllipseAttr->id() == aMajorAxis.first) {
    theStart = anEllipseAttr;
    theEnd = theEllipse->attribute(aMajorAxis.second);
  }
  else if (anEllipseAttr->id() == aMajorAxis.second) {
    theStart = theEllipse->attribute(aMajorAxis.first);
    theEnd = anEllipseAttr;
  }
  else if (anEllipseAttr->id() == aMinorAxis.first) {
    theStart = anEllipseAttr;
    theEnd = theEllipse->attribute(aMinorAxis.second);
  }
  else if (anEllipseAttr->id() == aMinorAxis.second) {
    theStart = theEllipse->attribute(aMinorAxis.first);
    theEnd = anEllipseAttr;
  }
}

static void processEllipticArcExtremities(SketchSolver_ConstraintType& theType,
                                          const ConstraintPtr& theConstraint,
                                          const StoragePtr& theStorage,
                                          std::vector<EntityWrapperPtr>& theAttributes,
                                          EntityWrapperPtr theExtremities[2])
{
  AttributePoint2DPtr aPointA, aPointB;
  FeaturePtr anOwnerA, anOwnerB;
  FeaturePtr aParentA, aParentB;
  getPointOwnerAndParent(theConstraint->refattr(SketchPlugin_Constraint::ENTITY_A()),
                         aPointA, anOwnerA, aParentA);
  getPointOwnerAndParent(theConstraint->refattr(SketchPlugin_Constraint::ENTITY_B()),
                         aPointB, anOwnerB, aParentB);

  AttributePtr anAxisStart, anAxisEnd, aPoint;
  FeaturePtr aConstruction, anEllipticArc;
  if (aParentA && aParentA == anOwnerB) {
    aPoint = aPointB;
    aConstruction = anOwnerA;
    anEllipticArc = anOwnerB;
  }
  else if (aParentB && aParentB == anOwnerA) {
    aPoint = aPointA;
    aConstruction = anOwnerB;
    anEllipticArc = anOwnerA;
  }

  if (!anEllipticArc || anEllipticArc->getKind() != SketchPlugin_EllipticArc::ID() ||
      (aPoint->id() != SketchPlugin_EllipticArc::START_POINT_ID() &&
       aPoint->id() != SketchPlugin_EllipticArc::END_POINT_ID()))
    return;

  findDiameterOnEllipse(aConstruction, anEllipticArc, anAxisStart, anAxisEnd);

  if (anAxisStart && anAxisEnd) {
    theAttributes[0] = theStorage->entity(aPoint);
    theAttributes[1] = theStorage->entity(anAxisStart);
    theAttributes[2] = theStorage->entity(anAxisEnd);
    theType = CONSTRAINT_PT_ON_CURVE;
    getCoincidentFeatureExtremities(theConstraint, theStorage, theExtremities);
  }
}

static void getPointFromArray(EntityWrapperPtr& theArray,
                              const ConstraintPtr& theConstraint,
                              const std::string& theIndexAttrId)
{
  if (theArray && theArray->type() == ENTITY_POINT_ARRAY) {
    AttributeIntegerPtr anIndexAttr = theConstraint->integer(theIndexAttrId);
    if (anIndexAttr) {
      PointArrayWrapperPtr aPointsArray =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointArrayWrapper>(theArray);
      theArray = aPointsArray->value(anIndexAttr->value());
    }
  }
}


void SketchSolver_ConstraintCoincidence::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;
  if (anAttributes.empty()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  mySolverConstraint = PlaneGCSSolver_Tools::createConstraint(
      myBaseConstraint, getType(),
      myAuxValue, anAttributes[0], anAttributes[1], anAttributes[2], anAttributes[3]);

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateCoincidence::GROUP());
  myStorage->notify(myBaseConstraint);
}

bool SketchSolver_ConstraintCoincidence::remove()
{
  myInSolver = false;
  myFeatureExtremities[0] = EntityWrapperPtr();
  myFeatureExtremities[1] = EntityWrapperPtr();
  if (myAuxValue) {
    std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);
    GCS::SET_pD aParams;
    aParams.insert(myAuxValue->scalar());
    aStorage->removeParameters(aParams);
  }
  return SketchSolver_Constraint::remove();
}

void SketchSolver_ConstraintCoincidence::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1]) {
    myType = CONSTRAINT_PT_PT_COINCIDENT;
    // if elliptic arc boundary point is connected with one of ellipse characteristics,
    // it should be changed from point-point coincidence to coincidence between point
    // and axis of the ellipse to decrease only 1 DoF instead of 2 DoF and avoid overconstraint.
    processEllipticArcExtremities(myType, myBaseConstraint, myStorage,
                                  theAttributes, myFeatureExtremities);
  } else if (theAttributes[2]) {
    myType = CONSTRAINT_PT_ON_CURVE;
    // point-on-bspline requires additional parameter
    if (theAttributes[2]->type() == ENTITY_BSPLINE) {
      std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
          std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);
      myAuxValue.reset(new PlaneGCSSolver_ScalarWrapper(aStorage->createParameter()));
    }
    else {
      // obtain extremity points of the coincident feature for further checking of multi-coincidence
      getCoincidentFeatureExtremities(myBaseConstraint, myStorage, myFeatureExtremities);
    }
  } else
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();

  // process internal coincidence with a point in the array of points
  getPointFromArray(theAttributes[0], myBaseConstraint,
                    SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_A());
  getPointFromArray(theAttributes[1], myBaseConstraint,
                    SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_B());
}

void SketchSolver_ConstraintCoincidence::notify(const FeaturePtr&      theFeature,
                                                PlaneGCSSolver_Update* theUpdater)
{
  PlaneGCSSolver_UpdateCoincidence* anUpdater =
      static_cast<PlaneGCSSolver_UpdateCoincidence*>(theUpdater);
  bool isAccepted = anUpdater->addCoincidence(myAttributes.front(), myAttributes.back());
  // additionally process internal coincidence, set point coincident with ellipse/elliptic arc
  // for correct processing further coincidences set by the user
  if (isAccepted &&
      myBaseConstraint->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID()) {
    AttributeRefAttrPtr aRefAttrA = myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
    AttributeRefAttrPtr aRefAttrB = myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());
    if (aRefAttrA && aRefAttrB) {
      AttributePoint2DPtr anAttrA, anAttrB;
      FeaturePtr anOwnerA, anOwnerB;
      FeaturePtr aParentA, aParentB;
      getPointOwnerAndParent(aRefAttrA, anAttrA, anOwnerA, aParentA);
      getPointOwnerAndParent(aRefAttrB, anAttrB, anOwnerB, aParentB);

      EntityWrapperPtr aPoint, anEntity;
      if (aParentA == anOwnerB) {
        aPoint = myStorage->entity(anAttrA);
        anEntity = myStorage->entity(anOwnerB);
      }
      else if (aParentB == anOwnerA) {
        aPoint = myStorage->entity(anAttrB);
        anEntity = myStorage->entity(anOwnerA);
      }
      if (aPoint && anEntity)
        anUpdater->addCoincidence(aPoint, anEntity);
    }
  }

  // additionally check the point is coincident to extremity of coincident feature
  if (myFeatureExtremities[0] && myFeatureExtremities[1]) {
    EntityWrapperPtr aPoint =
        myAttributes.front()->type() == ENTITY_POINT ? myAttributes.front() : myAttributes.back();

    for (int i = 0; i < 2; ++i)
      isAccepted = isAccepted && !anUpdater->isPointOnEntity(aPoint, myFeatureExtremities[i]);
  }

  if (isAccepted) {
    if (!myInSolver) {
      myInSolver = true;
      myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
    }
  } else {
    if (myInSolver) {
      myInSolver = false;
      myStorage->removeConstraint(myBaseConstraint);
    }
  }
}
