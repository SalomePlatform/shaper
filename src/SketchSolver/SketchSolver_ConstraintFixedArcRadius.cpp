#include <SketchSolver_ConstraintFixedArcRadius.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Arc.h>
#include <GeomAPI_Pnt2d.h>

SketchSolver_ConstraintFixedArcRadius::SketchSolver_ConstraintFixedArcRadius(FeaturePtr theFeature)
  : SketchSolver_ConstraintFixed(theFeature)
{
  myType = CONSTRAINT_RADIUS;
  process();
}

void SketchSolver_ConstraintFixedArcRadius::process()
{
  cleanErrorMsg();
  if (!myBaseFeature || !myStorage || myGroupID == GID_UNKNOWN) {
    // Not enough parameters are assigned
    return;
  }

  if (myBaseFeature->getKind() != SketchPlugin_Arc::ID()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  ParameterWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anEntities;
  getAttributes(aValue, anEntities);
  if (!myErrorMsg.empty() || anEntities.empty())
    return;
  fixFeature(anEntities.front());
}

void SketchSolver_ConstraintFixedArcRadius::fixFeature(EntityWrapperPtr theFeature)
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  // Calculate radius of the arc
  std::list<EntityWrapperPtr> aSubs = theFeature->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aSubIt = aSubs.begin();
  std::shared_ptr<GeomAPI_Pnt2d> aCenter = aBuilder->point(*aSubIt++);
  std::shared_ptr<GeomAPI_Pnt2d> aStart  = aBuilder->point(*aSubIt++);
  double aRadius = aCenter->distance(aStart);

  // Create constraint
  std::list<ConstraintWrapperPtr> aConstrList = aBuilder->createConstraint(ConstraintPtr(),
      myGroupID, mySketchID, myType, aRadius, EntityWrapperPtr(), EntityWrapperPtr(), theFeature);
  myRadiusConstraint = aConstrList.front();
  myStorage->addConstraint(ConstraintPtr(), myRadiusConstraint);
}

bool SketchSolver_ConstraintFixedArcRadius::remove()
{
  bool isFullyRemoved = true;
  if (myBaseFeature)
    isFullyRemoved = myStorage->removeEntity(myBaseFeature) && isFullyRemoved;
  if (myRadiusConstraint)
    isFullyRemoved = myStorage->removeConstraint(ConstraintPtr()) &&
                     myStorage->remove(myRadiusConstraint) && isFullyRemoved;
  return isFullyRemoved;
}
