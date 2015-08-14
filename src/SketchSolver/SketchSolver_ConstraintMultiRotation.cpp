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
    std::vector< std::vector<Slvs_hEntity> >& thePoints,
    std::vector< std::vector<Slvs_hEntity> >& theEntities)
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
  static const size_t MAX_POINTS = 3;
  std::vector<Slvs_hEntity> aPoints[MAX_POINTS]; // lists of points of features
  std::vector<Slvs_hEntity> anEntities;
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjectIter = anObjectList.begin();
  while (anObjectIter != anObjectList.end()) {
    for (size_t i = 0; i < MAX_POINTS; ++i)
      aPoints[i].clear();
    anEntities.clear();

    for (size_t i = 0; i <= myNumberOfCopies && anObjectIter != anObjectList.end(); i++, anObjectIter++) {
      aFeature = ModelAPI_Feature::feature(*anObjectIter);
      if (!aFeature)
        continue;
      anEntityID = changeEntity(aFeature, aType);
      anEntities.push_back(anEntityID);
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
        break;
      case SLVS_E_ARC_OF_CIRCLE:
        aPoints[0].push_back(anEntity.point[0]); // center of arc
        aPoints[1].push_back(anEntity.point[1]); // start point of arc
        aPoints[2].push_back(anEntity.point[2]); // end point of arc
        break;
      default:
        myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
        return;
      }
    }

    for (size_t i = 0; i < MAX_POINTS; ++i)
      if (!aPoints[i].empty())
        thePoints.push_back(aPoints[i]);
    if (!anEntities.empty())
      theEntities.push_back(anEntities);
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

  std::vector<std::vector<Slvs_hEntity> > anEntitiesAndCopies;
  getAttributes(myRotationCenter, myAngle, myPointsAndCopies, anEntitiesAndCopies);
  if (!myErrorMsg.empty())
    return;

  // Set the rotation center unchanged during constraint recalculation
  Slvs_Constraint aConstraint;
  if (!myStorage->isPointFixed(myRotationCenter, aConstraint.h, true)) {
    aConstraint = Slvs_MakeConstraint(
        SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_WHERE_DRAGGED, myGroup->getWorkplaneId(), 0.0,
        myRotationCenter, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
  }

  // Keep all objects unchanged (only initial object may be changed by user)
  myCircsAndCopies.clear();
  std::vector<std::vector<Slvs_hEntity> >::const_iterator anEntIt = anEntitiesAndCopies.begin();
  std::vector<Slvs_hEntity>::const_iterator aCpIt;
  for (; anEntIt != anEntitiesAndCopies.end(); ++anEntIt) {
    std::vector<Slvs_hEntity> aCircs;
    aCpIt = anEntIt->begin();
    // Obtain initial points
    Slvs_Entity anInitial = myStorage->getEntity(*aCpIt);
    if (anInitial.type == SLVS_E_POINT_IN_2D || anInitial.type == SLVS_E_POINT_IN_3D)
      myInitialPoints.insert(anInitial.h);
    else {
      for (int i = 0; i < 4 && anInitial.point[i] != SLVS_E_UNKNOWN; i++)
        myInitialPoints.insert(anInitial.point[i]);
    }

    // Fix the copies
    for (++aCpIt; aCpIt != anEntIt->end(); ++aCpIt) {
      const Slvs_Entity& anEntity = myStorage->getEntity(*aCpIt);
      std::vector<Slvs_hConstraint> aNewConstr;
      if (anEntity.type == SLVS_E_CIRCLE) {
        aCircs.push_back(anEntity.distance);
        // for circles we fix only center
        aNewConstr = myStorage->fixEntity(anEntity.point[0]);
      } else
        aNewConstr = myStorage->fixEntity(*aCpIt);
      mySlvsConstraints.insert(mySlvsConstraints.end(), aNewConstr.begin(), aNewConstr.end());
    }

    if (!aCircs.empty())
      myCircsAndCopies.push_back(aCircs);
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

  // Clear list of rotated points
  myPointsAndCopies.clear();
  myInitialPoints.clear();

  return true;
}

void SketchSolver_ConstraintMultiRotation::addFeature(FeaturePtr theFeature)
{
  SketchSolver_Constraint::addFeature(theFeature);

  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.find(theFeature);
  if (aFeatIt == myFeatureMap.end())
    return;

  // store list of points of the feature
  const Slvs_Entity& theEntity = myStorage->getEntity(aFeatIt->second);
  for (int i = 0; i < 4; i++)
    if (theEntity.point[i] != SLVS_E_UNKNOWN)
      myPointsJustUpdated.insert(theEntity.point[i]);
}

void SketchSolver_ConstraintMultiRotation::adjustConstraint()
{
  if (abs(myAngle) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  std::list<Slvs_Constraint> aCoincident = myStorage->getConstraintsByType(SLVS_C_POINTS_COINCIDENT);
  std::list<Slvs_Constraint>::const_iterator aCoIt;

  // Check overconstrained on rotation center (if it is coincident with other fixed point)
  Slvs_hConstraint aFixedCenter;
  if (myStorage->isPointFixed(myRotationCenter, aFixedCenter, false)) {
    Slvs_hConstraint aFixed;
    for (aCoIt = aCoincident.begin(); aCoIt != aCoincident.end(); ++aCoIt)
      if ((aCoIt->ptA == myRotationCenter && myStorage->isPointFixed(aCoIt->ptB, aFixed, true)) ||
          (aCoIt->ptB == myRotationCenter && myStorage->isPointFixed(aCoIt->ptA, aFixed, true))) {
        // Un-fix the center
        myStorage->removeConstraint(aFixedCenter);
        std::vector<Slvs_hConstraint>::iterator aSCIt = mySlvsConstraints.begin();
        for (; aSCIt != mySlvsConstraints.end(); ++aSCIt)
          if (*aSCIt == aFixedCenter) {
            mySlvsConstraints.erase(aSCIt);
            break;
          }
      }
  }

  // Obtain coordinates of rotation center
  Slvs_Entity aRotCenter = myStorage->getEntity(myRotationCenter);
  double aCenterXY[2];
  for (int i = 0; i < 2; i++)
    aCenterXY[i] = myStorage->getParameter(aRotCenter.param[i]).val;

  double cosA = cos(myAngle * PI / 180.0);
  double sinA = sin(myAngle * PI / 180.0);

  double aVec[2]; // coordinates of vector defining a direction from rotation center to a point

  // Update positions of all points to satisfy angles
  std::vector< std::vector<Slvs_hEntity> >::const_iterator aPointsIter = myPointsAndCopies.begin();
  std::vector<Slvs_hEntity>::const_iterator aCopyIter;
  for (; aPointsIter != myPointsAndCopies.end(); ++aPointsIter) {
    aCopyIter = aPointsIter->begin();
    const Slvs_Entity& anInitial = myStorage->getEntity(*aCopyIter);
    for (int i = 0; i < 2; i++)
      aVec[i] = myStorage->getParameter(anInitial.param[i]).val - aCenterXY[i];

    // if the point is coincident with another one which is temporary fixed (moved by user),
    // we will update its position correspondingly
    Slvs_hConstraint aFixed;
    for (aCoIt = aCoincident.begin(); aCoIt != aCoincident.end(); ++aCoIt) {
      if ((aCoIt->ptA == anInitial.h && myInitialPoints.find(aCoIt->ptB) != myInitialPoints.end()) ||
          (aCoIt->ptB == anInitial.h && myInitialPoints.find(aCoIt->ptA) != myInitialPoints.end())) {
        Slvs_hEntity anOtherId = aCoIt->ptA == anInitial.h ? aCoIt->ptB : aCoIt->ptA;
        if (!myStorage->isTemporary(aFixed) &&
            myPointsJustUpdated.find(anOtherId) == myPointsJustUpdated.end())
          continue; // nothing to change

        const Slvs_Entity& anOtherPnt = myStorage->getEntity(anOtherId);
        for (int i = 0; i < 2; i++) {
          Slvs_Param anInitParam = myStorage->getParameter(anInitial.param[i]);
          const Slvs_Param& anOtherParam = myStorage->getParameter(anOtherPnt.param[i]);
          anInitParam.val = anOtherParam.val;
          myStorage->updateParameter(anInitParam);
          aVec[i] = anOtherParam.val - aCenterXY[i];
        }
      }
    }

    // update copied points
    aCopyIter = aPointsIter->begin();
    for (++aCopyIter; aCopyIter != aPointsIter->end(); ++aCopyIter) {
      // rotate direction
      double aTemp = aVec[0] * cosA - aVec[1] * sinA;
      aVec[1] = aVec[0] * sinA + aVec[1] * cosA;
      aVec[0] = aTemp;

      const Slvs_Entity& aTarget = myStorage->getEntity(*aCopyIter);
      for (int i = 0; i < 2; i++) {
        Slvs_Param aParam = myStorage->getParameter(aTarget.param[i]);
        aParam.val = aCenterXY[i] + aVec[i];
        myStorage->updateParameter(aParam);
      }
    }
  }

  for (aPointsIter = myCircsAndCopies.begin(); aPointsIter != myCircsAndCopies.end(); ++aPointsIter) {
    aCopyIter = aPointsIter->begin();
    const Slvs_Entity& anInitial = myStorage->getEntity(*aCopyIter);
    const Slvs_Param& anInitRad = myStorage->getParameter(anInitial.param[0]);
    for (++aCopyIter; aCopyIter != aPointsIter->end(); ++aCopyIter) {
      const Slvs_Entity& aCopy = myStorage->getEntity(*aCopyIter);
      Slvs_Param aCopyRad = myStorage->getParameter(aCopy.param[0]);
      aCopyRad.val = anInitRad.val;
      myStorage->updateParameter(aCopyRad);
    }
  }

  myPointsJustUpdated.clear();
}
