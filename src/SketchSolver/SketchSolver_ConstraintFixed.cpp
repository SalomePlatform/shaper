#include <SketchSolver_ConstraintFixed.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>

#include <math.h>

SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(ConstraintPtr theConstraint)
  : SketchSolver_Constraint()
{
  myBaseConstraint = theConstraint;
  myType = CONSTRAINT_FIXED;
  myFixedAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      theConstraint->attribute(SketchPlugin_ConstraintRigid::ENTITY_A()));
}

SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(FeaturePtr theFeature)
  : SketchSolver_Constraint(),
    myBaseFeature(theFeature)
{
  myType = CONSTRAINT_FIXED;
  process();
}

void SketchSolver_ConstraintFixed::process()
{
  cleanErrorMsg();
  if ((!myBaseConstraint && !myBaseFeature) || !myStorage || myGroupID == GID_UNKNOWN) {
    // Not enough parameters are assigned
    return;
  }

  ParameterWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anEntities;
  getAttributes(aValue, anEntities);
  if (!myErrorMsg.empty() || anEntities.empty())
    return;
  fixFeature(anEntities.front());
}

void SketchSolver_ConstraintFixed::fixFeature(EntityWrapperPtr theFeature)
{
  // extract feature from the group
  if (theFeature->baseAttribute())
    myStorage->update(theFeature->baseAttribute(), GID_OUTOFGROUP);
  else if (theFeature->baseFeature())
    myStorage->update(theFeature->baseFeature(), GID_OUTOFGROUP);

  if (myBaseConstraint)
    myStorage->addConstraint(myBaseConstraint, std::list<ConstraintWrapperPtr>());
}

void SketchSolver_ConstraintFixed::getAttributes(
    ParameterWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  EntityWrapperPtr aSolverEntity;
  if (myBaseFeature) {
    // The feature is fixed.
    myStorage->update(myBaseFeature/*, myGroupID*/);
    aSolverEntity = myStorage->entity(myBaseFeature);
  } else if (myBaseConstraint) {
    // Constraint Fixed is added by user.
    // Get the attribute of constraint (it should be alone in the list of constraints).
    AttributePtr anAttr = myBaseConstraint->attribute(SketchPlugin_ConstraintRigid::ENTITY_A());
    AttributeRefAttrPtr aRefAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
    if (!aRefAttr || !aRefAttr->isInitialized()) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }

    myStorage->update(anAttr, myGroupID);
    aSolverEntity = myStorage->entity(anAttr);
  } else
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();

  if (aSolverEntity)
    theAttributes.push_back(aSolverEntity);
}


bool SketchSolver_ConstraintFixed::remove()
{
  cleanErrorMsg();
  // Move fixed entities back to the current group
  FeaturePtr aFeature = myBaseFeature;
  if (myBaseConstraint && myFixedAttribute) {
    if (myFixedAttribute->isObject())
      aFeature = ModelAPI_Feature::feature(myFixedAttribute->object());
    else {
      if (myFixedAttribute->attr().get())
        myStorage->update(AttributePtr(myFixedAttribute), myGroupID);
    }
  }
  if (aFeature)
    myStorage->update(aFeature, myGroupID);

  // Remove constraint or base feature
  if (myBaseConstraint) {
    bool isRemoved = false;
    if (aFeature)
      isRemoved = myStorage->removeEntity(aFeature);
    return SketchSolver_Constraint::remove() || isRemoved;
  } else if (myBaseFeature)
    myStorage->removeEntity(myBaseFeature);
  return true;
}
