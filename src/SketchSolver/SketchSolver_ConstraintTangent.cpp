#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>


void SketchSolver_ConstraintTangent::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  double aValue;
  std::vector<Slvs_hEntity> anEntID;
  getAttributes(aValue, anEntID);
  if (!myErrorMsg.empty())
    return;
  // Check the quantity of entities of each type and their order (arcs first)
  int aNbLines = 0;
  int aNbArcs = 0;
  Slvs_Entity anEntities[2];
  myType = SLVS_C_CURVE_CURVE_TANGENT;
  std::vector<Slvs_hEntity>::iterator anEntIter = anEntID.begin();
  for (; anEntIter != anEntID.end(); anEntIter++) {
    Slvs_Entity anEnt = myStorage->getEntity(*anEntIter);
    if (anEnt.type == SLVS_E_LINE_SEGMENT) {
      if (aNbLines == 0)
        anEntities[1 + aNbLines] = anEnt;
      aNbLines++;
      myType = SLVS_C_ARC_LINE_TANGENT;
    }
    else if (anEnt.type == SLVS_E_ARC_OF_CIRCLE) {
      if (aNbArcs < 2)
        anEntities[aNbArcs] = anEnt;
      aNbArcs++;
    }
  }

  if (aNbLines + aNbArcs != 2) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  } else if (aNbArcs < 1) {
    myErrorMsg = SketchSolver_Error::INCORRECT_TANGENCY_ATTRIBUTE();
    return;
  }

  // It is necessary to identify which points of entities are coincident
  int aSlvsOtherFlag = 0;
  int aSlvsOther2Flag = 0;
  // Obtain start and end points of entities
  Slvs_hEntity aPointsToFind[4];
  for (int i = 0; i < 2; i++) {
    int aShift = anEntities[i].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0;
    aPointsToFind[2*i]  = anEntities[i].point[aShift];
    aPointsToFind[2*i+1]= anEntities[i].point[aShift+1];
  }
  // Search coincident points
  bool isPointFound = false;
  for (int i = 0; i < 2 && !isPointFound; i++)
    for (int j = 2; j < 4 && !isPointFound; j++)
      if (myStorage->isCoincident(aPointsToFind[i], aPointsToFind[j])) {
        aSlvsOtherFlag = i;
        aSlvsOther2Flag = j - 2;
        isPointFound = true;
      }
  if (!isPointFound) {
    // There is no coincident points between tangential objects. Generate error message
    myErrorMsg = SketchSolver_Error::NO_COINCIDENT_POINTS();
    return;
  }

  Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(),
      getType(), myGroup->getWorkplaneId(), aValue,
      SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, anEntities[0].h, anEntities[1].h);
  aConstraint.other = aSlvsOtherFlag;
  aConstraint.other2 = aSlvsOther2Flag;
  aConstraint.h = myStorage->addConstraint(aConstraint);
  mySlvsConstraints.push_back(aConstraint.h);
  adjustConstraint();
}

