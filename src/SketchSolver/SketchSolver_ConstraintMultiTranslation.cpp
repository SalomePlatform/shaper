#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_MultiTranslation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <math.h>


void SketchSolver_ConstraintMultiTranslation::getAttributes(
    Slvs_hEntity& theStartPoint, Slvs_hEntity& theEndPoint,
    std::vector<std::vector<Slvs_hEntity> >& thePoints,
    std::vector<std::vector<Slvs_hEntity> >& theCircular)
{
  DataPtr aData = myBaseConstraint->data();
  AttributePtr aStartPointAttr = aData->attribute(SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributePtr aEndPointAttr = aData->attribute(SketchPlugin_MultiTranslation::END_POINT_ID());
  if (!aStartPointAttr || !aStartPointAttr->isInitialized() ||
      !aEndPointAttr || !aEndPointAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  int aType = SLVS_E_UNKNOWN; // type of created entity
  Slvs_hEntity anEntityID = myGroup->getAttributeId(aStartPointAttr);
  if (anEntityID == SLVS_E_UNKNOWN)
    anEntityID = changeEntity(aStartPointAttr, aType);
  theStartPoint = anEntityID;
  anEntityID = myGroup->getAttributeId(aEndPointAttr);
  if (anEntityID == SLVS_E_UNKNOWN)
    anEntityID = changeEntity(aEndPointAttr, aType);
  theEndPoint = anEntityID;

  // Lists of objects and number of copies
  AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  myNumberOfObjects = anInitialRefList->size();
  myNumberOfCopies = (size_t)std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_MultiTranslation::NUMBER_OF_COPIES_ID()))->value();
  AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aRefList) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  // Obtain all points of initial features and store them into separate lists
  // containing their translated copies.
  // Also all circles and arc collected too, because they will be constrained by equal radii.
  FeaturePtr aFeature;
  ResultConstructionPtr aRC;
  std::vector<Slvs_hEntity> aPoints[2]; // lists of points of features
  std::vector<Slvs_hEntity> aCircs;     // list of circular objects
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjectIter = anObjectList.begin();
  while (anObjectIter != anObjectList.end()) {
    aPoints[0].clear();
    aPoints[1].clear();
    aCircs.clear();

    for (size_t i = 0; i <= myNumberOfCopies && anObjectIter != anObjectList.end(); i++, anObjectIter++) {
      aFeature = ModelAPI_Feature::feature(*anObjectIter);
      if (!aFeature)
        continue;
      anEntityID = changeEntity(aFeature, aType);
      Slvs_Entity anEntity = myStorage->getEntity(anEntityID);
      switch (aType) {
      case SLVS_E_POINT_IN_2D:
      case SLVS_E_POINT_IN_3D:
        aPoints[0].push_back(anEntityID);
        break;
      case SLVS_E_LINE_SEGMENT:
        aPoints[0].push_back(anEntity.point[0]); // start point of line
        aPoints[1].push_back(anEntity.point[1]); // end point of line
        break;
      case SLVS_E_CIRCLE:
        aPoints[0].push_back(anEntity.point[0]); // center of circle
        aCircs.push_back(anEntityID);
        break;
      case SLVS_E_ARC_OF_CIRCLE:
        aPoints[0].push_back(anEntity.point[1]); // start point of arc
        aPoints[1].push_back(anEntity.point[2]); // end point of arc
        aCircs.push_back(anEntityID);
        break;
      default:
        myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
        return;
      }
    }

    if (!aPoints[0].empty())
      thePoints.push_back(aPoints[0]);
    if (!aPoints[1].empty())
      thePoints.push_back(aPoints[1]);
    if (!aCircs.empty())
      theCircular.push_back(aCircs);
  }
}

void SketchSolver_ConstraintMultiTranslation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  Slvs_hEntity aStartPoint, aEndPoint;
  std::vector<std::vector<Slvs_hEntity> > aPointsAndCopies;
  std::vector<std::vector<Slvs_hEntity> > aCircsAndCopies;
  getAttributes(aStartPoint, aEndPoint, aPointsAndCopies, aCircsAndCopies);
  if (!myErrorMsg.empty())
    return;

  // Create lines between neighbor translated points and make them parallel to the translation line.
  // Also these lines should have equal lengths.
  Slvs_Constraint aConstraint;
  Slvs_Entity aTranslationLine = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
      myGroup->getWorkplaneId(), aStartPoint, aEndPoint);
  aTranslationLine.h = myStorage->addEntity(aTranslationLine);
  myTranslationLine = aTranslationLine.h;
  std::vector<std::vector<Slvs_hEntity> >::iterator aCopyIter = aPointsAndCopies.begin();
  for (; aCopyIter != aPointsAndCopies.end(); aCopyIter++) {
    size_t aSize = aCopyIter->size();
    for (size_t i = 0; i < aSize - 1; i++) {
      Slvs_Entity aLine = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
          myGroup->getWorkplaneId(), (*aCopyIter)[i], (*aCopyIter)[i+1]);
      aLine.h = myStorage->addEntity(aLine);
      // Equal length constraint
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_EQUAL_LENGTH_LINES, myGroup->getWorkplaneId(), 0.0,
          SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aTranslationLine.h, aLine.h);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
      // Parallel constraint
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_PARALLEL, myGroup->getWorkplaneId(), 0.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN,
          aTranslationLine.h, aLine.h);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
    }
  }
  // Equal radii constraints
  for (aCopyIter = aCircsAndCopies.begin(); aCopyIter != aCircsAndCopies.end(); aCopyIter++) {
    size_t aSize = aCopyIter->size();
    for (size_t i = 0; i < aSize - 1; i++) {
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_EQUAL_RADIUS, myGroup->getWorkplaneId(), 0.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN,
          (*aCopyIter)[i], (*aCopyIter)[i+1]);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
    }
  }

  // Set the translation line unchanged during constraint recalculation
  for (int i = 0; i < 2; i++) {
    if (myStorage->isPointFixed(aTranslationLine.point[i], aConstraint.h, true))
      continue;
    aConstraint = Slvs_MakeConstraint(
        SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_WHERE_DRAGGED, myGroup->getWorkplaneId(), 0.0,
        aTranslationLine.point[i], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
  }

  adjustConstraint();
}

void SketchSolver_ConstraintMultiTranslation::update(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (!theConstraint || theConstraint == myBaseConstraint) {
    AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeDoublePtr aNbCopies = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        myBaseConstraint->attribute(SketchPlugin_MultiTranslation::NUMBER_OF_COPIES_ID()));
    if (anInitialRefList->size() != myNumberOfObjects ||
        (size_t)aNbCopies->value() != myNumberOfCopies) {
      remove(myBaseConstraint);
      process();
      return;
    }
  }
  SketchSolver_Constraint::update();
}

bool SketchSolver_ConstraintMultiTranslation::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  bool isFullyRemoved = true;
  std::vector<Slvs_hEntity>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++)
   isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;
  mySlvsConstraints.clear();

  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.begin();
  for (; aFeatIt != myFeatureMap.end(); aFeatIt++)
    myStorage->removeEntity(aFeatIt->second);

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();
  return true;
}

void SketchSolver_ConstraintMultiTranslation::adjustConstraint()
{
  Slvs_Entity aTranslationLine = myStorage->getEntity(myTranslationLine);
  // Check if the distance between point is 0, no need to resolve constraints (just wait another values)
  double aXY[4];
  for (int i = 0; i < 2; i++) {
    Slvs_Entity aPnt = myStorage->getEntity(aTranslationLine.point[i]);
    aXY[2*i] = myStorage->getParameter(aPnt.param[0]).val;
    aXY[2*i+1] = myStorage->getParameter(aPnt.param[1]).val;
  }
  double aDelta[2] = {aXY[2] - aXY[0], aXY[3] - aXY[1]};
  if (fabs(aDelta[0]) + fabs(aDelta[1]) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  // Update positions of all points to satisfy distances
  std::list<Slvs_Constraint> aParallel = myStorage->getConstraintsByType(SLVS_C_PARALLEL);
  std::list<Slvs_Constraint>::iterator aParIt = aParallel.begin();
  std::vector<Slvs_hConstraint>::iterator aCIt;
  Slvs_hConstraint aFixed; // temporary variable
  for (; aParIt != aParallel.end(); aParIt++) {
    for (aCIt = mySlvsConstraints.begin(); aCIt != mySlvsConstraints.end(); aCIt++)
      if (aParIt->h == *aCIt)
        break;
    if (aCIt == mySlvsConstraints.end())
      continue;
    Slvs_Entity aLine = myStorage->getEntity(aParIt->entityB);
    if (myStorage->isPointFixed(aLine.point[1], aFixed))
      continue;
    Slvs_Entity aStart = myStorage->getEntity(aLine.point[0]);
    Slvs_Entity aEnd = myStorage->getEntity(aLine.point[1]);
    for (int i = 0; i < 2; i++) {
      Slvs_Param aFrom = myStorage->getParameter(aStart.param[i]);
      Slvs_Param aTo = myStorage->getParameter(aEnd.param[i]);
      aTo.val = aFrom.val + aDelta[i];
      myStorage->updateParameter(aTo);
    }
  }

  // update positions of centers of arcs for correct radius calculation
  std::list<Slvs_Constraint> aRadii = myStorage->getConstraintsByType(SLVS_C_EQUAL_RADIUS);
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt;
  for (aParIt = aRadii.begin(); aParIt != aRadii.end(); aParIt++) {
    int aNbFound = 0; // number of arcs used in translation
    for (aFeatIt = myFeatureMap.begin(); aFeatIt != myFeatureMap.end(); aFeatIt++)
      if (aFeatIt->second == aParIt->entityA || aFeatIt->second == aParIt->entityB) {
        if (aFeatIt->first->getKind() == SketchPlugin_Arc::ID())
          aNbFound++;
        else
          break;
      }
    if (aNbFound != 2)
      continue;
    // two arcs were found, update their centers
    Slvs_Entity anArcA = myStorage->getEntity(aParIt->entityA);
    Slvs_Entity anArcB = myStorage->getEntity(aParIt->entityB);
    if (myStorage->isPointFixed(anArcB.point[0], aFixed))
      continue;
    Slvs_Entity aCenterA = myStorage->getEntity(anArcA.point[0]);
    Slvs_Entity aCenterB = myStorage->getEntity(anArcB.point[0]);
    for (int i = 0; i < 2; i++) {
      Slvs_Param aFrom = myStorage->getParameter(aCenterA.param[i]);
      Slvs_Param aTo = myStorage->getParameter(aCenterB.param[i]);
      aTo.val = aFrom.val + aDelta[i];
      myStorage->updateParameter(aTo);
    }
  }
}
