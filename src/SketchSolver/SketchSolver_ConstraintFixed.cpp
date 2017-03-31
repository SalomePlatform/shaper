// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintFixed.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>

#include <GeomDataAPI_Point2D.h>
#include <SketchPlugin_Feature.h>

#include <cmath>

// Find parameters of the feature that have been updated during the movement
static EntityWrapperPtr getChangedEntity(const FeaturePtr& theFeature,
                                         const StoragePtr& theStorage);


SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(ConstraintPtr theConstraint)
  : SketchSolver_Constraint(theConstraint)
{
  myType = CONSTRAINT_FIXED;
}

SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(FeaturePtr theFeature)
  : SketchSolver_Constraint(),
    myBaseFeature(theFeature)
{
  myType = CONSTRAINT_FIXED;
}

void SketchSolver_ConstraintFixed::blockEvents(bool isBlocked)
{
  if (myBaseFeature)
    myBaseFeature->data()->blockSendAttributeUpdated(isBlocked);
  if (myBaseConstraint)
    SketchSolver_Constraint::blockEvents(isBlocked);
}

void SketchSolver_ConstraintFixed::process()
{
  cleanErrorMsg();
  if ((!myBaseConstraint && !myBaseFeature) || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anEntities;
  getAttributes(aValue, anEntities);
  if (anEntities.empty())
    myErrorMsg = SketchSolver_Error::ALREADY_FIXED();
  if (!myErrorMsg.empty())
    return;
  fixFeature(anEntities.front());
}

void SketchSolver_ConstraintFixed::fixFeature(EntityWrapperPtr theFeature)
{
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theFeature);

  GCS::VEC_pD aParameters; // parameters of entity to be fixed

  // Collect parameters for each type of entity
  switch (theFeature->type()) {
  case ENTITY_POINT: {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theFeature);
    aParameters.push_back(aPoint->point()->x);
    aParameters.push_back(aPoint->point()->y);
    break;
    }
  case ENTITY_LINE: {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(anEntity->entity());
    aParameters.push_back(aLine->p1.x);
    aParameters.push_back(aLine->p1.y);
    aParameters.push_back(aLine->p2.x);
    aParameters.push_back(aLine->p2.y);
    break;
    }
  case ENTITY_CIRCLE: {
    std::shared_ptr<GCS::Circle> aCircle =
        std::dynamic_pointer_cast<GCS::Circle>(anEntity->entity());
    aParameters.push_back(aCircle->center.x);
    aParameters.push_back(aCircle->center.y);
    aParameters.push_back(aCircle->rad);
    break;
    }
  case ENTITY_ARC: {
    myFixedValues.reserve(4);
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEntity->entity());
    aParameters.push_back(anArc->center.x);
    aParameters.push_back(anArc->center.y);
    aParameters.push_back(anArc->rad);
    aParameters.push_back(anArc->startAngle);
    aParameters.push_back(anArc->endAngle);
    break;
    }
  default:
    break;
  }

  // Fix given list of parameters
  std::list<GCSConstraintPtr> aConstraints;
  myFixedValues.reserve(aParameters.size());
  GCS::VEC_pD::const_iterator anIt = aParameters.begin();
  for (int i = 0; anIt != aParameters.end(); ++anIt, ++i) {
    myFixedValues.push_back(**anIt);
    aConstraints.push_back(
        GCSConstraintPtr(new GCS::ConstraintEqual(*anIt, &myFixedValues[i])));
  }

  myConstraint = ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));

  if (myBaseConstraint)
    myStorage->addConstraint(myBaseConstraint, myConstraint);
  else
    myStorage->addTemporaryConstraint(myConstraint);
}

void SketchSolver_ConstraintFixed::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  if (myBaseFeature) {
    // if the feature is copy, do not move it
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(myBaseFeature);
    if (aSketchFeature && aSketchFeature->isCopy()) {
      myStorage->setNeedToResolve(true);
      return;
    }

    // The feature is fixed.
    EntityWrapperPtr aSolverEntity = getChangedEntity(myBaseFeature, myStorage);
    myStorage->update(myBaseFeature);
    if (aSolverEntity)
      theAttributes.push_back(aSolverEntity);
  } else if (myBaseConstraint) {
    // Constraint Fixed is added by user.
    // Get the attribute of constraint (it should be alone in the list of constraints).
    std::vector<EntityWrapperPtr> anAttributes;
    SketchSolver_Constraint::getAttributes(theValue, anAttributes);
    std::vector<EntityWrapperPtr>::const_iterator anIt = anAttributes.begin();
    for (; anIt != anAttributes.end(); ++anIt)
      if (*anIt)
        theAttributes.push_back(*anIt);
  } else
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
}




// ====================   Auxiliary functions   ===============================
static bool isSameCoordinates(const AttributePoint2DPtr& thePointAttr,
                              const PointWrapperPtr& thePointWrapper)
{
  GCSPointPtr aGCSPoint = thePointWrapper->point();
  return fabs(*aGCSPoint->x - thePointAttr->x()) < tolerance &&
         fabs(*aGCSPoint->y - thePointAttr->y()) < tolerance;
}

EntityWrapperPtr getChangedEntity(const FeaturePtr& theFeature,
                                  const StoragePtr& theStorage)
{
  std::list<AttributePtr> aPoints = theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<EntityWrapperPtr> aChangedPoints;

  std::list<AttributePtr>::const_iterator aPIt = aPoints.begin();
  for (; aPIt != aPoints.end(); ++aPIt) {
    AttributePoint2DPtr aPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*aPIt);
    EntityWrapperPtr anEnt = theStorage->entity(*aPIt);
    if (anEnt) {
      PointWrapperPtr aPW = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anEnt);
      if (!isSameCoordinates(aPnt, aPW))
        aChangedPoints.push_back(anEnt);
    } else {
      theStorage->update(*aPIt);
      aChangedPoints.push_back(theStorage->entity(*aPIt));
    }
  }

  EntityWrapperPtr aChanged;
  if (aChangedPoints.size() == 1)
    aChanged = aChangedPoints.front();
  else if (!aChangedPoints.empty()) // update whole feature
    aChanged = theStorage->entity(theFeature);
  return aChanged;
}
