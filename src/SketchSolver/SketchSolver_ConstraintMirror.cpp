#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

////#include <ModelAPI_AttributeDouble.h>
////#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
////#include <ModelAPI_ResultConstruction.h>
////
////#include <GeomAPI_Dir2d.h>
////#include <GeomAPI_XY.h>
////
////#include <cmath>

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
  myStorage->update(aMirLineAttr, myGroupID);
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

////      anEntity = changeEntity(aFeature, aType);
////      // Sort entities by their type
////      std::vector<Slvs_Entity>::iterator anIt = aList->begin();
////      for (; anIt != aList->end(); anIt++)
////        if (aType < anIt->type)
////          break;
//////      aList->push_back(myStorage->getEntity(anEntity));
////      aList->insert(anIt, myStorage->getEntity(anEntity));
      myStorage->update(aFeature, myGroupID);
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
////  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
////    update(myBaseConstraint);

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
  SketchSolver_Constraint::update();
}

////bool SketchSolver_ConstraintMirror::remove(ConstraintPtr theConstraint)
////{
////  cleanErrorMsg();
////  if (theConstraint && theConstraint != myBaseConstraint)
////    return false;
////  bool isFullyRemoved = true;
////  std::vector<Slvs_hEntity>::iterator aCIter = mySlvsConstraints.begin();
////  for (; aCIter != mySlvsConstraints.end(); aCIter++)
////   isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;
////  mySlvsConstraints.clear();
////
////  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.begin();
////  for (; aFeatIt != myFeatureMap.end(); aFeatIt++)
////    myStorage->removeEntity(aFeatIt->second);
////
////  if (isFullyRemoved) {
////    myFeatureMap.clear();
////    myAttributeMap.clear();
////    myValueMap.clear();
////  } else
////    cleanRemovedEntities();
////  return true;
////}
////
////bool SketchSolver_ConstraintMirror::checkAttributesChanged(ConstraintPtr theConstraint)
////{
////  // First of all, check the mirror line is changed.
////  // It may be changed to one of mirrored lines, which is already in this constraint
////  // (this case is not marked as attribute changing)
////  ConstraintPtr aConstraint = theConstraint ? theConstraint : myBaseConstraint;
////  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
////  if (!aRefAttr || !aRefAttr->isObject() || !aRefAttr->object())
////    return true;
////  FeaturePtr aMirrorLine = ModelAPI_Feature::feature(aRefAttr->object());
////  if (!aMirrorLine)
////    return true;
////
////  std::map<FeaturePtr, Slvs_hEntity>::iterator aMirrorIter = myFeatureMap.find(aMirrorLine);
////  if (aMirrorIter == myFeatureMap.end())
////    return true;
////
////  // Check the entity is not used as mirror line
////  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
////  for (; aCIter != mySlvsConstraints.end(); aCIter++) {
////    Slvs_Constraint aMirrorConstr = myStorage->getConstraint(*aCIter);
////    if (aMirrorConstr.type != SLVS_C_SYMMETRIC_LINE)
////      continue;
////    if (aMirrorConstr.entityA != aMirrorIter->second)
////      return true;
////    else break; // check just one symmetric constraint
////  }
////
////  // Base verification
////  return SketchSolver_Constraint::checkAttributesChanged(theConstraint);
////}

////void SketchSolver_ConstraintMirror::makeMirrorEntity(
////    const Slvs_Entity& theBase,
////    const Slvs_Entity& theMirror,
////    const double theMirrorLine[]) const
////{
////  Slvs_hEntity aBasePoint[4];
////  Slvs_hEntity aMirrorPoint[4];
////  for (int i = 0; i < 4; i++) {
////    aBasePoint[i] = theBase.point[i];
////    aMirrorPoint[i] = theMirror.point[i];
////  }
////  if (theBase.type == SLVS_E_ARC_OF_CIRCLE) {
////    Slvs_hEntity aTmp = aMirrorPoint[2];
////    aMirrorPoint[2] = aMirrorPoint[1];
////    aMirrorPoint[1] = aTmp;
////    adjustArcPoints(theBase);
////  }
////  if (theBase.type == SLVS_E_POINT_IN_2D || theBase.type == SLVS_E_POINT_IN_3D) {
////    aBasePoint[0] = theBase.h;
////    aMirrorPoint[0] = theMirror.h;
////  }
////
////  // Mirror line parameters
////  std::shared_ptr<GeomAPI_XY> aLinePoints[2];
////  for (int i = 0; i < 2; i++)
////    aLinePoints[i] = std::shared_ptr<GeomAPI_XY>(
////        new GeomAPI_XY(theMirrorLine[2*i], theMirrorLine[2*i+1]));
////  // direction of a mirror line
////  std::shared_ptr<GeomAPI_Dir2d> aDir = std::shared_ptr<GeomAPI_Dir2d>(
////    new GeomAPI_Dir2d(aLinePoints[1]->added(aLinePoints[0]->multiplied(-1.0))));
////  // orthogonal direction
////  aDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir->y(), -aDir->x()));
////
////  Slvs_hConstraint aFixed; // transient variable
////  for (int i = 0; i < 4; i++) {
////    if (aBasePoint[i] == SLVS_E_UNKNOWN || aMirrorPoint[i] == SLVS_E_UNKNOWN)
////      continue;
////    // check the mirror point is not fixed
////    if (myStorage->isPointFixed(aMirrorPoint[i], aFixed, true))
////      continue;
////
////    Slvs_Entity aPointEnt = myStorage->getEntity(aBasePoint[i]);
////    double aBaseX = myStorage->getParameter(aPointEnt.param[0]).val;
////    double aBaseY = myStorage->getParameter(aPointEnt.param[1]).val;
////    std::shared_ptr<GeomAPI_XY> aPoint = std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(aBaseX, aBaseY));
////
////    std::shared_ptr<GeomAPI_XY> aVec = std::shared_ptr<GeomAPI_XY>(
////        new GeomAPI_XY(aPoint->x() - aLinePoints[0]->x(), aPoint->y() - aLinePoints[0]->y()));
////    double aDist = aVec->dot(aDir->xy());
////    aPoint = aPoint->added(aDir->xy()->multiplied(-2.0 * aDist));
////
////    Slvs_Entity aMirrorEnt = myStorage->getEntity(aMirrorPoint[i]);
////    Slvs_Param aParam = Slvs_MakeParam(aMirrorEnt.param[0], myGroup->getId(), aPoint->x());
////    myStorage->updateParameter(aParam);
////    aParam = Slvs_MakeParam(aMirrorEnt.param[1], myGroup->getId(), aPoint->y());
////    myStorage->updateParameter(aParam);
////  }
////}
////
////void SketchSolver_ConstraintMirror::adjustArcPoints(const Slvs_Entity& theArc) const
////{
////  Slvs_Param aParam;
////  Slvs_Entity aPoint;
////  double anArcParams[3][2];
////  for (int i = 0; i < 3; i++) {
////    aPoint = myStorage->getEntity(theArc.point[i]);
////    for (int j = 0; j < 2; j++) {
////      aParam = myStorage->getParameter(aPoint.param[j]);
////      anArcParams[i][j] = aParam.val;
////      if (i > 0)
////        anArcParams[i][j] -= anArcParams[0][j];
////    }
////  }
////  double aRad2 = anArcParams[1][0] * anArcParams[1][0] + anArcParams[1][1] * anArcParams[1][1];
////  double aDist2 = anArcParams[2][0] * anArcParams[2][0] + anArcParams[2][1] * anArcParams[2][1];
////  if (std::fabs(aRad2 - aDist2) < tolerance)
////    return; // nothing to update (last point already on the arc)
////  if (aDist2 < tolerance)
////    return; // unable to update
////  double aCoeff = std::sqrt(aRad2 / aDist2);
////  anArcParams[2][0] *= aCoeff;
////  anArcParams[2][1] *= aCoeff;
////
////  // Update last point
////  aPoint = myStorage->getEntity(theArc.point[2]);
////  for (int i = 0; i < 2; i++) {
////    aParam = Slvs_MakeParam(aPoint.param[i], myGroup->getId(),
////        anArcParams[0][i] + anArcParams[2][i]);
////    myStorage->updateParameter(aParam);
////  }
////}

void SketchSolver_ConstraintMirror::adjustConstraint()
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  const std::list<ConstraintWrapperPtr>& aConstraints = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aConstraints.begin();
  for (; aCIt != aConstraints.end(); ++aCIt)
    if ((*aCIt)->type() == CONSTRAINT_SYMMETRIC)
      aBuilder->adjustConstraint(*aCIt);

////  AttributeRefAttrPtr aMirLineAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
////  if (!aMirLineAttr || !aMirLineAttr->isInitialized() || !aMirLineAttr->isObject()) {
////    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
////    return;
////  }
////  ResultConstructionPtr aRC =
////      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aMirLineAttr->object());
////  FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
////    std::dynamic_pointer_cast<ModelAPI_Feature>(aMirLineAttr->object());
////  std::map<FeaturePtr, Slvs_hEntity>::iterator aMirLineIter = myFeatureMap.find(aFeature);
////  if (aMirLineIter == myFeatureMap.end())
////    return;
////  Slvs_Entity aMirrorLine = myStorage->getEntity(aMirLineIter->second);
////
////  Slvs_Constraint aMirror;
////  double aStartEnd[4];
////  for (int i = 0; i < 2; i++) {
////    Slvs_Entity aPoint = myStorage->getEntity(aMirrorLine.point[i]);
////    for (int j = 0; j < 2; j++)
////      aStartEnd[2*i+j] = myStorage->getParameter(aPoint.param[j]).val;
////  }
////
////  // Calculate length of the mirror line and create temporary constraint
////  double aLength = sqrt((aStartEnd[2] - aStartEnd[0]) * (aStartEnd[2] - aStartEnd[0]) +
////                        (aStartEnd[3] - aStartEnd[1]) * (aStartEnd[3] - aStartEnd[1]));
////  if (aLength < tolerance) {
////    if (myMirrorLineLength < 1.0)
////      myMirrorLineLength = 1.0;
////  } else
////    myMirrorLineLength = aLength;
////  std::list<Slvs_Constraint> aDist = myStorage->getConstraintsByType(SLVS_C_PT_PT_DISTANCE);
////  std::list<Slvs_Constraint>::const_iterator aDIt = aDist.begin();
////  for (; aDIt != aDist.end(); ++aDIt)
////    if ((aDIt->ptA == aMirrorLine.point[0] && aDIt->ptB == aMirrorLine.point[1]) ||
////        (aDIt->ptA == aMirrorLine.point[1] && aDIt->ptB == aMirrorLine.point[0]))
////      break; // length of mirror line is already set
////  if (aDIt == aDist.end()) {
////    // check the points of mirror line is not fixed
////    Slvs_hConstraint aFixed;
////    if (!myStorage->isPointFixed(aMirrorLine.point[0], aFixed, true) ||
////        !myStorage->isPointFixed(aMirrorLine.point[1], aFixed, true)) {
////      // Add length constraint
////      aMirror = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_PT_PT_DISTANCE,
////          myGroup->getWorkplaneId(), aLength, aMirrorLine.point[0], aMirrorLine.point[1],
////          SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////      aMirror.h = myStorage->addConstraint(aMirror);
////      myStorage->addTemporaryConstraint(aMirror.h);
////    }
////  }
////
////  // Search mirror between middle points on the arcs and recompute their coordinates
////  std::map<Slvs_hEntity, Slvs_hEntity> aPointsOnCircles;
////  std::list<Slvs_Constraint> aMirrorPonCirc;
////  std::list<Slvs_Constraint> aPonCirc = myStorage->getConstraintsByType(SLVS_C_PT_ON_CIRCLE);
////  std::vector<Slvs_hConstraint>::iterator aConstrIter = mySlvsConstraints.begin();
////  for (; aConstrIter != mySlvsConstraints.end(); aConstrIter++) {
////    aMirror = myStorage->getConstraint(*aConstrIter);
////    if (aMirror.type != SLVS_C_SYMMETRIC_LINE)
////      continue;
////    if (aMirror.entityA != aMirrorLine.h)
////      continue; // don't update another Mirror constraints
////    Slvs_Constraint aPonCircA, aPonCircB;
////    aPonCircA.h = SLVS_E_UNKNOWN;
////    aPonCircB.h = SLVS_E_UNKNOWN;
////    std::list<Slvs_Constraint>::iterator aPtIter = aPonCirc.begin();
////    for (; aPtIter != aPonCirc.end(); aPtIter++) {
////      if (aMirror.ptA == aPtIter->ptA)
////        aPonCircA = *aPtIter;
////      if (aMirror.ptB == aPtIter->ptA)
////        aPonCircB = *aPtIter;
////    }
////    if (aPonCircA.h == SLVS_E_UNKNOWN || aPonCircB.h == SLVS_E_UNKNOWN)
////      continue;
////    aMirrorPonCirc.push_back(aMirror);
////    // Store point IDs to avoid their recalculation twice
////    aPointsOnCircles[aPonCircA.ptA] = aPonCircA.entityA;
////    aPointsOnCircles[aPonCircB.ptA] = aPonCircB.entityA;
////  }
////
////  // Recalculate positions of mirroring points
////  std::list<Slvs_Constraint> aMirrorList = myStorage->getConstraintsByType(SLVS_C_SYMMETRIC_LINE);
////  std::list<Slvs_Constraint>::iterator aMirIter = aMirrorList.begin();
////  for (; aMirIter != aMirrorList.end(); aMirIter++) {
////    if (aMirIter->entityA != aMirrorLine.h)
////      continue; // don't update another Mirror constraints
////    if (aPointsOnCircles.find(aMirIter->ptA) != aPointsOnCircles.end())
////      continue; // Avoid mirroring points on circles
////    Slvs_Entity aBase = myStorage->getEntity(aMirIter->ptA);
////    Slvs_Entity aMirror = myStorage->getEntity(aMirIter->ptB);
////    makeMirrorEntity(aBase, aMirror, aStartEnd);
////  }
////
////  bool aNeedToResolve = myStorage->isNeedToResolve();
////  for (aMirIter = aMirrorPonCirc.begin(); aMirIter != aMirrorPonCirc.end(); aMirIter++) {
////    // Make centers of arcs symmetric
////    Slvs_Entity aBaseArc = myStorage->getEntity(aPointsOnCircles[aMirIter->ptA]);
////    Slvs_Entity aBasePoint = myStorage->getEntity(aBaseArc.point[0]);
////    Slvs_Entity aMirrorArc = myStorage->getEntity(aPointsOnCircles[aMirIter->ptB]);
////    Slvs_Entity aMirrorPoint = myStorage->getEntity(aMirrorArc.point[0]);
////    makeMirrorEntity(aBasePoint, aMirrorPoint, aStartEnd);
////    // Calculate middle point for base arc and mirrored point on mirror arc
////    aBasePoint = myStorage->getEntity(aMirIter->ptA);
////    Slvs_Param aParamX = myStorage->getParameter(aBasePoint.param[0]);
////    Slvs_Param aParamY = myStorage->getParameter(aBasePoint.param[1]);
////    calculateMiddlePoint(aBaseArc, 0.5, aParamX.val, aParamY.val);
////    myStorage->updateParameter(aParamX);
////    myStorage->updateParameter(aParamY);
////    aMirrorPoint = myStorage->getEntity(aMirIter->ptB);
////    aParamX = myStorage->getParameter(aMirrorPoint.param[0]);
////    aParamY = myStorage->getParameter(aMirrorPoint.param[1]);
////    calculateMiddlePoint(aMirrorArc, 0.5, aParamX.val, aParamY.val);
////    myStorage->updateParameter(aParamX);
////    myStorage->updateParameter(aParamY);
////  }
////  // Restore previous value to avoid looped recalculations of sketch
////  myStorage->setNeedToResolve(aNeedToResolve);
}
