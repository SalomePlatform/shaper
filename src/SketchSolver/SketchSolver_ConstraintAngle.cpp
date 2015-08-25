#include <SketchSolver_ConstraintAngle.h>

#include <cmath>

void SketchSolver_ConstraintAngle::getAttributes(
    double& theValue, std::vector<Slvs_hEntity>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);

  myAngle = theValue;
}


void SketchSolver_ConstraintAngle::adjustConstraint()
{
  Slvs_Constraint aConstraint = myStorage->getConstraint(mySlvsConstraints.front());

  double aLineDir[2][2] = { {0.0, 0.0}, {0.0, 0.0} };
  Slvs_hEntity anEnt[2] = {aConstraint.entityA, aConstraint.entityB};
  for (int i = 0; i < 2; i++) {
    const Slvs_Entity& aLine = myStorage->getEntity(anEnt[i]);
    double aCoef = -1.0;
    for (int j = 0; j < 2; j++, aCoef += 2.0) {
      const Slvs_Entity& aPoint = myStorage->getEntity(aLine.point[j]);
      for (int k = 0; k < 2; k++)
        aLineDir[i][k] += aCoef * myStorage->getParameter(aPoint.param[k]).val;
    }
  }
  double aDot = aLineDir[0][0] * aLineDir[1][0] + aLineDir[0][1] * aLineDir[1][1];

  aConstraint.other = aDot * (90.0 - fabs(aConstraint.valA)) < 0.0;
  if ((90.0 - fabs(aConstraint.valA)) * (90.0 - fabs(myAngle)) < 0.0)
    aConstraint.other = !aConstraint.other;
  myAngle = aConstraint.valA;
  myStorage->updateConstraint(aConstraint);
}
