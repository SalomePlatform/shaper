#include <SketchSolver_ConstraintAngle.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

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

  bool isFixed[2][2];
  std::shared_ptr<GeomAPI_Pnt2d> aPoints[2][2]; // start and end points of lines
  Slvs_hConstraint aFixedConstraint;
  double aLineDir[2][2] = { {0.0, 0.0}, {0.0, 0.0} };
  Slvs_hEntity anEnt[2] = {aConstraint.entityA, aConstraint.entityB};
  for (int i = 0; i < 2; i++) {
    const Slvs_Entity& aLine = myStorage->getEntity(anEnt[i]);
    double aCoef = -1.0;
    for (int j = 0; j < 2; j++, aCoef += 2.0) {
      const Slvs_Entity& aPoint = myStorage->getEntity(aLine.point[j]);
      double aCoords[2];
      for (int k = 0; k < 2; k++) {
        aCoords[k] = myStorage->getParameter(aPoint.param[k]).val;
        aLineDir[i][k] += aCoef * aCoords[k];
      }
      isFixed[i][j] = myStorage->isPointFixed(aPoint.h, aFixedConstraint, true);
      aPoints[i][j] = std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aCoords[0], aCoords[1]));
    }
  }
  double aDot = aLineDir[0][0] * aLineDir[1][0] + aLineDir[0][1] * aLineDir[1][1];

  aConstraint.other = aDot * (90.0 - fabs(aConstraint.valA)) < 0.0;
  if ((90.0 - fabs(aConstraint.valA)) * (90.0 - fabs(myAngle)) < 0.0)
    aConstraint.other = !aConstraint.other;
  myStorage->updateConstraint(aConstraint);

  if (fabs(myAngle - aConstraint.valA) < tolerance)
    return; // the angle was not changed, no need to recalculate positions of lines
  myAngle = aConstraint.valA;

  if (isFixed[0][0] && isFixed[0][1] && isFixed[1][0] && isFixed[1][1])
    return; // both lines are fixed => no need to update them

  // Recalculate positions of lines to avoid conflicting constraints
  // while changing angle value several times
  std::shared_ptr<GeomAPI_Lin2d> aLine[2] = {
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoints[0][0], aPoints[0][1])),
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoints[1][0], aPoints[1][1]))
  };
  std::shared_ptr<GeomAPI_Pnt2d> anIntersection = aLine[0]->intersect(aLine[1]);
  if (!anIntersection)
    return;
  double aDist[2][2];
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      aDist[i][j] = anIntersection->distance(aPoints[i][j]);
  std::shared_ptr<GeomAPI_Dir2d> aDir[2];
  for (int i = 0; i < 2; i++)
    if (aDist[i][1] > aDist[i][0])
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(
          aPoints[i][1]->xy()->decreased(anIntersection->xy())));
    else
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(
          aPoints[i][0]->xy()->decreased(anIntersection->xy())));

  double cosA = cos(myAngle * PI / 180.0);
  double sinA = sin(myAngle * PI / 180.0);
  if (aDir[0]->cross(aDir[1]) < 0.0)
    sinA *= -1.0;
  int aLineToUpd = 1;
  if (isFixed[1][0] && isFixed[1][1]) {
    sinA *= -1.0;
    aLineToUpd = 0;
  }
  double x = aDir[1-aLineToUpd]->x() * cosA - aDir[1-aLineToUpd]->y() * sinA;
  double y = aDir[1-aLineToUpd]->x() * sinA + aDir[1-aLineToUpd]->y() * cosA;

  std::shared_ptr<GeomAPI_Pnt2d> aNewPoints[2];
  for (int i = 0; i < 2; i++) {
    aNewPoints[i] = std::shared_ptr<GeomAPI_Pnt2d>(
        new GeomAPI_Pnt2d(anIntersection->x() + x * aDist[aLineToUpd][i],
                          anIntersection->y() + y * aDist[aLineToUpd][i]));
  }

  std::shared_ptr<GeomAPI_XY> aDelta;
  if (isFixed[aLineToUpd][0] && !isFixed[aLineToUpd][1])
    aDelta = aPoints[aLineToUpd][0]->xy()->decreased(aNewPoints[0]->xy());
  else if (!isFixed[aLineToUpd][0] && isFixed[aLineToUpd][1])
    aDelta = aPoints[aLineToUpd][1]->xy()->decreased(aNewPoints[1]->xy());
  if (aDelta) {
    for (int i = 0; i < 2; i++) {
      aNewPoints[i]->setX(aNewPoints[i]->x() + aDelta->x());
      aNewPoints[i]->setY(aNewPoints[i]->y() + aDelta->y());
    }
  }

  // Update positions of points
  const Slvs_Entity& anUpdLine = myStorage->getEntity(anEnt[aLineToUpd]);
  Slvs_Param aParam;
  for (int i = 0; i < 2; i++) {
    const Slvs_Entity& aPoint = myStorage->getEntity(anUpdLine.point[i]);
    aParam = myStorage->getParameter(aPoint.param[0]);
    aParam.val = aNewPoints[i]->x();
    myStorage->updateParameter(aParam);
    aParam = myStorage->getParameter(aPoint.param[1]);
    aParam.val = aNewPoints[i]->y();
    myStorage->updateParameter(aParam);
  }
}
