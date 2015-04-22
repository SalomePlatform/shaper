#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <GeomAPI_XY.h>


void SketchSolver_ConstraintDistance::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  double aValue;
  std::vector<Slvs_hEntity> anEntities;
  getAttributes(aValue, anEntities);
  if (!myErrorMsg.empty())
    return;

  // Obtain entities to identify the type of distance
  static const int aNbPoints = 2;
  Slvs_hEntity aPoint[aNbPoints] = {SLVS_E_UNKNOWN, SLVS_E_UNKNOWN};
  Slvs_hEntity aLine = SLVS_E_UNKNOWN;
  myType = SLVS_C_PT_PT_DISTANCE;
  int aPtPos = 0;
  std::vector<Slvs_hEntity>::iterator anEntIter = anEntities.begin();
  for (; anEntIter != anEntities.end(); anEntIter++) {
    if (*anEntIter == SLVS_E_UNKNOWN)
      continue;
    Slvs_Entity anEnt = myStorage->getEntity(*anEntIter);
    if (anEnt.type == SLVS_E_POINT_IN_2D || anEnt.type == SLVS_E_POINT_IN_3D) {
      if (aPtPos >= aNbPoints) {
        myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
        return;
      }
      aPoint[aPtPos++] = *anEntIter;
    }
    else if (anEnt.type == SLVS_E_LINE_SEGMENT) {
      if (myType == SLVS_C_PT_LINE_DISTANCE) {
        myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
        return;
      }
      aLine = *anEntIter;
      myType = SLVS_C_PT_LINE_DISTANCE;
    }
  }

  Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(),
      getType(), myGroup->getWorkplaneId(), aValue, aPoint[0], aPoint[1], aLine, SLVS_E_UNKNOWN);
  aConstraint.h = myStorage->addConstraint(aConstraint);
  mySlvsConstraints.push_back(aConstraint.h);

  myPrevValue = 0.0;
  adjustConstraint();
}

void SketchSolver_ConstraintDistance::adjustConstraint()
{
  // Adjust point-line distance
  if (getType() != SLVS_C_PT_LINE_DISTANCE)
    return;

  // Check the sign of distance is changed
  Slvs_Constraint aConstraint = myStorage->getConstraint(mySlvsConstraints.front());
  if (fabs(myPrevValue) == fabs(aConstraint.valA)) {
    aConstraint.valA = myPrevValue;
    myStorage->updateConstraint(aConstraint);
    return;
  }

  // Get constraint parameters and check the sign of constraint value
  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++) {
    aConstraint = myStorage->getConstraint(*aCIter);
    Slvs_Entity aLine = myStorage->getEntity(aConstraint.entityA);
    // Obtain point and line coordinates
    Slvs_hEntity aPointID[3] = {aConstraint.ptA, aLine.point[0], aLine.point[1]};
    std::shared_ptr<GeomAPI_XY> aPoints[3];
    for (int i = 0; i < 3; i++) {
      Slvs_Entity aPoint = myStorage->getEntity(aPointID[i]);
      Slvs_Param aParams[2] = {
          myStorage->getParameter(aPoint.param[0]),
          myStorage->getParameter(aPoint.param[1])};
      aPoints[i] = std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(aParams[0].val, aParams[1].val));
    }
    std::shared_ptr<GeomAPI_XY> aLineVec = aPoints[2]->decreased(aPoints[1]);
    std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoints[0]->decreased(aPoints[1]);
    if (aPtLineVec->cross(aLineVec) * aConstraint.valA < 0.0) {
      aConstraint.valA *= -1.0;
      myStorage->updateConstraint(aConstraint);
    }
  }
}

void SketchSolver_ConstraintDistance::update(ConstraintPtr theConstraint)
{
  Slvs_Constraint aConstraint = myStorage->getConstraint(mySlvsConstraints.front());
  myPrevValue = aConstraint.valA;
  SketchSolver_Constraint::update(theConstraint);
}
