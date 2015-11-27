#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Group.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(FeaturePtr theFeature)
  : SketchSolver_ConstraintRigid(theFeature)
{
  process();
}

void SketchSolver_ConstraintMovement::process()
{
  cleanErrorMsg();
  if (!myBaseFeature || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  double aValue;
  std::vector<Slvs_hEntity> anEntities;
  bool isFullyMoved;
  getAttributes(aValue, anEntities, isFullyMoved);
  if (!myErrorMsg.empty() || (myFeatureMap.empty() && myAttributeMap.empty()))
    return;

  if (isFullyMoved)
    fixFeature();
  else {
    std::vector<Slvs_hEntity>::iterator anEntIt = anEntities.begin();
    for (; anEntIt != anEntities.end(); ++anEntIt)
      fixPoint(*anEntIt);
  }
}


void SketchSolver_ConstraintMovement::getAttributes(
    double& theValue,
    std::vector<Slvs_hEntity>& theAttributes,
    bool& theIsFullyMoved)
{
  bool isComplexFeature = false;
  theValue = 0.0;
  theIsFullyMoved = true;
  int aType = SLVS_E_UNKNOWN; // type of created entity
  Slvs_hEntity anEntityID = SLVS_E_UNKNOWN;
  Slvs_hEntity anEntMaxID = myStorage->entityMaxID();
  anEntityID = myGroup->getFeatureId(myBaseFeature);
  if (anEntityID == SLVS_E_UNKNOWN) {
    anEntityID = changeEntity(myBaseFeature, aType);
    if (anEntityID == SLVS_E_UNKNOWN) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }

    // Check the entity is complex
    Slvs_Entity anEntity = myStorage->getEntity(anEntityID);
    if (anEntity.point[0] != SLVS_E_UNKNOWN)
      isComplexFeature = true;
    else // simple entity
      theAttributes.push_back(anEntityID);
  }
  else {
     myFeatureMap[myBaseFeature] = anEntityID;
     isComplexFeature = true;
  }

  int aNbOutOfGroup = 0;
  if (isComplexFeature) {
     std::list<AttributePtr> aPoints =
        myBaseFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
     std::list<AttributePtr>::iterator anIt = aPoints.begin();
     for (; anIt != aPoints.end(); ++anIt) {
       std::map<AttributePtr, Slvs_hEntity>::const_iterator aFound = myAttributeMap.find(*anIt);
       Slvs_hEntity anAttr = aFound != myAttributeMap.end() ?
                             aFound->second : myGroup->getAttributeId(*anIt);
       Slvs_Entity anAttrEnt = myStorage->getEntity(anAttr);

       // Check the attribute changes coordinates
       std::shared_ptr<GeomDataAPI_Point2D> aPt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
       // Check the entity is not lying in the current group or it is not moved
       if (anAttr == SLVS_E_UNKNOWN || anAttrEnt.group != myGroup->getId() ||
           (anAttr <= anEntMaxID && !isMoved(aPt, anAttrEnt))) {
         if (anAttrEnt.group == SLVS_G_OUTOFGROUP)
           ++aNbOutOfGroup;
         theIsFullyMoved = false;
       }
       else {
         theAttributes.push_back(anAttr);
         // update point coordinates
         Slvs_Entity anAttrEnt = myStorage->getEntity(anAttr);
         double aNewPos[2] = {aPt->x(), aPt->y()};
         for (int i = 0; i < 2; i++) {
           Slvs_Param aParam = myStorage->getParameter(anAttrEnt.param[i]);
           aParam.val = aNewPos[i];
           myStorage->updateParameter(aParam);
         }
       }
     }
  }

  // Additional checking, which leads to fix whole feature, if it has fixed points
  if (!theIsFullyMoved) {
    Slvs_Entity aFeature = myStorage->getEntity(anEntityID);
    int aNbPoints = 4;
    while (aNbPoints > 0 && aFeature.point[aNbPoints-1] == SLVS_E_UNKNOWN)
      --aNbPoints;
    if (aNbPoints == aNbOutOfGroup + (int)theAttributes.size()) {
      theIsFullyMoved = true;
      return;
    }
  }

  // Leave only points which are used in constraints
  if (myStorage->isUsedByConstraints(anEntityID))
    return;
  std::vector<Slvs_hEntity>::iterator anIt = theAttributes.begin();
  while (anIt != theAttributes.end()) {
    if (myStorage->isUsedByConstraints(*anIt))
      ++anIt;
    else {
      int aShift = anIt - theAttributes.begin();
      theAttributes.erase(anIt);
      anIt = theAttributes.begin() + aShift;
    }
  }
}

bool SketchSolver_ConstraintMovement::isMoved(
    std::shared_ptr<GeomDataAPI_Point2D> thePoint, const Slvs_Entity& theEntity)
{
  double aDeltaX = myStorage->getParameter(theEntity.param[0]).val;
  double aDeltaY = myStorage->getParameter(theEntity.param[1]).val;
  aDeltaX -= thePoint->x();
  aDeltaY -= thePoint->y();
  return aDeltaX * aDeltaX + aDeltaY * aDeltaY >= tolerance * tolerance;
}

void SketchSolver_ConstraintMovement::fixFeature()
{
  Slvs_hEntity anEntID = fixedEntity();

  std::string aKind;
  std::map<FeaturePtr, Slvs_hEntity>::const_iterator aFIt = myFeatureMap.begin();
  for (; aFIt != myFeatureMap.end() && aKind.empty(); ++aFIt)
    if (aFIt->second == anEntID)
      aKind = aFIt->first->getKind();
  std::map<AttributePtr, Slvs_hEntity>::const_iterator anAtIt = myAttributeMap.begin();
  for (; anAtIt != myAttributeMap.end() && aKind.empty(); ++anAtIt)
    if (anAtIt->second == anEntID)
      aKind = anAtIt->first->attributeType();

  if (aKind == SketchPlugin_Line::ID()) {
    Slvs_Entity aLine = myStorage->getEntity(anEntID);
    fixLine(aLine);
  }
  else if (aKind == SketchPlugin_Arc::ID()) {
    Slvs_Entity anArc = myStorage->getEntity(anEntID);
    fixArc(anArc);
  }
  else if (aKind == SketchPlugin_Circle::ID()) {
    Slvs_Entity aCirc = myStorage->getEntity(anEntID);
    fixCircle(aCirc);
  }
  else if (aKind == SketchPlugin_Point::ID() || aKind == GeomDataAPI_Point2D::typeId()) {
    fixPoint(anEntID);
  }
}

