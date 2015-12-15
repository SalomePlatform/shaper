#include <SketchSolver_Constraint.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
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
    myGroupID(GID_UNKNOWN),
    myType(CONSTRAINT_UNKNOWN)
{
}

void SketchSolver_Constraint::process(StoragePtr theStorage,
                                      const GroupID& theGroupID,
                                      const EntityID& theSketchID)
{
  myStorage = theStorage;
  myGroupID = theGroupID;
  mySketchID = theSketchID;
  // Process constraint according to its type
  process();
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
  return CONSTRAINT_UNKNOWN;
}

void SketchSolver_Constraint::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    // Not enough parameters are assigned
    return;
  }

  SketchSolver_ConstraintType aConstrType = getType();
  double aValue;
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

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aNewConstraints = aBuilder->createConstraint(
      myBaseConstraint, myGroupID, mySketchID, aConstrType,
      aValue, anAttributes[0], anAttributes[1], anAttributes[2], anAttributes[3]);
  myStorage->addConstraint(myBaseConstraint, aNewConstraints);

  adjustConstraint();
}

void SketchSolver_Constraint::update()
{
  cleanErrorMsg();

  std::list<ConstraintWrapperPtr> aWrapper = myStorage->constraint(myBaseConstraint);
  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::VALUE()));
  if (aValueAttr) {
    std::list<ConstraintWrapperPtr>::iterator aWIt = aWrapper.begin();
    for (; aWIt != aWrapper.end(); ++aWIt)
      (*aWIt)->setValue(aValueAttr->value());
  }
  myStorage->addConstraint(myBaseConstraint, aWrapper);

  adjustConstraint();
}

bool SketchSolver_Constraint::remove()
{
  cleanErrorMsg();
  return myStorage->removeConstraint(myBaseConstraint);
}

void SketchSolver_Constraint::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  static const int anInitNbOfAttr = 4;
  theAttributes.assign(anInitNbOfAttr, EntityWrapperPtr());

  DataPtr aData = myBaseConstraint->data();
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  myType = TYPE(myBaseConstraint);

  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()));
  theValue = aValueAttr ? aValueAttr->value() : 0.0;

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

    myStorage->update(*anIter, myGroupID);
    EntityWrapperPtr anEntity = myStorage->entity(*anIter);

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

bool SketchSolver_Constraint::isUsed(FeaturePtr theFeature) const
{
  const std::list<ConstraintWrapperPtr>& aCList = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aCList.begin();
  for (; aCIt != aCList.end(); ++aCIt)
    if ((*aCIt)->isUsed(theFeature))
      return true;

  std::list<AttributePtr> anAttrList = theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::const_iterator anAttrIt = anAttrList.begin();
  for (; anAttrIt != anAttrList.end(); ++ anAttrIt)
    if (isUsed(*anAttrIt))
      return true;

  return false;
}

bool SketchSolver_Constraint::isUsed(AttributePtr theAttribute) const
{
  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject())
      return isUsed(ModelAPI_Feature::feature(aRefAttr->object()));
    else
      anAttribute = aRefAttr->attr();
  }

  const std::list<ConstraintWrapperPtr>& aCList = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aCList.begin();
  for (; aCIt != aCList.end(); ++aCIt)
    if ((*aCIt)->isUsed(theAttribute))
      return true;
  return false;
}

void SketchSolver_Constraint::makeTemporary() const
{
  myStorage->setTemporary(myBaseConstraint);
}

