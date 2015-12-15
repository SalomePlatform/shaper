#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_Error.h>


void SketchSolver_ConstraintLength::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] || 
      theAttributes[2]->type() != ENTITY_LINE) {
    theAttributes.clear();
    return;
  }

  // Get boundary points of line segment and create point-point distance constraint
  std::list<EntityWrapperPtr> aSubs = theAttributes[2]->subEntities();
  theAttributes.assign(theAttributes.size(), EntityWrapperPtr());
  std::vector<EntityWrapperPtr>::iterator anAttrIt = theAttributes.begin();
  std::list<EntityWrapperPtr>::const_iterator aSubIt = aSubs.begin();
  for (; aSubIt != aSubs.end(); ++aSubIt, ++anAttrIt)
    *anAttrIt = *aSubIt;

  myType = CONSTRAINT_PT_PT_DISTANCE;
}

