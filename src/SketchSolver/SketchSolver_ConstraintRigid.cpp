#include <SketchSolver_ConstraintRigid.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Group.h>

#include <SketchPlugin_ConstraintRigid.h>

SketchSolver_ConstraintRigid::SketchSolver_ConstraintRigid(FeaturePtr theFeature)
  : SketchSolver_Constraint(),
    myBaseFeature(theFeature)
{
  process();
}

void SketchSolver_ConstraintRigid::process()
{
  cleanErrorMsg();
  if ((!myBaseConstraint && !myBaseFeature) || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  double aValue;
  std::vector<Slvs_hEntity> anEntities;
  getAttributes(aValue, anEntities);

  Slvs_Constraint aConstraint;
  std::vector<Slvs_hConstraint>::iterator aConstrIter = mySlvsConstraints.begin();
  bool isEmpty = aConstrIter == mySlvsConstraints.end();
  std::vector<Slvs_hEntity>::const_iterator anEntIter = anEntities.begin();
  for (; anEntIter != anEntities.end(); anEntIter++) {
    if (isEmpty) { // create new constraint
      aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(),
          aValue, *anEntIter, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
      if (!myBaseConstraint)
        myStorage->addTemporaryConstraint(aConstraint.h);
    } else { // update already existent constraint
      aConstraint = myStorage->getConstraint(*aConstrIter);
      aConstraint.ptA = *anEntIter;
      myStorage->addConstraint(aConstraint);
      aConstrIter++;
      isEmpty = aConstrIter == mySlvsConstraints.end();
    }
  }
}


void SketchSolver_ConstraintRigid::getAttributes(
    double& theValue,
    std::vector<Slvs_hEntity>& theAttributes)
{
  theValue = 0.0;
  int aType = SLVS_E_UNKNOWN; // type of created entity
  Slvs_hEntity anEntityID = SLVS_E_UNKNOWN;
  if (myBaseConstraint) {
    // Get the attribute of constraint
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        myBaseConstraint->attribute(SketchPlugin_ConstraintRigid::ENTITY_A()));
    if (!aRefAttr) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }
    anEntityID = myGroup->getAttributeId(aRefAttr);
    if (anEntityID == SLVS_E_UNKNOWN)
      anEntityID = changeEntity(aRefAttr, aType);
  } else {
    anEntityID = myGroup->getFeatureId(myBaseFeature);
    if (anEntityID == SLVS_E_UNKNOWN)
      anEntityID = changeEntity(myBaseFeature, aType);
  }

  // Check the entity is complex
  Slvs_Entity anEntity = myStorage->getEntity(anEntityID);
  if (anEntity.point[0] != SLVS_E_UNKNOWN) {
    for (int i = 0; i < 4 && anEntity.point[i]; i++)
      theAttributes.push_back(anEntity.point[i]);
  } else // simple entity
    theAttributes.push_back(anEntityID);
}

bool SketchSolver_ConstraintRigid::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  bool isFullyRemoved = true;
  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++)
    isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();
  return true;
}

