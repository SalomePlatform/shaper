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

  bool isFixed[2][2];
  std::shared_ptr<GeomAPI_Pnt2d> aPoints[2][2]; // start and end points of lines
  const std::list<EntityWrapperPtr>& aConstrLines = aConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aCLIt = aConstrLines.begin();
  for (int i = 0; aCLIt != aConstrLines.end(); ++i, ++aCLIt) {
    const std::list<EntityWrapperPtr>& aLinePoints = (*aCLIt)->subEntities();
    std::list<EntityWrapperPtr>::const_iterator aLPIt = aLinePoints.begin();
    for (int j = 0; aLPIt != aLinePoints.end(); ++j, ++aLPIt) {
      isFixed[i][j] = ((*aLPIt)->group() != myGroupID);
      aPoints[i][j] = aBuilder->point(*aLPIt);
    }
  }

  if (isFixed[0][0] && isFixed[0][1] && isFixed[1][0] && isFixed[1][1])
    return; // both lines are fixed => no need to update them

  std::shared_ptr<GeomAPI_Lin2d> aLine[2] = {
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoints[0][0], aPoints[0][1])),
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoints[1][0], aPoints[1][1]))
  };
  std::shared_ptr<GeomAPI_Pnt2d> anIntersection = aLine[0]->intersect(aLine[1]);
  if (!anIntersection)
    return;
  double aDist[2][2];
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      aDist[i][j] = anIntersection->distance(aPoints[i][j]);
      if (fabs(aDist[i][j]) <= tolerance)
        aDist[i][j] = 0.0;
    }
    if (aDist[i][0] > tolerance && aDist[i][1] > tolerance &&
        aDist[i][0] + aDist[i][1] < aPoints[i][0]->distance(aPoints[i][1]) + 2.0 * tolerance) {
      // the intersection point is an inner point of the line,
      // we change the sign of distance till start point to calculate correct coordinates
      // after rotation
      aDist[i][0] *= -1.0;
    }
  }
  std::shared_ptr<GeomAPI_Dir2d> aDir[2];
  for (int i = 0; i < 2; i++)
    if (aDist[i][1] > fabs(aDist[i][0]))
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(
          aPoints[i][1]->xy()->decreased(anIntersection->xy())));
    else {
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(
          aPoints[i][0]->xy()->decreased(anIntersection->xy())));
      // main direction is opposite => change signs
      if (aDist[i][0] < 0.0) {
        aDist[i][0] *= -1.0;
        aDist[i][1] *= -1.0;
      }
    }

  // Recalculate positions of lines to avoid conflicting constraints
  // while changing angle value several times
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
  std::list<EntityWrapperPtr>::const_iterator anUpdLine = aConstrLines.begin();
  if (aLineToUpd > 0) ++anUpdLine;
  const std::list<EntityWrapperPtr>& anUpdPoints = (*anUpdLine)->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aPIt = anUpdPoints.begin();
  for (int i = 0; aPIt != anUpdPoints.end(); ++aPIt, ++i) {
    double aCoord[2] = {aNewPoints[i]->x(), aNewPoints[i]->y()};
    const std::list<ParameterWrapperPtr>& aParams = (*aPIt)->parameters();
    std::list<ParameterWrapperPtr>::const_iterator aParIt = aParams.begin();
    for (int j = 0; aParIt != aParams.end(); ++j, ++aParIt)
      (*aParIt)->setValue(aCoord[j]);
  }

  aBuilder->adjustConstraint(aConstraint);
  myStorage->addConstraint(myBaseConstraint, aConstraint);
}
