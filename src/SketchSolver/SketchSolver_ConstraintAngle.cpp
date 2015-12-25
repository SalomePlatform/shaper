#include <SketchSolver_ConstraintAngle.h>
#include <SketchSolver_Manager.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <cmath>

void SketchSolver_ConstraintAngle::getAttributes(
    double& theValue, std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);

  myAngle = theValue;
}


void SketchSolver_ConstraintAngle::adjustConstraint()
{
  static const double aTol = 1000. * tolerance;
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint).front();
  if (fabs(myAngle - aConstraint->value()) < aTol)
    return;
  myAngle = aConstraint->value();
  aBuilder->adjustConstraint(aConstraint);
  myStorage->addConstraint(myBaseConstraint, aConstraint);
}
