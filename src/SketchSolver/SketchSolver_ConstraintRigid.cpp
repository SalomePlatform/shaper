#include <SketchSolver_ConstraintRigid.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Group.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintRigid.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>

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
  if (!myErrorMsg.empty())
    return;

  Slvs_Constraint aConstraint;
  std::vector<Slvs_hConstraint>::iterator aConstrIter = mySlvsConstraints.begin();
  bool isEmpty = aConstrIter == mySlvsConstraints.end();

  // Check the fixed entity is an arc
  if (isEmpty) {
    if (!myFeatureMap.empty() && myFeatureMap.begin()->first->getKind() == SketchPlugin_Arc::ID()) {
      Slvs_Entity anArc = myStorage->getEntity(myFeatureMap.begin()->second);
      fixArc(anArc);
      return;
    }
  }

  std::vector<Slvs_hEntity>::const_iterator anEntIter = anEntities.begin();
  for (; anEntIter != anEntities.end(); anEntIter++) {
    if (*anEntIter == SLVS_E_UNKNOWN)
      continue;
    Slvs_hConstraint aConstrID = myStorage->isPointFixed(*anEntIter);
    bool isForceUpdate = (aConstrID != SLVS_E_UNKNOWN && !myBaseConstraint &&
                          myStorage->isTemporary(aConstrID));
    if (isEmpty && !isForceUpdate) { // create new constraint
      if (aConstrID != SLVS_E_UNKNOWN)
        continue; // the coincident point is already fixed
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(),
          aValue, *anEntIter, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
      if (!myBaseConstraint)
        myStorage->addConstraintWhereDragged(aConstraint.h);
    } else { // update already existent constraint
      if (aConstrID == SLVS_E_UNKNOWN || myBaseConstraint)
        aConstrID = *aConstrIter;
      aConstraint = myStorage->getConstraint(aConstrID);
      aConstraint.ptA = *anEntIter;
      myStorage->addConstraint(aConstraint);
      if (!myBaseConstraint)
        myStorage->addConstraintWhereDragged(aConstraint.h);
      if (!isEmpty) {
        aConstrIter++;
        isEmpty = aConstrIter == mySlvsConstraints.end();
      }
    }
  }

  if (!myFeatureMap.empty() && myFeatureMap.begin()->first->getKind() == SketchPlugin_Circle::ID()) {
    // Fix radius of a circle
    AttributeDoublePtr aRadiusAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        myFeatureMap.begin()->first->attribute(SketchPlugin_Circle::RADIUS_ID()));
    aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_DIAMETER, myGroup->getWorkplaneId(),
      aRadiusAttr->value() * 2.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, myFeatureMap.begin()->second, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
    if (!myBaseConstraint)
      myStorage->addConstraintWhereDragged(aConstraint.h);
  }
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

void SketchSolver_ConstraintRigid::adjustConstraint()
{
  if (myFeatureMap.empty() || (
      myFeatureMap.begin()->first->getKind() != SketchPlugin_Arc::ID() && 
      myFeatureMap.begin()->first->getKind() != SketchPlugin_Circle::ID()))
    return;
  FeaturePtr aFeature = myFeatureMap.begin()->first;

  // Search radius constraints and update them
  Slvs_Constraint aConstraint;
  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++) {
    aConstraint = myStorage->getConstraint(*aCIter);
    if (aConstraint.type != SLVS_C_DIAMETER)
      continue;
    double aRadius = 0.0;
    if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      std::shared_ptr<GeomAPI_Pnt2d> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
      std::shared_ptr<GeomAPI_Pnt2d> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->attribute(SketchPlugin_Arc::START_ID()))->pnt();
      aRadius = aCenter->distance(aStart);
    } else {
      aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
          aFeature->attribute(SketchPlugin_Circle::RADIUS_ID()))->value();
    }

    aConstraint.valA = aRadius * 2.0;
    *aCIter = myStorage->updateConstraint(aConstraint);
  }
}


bool SketchSolver_ConstraintRigid::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  bool isFullyRemoved = true;
  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++)
    isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();
  return true;
}


void SketchSolver_ConstraintRigid::fixArc(const Slvs_Entity& theArc)
{
  Slvs_Constraint aConstraint;
  Slvs_hConstraint aConstrID = myStorage->isPointFixed(theArc.point[0]);
  int aPointsToFix = 2; // number of fixed points for the arc
  if (aConstrID != SLVS_E_UNKNOWN)
    aPointsToFix--;

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

  for (int i = 1; aPointsToFix > 0; i++, aPointsToFix--) {
    aConstrID = myStorage->isPointFixed(theArc.point[i]);
    if (aConstrID != SLVS_E_UNKNOWN)
      continue; // the coincident point is already fixed
    aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(),
        0.0, theArc.point[i], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
    if (!myBaseConstraint)
      myStorage->addConstraintWhereDragged(aConstraint.h);
  }

  // Fix radius of the arc
  bool isExists = false;
  std::list<Slvs_Constraint> aDiamConstraints = myStorage->getConstraintsByType(SLVS_C_DIAMETER);
  std::list<Slvs_Constraint>::iterator anIt = aDiamConstraints.begin();
  for (; anIt != aDiamConstraints.end() && !isExists; anIt)
    if (anIt->entityA == myFeatureMap.begin()->second)
      isExists = true;
  if (!isExists) {
    aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_DIAMETER, myGroup->getWorkplaneId(),
      aRadius * 2.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, myFeatureMap.begin()->second, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
    if (!myBaseConstraint)
      myStorage->addConstraintWhereDragged(aConstraint.h);
  }
}

