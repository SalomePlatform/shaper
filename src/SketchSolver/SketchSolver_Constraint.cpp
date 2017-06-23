// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

#include <SketchSolver_Constraint.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_AttributeBuilder.h>
#include <PlaneGCSSolver_Tools.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>

#include <GeomAPI_Dir2d.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>

#include <math.h>

SketchSolver_Constraint::SketchSolver_Constraint(
    ConstraintPtr  theConstraint)
  : myBaseConstraint(theConstraint),
    myType(CONSTRAINT_UNKNOWN)
{
}

void SketchSolver_Constraint::process(StoragePtr theStorage, bool theEvensBlocked)
{
  myStorage = theStorage;
  blockEvents(theEvensBlocked);
  // Process constraint according to its type
  process();
}

void SketchSolver_Constraint::blockEvents(bool isBlocked)
{
  myBaseConstraint->data()->blockSendAttributeUpdated(isBlocked);
}


SketchSolver_ConstraintType SketchSolver_Constraint::TYPE(ConstraintPtr theConstraint)
{
  const std::string& aType = theConstraint->getKind();
  if (aType == SketchPlugin_ConstraintCoincidence::ID())
    return CONSTRAINT_COINCIDENCE;
  else if (aType == SketchPlugin_ConstraintRigid::ID())
    return CONSTRAINT_FIXED;
  else if (aType == SketchPlugin_ConstraintHorizontal::ID())
    return CONSTRAINT_HORIZONTAL;
  else if (aType == SketchPlugin_ConstraintVertical::ID())
    return CONSTRAINT_VERTICAL;
  else if (aType == SketchPlugin_ConstraintAngle::ID())
    return CONSTRAINT_ANGLE;
  else if (aType == SketchPlugin_ConstraintDistance::ID())
    return CONSTRAINT_DISTANCE;
  else if (aType == SketchPlugin_ConstraintEqual::ID())
    return CONSTRAINT_EQUAL;
  else if (aType == SketchPlugin_ConstraintLength::ID())
    return CONSTRAINT_PT_PT_DISTANCE;
  else if (aType == SketchPlugin_ConstraintMirror::ID())
    return CONSTRAINT_SYMMETRIC;
  else if (aType == SketchPlugin_ConstraintParallel::ID())
    return CONSTRAINT_PARALLEL;
  else if (aType == SketchPlugin_ConstraintPerpendicular::ID())
    return CONSTRAINT_PERPENDICULAR;
  else if (aType == SketchPlugin_ConstraintRadius::ID())
    return CONSTRAINT_RADIUS;
  else if (aType == SketchPlugin_ConstraintTangent::ID())
    return CONSTRAINT_TANGENT;
  else if (aType == SketchPlugin_ConstraintCollinear::ID())
    return CONSTRAINT_COLLINEAR;
  else if (aType == SketchPlugin_ConstraintMiddle::ID())
    return CONSTRAINT_MIDDLE_POINT;
  return CONSTRAINT_UNKNOWN;
}

void SketchSolver_Constraint::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  SketchSolver_ConstraintType aConstrType = getType();
  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;
  if (anAttributes.empty()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }
  if (aConstrType == CONSTRAINT_UNKNOWN)
    aConstrType = getType();

  ConstraintWrapperPtr aNewConstraint = PlaneGCSSolver_Tools::createConstraint(
      myBaseConstraint, aConstrType,
      aValue, anAttributes[0], anAttributes[1], anAttributes[2], anAttributes[3]);
  myStorage->addConstraint(myBaseConstraint, aNewConstraint);

  adjustConstraint();
}

void SketchSolver_Constraint::update()
{
  cleanErrorMsg();

  // Get list of attributes of the constraint and compare it with previously stored.
  // If the lists are different, fully rebuild constraint
  std::set<EntityWrapperPtr> anAttributes;
  for (int anEntIndex = 0; anEntIndex < 4; ++anEntIndex) {
    AttributePtr anAttr =
        myBaseConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(anEntIndex));
    if (!anAttr)
      continue;

    if (myBaseConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
      FeaturePtr aFeat = ModelAPI_Feature::feature(aRefAttr->object());
      if (aFeat) {
        // Workaround for the Length constraint: add points of line, not line itself
        anAttributes.insert(myStorage->entity(aFeat->attribute(SketchPlugin_Line::START_ID())));
        anAttributes.insert(myStorage->entity(aFeat->attribute(SketchPlugin_Line::END_ID())));
      }
    } else
      anAttributes.insert(myStorage->entity(anAttr));
  }

  std::set<EntityWrapperPtr>::iterator aFound;
  std::list<EntityWrapperPtr>::const_iterator anAttrIt = myAttributes.begin();
  for (; anAttrIt != myAttributes.end() && !anAttributes.empty(); ++anAttrIt)
    anAttributes.erase(*anAttrIt);

  if (!anAttributes.empty()) {
    remove();
    process();
    return;
  }

  AttributePtr aValueAttr = myBaseConstraint->attribute(SketchPlugin_Constraint::VALUE());
  if (aValueAttr)
    myStorage->update(aValueAttr);

  adjustConstraint();
}

bool SketchSolver_Constraint::remove()
{
  cleanErrorMsg();
  myType = CONSTRAINT_UNKNOWN;
  myStorage->unsubscribeUpdates(this);
  return myStorage->removeConstraint(myBaseConstraint);
}

void SketchSolver_Constraint::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  static const int anInitNbOfAttr = 4;
  theAttributes.assign(anInitNbOfAttr, EntityWrapperPtr());
  myAttributes.clear();

  DataPtr aData = myBaseConstraint->data();

  myType = TYPE(myBaseConstraint);

  AttributePtr aValueAttr = aData->attribute(SketchPlugin_Constraint::VALUE());
  if (aValueAttr) {
    PlaneGCSSolver_AttributeBuilder aValueBuilder;
    theValue = aValueBuilder.createAttribute(aValueAttr);
    myStorage->addEntity(aValueAttr, theValue);
  }

  int aPtInd = 0; // index of first point in the list of attributes
  int aEntInd = 2; // index of first entity in the list of attributes
  std::list<AttributePtr> aConstrAttrs = aData->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anIter = aConstrAttrs.begin();
  for (; anIter != aConstrAttrs.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (!aRefAttr || !aRefAttr->isInitialized()) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }

    myStorage->update(*anIter, true);
    EntityWrapperPtr anEntity = myStorage->entity(*anIter);
    myAttributes.push_back(anEntity);

    SketchSolver_EntityType aType = anEntity->type();
    if (aType == ENTITY_UNKNOWN)
      continue;
    else if (aType == ENTITY_POINT)
      theAttributes[aPtInd++] = anEntity; // the point is created
    else { // another entity (not a point) is created
      if (aEntInd < anInitNbOfAttr)
        theAttributes[aEntInd] = anEntity;
      else
        theAttributes.push_back(anEntity);
      aEntInd++;
    }
  }
}
