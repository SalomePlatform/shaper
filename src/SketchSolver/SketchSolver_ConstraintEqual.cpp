#include <SketchSolver_ConstraintEqual.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>


void SketchSolver_ConstraintEqual::process()
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

  // Check the quantity of entities of each type
  int aNbLines = 0;
  int aNbArcs = 0;
  int aNbCircs = 0;
  bool isArcFirst = false; // in line-arc equivalence, the line should be first
  std::vector<Slvs_hEntity>::iterator anEntIter = anEntities.begin();
  for (; anEntIter != anEntities.end(); anEntIter++) {
    Slvs_Entity anEnt = myStorage->getEntity(*anEntIter);
    if (anEnt.type == SLVS_E_LINE_SEGMENT)
      aNbLines++;
    else if (anEnt.type == SLVS_E_CIRCLE)
      aNbCircs++;
    else if (anEnt.type == SLVS_E_ARC_OF_CIRCLE) {
      aNbArcs++;
      isArcFirst = (aNbLines == 0);
    }
  }

  if (aNbLines + aNbArcs + aNbCircs != 2 ||
      (aNbLines == aNbCircs && aNbArcs == 0)) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  switch (aNbLines) {
  case 0:
    myType = SLVS_C_EQUAL_RADIUS;
    break;
  case 1:
    myType = SLVS_C_EQUAL_LINE_ARC_LEN;
    if (isArcFirst) { // change the order of arc and line
      Slvs_hEntity aTmp = anEntities[2];
      anEntities[2] = anEntities[3];
      anEntities[3] = aTmp;
    }
    break;
  default:
    myType = SLVS_C_EQUAL_LENGTH_LINES;
    break;
  }

  Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(),
      getType(), myGroup->getWorkplaneId(), aValue,
      anEntities[0], anEntities[1], anEntities[2], anEntities[3]);
  aConstraint.h = myStorage->addConstraint(aConstraint);
  mySlvsConstraints.push_back(aConstraint.h);
  adjustConstraint();
}

