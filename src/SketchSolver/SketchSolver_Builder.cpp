// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Builder.cpp
// Created: 25 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchSolver_Builder.h"
#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintAngle.h>
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_ConstraintCollinear.h>
#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_ConstraintEqual.h>
#include <SketchSolver_ConstraintFixed.h>
#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_ConstraintMiddle.h>
#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_ConstraintMultiTranslation.h>

#ifdef _DEBUG
#include <Events_InfoMessage.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>
#include <SketchSolver_Error.h>
#endif

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>

#include <cmath>

SolverConstraintPtr SketchSolver_Builder::createConstraint(ConstraintPtr theConstraint) const
{
  SolverConstraintPtr aResult;
  DataPtr aData = theConstraint->data();

#ifdef _DEBUG
  // Verify attributes of constraint and generate errors
  std::list<AttributePtr> anAttrList = aData->attributes(std::string());
  std::list<AttributePtr>::iterator anIter = anAttrList.begin();
  for (; anIter != anAttrList.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (aRefAttr) {
      if (aRefAttr->isObject() && aRefAttr->object()) {
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aRefAttr->object());
        if (!aRC)
          Events_InfoMessage("SketchSolver_Builder",
                             SketchSolver_Error::NEED_OBJECT_NOT_FEATURE(), this).send();
      }
      continue;
    }
    AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIter);
    if (aRefList) {
      std::list<ObjectPtr> aList = aRefList->list();
      std::list<ObjectPtr>::iterator aListIter = aList.begin();
      for (; aListIter != aList.end(); aListIter++) {
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aListIter);
        if (*aListIter && !aRC)
          Events_InfoMessage("SketchSolver_Builder",
            SketchSolver_Error::NEED_OBJECT_NOT_FEATURE(), this).send();
      }
    }
  }
#endif

  if (theConstraint->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintCoincidence(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintCollinear::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintCollinear(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintDistance::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintDistance(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintEqual::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintEqual(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintLength(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintMiddle::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMiddle(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMirror(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintTangent::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintTangent(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintRigid::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintFixed(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_MultiTranslation::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMultiTranslation(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_MultiRotation::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMultiRotation(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintAngle::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintAngle(theConstraint));
  }
  // All other types of constraints
  return SolverConstraintPtr(new SketchSolver_Constraint(theConstraint));
}

SolverConstraintPtr SketchSolver_Builder::createMovementConstraint(FeaturePtr theMovedFeature) const
{
  return SolverConstraintPtr(new SketchSolver_ConstraintFixed(theMovedFeature));
}
