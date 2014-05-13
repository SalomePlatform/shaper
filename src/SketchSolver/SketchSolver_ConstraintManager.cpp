// File:    SketchSolver_ConstraintManager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintManager.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_Line.h>


/// This value is used to give unique index to the groups
static Slvs_hGroup myGroupIndexer = 0;

// ========================================================
// ========= SketchSolver_ConstraintManager ===============
// ========================================================
SketchSolver_ConstraintManager::SketchSolver_ConstraintManager()
{
  myGroups.clear();
}

SketchSolver_ConstraintManager::~SketchSolver_ConstraintManager()
{
  myGroups.clear();
}

void SketchSolver_ConstraintManager::findGroups(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint, 
              std::vector<Slvs_hGroup>&                  theGroupIDs) const
{
  std::vector<SketchSolver_ConstraintGroup>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aGroupIter->isInteract(theConstraint))
      theGroupIDs.push_back(aGroupIter->getId());
}


// ========================================================
// =========  SketchSolver_ConstraintGroup  ===============
// ========================================================

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::SketchSolver_ConstraintGroup()
  : myID(++myGroupIndexer),
    myParamMaxID(0),
    myEntityMaxID(0),
    myConstrMaxID(0),
    myConstraintMap()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();

  // The workplane will be initialized on first constraint, so its handle is NULL meanwhile
  myWorkplane.h = 0;

  // Nullify all elements of the set of equations
  myConstrSet.param = 0;
  myConstrSet.entity = 0;
  myConstrSet.constraint = 0;
  myConstrSet.failed = 0;
}

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::~SketchSolver_ConstraintGroup()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();
  myConstraintMap.clear();

  if (myConstrSet.param)
    delete [] myConstrSet.param;
  if (myConstrSet.entity)
    delete [] myConstrSet.entity;
  if (myConstrSet.constraint)
    delete [] myConstrSet.constraint;
  if (myConstrSet.failed)
    delete [] myConstrSet.failed;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isInteract(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  /// \todo Should be implemented
  return false;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addConstraint(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  if (myWorkplane.h == 0)
  {
    // Create workplane when first constraint is added
    std::list< boost::shared_ptr<ModelAPI_Attribute> > aWPAttr;
    theConstraint->getSketchParameters(aWPAttr);
    if (!addWorkplane(aWPAttr))
      return false;
  }

  // Create constraint parameters
  double aDistance = 0.0; // scalar value of the constraint
  boost::shared_ptr<ModelAPI_AttributeDouble> aDistAttr =
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theConstraint->data()->attribute(CONSTRAINT_ATTR_VALUE));
  if (aDistAttr.get())
    aDistance = aDistAttr->value();

  Slvs_hEntity aPtA, aPtB, aEntityA, aEntityB; // parameters of the constraint
  boost::shared_ptr<ModelAPI_Attribute> aEntAttr = theConstraint->data()->attribute(CONSTRAINT_ATTR_POINT_A);
  aPtA = addEntity(aEntAttr);
  if (aPtA == 0) return false;
  aEntAttr = theConstraint->data()->attribute(CONSTRAINT_ATTR_POINT_B);
  aPtB = addEntity(aEntAttr);
  if (aPtB == 0) return false;
  aEntAttr = theConstraint->data()->attribute(CONSTRAINT_ATTR_ENTITY_A);
  aEntityA = addEntity(aEntAttr);
  if (aEntityA == 0) return false;
  aEntAttr = theConstraint->data()->attribute(CONSTRAINT_ATTR_ENTITY_B);
  aEntityB = addEntity(aEntAttr);
  if (aEntityB == 0) return false;

  // Constraint type
  int aConstrType = getConstraintType(theConstraint);
  if (aConstrType == 0) return false;

  // Create SolveSpace constraint structure
  Slvs_Constraint aConstraint = 
    Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType, myWorkplane.h, 
                        aDistance, aPtA, aPtB, aEntityA, aEntityB);
  myConstraints.push_back(aConstraint);
  myConstraintMap[theConstraint] = *(myConstraints.rbegin());

  return true;
}

Slvs_hEntity SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addEntity(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  /// \todo Should be implemented
  return 0;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addWorkplane(
                std::list< boost::shared_ptr<ModelAPI_Attribute> >& theParams)
{
  /// \todo Should be implemented
  return false;
}

int SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::getConstraintType(
                const boost::shared_ptr<SketchPlugin_Constraint>& theConstraint) const
{
  if (theConstraint->getKind() == SketchPlugin_ConstraintDistance().getKind())
  {
    boost::shared_ptr<ModelAPI_Attribute> aPtA  = theConstraint->data()->attribute(CONSTRAINT_ATTR_POINT_A);
    boost::shared_ptr<ModelAPI_Attribute> aPtB  = theConstraint->data()->attribute(CONSTRAINT_ATTR_POINT_B);
    boost::shared_ptr<ModelAPI_Attribute> aEntA = theConstraint->data()->attribute(CONSTRAINT_ATTR_ENTITY_A);
    boost::shared_ptr<ModelAPI_Attribute> aEntB = theConstraint->data()->attribute(CONSTRAINT_ATTR_ENTITY_B);
    boost::shared_ptr<ModelAPI_AttributeDouble> aDistance = 
      boost::shared_dynamic_cast<ModelAPI_AttributeDouble>(theConstraint->data()->attribute(CONSTRAINT_ATTR_VALUE));
    if (aPtA.get()) // ptA is an attribute of the constraint
    {
//      if (aEntA.get()) // entityA is an attribute of the constraint
//      {
//        if (aEntA->feature()->getKind() == SketchPlugin_Line().getKind()) // entityA is a line
//        {
//          if (aEntB.get() && aEntB->feature()->getKind() == SketchPlugin_Line().getKind()) // entityB is a line too
//            return SLVS_C_ANGLE;
//          else if (aPtB.get()) // ptB is also an attribute of the constraint
//            return SLVS_C_PROJ_PT_DISTANCE;
//          else
//            return SLVS_C_PT_LINE_DISTANCE;
//        }
//        /// \todo Implement other point-entity distances
//      }
//      else 
      if (aPtB.get()) // ptB is an attribute of the constrtaint => point-point distance
      {
        if (aDistance->value() == 0.0)
          return SLVS_C_POINTS_COINCIDENT;
        else 
          return SLVS_C_PT_PT_DISTANCE;
      }
    }
    else if (aEntA.get() && !aEntB.get() && !aPtB.get())
      return SLVS_C_DIAMETER;
    return SLVS_C_UNKNOWN;
  }
  /// \todo Implement other kind of constrtaints

  return SLVS_C_UNKNOWN;
}
