#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_MultiTranslation.h>


void SketchSolver_ConstraintMultiTranslation::getAttributes(
    EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
    std::list< std::list<EntityWrapperPtr> >& theEntities)
{
  DataPtr aData = myBaseConstraint->data();
  AttributePtr aStartPointAttr = aData->attribute(SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributePtr aEndPointAttr = aData->attribute(SketchPlugin_MultiTranslation::END_POINT_ID());
  if (!aStartPointAttr || !aStartPointAttr->isInitialized() ||
      !aEndPointAttr || !aEndPointAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = CONSTRAINT_MULTI_TRANSLATION;

  myStorage->update(aStartPointAttr);
  theStartPoint = myStorage->entity(aStartPointAttr);
  myStorage->update(aEndPointAttr);
  theEndPoint = myStorage->entity(aEndPointAttr);

  getEntitiesAndCopies(theEntities);

}

void SketchSolver_ConstraintMultiTranslation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    /// TODO: Put error message here
    return;
  }

  EntityWrapperPtr aStartPoint, aEndPoint;
  std::list<std::list<EntityWrapperPtr> > anEntitiesAndCopies;
  getAttributes(aStartPoint, aEndPoint, anEntitiesAndCopies);
  if (!myErrorMsg.empty())
    return;

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aTransConstraints;

  std::list< std::list<EntityWrapperPtr> >::iterator anEntIt = anEntitiesAndCopies.begin();
  for (; anEntIt != anEntitiesAndCopies.end(); ++anEntIt) {
    std::list<ConstraintWrapperPtr> aNewConstraints =
        aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID, myType,
        0.0, aStartPoint, aEndPoint, *anEntIt);
    aTransConstraints.insert(aTransConstraints.end(), aNewConstraints.begin(), aNewConstraints.end());
  }
  myStorage->addConstraint(myBaseConstraint, aTransConstraints);

  myAdjusted = false;
  adjustConstraint();
}

const std::string& SketchSolver_ConstraintMultiTranslation::nameNbObjects()
{
  return SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID();
}
