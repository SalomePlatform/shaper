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
    if ((*anIt1)->type() != ENTITY_POINT)
      continue;
    std::shared_ptr<GeomAPI_Pnt2d> aPt1 = aBuilder->point(*anIt1);
    for (anIt2 = aStartIt2; anIt2 != aPoints2.end(); ++anIt2) {
      if ((*anIt2)->type() != ENTITY_POINT)
        continue;
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
