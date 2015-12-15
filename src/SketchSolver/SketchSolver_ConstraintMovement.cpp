#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomDataAPI_Point2D.h>


void SketchSolver_ConstraintMovement::process()
{
  cleanErrorMsg();
  if (!myBaseFeature || !myStorage || myGroupID == GID_UNKNOWN) {
    // Not enough parameters are initialized
    return;
  }

  ParameterWrapperPtr aValue;
  getAttributes(aValue, myMovedEntities);
  if (!myErrorMsg.empty() || myMovedEntities.empty()) {
    // Nothing to move, clear the feature to avoid changing its group
    // after removing the Movement constraint.
    myBaseFeature = FeaturePtr();
    return;
  }

  std::vector<EntityWrapperPtr>::iterator anEntIt = myMovedEntities.begin();
  for (; anEntIt != myMovedEntities.end(); ++anEntIt)
    fixFeature(*anEntIt);
}


static std::list<EntityWrapperPtr> movedEntities(
    EntityWrapperPtr theOld, StoragePtr theOldStorage,
    EntityWrapperPtr theNew, StoragePtr theNewStorage)
{
  bool isFullyMoved = true;
  std::list<EntityWrapperPtr> aMoved;
  if (theOld->isEqual(theNew))
    return aMoved;

  std::list<EntityWrapperPtr> anOldSubs = theOld->subEntities();
  std::list<EntityWrapperPtr> aNewSubs = theNew->subEntities();
  std::list<EntityWrapperPtr>::const_iterator anOldIt = anOldSubs.begin();
  std::list<EntityWrapperPtr>::const_iterator aNewIt = aNewSubs.begin();
  for (; anOldIt != anOldSubs.end() && aNewIt != aNewSubs.end(); ++anOldIt, ++aNewIt) {
    std::list<EntityWrapperPtr> aMovedSubs = movedEntities(
        *anOldIt, theOldStorage, *aNewIt, theNewStorage);
    if (aMovedSubs.size() != 1 || aMovedSubs.front() != *anOldIt)
      isFullyMoved = false;
    aMoved.insert(aMoved.end(), aMovedSubs.begin(), aMovedSubs.end());
  }
  if (isFullyMoved) {
    aMoved.clear();
    aMoved.push_back(theOld);
  }
  return aMoved;
}


void SketchSolver_ConstraintMovement::getAttributes(
    ParameterWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  // There will be build entities, according to the fixed feature, in the separate storage
  // to check whether any point is moved
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  StoragePtr anOtherStorage = aBuilder->createStorage(myGroupID);
  anOtherStorage->setSketch(myStorage->sketch());
  if (!anOtherStorage->update(myBaseFeature, myGroupID))
    return;
  EntityWrapperPtr aNewEntity = anOtherStorage->entity(myBaseFeature);
  EntityWrapperPtr anOldEntity = myStorage->entity(myBaseFeature);

  std::list<EntityWrapperPtr> aMoved;
  if (aNewEntity && anOldEntity)
    aMoved = movedEntities(anOldEntity, myStorage, aNewEntity, anOtherStorage);
  else {
    // get attributes moved
    std::list<AttributePtr> anAttrList =
        myBaseFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<AttributePtr>::const_iterator anIt = anAttrList.begin();
    for (; anIt != anAttrList.end(); ++anIt) {
      aNewEntity = anOtherStorage->entity(*anIt);
      anOldEntity = myStorage->entity(*anIt);
      if (!aNewEntity || !anOldEntity)
        continue;
      std::list<EntityWrapperPtr> aMovedAttr = movedEntities(
          anOldEntity, myStorage, aNewEntity, anOtherStorage);
      aMoved.insert(aMoved.end(), aMovedAttr.begin(), aMovedAttr.end());
    }
  }
  theAttributes.clear();
  theAttributes.insert(theAttributes.begin(), aMoved.begin(), aMoved.end());
}
