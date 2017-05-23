// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintFixed.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_EntityDestroyer.h>
#include <PlaneGCSSolver_FeatureBuilder.h>
#include <PlaneGCSSolver_PointWrapper.h>

#include <GeomDataAPI_Point2D.h>
#include <SketchPlugin_Feature.h>

#include <cmath>

// Verify the points are equal
static bool isEqual(const GCS::Point& thePoint1, const GCS::Point& thePoint2);
// Verify the entities are equal
static bool isEqual(const EntityWrapperPtr& theEntity1, const EntityWrapperPtr& theEntity2);
// Convert entity to the list of parameters
static GCS::VEC_pD toParameters(const EntityWrapperPtr& theEntity);


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

  EntityWrapperPtr aBaseEntity;
  getAttributes(aBaseEntity, myFixedEntity);
  if (!aBaseEntity) {
    moveFeature(); // remove myFixed entity
    myErrorMsg = SketchSolver_Error::ALREADY_FIXED();
  }
  if (!myErrorMsg.empty())
    return;

  fixFeature(aBaseEntity);
}

void SketchSolver_ConstraintFixed::fixFeature(EntityWrapperPtr theFeature)
{
  GCS::VEC_pD aParameters = toParameters(theFeature);

  // Fix given list of parameters
  std::list<GCSConstraintPtr> aConstraints;
  myFixedValues.reserve(aParameters.size());
  GCS::VEC_pD::const_iterator anIt = aParameters.begin();
  for (int i = 0; anIt != aParameters.end(); ++anIt, ++i) {
    myFixedValues.push_back(**anIt);
    aConstraints.push_back(
        GCSConstraintPtr(new GCS::ConstraintEqual(&myFixedValues[i], *anIt)));
  }

  myConstraint = ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));

  if (myBaseConstraint)
    myStorage->addConstraint(myBaseConstraint, myConstraint);
  else
    myStorage->addTemporaryConstraint(myConstraint);
}

void SketchSolver_ConstraintFixed::getAttributes(EntityWrapperPtr& theBaseEntity,
                                                 EntityWrapperPtr& theFixedEntity)
{
  if (myBaseFeature) {
    // if the feature is copy, do not move it
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(myBaseFeature);
    if (aSketchFeature && aSketchFeature->isCopy()) {
      myStorage->setNeedToResolve(true);
      return;
    }

    bool fixFullFeature = false;
    theBaseEntity = EntityWrapperPtr();
    theFixedEntity = EntityWrapperPtr();

    // The feature is fixed.
    PlaneGCSSolver_FeatureBuilder aBuilder;
    std::list<AttributePtr> aBaseAttr = myBaseFeature->data()->attributes(std::string());
    std::list<AttributePtr>::const_iterator anIt = aBaseAttr.begin();
    for (; anIt != aBaseAttr.end(); ++anIt) {
      EntityWrapperPtr anEntity = aBuilder.createAttribute(*anIt);
      EntityWrapperPtr aBaseEntity = myStorage->entity(*anIt);
      if (!anEntity || !aBaseEntity)
        continue;

      if (!fixFullFeature && anEntity->type() == ENTITY_POINT &&
          !isEqual(anEntity, aBaseEntity)) {
        if (theFixedEntity)
          fixFullFeature = true;
        theFixedEntity = anEntity;
        theBaseEntity = aBaseEntity;
      }
    }

    if (fixFullFeature) {
      theFixedEntity = aBuilder.createFeature(myBaseFeature);
      theBaseEntity = myStorage->entity(myBaseFeature);
      if (isEqual(theBaseEntity, theFixedEntity))
        theBaseEntity = EntityWrapperPtr(); // do not want to fix already fixed entity
    }

  } else if (myBaseConstraint) {
    // Constraint Fixed is added by user.
    // Get the attribute of constraint (it should be alone in the list of constraints).
    EntityWrapperPtr aValue;
    std::vector<EntityWrapperPtr> anAttributes;
    SketchSolver_Constraint::getAttributes(aValue, anAttributes);
    std::vector<EntityWrapperPtr>::const_iterator anIt = anAttributes.begin();
    for (; anIt != anAttributes.end(); ++anIt)
      if (*anIt)
        theBaseEntity = *anIt;
  } else
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
}

void SketchSolver_ConstraintFixed::moveFeature()
{
  if (!myFixedEntity)
    return;

  GCS::VEC_pD aFixedParams = toParameters(myFixedEntity);
  for (int i = 0; i < aFixedParams.size() && i < myFixedValues.size(); ++i)
    myFixedValues[i] = *(aFixedParams[i]);

  // Remove fixed entity due to its parameters already copied into the constraint
  PlaneGCSSolver_EntityDestroyer aDestroyer;
  aDestroyer.remove(myFixedEntity);
  std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage)->removeParameters(
      aDestroyer.parametersToRemove());

  myFixedEntity = EntityWrapperPtr();
}




// ====================   Auxiliary functions   ===============================
GCS::VEC_pD toParameters(const EntityWrapperPtr& theEntity)
{
  GCS::VEC_pD aParameters;
  if (!theEntity)
    return aParameters;

  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);

  // Collect parameters for each type of entity
  switch (theEntity->type()) {
  case ENTITY_POINT: {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
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

  return aParameters;
}

bool isEqual(const GCS::Point& thePoint1, const GCS::Point& thePoint2)
{
  return fabs((*thePoint1.x) - (*thePoint2.x)) <= tolerance &&
         fabs((*thePoint1.y) - (*thePoint2.y)) <= tolerance;
}

bool isEqual(const EntityWrapperPtr& theEntity1, const EntityWrapperPtr& theEntity2)
{
  GCS::VEC_pD aParamList1 = toParameters(theEntity1);
  GCS::VEC_pD aParamList2 = toParameters(theEntity2);

  GCS::VEC_pD::const_iterator anIt1 = aParamList1.begin();
  GCS::VEC_pD::const_iterator anIt2 = aParamList2.begin();
  for (; anIt1 != aParamList1.end() && anIt2 != aParamList2.end(); ++anIt1, ++anIt2)
    if (fabs((**anIt1) - (**anIt2)) > tolerance)
      return false;

  return anIt1 == aParamList1.end() && anIt2 == aParamList2.end();
}
