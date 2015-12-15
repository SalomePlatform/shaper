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
  // Adjust point-line distance
  if (getType() != CONSTRAINT_PT_LINE_DISTANCE)
    return;

  // Check the sign of distance is changed
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint).front();
  if (fabs(myPrevValue) == fabs(aConstraint->value())) {
    aConstraint->setValue(myPrevValue);
    myStorage->addConstraint(myBaseConstraint, aConstraint);
    return;
  }

  // Get constraint parameters and check the sign of constraint value
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::shared_ptr<GeomAPI_Pnt2d> aPoint;
  std::shared_ptr<GeomAPI_Lin2d> aLine;
  std::list<EntityWrapperPtr> aSubs = aConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++aSIt) {
    if ((*aSIt)->type() == ENTITY_POINT)
      aPoint = aBuilder->point(*aSIt);
    else if ((*aSIt)->type() == ENTITY_LINE)
      aLine = aBuilder->line(*aSIt);
  }

  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());
  if (aPtLineVec->cross(aLineVec) * aConstraint->value() < 0.0 || myIsNegative) {
    aConstraint->setValue(aConstraint->value() * (-1.0));
    myStorage->addConstraint(myBaseConstraint, aConstraint);
    myIsNegative = true;
  }
}

void SketchSolver_ConstraintDistance::update()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint).front();
  myPrevValue = aConstraint->value();

  SketchSolver_Constraint::update();
}
