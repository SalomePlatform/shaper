#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <ModelAPI_AttributeRefList.h>

void SketchSolver_ConstraintMirror::getAttributes(
    EntityWrapperPtr& theMirrorLine,
    std::vector<EntityWrapperPtr>& theBaseEntities,
    std::vector<EntityWrapperPtr>& theMirrorEntities)
{
  AttributePtr aMirLineAttr = myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A());
  AttributeRefAttrPtr aMirLineRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aMirLineAttr);
  if (!aMirLineRefAttr || !aMirLineRefAttr->isInitialized() || !aMirLineRefAttr->isObject()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = TYPE(myBaseConstraint);
  myStorage->update(aMirLineAttr/*, myGroupID*/);
  theMirrorLine = myStorage->entity(aMirLineAttr);

  // Create SolveSpace entity for all features
  AttributeRefListPtr aBaseRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributeRefListPtr aMirroredRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_C()));
  myNumberOfObjects = aMirroredRefList->size();
  if (!aBaseRefList || !aMirroredRefList) {
    myErrorMsg = SketchSolver_Error::INCORRECT_MIRROR_ATTRIBUTE();
    return;
  }

  std::list<ObjectPtr> aBaseList = aBaseRefList->list();
  std::list<ObjectPtr> aMirroredList = aMirroredRefList->list();

  FeaturePtr aFeature;
  for (int i = 0; i < 2; i++) {
    std::list<ObjectPtr>::iterator anIter = i == 0 ? aBaseList.begin() : aMirroredList.begin();
    std::list<ObjectPtr>::iterator aEndIter = i == 0 ? aBaseList.end() : aMirroredList.end();
    std::vector<EntityWrapperPtr>* aList = i == 0 ? &theBaseEntities : & theMirrorEntities;
    for ( ; anIter != aEndIter; anIter++) {
      aFeature = ModelAPI_Feature::feature(*anIter);
      if (!aFeature)
        continue;

      myStorage->update(aFeature/*, myGroupID*/);
      aList->push_back(myStorage->entity(aFeature));
    }
  }

  if (theBaseEntities.size() > theMirrorEntities.size())
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
}

void SketchSolver_ConstraintMirror::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aMirrorLine;
  std::vector<EntityWrapperPtr> aBaseList;
  std::vector<EntityWrapperPtr> aMirrorList;
  getAttributes(aMirrorLine, aBaseList, aMirrorList);
  if (!myErrorMsg.empty())
    return;

  if (aBaseList.size() != aMirrorList.size()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_MIRROR_ATTRIBUTE();
    return;
  }

  std::list<ConstraintWrapperPtr> aNewConstraints;
  SketchSolver_ConstraintType aConstrType = getType();
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aMirConstrList;

  std::vector<EntityWrapperPtr>::iterator aBIt = aBaseList.begin();
  std::vector<EntityWrapperPtr>::iterator aMIt = aMirrorList.begin();
  for (; aBIt != aBaseList.end(); ++aBIt, ++aMIt) {
    if ((*aBIt)->type() == ENTITY_ARC) {
      // add new points on arcs and mirror them
      EntityWrapperPtr aBasePnt = myStorage->calculateMiddlePoint(*aBIt, 0.5);
      EntityWrapperPtr aMirrPnt = myStorage->calculateMiddlePoint(*aMIt, 0.5);
      // point on base arc
      aNewConstraints = aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID,
          CONSTRAINT_PT_ON_CIRCLE, 0.0, aBasePnt, EntityWrapperPtr(), *aBIt);
      aMirConstrList.insert(aMirConstrList.end(), aNewConstraints.begin(), aNewConstraints.end());
      // point on mirrored arc
      aNewConstraints = aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID,
          CONSTRAINT_PT_ON_CIRCLE, 0.0, aMirrPnt, EntityWrapperPtr(), *aMIt);
      aMirConstrList.insert(aMirConstrList.end(), aNewConstraints.begin(), aNewConstraints.end());
      // mirror these points
      aNewConstraints = aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID,
          aConstrType, 0.0, aBasePnt, aMirrPnt, aMirrorLine);
      aMirConstrList.insert(aMirConstrList.end(), aNewConstraints.begin(), aNewConstraints.end());
    }
    aNewConstraints = aBuilder->createConstraint(
        myBaseConstraint, myGroupID, mySketchID, aConstrType,
        0.0, *aBIt, *aMIt, aMirrorLine);
    aMirConstrList.insert(aMirConstrList.end(), aNewConstraints.begin(), aNewConstraints.end());
  }

  myStorage->addConstraint(myBaseConstraint, aMirConstrList);
}


void SketchSolver_ConstraintMirror::update()
{
  cleanErrorMsg();
  AttributeRefListPtr aMirroredRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
    myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_C()));
  if (aMirroredRefList->size() != myNumberOfObjects) {
    remove();
    process();
    return;
  }
  adjustConstraint();
}

void SketchSolver_ConstraintMirror::adjustConstraint()
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  const std::list<ConstraintWrapperPtr>& aConstraints = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aConstraints.begin();
  for (; aCIt != aConstraints.end(); ++aCIt)
    if ((*aCIt)->type() == CONSTRAINT_SYMMETRIC)
      aBuilder->adjustConstraint(*aCIt);
}
