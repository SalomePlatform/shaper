#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <GeomAPI_Pnt2d.h>


/// \brief Check whether the entities has only one shared point
static bool hasSingleCoincidence(EntityWrapperPtr theEntity1, EntityWrapperPtr theEntity2)
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  const std::list<EntityWrapperPtr>& aPoints1 = theEntity1->subEntities();
  const std::list<EntityWrapperPtr>& aPoints2 = theEntity2->subEntities();

  std::list<EntityWrapperPtr>::const_iterator aStartIt1 = aPoints1.begin();
  if (theEntity1->type() == ENTITY_ARC) ++aStartIt1; // skip center of arc
  std::list<EntityWrapperPtr>::const_iterator aStartIt2 = aPoints2.begin();
  if (theEntity2->type() == ENTITY_ARC) ++aStartIt2; // skip center of arc

  int aNbCoinc = 0;
  std::list<EntityWrapperPtr>::const_iterator anIt1, anIt2;
  for (anIt1 = aStartIt1; anIt1 != aPoints1.end(); ++anIt1) {
    std::shared_ptr<GeomAPI_Pnt2d> aPt1 = aBuilder->point(*anIt1);
    for (anIt2 = aStartIt2; anIt2 != aPoints2.end(); ++anIt2) {
      std::shared_ptr<GeomAPI_Pnt2d> aPt2 = aBuilder->point(*anIt2);
      if (aPt1->distance(aPt2) < tolerance)
        ++aNbCoinc;
    }
  }
  return aNbCoinc == 1;
}


void SketchSolver_ConstraintTangent::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] || !theAttributes[3]) {
    theAttributes.clear();
    return;
  }

  // Check the quantity of entities of each type and their order (arcs first)
  int aNbLines = 0;
  int aNbArcs = 0;
  bool isSwap = false; // whether need to swap arguments (arc goes before line)
  std::vector<EntityWrapperPtr>::iterator anEntIt = theAttributes.begin() + 2;
  for (; anEntIt != theAttributes.end(); ++anEntIt) {
    if ((*anEntIt)->type() == ENTITY_LINE)
      ++aNbLines;
    else if ((*anEntIt)->type() == ENTITY_ARC) {
      ++aNbArcs;
      isSwap = aNbLines > 0;
    }
  }

  if (aNbArcs < 1) {
    myErrorMsg = SketchSolver_Error::INCORRECT_TANGENCY_ATTRIBUTE();
    return;
  }
  if (aNbLines == 1 && aNbArcs == 1)
    myType = CONSTRAINT_TANGENT_ARC_LINE;
  else if (aNbArcs == 2)
    myType = CONSTRAINT_TANGENT_ARC_ARC;
  else {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  if (!hasSingleCoincidence(theAttributes[2], theAttributes[3]))
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();

  if (isSwap) {
    EntityWrapperPtr aTemp = theAttributes[2];
    theAttributes[2] = theAttributes[3];
    theAttributes[3] = aTemp;
  }
}


////void SketchSolver_ConstraintTangent::process()
////{
////  cleanErrorMsg();
////  if (!myBaseConstraint || !myStorage || myGroup == 0) {
////    /// TODO: Put error message here
////    return;
////  }
////  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
////    update(myBaseConstraint);
////
////  double aValue;
////  std::vector<Slvs_hEntity> anEntID;
////  getAttributes(aValue, anEntID);
////  if (!myErrorMsg.empty())
////    return;
////  // Check the quantity of entities of each type and their order (arcs first)
////  int aNbLines = 0;
////  int aNbArcs = 0;
////  Slvs_Entity anEntities[2];
////  myType = SLVS_C_CURVE_CURVE_TANGENT;
////  std::vector<Slvs_hEntity>::iterator anEntIter = anEntID.begin();
////  for (; anEntIter != anEntID.end(); anEntIter++) {
////    Slvs_Entity anEnt = myStorage->getEntity(*anEntIter);
////    if (anEnt.type == SLVS_E_LINE_SEGMENT) {
////      if (aNbLines == 0)
////        anEntities[1 + aNbLines] = anEnt;
////      aNbLines++;
////      myType = SLVS_C_ARC_LINE_TANGENT;
////    }
////    else if (anEnt.type == SLVS_E_ARC_OF_CIRCLE) {
////      if (aNbArcs < 2)
////        anEntities[aNbArcs] = anEnt;
////      aNbArcs++;
////    }
////  }
////
////  if (aNbLines + aNbArcs != 2) {
////    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
////    return;
////  } else if (aNbArcs < 1) {
////    myErrorMsg = SketchSolver_Error::INCORRECT_TANGENCY_ATTRIBUTE();
////    return;
////  }
////
////  // It is necessary to identify which points of entities are coincident
////  int aSlvsOtherFlag = 0;
////  int aSlvsOther2Flag = 0;
////  // Obtain start and end points of entities
////  Slvs_hEntity aPointsToFind[4];
////  for (int i = 0; i < 2; i++) {
////    int aShift = anEntities[i].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0;
////    aPointsToFind[2*i]  = anEntities[i].point[aShift];
////    aPointsToFind[2*i+1]= anEntities[i].point[aShift+1];
////  }
////  // Search coincident points
////  bool isPointFound = false;
////  for (int i = 0; i < 2 && !isPointFound; i++)
////    for (int j = 2; j < 4 && !isPointFound; j++)
////      if (myStorage->isEqual(aPointsToFind[i], aPointsToFind[j])) {
////        aSlvsOtherFlag = i;
////        aSlvsOther2Flag = j - 2;
////        isPointFound = true;
////      }
////  if (!isPointFound) {
////    // There is no coincident points between tangential objects. Generate error message
////    myErrorMsg = SketchSolver_Error::NO_COINCIDENT_POINTS();
////    return;
////  }
////
////  Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(),
////      getType(), myGroup->getWorkplaneId(), aValue,
////      SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, anEntities[0].h, anEntities[1].h);
////  aConstraint.other = aSlvsOtherFlag;
////  aConstraint.other2 = aSlvsOther2Flag;
////  aConstraint.h = myStorage->addConstraint(aConstraint);
////  mySlvsConstraints.push_back(aConstraint.h);
////  adjustConstraint();
////}

