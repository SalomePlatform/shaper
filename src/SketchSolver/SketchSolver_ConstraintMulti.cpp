#include <SketchSolver_ConstraintMulti.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Line.h>

void SketchSolver_ConstraintMulti::getEntities(std::list<EntityWrapperPtr>& theEntities)
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
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
  if ((myNumberOfCopies + 1) * myNumberOfObjects != aRefList->size()) // execute for the feature is not called yet
    myNumberOfCopies = aRefList->size() / myNumberOfObjects - 1;

  while (anObjIt != anObjectList.end()) {
    aFeature = ModelAPI_Feature::feature(*anObjIt++);
    if (!aFeature)
      continue;

    myStorage->update(aFeature);
    theEntities.push_back(myStorage->entity(aFeature));
    myFeatures.insert(aFeature);
    for (int i = 0; i < myNumberOfCopies && anObjIt != anObjectList.end(); ++i, ++anObjIt) {
      // just add copied features into the list of objects
      aFeature = ModelAPI_Feature::feature(*anObjIt);
      if (aFeature)
        myFeatures.insert(aFeature);
    }
  }
}

bool SketchSolver_ConstraintMulti::remove()
{
  myFeatures.clear();
  return SketchSolver_Constraint::remove();
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
  bool isUpdated= anInitialRefList->size() != myNumberOfObjects || aNbObjects->value()-1 != myNumberOfCopies;
  if (!isUpdated) {
    // additional check that the features and their copies are changed
    AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    if (aRefList && aRefList->size() != 0) {
      FeaturePtr aFeature;
      std::list<ObjectPtr> anObjectList = aRefList->list();
      std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
      for (; anObjIt != anObjectList.end(); ++anObjIt) {
        aFeature = ModelAPI_Feature::feature(*anObjIt);
        if (aFeature && myFeatures.find(aFeature) == myFeatures.end()) {
          isUpdated = true;
          break;
        }
      }
    } else
      isUpdated = true;
  }
  if (isUpdated) {
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
  AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aRefList || aRefList->size() == 0) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr aFeature;
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
  while (anObjIt != anObjectList.end()) {
    aFeature = ModelAPI_Feature::feature(*anObjIt++);
    if (!aFeature)
      continue;

    // Fill lists of coordinates of points composing a feature
    std::list<double> aX, aY;
    std::list<double>::iterator aXIt, aYIt;
    double aXCoord, aYCoord;
    EntityWrapperPtr anEntity = myStorage->entity(aFeature);
    std::list<EntityWrapperPtr> aSubs = anEntity->subEntities();
    std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
    for (; aSIt != aSubs.end(); ++aSIt) {
      if ((*aSIt)->type() != ENTITY_POINT)
        continue;
      std::list<ParameterWrapperPtr> aParameters = (*aSIt)->parameters();
      aXCoord = aParameters.front()->value();
      aYCoord = aParameters.back()->value();
      getRelative(aXCoord, aYCoord, aXCoord, aYCoord);
      aX.push_back(aXCoord);
      aY.push_back(aYCoord);
    }

    // Calculate positions of copied features
    for (int i = 0; i < myNumberOfCopies && anObjIt != anObjectList.end(); ++i, ++anObjIt) {
      aFeature = ModelAPI_Feature::feature(*anObjIt);
      if (!aFeature)
        continue;
      anEntity = myStorage->entity(aFeature);

      if (!anEntity || !myStorage->isEventsBlocked())
        aFeature->data()->blockSendAttributeUpdated(true);

      std::list<AttributePtr> aPoints;
      if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
        aPoints.push_back(aFeature->attribute(SketchPlugin_Arc::CENTER_ID()));
        aPoints.push_back(aFeature->attribute(SketchPlugin_Arc::START_ID()));
        aPoints.push_back(aFeature->attribute(SketchPlugin_Arc::END_ID()));
      } else if (aFeature->getKind() == SketchPlugin_Line::ID()) {
        aPoints.push_back(aFeature->attribute(SketchPlugin_Line::START_ID()));
        aPoints.push_back(aFeature->attribute(SketchPlugin_Line::END_ID()));
      } else
        aPoints = aFeature->data()->attributes(GeomDataAPI_Point2D::typeId());

      std::list<AttributePtr>::iterator aPtIt = aPoints.begin();
      for (aXIt = aX.begin(), aYIt = aY.begin(); aPtIt != aPoints.end(); ++aXIt, ++aYIt, ++aPtIt) {
        transformRelative(*aXIt, *aYIt);
        getAbsolute(*aXIt, *aYIt, aXCoord, aYCoord);

        std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*aPtIt);
        aPoint2D->setValue(aXCoord, aYCoord);
      }

      // update feature in the storage if it is used by another constraints
      if (anEntity)
        myStorage->update(aFeature);

      if (!anEntity || !myStorage->isEventsBlocked())
        aFeature->data()->blockSendAttributeUpdated(false);
    }
  }

  myAdjusted = true;
}
