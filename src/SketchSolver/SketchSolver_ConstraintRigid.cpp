#include <SketchSolver_ConstraintRigid.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Group.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>

#include <math.h>

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
  if (!myErrorMsg.empty() || (myFeatureMap.empty() && myAttributeMap.empty()))
    return;
  fixFeature();
}

void SketchSolver_ConstraintRigid::update(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint == myBaseConstraint &&
      theConstraint->getKind() == myBaseConstraint->getKind() &&
      checkAttributesChanged(theConstraint)) {
    // remove previous constraint and set the given one
    remove(myBaseConstraint);
    myBaseConstraint = theConstraint;
    process();
  }
}

static void fixEntity(StoragePtr theStorage, const Slvs_hEntity& theEntID)
{
  Slvs_Entity anEntity = theStorage->getEntity(theEntID);
  anEntity.group = SLVS_G_OUTOFGROUP;
  theStorage->updateEntity(anEntity);
  // move out of group all sub-entities
  for (int i = 0; i < 4; ++i)
    if (anEntity.point[i] != SLVS_E_UNKNOWN)
      fixEntity(theStorage, anEntity.point[i]);
  // move out of group the radius of circle
  if (anEntity.distance != SLVS_E_UNKNOWN)
    fixEntity(theStorage, anEntity.distance);
  // move out of group parameters
  for (int i = 0; i < 4; ++i)
    if (anEntity.param[i] != SLVS_E_UNKNOWN) {
      Slvs_Param aParam = theStorage->getParameter(anEntity.param[i]);
      aParam.group = SLVS_G_OUTOFGROUP;
      theStorage->updateParameter(aParam);
    }
}

void SketchSolver_ConstraintRigid::fixFeature()
{
  Slvs_hEntity anEntID = fixedEntity();
  if (anEntID != SLVS_E_UNKNOWN)
    fixEntity(myStorage, anEntID);
}

Slvs_hEntity SketchSolver_ConstraintRigid::fixedEntity() const
{
  Slvs_hEntity anEntID = SLVS_E_UNKNOWN;
  if (myBaseConstraint) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    if (aRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      std::map<FeaturePtr, Slvs_hEntity>::const_iterator aFound = myFeatureMap.find(aFeature);
      if (aFound != myFeatureMap.end())
        anEntID = aFound->second;
    } else {
      std::map<AttributePtr, Slvs_hEntity>::const_iterator aFound = myAttributeMap.find(aRefAttr->attr());
      if (aFound != myAttributeMap.end())
        anEntID = aFound->second;
    }
  }
  else if (myBaseFeature) {
    std::map<FeaturePtr, Slvs_hEntity>::const_iterator aFound = myFeatureMap.find(myBaseFeature);
    if (aFound != myFeatureMap.end())
      anEntID = aFound->second;
  }
  return anEntID;
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
    if (!aRefAttr || !aRefAttr->isInitialized()) {
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
    else
      myFeatureMap[myBaseFeature] = anEntityID;
  }

  if (anEntityID == SLVS_E_UNKNOWN) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
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
  for (; aCIter != mySlvsConstraints.end(); ++aCIter)
    isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;

  std::map<FeaturePtr, Slvs_hEntity>::iterator aFIter = myFeatureMap.begin();
  for (; aFIter != myFeatureMap.end(); ++aFIter)
    isFullyRemoved = myStorage->removeEntity(aFIter->second) && isFullyRemoved;

  std::map<AttributePtr, Slvs_hEntity>::iterator anAtIter = myAttributeMap.begin();
  for (; anAtIter != myAttributeMap.end(); ++anAtIter)
    isFullyRemoved = myStorage->removeEntity(anAtIter->second) && isFullyRemoved;

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
    mySlvsConstraints.clear();
  } else
    cleanRemovedEntities();
  return true;
}

Slvs_hConstraint SketchSolver_ConstraintRigid::fixPoint(const Slvs_hEntity& thePointID)
{
  if (thePointID == SLVS_E_UNKNOWN)
    return SLVS_C_UNKNOWN;

  Slvs_Constraint aConstraint;
  Slvs_hConstraint aConstrID = SLVS_E_UNKNOWN;
  bool isFixed = myStorage->isPointFixed(thePointID, aConstrID, true);
  bool isForceUpdate = (isFixed && !myBaseConstraint &&
                        myStorage->isTemporary(aConstrID));
  if (!isForceUpdate) { // create new constraint
    if (isFixed) return aConstrID;
    aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(),
        0.0, thePointID, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
    if (!myBaseConstraint)
      myStorage->addConstraintWhereDragged(aConstraint.h);
  } else { // update already existent constraint
    if (!isFixed || aConstrID == SLVS_C_UNKNOWN || myBaseConstraint)
      return SLVS_C_UNKNOWN;
    aConstraint = myStorage->getConstraint(aConstrID);
    aConstraint.ptA = thePointID;
    myStorage->addConstraint(aConstraint);
    if (!myBaseConstraint)
      myStorage->addConstraintWhereDragged(aConstraint.h);
  }
  return aConstraint.h;
}

void SketchSolver_ConstraintRigid::fixLine(const Slvs_Entity& theLine)
{
  Slvs_Constraint anEqual;
  if (myStorage->isAxisParallel(theLine.h)) {
    // Fix one point and a line length
    Slvs_hConstraint aFixed;
    if (!myStorage->isPointFixed(theLine.point[0], aFixed, true) &&
        !myStorage->isPointFixed(theLine.point[1], aFixed, true))
      fixPoint(theLine.point[0]);
    if (!myStorage->isUsedInEqual(theLine.h, anEqual)) {
      // Check the distance is not set yet
      std::list<Slvs_Constraint> aDistConstr = myStorage->getConstraintsByType(SLVS_C_PT_PT_DISTANCE);
      std::list<Slvs_Constraint>::const_iterator aDIt = aDistConstr.begin();
      for (; aDIt != aDistConstr.end(); aDIt++)
        if ((aDIt->ptA == theLine.point[0] && aDIt->ptB == theLine.point[1]) ||
            (aDIt->ptA == theLine.point[1] && aDIt->ptB == theLine.point[0]))
          return;
      // Calculate distance between points on the line
      double aCoords[4];
      for (int i = 0; i < 2; i++) {
        Slvs_Entity aPnt = myStorage->getEntity(theLine.point[i]);
        for (int j = 0; j < 2; j++) {
          Slvs_Param aParam = myStorage->getParameter(aPnt.param[j]);
          aCoords[2*i+j] = aParam.val;
        }
      }
      double aLength = sqrt((aCoords[2] - aCoords[0]) * (aCoords[2] - aCoords[0]) + 
                            (aCoords[3] - aCoords[1]) * (aCoords[3] - aCoords[1]));
      // fix line length
      Slvs_Constraint aDistance = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
          SLVS_C_PT_PT_DISTANCE, myGroup->getWorkplaneId(), aLength,
          theLine.point[0], theLine.point[1], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      aDistance.h = myStorage->addConstraint(aDistance);
      mySlvsConstraints.push_back(aDistance.h);
    }
    return;
  }
  else if (myStorage->isUsedInEqual(theLine.h, anEqual)) {
    // Check another entity of Equal is already fixed
    Slvs_hEntity anOtherEntID = anEqual.entityA == theLine.h ? anEqual.entityB : anEqual.entityA;
    if (myStorage->isEntityFixed(anOtherEntID, true)) {
      // Fix start point of the line (if end point is not fixed yet) ...
      Slvs_hConstraint anEndFixedID = SLVS_E_UNKNOWN;
      bool isFixed = myStorage->isPointFixed(theLine.point[1], anEndFixedID, true);
      if (isFixed == SLVS_E_UNKNOWN)
        fixPoint(theLine.point[0]);
      // ...  and create fixed point lying on this line
      Slvs_hEntity aPointToCopy = anEndFixedID == SLVS_E_UNKNOWN ? theLine.point[1] : theLine.point[0];
      // Firstly, search already fixed point on line
      bool isPonLineFixed = false;
      Slvs_hEntity aFixedPoint;
      std::list<Slvs_Constraint> aPonLineList = myStorage->getConstraintsByType(SLVS_C_PT_ON_LINE);
      std::list<Slvs_Constraint>::const_iterator aPLIter = aPonLineList.begin();
      for (; aPLIter != aPonLineList.end() && !isPonLineFixed; aPLIter++)
        if (aPLIter->entityA == theLine.h) {
          isPonLineFixed = myStorage->isPointFixed(aPLIter->ptA, anEndFixedID);
          aFixedPoint = aPLIter->ptA;
        }

      if (isPonLineFixed) { // update existent constraint
        myStorage->copyEntity(aPointToCopy, aFixedPoint);
      } else { // create new constraint
        Slvs_hEntity aCopied = myStorage->copyEntity(aPointToCopy);
        Slvs_Constraint aPonLine = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_PT_ON_LINE,
            myGroup->getWorkplaneId(), 0.0, aCopied, SLVS_E_UNKNOWN, theLine.h, SLVS_E_UNKNOWN);
        aPonLine.h = myStorage->addConstraint(aPonLine);
        mySlvsConstraints.push_back(aPonLine.h);
        fixPoint(aCopied);
      }
      return;
    }
  }

  for (int i = 0; i < 2; i++)
    fixPoint(theLine.point[i]);
}

void SketchSolver_ConstraintRigid::fixCircle(const Slvs_Entity& theCircle)
{
  bool isFixRadius = true;
  // Verify the arc is under Equal constraint
  Slvs_Constraint anEqual;
  if (myStorage->isUsedInEqual(theCircle.h, anEqual)) {
    // Check another entity of Equal is already fixed
    Slvs_hEntity anOtherEntID = anEqual.entityA == theCircle.h ? anEqual.entityB : anEqual.entityA;
    if (myStorage->isEntityFixed(anOtherEntID, true))
      isFixRadius = false;
  }

  fixPoint(theCircle.point[0]);

  if (isFixRadius) {
    // Search the radius is already fixed
    std::list<Slvs_Constraint> aDiamConstr = myStorage->getConstraintsByType(SLVS_C_DIAMETER);
    std::list<Slvs_Constraint>::const_iterator aDiamIter = aDiamConstr.begin();
    for (; aDiamIter != aDiamConstr.end(); aDiamIter++)
      if (aDiamIter->entityA == theCircle.h)
        return;

    // Fix radius of a circle
    AttributeDoublePtr aRadiusAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        myFeatureMap.begin()->first->attribute(SketchPlugin_Circle::RADIUS_ID()));
    Slvs_Constraint aFixedR = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_DIAMETER,
        myGroup->getWorkplaneId(), aRadiusAttr->value() * 2.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN,
        myFeatureMap.begin()->second, SLVS_E_UNKNOWN);
    aFixedR.h = myStorage->addConstraint(aFixedR);
    mySlvsConstraints.push_back(aFixedR.h);
  }
}

void SketchSolver_ConstraintRigid::fixArc(const Slvs_Entity& theArc)
{
  bool isFixRadius = true;
  std::list<Slvs_hEntity> aPointsToFix;
  aPointsToFix.push_back(theArc.point[1]);
  aPointsToFix.push_back(theArc.point[2]);

  // Verify the arc is under Equal constraint
  Slvs_Constraint anEqual;
  if (myStorage->isUsedInEqual(theArc.h, anEqual)) {
    // Check another entity of Equal is already fixed
    Slvs_hEntity anOtherEntID = anEqual.entityA == theArc.h ? anEqual.entityB : anEqual.entityA;
    if (myStorage->isEntityFixed(anOtherEntID, true)) {
      isFixRadius = false;
      Slvs_Entity anOtherEntity = myStorage->getEntity(anOtherEntID);
      if (anOtherEntity.type == SLVS_E_LINE_SEGMENT) {
        aPointsToFix.pop_back();
        aPointsToFix.push_back(theArc.point[0]);
      }
    }
  }

  Slvs_hConstraint aConstrID;
  int aNbPointsToFix = 2; // number of fixed points for the arc
  if (myStorage->isPointFixed(theArc.point[0], aConstrID, true))
    aNbPointsToFix--;

  // Radius of the arc
  FeaturePtr aFeature = myFeatureMap.begin()->first;
  std::shared_ptr<GeomAPI_Pnt2d> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aFeature->attribute(SketchPlugin_Arc::START_ID()))->pnt();
  double aRadius = aCenter->distance(aStart);

  // Update end point of the arc to be on a curve
  std::shared_ptr<GeomAPI_Pnt2d> anEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aFeature->attribute(SketchPlugin_Arc::END_ID()))->pnt();
  double aDistance = anEnd->distance(aCenter);
  std::shared_ptr<GeomAPI_XY> aDir = anEnd->xy()->decreased(aCenter->xy());
  if (aDistance < tolerance)
    aDir = aStart->xy()->decreased(aCenter->xy())->multiplied(-1.0);
  else
    aDir = aDir->multiplied(aRadius / aDistance);
  double xy[2] = {aCenter->x() + aDir->x(), aCenter->y() + aDir->y()};
  Slvs_Entity aEndPoint = myStorage->getEntity(theArc.point[2]);
  for (int i = 0; i < 2; i++) {
    Slvs_Param aParam = myStorage->getParameter(aEndPoint.param[i]);
    aParam.val = xy[i];
    myStorage->updateParameter(aParam);
  }

  std::list<Slvs_hEntity>::iterator aPtIt = aPointsToFix.begin();
  for (; aNbPointsToFix > 0; aPtIt++, aNbPointsToFix--)
    fixPoint(*aPtIt);

  if (isFixRadius) {
    // Fix radius of the arc
    bool isExists = false;
    std::list<Slvs_Constraint> aDiamConstraints = myStorage->getConstraintsByType(SLVS_C_DIAMETER);
    std::list<Slvs_Constraint>::iterator anIt = aDiamConstraints.begin();
    for (; anIt != aDiamConstraints.end() && !isExists; anIt++)
      if (anIt->entityA == theArc.h)
        isExists = true;
    if (!isExists) {
      Slvs_Constraint aFixedR = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_DIAMETER,
          myGroup->getWorkplaneId(), aRadius * 2.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN,
          myFeatureMap.begin()->second, SLVS_E_UNKNOWN);
      aFixedR.h = myStorage->addConstraint(aFixedR);
      mySlvsConstraints.push_back(aFixedR.h);
      if (!myBaseConstraint)
        myStorage->addConstraintWhereDragged(aFixedR.h);
    }
  }
}
