#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Group.h>

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

  if (isComplexFeature) {
     std::list<AttributePtr> aPoints =
        myBaseFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
     std::list<AttributePtr>::iterator anIt = aPoints.begin();
     for (; anIt != aPoints.end(); ++anIt) {
       Slvs_hEntity anAttr = myGroup->getAttributeId(*anIt);

       // Check the attribute changes coordinates
       std::shared_ptr<GeomDataAPI_Point2D> aPt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
       if (isMoved(aPt, anAttr)) {
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
       else
         theIsFullyMoved = false;
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
    std::shared_ptr<GeomDataAPI_Point2D> thePoint, Slvs_hEntity theEntity)
{
  Slvs_Entity anAttrEnt = myStorage->getEntity(theEntity);
  double aDeltaX = myStorage->getParameter(anAttrEnt.param[0]).val;
  double aDeltaY = myStorage->getParameter(anAttrEnt.param[1]).val;
  aDeltaX -= thePoint->x();
  aDeltaY -= thePoint->y();
  return aDeltaX * aDeltaX + aDeltaY * aDeltaY >= tolerance * tolerance;
}
