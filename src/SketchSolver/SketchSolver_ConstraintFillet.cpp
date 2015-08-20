#include <SketchSolver_ConstraintFillet.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>


void SketchSolver_ConstraintFillet::getAttributes(
    double& theValue, std::vector<Slvs_hEntity>& theAttributes)
{
  theAttributes.clear();

  DataPtr aData = myBaseConstraint->data();
  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    aData->attribute(SketchPlugin_Constraint::VALUE()));
  theValue = aValueAttr ? aValueAttr->value() : 0.0;

  std::list<AttributePtr> aBaseAttrs = aData->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anIter = aBaseAttrs.begin();
  for (; anIter != aBaseAttrs.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (!aRefAttr || !aRefAttr->isInitialized()) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }

    int aType = SLVS_E_UNKNOWN; // type of created entity
    Slvs_hEntity anEntity = myGroup->getAttributeId(aRefAttr);
    if (anEntity == SLVS_E_UNKNOWN)
      anEntity = changeEntity(aRefAttr, aType);

    if (aType == SLVS_E_UNKNOWN)
      continue;
    theAttributes.push_back(anEntity);
  }

  // Fillet objects
  AttributeRefListPtr aFilletRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
    aData->attribute(SketchPlugin_Constraint::ENTITY_C()));
  if (!aFilletRefList || !aFilletRefList->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }
  std::list<ObjectPtr> aFilletList = aFilletRefList->list();
  if (aFilletList.size() < theAttributes.size() + 1) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }
  FeaturePtr aFilletFeature;
  ResultConstructionPtr aRC;
  int aType;
  std::list<ObjectPtr>::iterator aFilIter = aFilletList.begin();
  for (int indEnt = 0; aFilIter != aFilletList.end(); aFilIter++, indEnt++) {
    aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aFilIter);
    aFilletFeature = aRC ? aRC->document()->feature(aRC) :
        std::dynamic_pointer_cast<ModelAPI_Feature>(*aFilIter);
    if (!aFilletFeature) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }
    Slvs_hEntity anEntity = changeEntity(aFilletFeature, aType);
    theAttributes.push_back(anEntity);
  }
}

void SketchSolver_ConstraintFillet::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  double aValue;
  std::vector<Slvs_hEntity> anEntID;
  getAttributes(aValue, anEntID);
  if (!myErrorMsg.empty())
    return;

  // Obtain the entities itself:
  // First two are base entities
  // Second two are trimmed them
  // Last one is a fillet arc
  std::vector<Slvs_Entity> anEntities;
  std::vector<Slvs_hEntity>::iterator anIt = anEntID.begin();
  for (; anIt != anEntID.end(); anIt++)
    anEntities.push_back(myStorage->getEntity(*anIt));

  // Check the base entities have a coincident point
  Slvs_hEntity aPointsToFind[4];
  for (int i = 0; i < 2; i++) {
    int aShift = anEntities[i].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0;
    aPointsToFind[2*i]  = anEntities[i].point[aShift];
    aPointsToFind[2*i+1]= anEntities[i].point[aShift+1];
  }
  // Search coincident points
  int aCoincInd[2]; // indices of coincident points on each entity (0 - first, 1 - last)
  bool isPointFound = false;
  for (int i = 0; i < 2 && !isPointFound; i++)
    for (int j = 2; j < 4 && !isPointFound; j++)
      if (myStorage->isEqual(aPointsToFind[i], aPointsToFind[j])) {
        aCoincInd[0] = i;
        aCoincInd[1] = j - 2;
        isPointFound = true;
      }
  if (!isPointFound) {
    // There is no coincident points between tangential objects. Generate error message
    myErrorMsg = SketchSolver_Error::NO_COINCIDENT_POINTS();
    return;
  }

  // For correct result, move floating points of fillet on the middle points of base objects
  Slvs_Entity aPoint;
  Slvs_Param aParam;
  double anArcPoints[3][2];
  for (int i = 0; i < 2; i++) {
    calculateMiddlePoint(anEntities[i], 0.1 + 0.8 * aCoincInd[i], anArcPoints[i+1][0], anArcPoints[i+1][1]);
    aPoint = myStorage->getEntity(anEntities[i+2].point[aCoincInd[i]]);
    for (int j = 0; j < 2; j++) {
      aParam = myStorage->getParameter(aPoint.param[j]);
      aParam.val = anArcPoints[i+1][j];
      aPoint.param[j] = myStorage->updateParameter(aParam);
    }
  }
  anArcPoints[0][0] = 0.5 * (anArcPoints[1][0] + anArcPoints[2][0]);
  anArcPoints[0][1] = 0.5 * (anArcPoints[1][1] + anArcPoints[2][1]);
  // Check the arc is need to be reversed
  double aSharedPoint[2];
  aPoint = myStorage->getEntity(aPointsToFind[aCoincInd[0]]);
  for (int j = 0; j < 2; j++)
    aSharedPoint[j] = myStorage->getParameter(aPoint.param[j]).val;
  double aCross = (anArcPoints[1][0] - anArcPoints[0][0]) * (aSharedPoint[1] - anArcPoints[0][1]) -
                  (anArcPoints[1][1] - anArcPoints[0][1]) * (aSharedPoint[0] - anArcPoints[0][0]);
  if (aCross < 0.0) { // reverse fillet arc
    double aTmp;
    for (int j = 0; j < 2; j++) {
      aTmp = anArcPoints[1][j];
      anArcPoints[1][j] = anArcPoints[2][j];
      anArcPoints[2][j] = aTmp;
    }
  }
  // update fillet arc coordinates
  for (int indArcPt = 0; indArcPt < 3; indArcPt++) {
    aPoint = myStorage->getEntity(anEntities.back().point[indArcPt]);
    for (int j = 0; j < 2; j++) {
      aParam = myStorage->getParameter(aPoint.param[j]);
      aParam.val = anArcPoints[indArcPt][j];
      aPoint.param[j] = myStorage->updateParameter(aParam);
    }
  }

  for (int indEnt = 0; indEnt < 2; indEnt++) {
    int aShift = anEntities[indEnt].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0;
    // one point of fillet object should be coincident with the point on base, non-coincident with another base object
    Slvs_hEntity aBaseID = anEntities[indEnt].point[1 - aCoincInd[indEnt] + aShift];
    Slvs_hEntity aFilletID = anEntities[2 + indEnt].point[1 - aCoincInd[indEnt] + aShift];
    Slvs_Constraint aCoincConstr = Slvs_MakeConstraint(
        SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_POINTS_COINCIDENT, myGroup->getWorkplaneId(),
        0.0, aBaseID, aFilletID, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aCoincConstr.h = myStorage->addConstraint(aCoincConstr);
    mySlvsConstraints.push_back(aCoincConstr.h);

    // another point of fillet object should be placed on the base object
    Slvs_Constraint aPonCurveConstr;
    if (anEntities[indEnt].type == SLVS_E_ARC_OF_CIRCLE) {
      // centers of arcs should be coincident
      aBaseID = anEntities[indEnt].point[0];
      aFilletID = anEntities[2 + indEnt].point[0];
      aPonCurveConstr = Slvs_MakeConstraint(
          SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_POINTS_COINCIDENT, myGroup->getWorkplaneId(),
          0.0, aBaseID, aFilletID, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    } else {
      aFilletID = anEntities[2 + indEnt].point[aCoincInd[indEnt]];
      aPonCurveConstr = Slvs_MakeConstraint(
          SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_PT_ON_LINE, myGroup->getWorkplaneId(),
          0.0, aFilletID, SLVS_E_UNKNOWN, anEntities[indEnt].h, SLVS_E_UNKNOWN);
    }
    aPonCurveConstr.h = myStorage->addConstraint(aPonCurveConstr);
    mySlvsConstraints.push_back(aPonCurveConstr.h);
  }
}

bool SketchSolver_ConstraintFillet::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  bool isFullyRemoved = true;
  std::vector<Slvs_hEntity>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++)
   isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;
  mySlvsConstraints.clear();

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();
  return true;
}

