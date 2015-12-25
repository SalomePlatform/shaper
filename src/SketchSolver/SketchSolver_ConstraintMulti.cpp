#include <SketchSolver_ConstraintMulti.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>

void SketchSolver_ConstraintMulti::getEntitiesAndCopies(
    std::list< std::list<EntityWrapperPtr> >& theEntAndCopies)
{
  myAdjusted = false;
  DataPtr aData = myBaseConstraint->data();

  // Lists of objects and number of copies
  AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  myNumberOfObjects = anInitialRefList->size();
  myNumberOfCopies = aData->integer(nameNbObjects())->value() - 1;
  if (myNumberOfCopies <= 0)
    return;

  AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aRefList || aRefList->size() == 0) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr aFeature;
  std::list<EntityWrapperPtr> anEntities; // list of transformed entities
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
  if (myNumberOfCopies + 1 != aRefList->size()) // execute for the feature is not called yet
    myNumberOfCopies = aRefList->size() - 1;

  while (anObjIt != anObjectList.end()) {
    anEntities.clear();
    for (int i = 0; i <= myNumberOfCopies && anObjIt != anObjectList.end(); ++i, ++anObjIt) {
      aFeature = ModelAPI_Feature::feature(*anObjIt);
      if (!aFeature)
        continue;

      myStorage->update(aFeature);
      anEntities.push_back(myStorage->entity(aFeature));
    }
    if (!anEntities.empty())
      theEntAndCopies.push_back(anEntities);
  }
}

void SketchSolver_ConstraintMulti::update()
{
  update(false);
}


void SketchSolver_ConstraintMulti::update(bool isForce)
{
  cleanErrorMsg();
  AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeIntegerPtr aNbObjects = myBaseConstraint->integer(nameNbObjects());
  if (anInitialRefList->size() != myNumberOfObjects || aNbObjects->value()-1 != myNumberOfCopies) {
    remove();
    process();
    return;
  }

  // update derivative object
  updateLocal();
  if (isForce)
    myAdjusted = false;
  // update parent object
  SketchSolver_Constraint::update();
}

void SketchSolver_ConstraintMulti::adjustConstraint()
{
  if (myAdjusted)
    return; // constraint already adjusted, don't do it once again

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  const std::list<ConstraintWrapperPtr>& aConstraints = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator anIt = aConstraints.begin();
  for (; anIt != aConstraints.end(); ++anIt)
    aBuilder->adjustConstraint(*anIt);
  myStorage->addConstraint(myBaseConstraint, aConstraints);

  myAdjusted = true;
}
