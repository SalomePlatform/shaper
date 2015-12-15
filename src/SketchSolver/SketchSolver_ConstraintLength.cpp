#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>


void SketchSolver_ConstraintLength::process()
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

  // Check the entity is a line
  Slvs_Entity aLine = myStorage->getEntity(anEntities[2]);
  if (aLine.type != SLVS_E_LINE_SEGMENT){
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(),
      getType(), myGroup->getWorkplaneId(), aValue,
      aLine.point[0], aLine.point[1], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
  aConstraint.h = myStorage->addConstraint(aConstraint);
  mySlvsConstraints.push_back(aConstraint.h);
  adjustConstraint();
}

void SketchSolver_ConstraintLength::adjustConstraint()
{
  // No need to store the line, which length is constrained
  // Upd: The line need to be stored to check that constraint
  //      is changed/unchanged during modifications in GUI
  //myFeatureMap.clear();
}

