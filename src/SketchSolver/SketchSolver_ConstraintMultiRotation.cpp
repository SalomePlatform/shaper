#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_MultiRotation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <math.h>

static double squareDistance(
    StoragePtr theStorage, const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2)
{
  Slvs_Entity aPoint1 = theStorage->getEntity(thePoint1);
  Slvs_Entity aPoint2 = theStorage->getEntity(thePoint2);
  double x1 = theStorage->getParameter(aPoint1.param[0]).val;
  double y1 = theStorage->getParameter(aPoint1.param[1]).val;
  double x2 = theStorage->getParameter(aPoint2.param[0]).val;
  double y2 = theStorage->getParameter(aPoint2.param[1]).val;
  return (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
}

void SketchSolver_ConstraintMultiRotation::getAttributes(
    Slvs_hEntity& theCenter, double& theAngle,
    std::vector<std::vector<Slvs_hEntity> >& thePoints,
    std::vector<std::vector<Slvs_hEntity> >& theCircular)
{
  DataPtr aData = myBaseConstraint->data();
  theAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_MultiRotation::ANGLE_ID()))->value();

  AttributePtr aCenterAttr = aData->attribute(SketchPlugin_MultiRotation::CENTER_ID());
  if (!aCenterAttr || !aCenterAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }
  int aType = SLVS_E_UNKNOWN; // type of created entity
  Slvs_hEntity anEntityID = myGroup->getAttributeId(aCenterAttr);
  if (anEntityID == SLVS_E_UNKNOWN)
    anEntityID = changeEntity(aCenterAttr, aType);
  theCenter = anEntityID;

  // Lists of objects and number of copies
  AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  myNumberOfObjects = anInitialRefList->size();
  myNumberOfCopies = (size_t) aData->integer(SketchPlugin_MultiRotation::NUMBER_OF_COPIES_ID())->value();
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

void SketchSolver_ConstraintMultiRotation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  Slvs_hEntity aCenter;
  std::vector<std::vector<Slvs_hEntity> > aPointsAndCopies;
  std::vector<std::vector<Slvs_hEntity> > aCircsAndCopies;
  getAttributes(aCenter, myAngle, aPointsAndCopies, aCircsAndCopies);
  if (!myErrorMsg.empty())
    return;

  myAuxLines.clear();

  // Create lines between neighbor rotated points and make angle between them equal to anAngle.
  // Also these lines should have equal lengths.
  Slvs_Constraint aConstraint;
  myRotationCenter = aCenter;
  Slvs_Entity aPrevLine;
  std::vector<std::vector<Slvs_hEntity> >::iterator aCopyIter = aPointsAndCopies.begin();
  for (; aCopyIter != aPointsAndCopies.end(); aCopyIter++) {
    size_t aSize = aCopyIter->size();
    if (aSize <= 1) continue;

    aPrevLine = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
          myGroup->getWorkplaneId(), aCenter, (*aCopyIter)[0]);
    aPrevLine.h = myStorage->addEntity(aPrevLine);
    std::vector<Slvs_hEntity> anEqualLines(1, aPrevLine.h);
    for (size_t i = 1; i < aSize; i++) {
      Slvs_Entity aLine = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
          myGroup->getWorkplaneId(), aCenter, (*aCopyIter)[i]);
      aLine.h = myStorage->addEntity(aLine);
      // Equal length constraint
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_EQUAL_LENGTH_LINES, myGroup->getWorkplaneId(), 0.0,
          SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aPrevLine.h, aLine.h);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
      // Angle constraint
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_ANGLE, myGroup->getWorkplaneId(), fabs(myAngle), SLVS_E_UNKNOWN, SLVS_E_UNKNOWN,
          aPrevLine.h, aLine.h);
      if (myAngle < 0.0) // clockwise rotation
        aConstraint.other = true;
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);

      aPrevLine = aLine;
      anEqualLines.push_back(aPrevLine.h);
    }

    myAuxLines.push_back(anEqualLines);
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

  // Set the rotation center unchanged during constraint recalculation
  if (!myStorage->isPointFixed(myRotationCenter, aConstraint.h, true)) {
    aConstraint = Slvs_MakeConstraint(
        SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_WHERE_DRAGGED, myGroup->getWorkplaneId(), 0.0,
        myRotationCenter, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
  }

  adjustConstraint();
}

void SketchSolver_ConstraintMultiRotation::update(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (!theConstraint || theConstraint == myBaseConstraint) {
    AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeIntegerPtr aNbCopies = myBaseConstraint->integer(SketchPlugin_MultiRotation::NUMBER_OF_COPIES_ID());
    if (anInitialRefList->size() != myNumberOfObjects || aNbCopies->value() != myNumberOfCopies) {
      remove(myBaseConstraint);
      process();
      return;
    }
  }

  // update angle value
  myAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      myBaseConstraint->attribute(SketchPlugin_MultiRotation::ANGLE_ID()))->value();

  SketchSolver_Constraint::update();
}

bool SketchSolver_ConstraintMultiRotation::remove(ConstraintPtr theConstraint)
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
  myStorage->removeUnusedEntities();

  std::map<FeaturePtr, Slvs_hEntity> aFeatureMapCopy = myFeatureMap;

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();

  // Restore initial features
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFIter = aFeatureMapCopy.begin();
  for (; aFIter != aFeatureMapCopy.end(); ++aFIter)
  {
    if (myFeatureMap.find(aFIter->first) != myFeatureMap.end())
      continue; // the feature was not removed
    Slvs_hEntity anEntity = myGroup->getFeatureId(aFIter->first);
    if (anEntity != SLVS_E_UNKNOWN)
      myFeatureMap[aFIter->first] = anEntity;
  }

  return true;
}

void SketchSolver_ConstraintMultiRotation::adjustConstraint()
{
  if (abs(myAngle) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  // Check the lengths of auxiliary lines are zero.
  // If they become zero, remove corresponding Angle constraints.
  // It they become non-zero (but were zero recently), add Angle constraint.
  std::vector<Slvs_hConstraint>::iterator aConstr = mySlvsConstraints.begin();
  std::map<Slvs_hEntity, Slvs_hEntity> anEqualLines;
  bool isFirstRemoved = false;
  for (; aConstr != mySlvsConstraints.end();
       isFirstRemoved ? aConstr = mySlvsConstraints.begin() : ++aConstr) {
    isFirstRemoved = false;
    Slvs_Constraint aConstraint = myStorage->getConstraint(*aConstr);
    if (aConstraint.type == SLVS_C_ANGLE || aConstraint.type == SLVS_C_EQUAL_LENGTH_LINES) {
      Slvs_Entity aLine = myStorage->getEntity(aConstraint.entityA);
      // Line length became zero => remove constraint
      if (squareDistance(myStorage, aLine.point[0], aLine.point[1]) < tolerance * tolerance) {
        myStorage->removeConstraint(aConstraint.h);
        isFirstRemoved = aConstr == mySlvsConstraints.begin();
        std::vector<Slvs_hConstraint>::iterator aTmpIter = aConstr;
        if (!isFirstRemoved)
          --aConstr;
        mySlvsConstraints.erase(aTmpIter);
      }
      // Store the lines into the map
      anEqualLines[aConstraint.entityB] = aConstraint.entityA;
    }
  }
  // Create Angle and Equal constraints for non-degenerated lines
  AuxLinesList::iterator anIt = myAuxLines.begin();
  for (; anIt != myAuxLines.end(); ++anIt) {
    if (anEqualLines.find(anIt->back()) != anEqualLines.end())
      continue;

    std::vector<Slvs_hEntity>::iterator anEqLinesIt = anIt->begin();
    Slvs_hEntity aPrevLine = (*anEqLinesIt);
    // Check the length of the line
    Slvs_Entity aLine = myStorage->getEntity(aPrevLine);
    if (squareDistance(myStorage, aLine.point[0], aLine.point[1]) < tolerance * tolerance)
      continue;

    for (++anEqLinesIt; anEqLinesIt != anIt->end(); ++anEqLinesIt) {
      Slvs_hEntity aLine = (*anEqLinesIt);
      // Equal length constraint
      Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_EQUAL_LENGTH_LINES, myGroup->getWorkplaneId(), 0.0,
          SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aPrevLine, aLine);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
      // Angle constraint
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_ANGLE, myGroup->getWorkplaneId(), fabs(myAngle), SLVS_E_UNKNOWN, SLVS_E_UNKNOWN,
          aPrevLine, aLine);
      if (myAngle < 0.0) // clockwise rotation
        aConstraint.other = true;
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);

      aPrevLine = aLine;
    }
  }

  // Obtain coordinates of rotation center
  Slvs_Entity aRotCenter = myStorage->getEntity(myRotationCenter);
  double aCenterXY[2];
  for (int i = 0; i < 2; i++)
    aCenterXY[i] = myStorage->getParameter(aRotCenter.param[i]).val;

  double cosA = cos(myAngle * PI / 180.0);
  double sinA = sin(myAngle * PI / 180.0);

  // Update positions of all points to satisfy angles
  std::list<Slvs_Constraint> aConstrAngle = myStorage->getConstraintsByType(SLVS_C_ANGLE);
  std::list<Slvs_Constraint>::iterator anAngIt = aConstrAngle.begin();
  std::vector<Slvs_hConstraint>::iterator aCIt;
  Slvs_hConstraint aFixed; // temporary variable
  double aVec[2]; // coordinates of vector defining a line
  Slvs_Param aTarget[2]; // parameter to be changed
  for (; anAngIt != aConstrAngle.end(); anAngIt++) {
    for (aCIt = mySlvsConstraints.begin(); aCIt != mySlvsConstraints.end(); aCIt++)
      if (anAngIt->h == *aCIt)
        break;
    if (aCIt == mySlvsConstraints.end())
      continue;
    Slvs_Entity aLineA = myStorage->getEntity(anAngIt->entityA);
    Slvs_Entity aLineB = myStorage->getEntity(anAngIt->entityB);
    if (myStorage->isPointFixed(aLineB.point[1], aFixed))
      continue;
    Slvs_Entity aPointA = myStorage->getEntity(aLineA.point[1]);
    Slvs_Entity aPointB = myStorage->getEntity(aLineB.point[1]);
    for (int i = 0; i < 2; i++) {
      aVec[i] = myStorage->getParameter(aPointA.param[i]).val - aCenterXY[i];
      aTarget[i] = myStorage->getParameter(aPointB.param[i]);
    }
    aTarget[0].val = aCenterXY[0] + aVec[0] * cosA - aVec[1] * sinA;
    aTarget[1].val = aCenterXY[1] + aVec[0] * sinA + aVec[1] * cosA;
    myStorage->updateParameter(aTarget[0]);
    myStorage->updateParameter(aTarget[1]);

    anAngIt->valA = myAngle;
    myStorage->updateConstraint(*anAngIt);
  }

  // update positions of centers of arcs for correct radius calculation
  std::list<Slvs_Constraint> aRadii = myStorage->getConstraintsByType(SLVS_C_EQUAL_RADIUS);
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt;
  for (anAngIt = aRadii.begin(); anAngIt != aRadii.end(); anAngIt++) {
    int aNbFound = 0; // number of arcs used in translation
    for (aFeatIt = myFeatureMap.begin(); aFeatIt != myFeatureMap.end(); aFeatIt++)
      if (aFeatIt->second == anAngIt->entityA || aFeatIt->second == anAngIt->entityB) {
        if (aFeatIt->first->getKind() == SketchPlugin_Arc::ID())
          aNbFound++;
        else
          break;
      }
    if (aNbFound != 2)
      continue;
    // two arcs were found, update their centers
    Slvs_Entity anArcA = myStorage->getEntity(anAngIt->entityA);
    Slvs_Entity anArcB = myStorage->getEntity(anAngIt->entityB);
    if (myStorage->isPointFixed(anArcB.point[0], aFixed))
      continue;
    Slvs_Entity aCenterA = myStorage->getEntity(anArcA.point[0]);
    Slvs_Entity aCenterB = myStorage->getEntity(anArcB.point[0]);
    for (int i = 0; i < 2; i++) {
      aVec[i] = myStorage->getParameter(aCenterA.param[i]).val - aCenterXY[i];
      aTarget[i] = myStorage->getParameter(aCenterB.param[i]);
    }
    aTarget[0].val = aCenterXY[0] + aVec[0] * cosA - aVec[1] * sinA;
    aTarget[1].val = aCenterXY[1] + aVec[0] * sinA + aVec[1] * cosA;
    myStorage->updateParameter(aTarget[0]);
    myStorage->updateParameter(aTarget[1]);
  }
}
