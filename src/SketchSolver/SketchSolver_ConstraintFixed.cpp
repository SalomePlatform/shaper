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

// Verify the entities are equal
static bool isEqual(const EntityWrapperPtr& theEntity1, const EntityWrapperPtr& theEntity2);


SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(ConstraintPtr theConstraint)
  : SketchSolver_Constraint(theConstraint)
{
  myType = CONSTRAINT_FIXED;
}

#ifndef SUPPORT_NEW_MOVE
SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(FeaturePtr theFeature)
  : SketchSolver_Constraint(),
    myBaseFeature(theFeature)
{
  myType = CONSTRAINT_FIXED;
}
#endif

void SketchSolver_ConstraintFixed::blockEvents(bool isBlocked)
{
#ifndef SUPPORT_NEW_MOVE
  if (myBaseFeature)
    myBaseFeature->data()->blockSendAttributeUpdated(isBlocked);
  if (myBaseConstraint)
#endif
  SketchSolver_Constraint::blockEvents(isBlocked);
}

void SketchSolver_ConstraintFixed::process()
{
  cleanErrorMsg();
#ifdef SUPPORT_NEW_MOVE
  if (!myBaseConstraint || !myStorage) {
#else
  if ((!myBaseConstraint && !myBaseFeature) || !myStorage) {
#endif
    // Not enough parameters are assigned
    return;
  }

#ifdef SUPPORT_NEW_MOVE
  EntityWrapperPtr aBaseEntity = entityToFix();
  if (!aBaseEntity)
    myErrorMsg = SketchSolver_Error::ALREADY_FIXED();
#else
  EntityWrapperPtr aBaseEntity;
  getAttributes(aBaseEntity, myFixedEntity);
  if (!aBaseEntity) {
    moveFeature(); // remove myFixed entity
    myErrorMsg = SketchSolver_Error::ALREADY_FIXED();
  }
#endif
  if (!myErrorMsg.empty())
    return;

#ifdef SUPPORT_NEW_MOVE
  ConstraintWrapperPtr aConstraint = fixFeature(aBaseEntity);
  myStorage->addConstraint(myBaseConstraint, aConstraint);
#else
  myConstraint = fixFeature(aBaseEntity);
  if (myBaseConstraint)
    myStorage->addConstraint(myBaseConstraint, myConstraint);
  else
    myStorage->addMovementConstraint(myConstraint);
#endif
}

ConstraintWrapperPtr SketchSolver_ConstraintFixed::fixFeature(EntityWrapperPtr theFeature)
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

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));
}

#ifdef SUPPORT_NEW_MOVE
EntityWrapperPtr SketchSolver_ConstraintFixed::entityToFix()
{
  // Constraint Fixed is added by user.
  // Get the attribute of constraint (it should be alone in the list of constraints).
  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  SketchSolver_Constraint::getAttributes(aValue, anAttributes);
  std::vector<EntityWrapperPtr>::const_iterator anIt = anAttributes.begin();
  for (; anIt != anAttributes.end(); ++anIt)
    if (*anIt)
      return *anIt;
  return EntityWrapperPtr();
}
#else
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

    // The feature is fixed.
    PlaneGCSSolver_FeatureBuilder aBuilder;
    std::list<AttributePtr> aBaseAttr = myBaseFeature->data()->attributes(std::string());
    std::list<AttributePtr>::const_iterator anIt = aBaseAttr.begin();
    for (; anIt != aBaseAttr.end(); ++anIt)
      aBuilder.createAttribute(*anIt);
    theFixedEntity = aBuilder.createFeature(myBaseFeature);

    theBaseEntity = myStorage->entity(myBaseFeature);
    if (isEqual(theBaseEntity, theFixedEntity))
      theBaseEntity = EntityWrapperPtr(); // do not want to fix already fixed entity

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
#endif

GCS::VEC_pD SketchSolver_ConstraintFixed::toParameters(const EntityWrapperPtr& theEntity)
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
#ifndef SUPPORT_NEW_MOVE
    aParameters.push_back(aCircle->rad);
#endif
    break;
    }
  case ENTITY_ARC: {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEntity->entity());
    aParameters.push_back(anArc->center.x);
    aParameters.push_back(anArc->center.y);
#ifndef SUPPORT_NEW_MOVE
    aParameters.push_back(anArc->rad);
    aParameters.push_back(anArc->startAngle);
    aParameters.push_back(anArc->endAngle);
#endif
    break;
    }
  default:
    break;
  }

  return aParameters;
}

#ifndef SUPPORT_NEW_MOVE
// ====================   Auxiliary functions   ===============================
bool isEqual(const EntityWrapperPtr& theEntity1, const EntityWrapperPtr& theEntity2)
{
  GCS::VEC_pD aParamList1 = SketchSolver_ConstraintFixed::toParameters(theEntity1);
  GCS::VEC_pD aParamList2 = SketchSolver_ConstraintFixed::toParameters(theEntity2);

  GCS::VEC_pD::const_iterator anIt1 = aParamList1.begin();
  GCS::VEC_pD::const_iterator anIt2 = aParamList2.begin();
  for (; anIt1 != aParamList1.end() && anIt2 != aParamList2.end(); ++anIt1, ++anIt2)
    if (fabs((**anIt1) - (**anIt2)) > tolerance)
      return false;

  return anIt1 == aParamList1.end() && anIt2 == aParamList2.end();
}
#endif
