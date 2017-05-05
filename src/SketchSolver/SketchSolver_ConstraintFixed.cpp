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

void SketchSolver_ConstraintFixed::blockEvents(bool isBlocked)
{
  SketchSolver_Constraint::blockEvents(isBlocked);
}

void SketchSolver_ConstraintFixed::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aBaseEntity = entityToFix();
  if (!aBaseEntity)
    myErrorMsg = SketchSolver_Error::ALREADY_FIXED();
  if (!myErrorMsg.empty())
    return;

  ConstraintWrapperPtr aConstraint = fixFeature(aBaseEntity);
  myStorage->addConstraint(myBaseConstraint, aConstraint);
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
    break;
    }
  case ENTITY_ARC: {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEntity->entity());
    aParameters.push_back(anArc->center.x);
    aParameters.push_back(anArc->center.y);
    break;
    }
  default:
    break;
  }

  return aParameters;
}
