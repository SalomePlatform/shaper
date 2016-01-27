#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <math.h>


void SketchSolver_ConstraintDistance::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1])
    myType = CONSTRAINT_PT_PT_DISTANCE;
  else if (theAttributes[2] && theAttributes[2]->type() == ENTITY_LINE)
    myType = CONSTRAINT_PT_LINE_DISTANCE;
  else
    theAttributes.clear();

  myPrevValue = 0.0;
}

void SketchSolver_ConstraintDistance::adjustConstraint()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint).front();

  // Adjust point-point distance if the points are equal
  if (getType() == CONSTRAINT_PT_PT_DISTANCE) {
    const std::list<EntityWrapperPtr>& aSubs = aConstraint->entities();
    if (aSubs.front()->isEqual(aSubs.back())) {
      // Change X coordinate of second point to eliminate coincidence
      ParameterWrapperPtr aX = aSubs.back()->parameters().front();
      aX->setValue(aX->value() + 1.0);
      myStorage->update(aX);
    }
    return;
  }

  // Adjust point-line distance
  if (fabs(myPrevValue) == fabs(aConstraint->value())) {
    // sign of distance is not changed
    aConstraint->setValue(myPrevValue);
    myStorage->addConstraint(myBaseConstraint, aConstraint);
    return;
  }

  // Adjust the sign of constraint value
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  aBuilder->adjustConstraint(aConstraint);
  myStorage->addConstraint(myBaseConstraint, aConstraint);
}

void SketchSolver_ConstraintDistance::update()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint).front();
  myPrevValue = aConstraint->value();

  SketchSolver_Constraint::update();
}
