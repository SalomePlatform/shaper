#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

////#include <SketchPlugin_Point.h>
////#include <GeomDataAPI_Point2D.h>
////
////#include <map>

void SketchSolver_ConstraintCoincidence::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1]) {
    myType = CONSTRAINT_PT_PT_COINCIDENT;
    // Set the slave (second) point the same as master (first) point.
    // This will allow to skip adding point-point coincidence to the set of constraints
    // and give us speed-up in solving the set of equations
    myStorage->addCoincidentPoints(theAttributes[0], theAttributes[1]);
  }
  else if (theAttributes[2]) {
    // check the type of entity (line or circle)
    SketchSolver_EntityType anEntType = theAttributes[2]->type();
    if (anEntType == ENTITY_LINE)
      myType = CONSTRAINT_PT_ON_LINE;
    else if (anEntType == ENTITY_CIRCLE || anEntType == ENTITY_ARC)
      myType = CONSTRAINT_PT_ON_CIRCLE;
    else
      myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
  } else
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
}


static bool isBase(const std::list<ConstraintWrapperPtr>& theConstraints, AttributePtr theAttribute)
{
  AttributePtr anAttribute = theAttribute;
  FeaturePtr aFeature;
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject())
      aFeature = ModelAPI_Feature::feature(aRefAttr->object());
    else
      anAttribute = aRefAttr->attr();
  }

  std::list<ConstraintWrapperPtr>::const_iterator aCIt = theConstraints.begin();
  for (; aCIt != theConstraints.end(); ++aCIt) {
    std::list<EntityWrapperPtr> aSubs = (*aCIt)->entities();
    std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
    for (; aSIt != aSubs.end(); ++aSIt)
      if ((aFeature && (*aSIt)->isBase(aFeature)) ||
         (!aFeature && (*aSIt)->isBase(anAttribute)))
        return true;
  }
  return false;
}

////bool SketchSolver_ConstraintCoincidence::isCoincide(
////    std::shared_ptr<SketchSolver_ConstraintCoincidence> theConstraint) const
////{
////  std::list<ConstraintPtr>::const_iterator anOtherCIt = theConstraint->myConstraints.begin();
////  for (; anOtherCIt != theConstraint->myConstraints.end(); ++anOtherCIt) {
////    std::list<ConstraintPtr>::const_iterator aCIt = myConstraints.begin();
////    for (; aCIt != myConstraints.end(); ++aCIt) {
////      std::list<ConstraintWrapperPtr> aWrappers = myStorage->constraint(*aCIt);
////      if (isBase(aWrappers, (*anOtherCIt)->attribute(SketchPlugin_Constraint::ENTITY_A())) ||
////         (isBase(aWrappers, (*anOtherCIt)->attribute(SketchPlugin_Constraint::ENTITY_B()))))
////        return true;
////    }
////  }
////  return false;
////}
////
////void SketchSolver_ConstraintCoincidence::attach(
////    std::shared_ptr<SketchSolver_ConstraintCoincidence> theConstraint)
////{
////  cleanErrorMsg();
////  ConstraintWrapperPtr aFirstConstraint = myStorage->constraint(myConstraints.front()).front();
////  EntityWrapperPtr aMasterPoint = aFirstConstraint->entities().front();
////
////  // Set points from theConstraint equal to master point
////  const std::list<ConstraintPtr>& anOtherConstraints = theConstraint->myConstraints;
////  std::list<ConstraintPtr>::const_iterator anOtherCIt = anOtherConstraints.begin();
////  bool isFirstUpd = false;
////  for (; anOtherCIt != anOtherConstraints.end(); ++anOtherCIt) {
////    std::list<ConstraintWrapperPtr> aSolverConstraints = myStorage->constraint(*anOtherCIt);
////    std::list<ConstraintWrapperPtr>::iterator aSCIt = aSolverConstraints.begin();
////    // First point in each constraint is the same (master point of theConstraint),
////    // it should be updated only once.
////    if (!isFirstUpd) {
////      EntityWrapperPtr anOtherMasterPoint = (*aSCIt)->entities().front();
////      myStorage->addCoincidentPoints(aMasterPoint, anOtherMasterPoint);
////      isFirstUpd = true;
////    }
////    // Update all slave points
////    for (; aSCIt != aSolverConstraints.end(); ++aSCIt) {
////      std::list<EntityWrapperPtr> aSubs = (*aSCIt)->entities();
////      std::list<EntityWrapperPtr>::iterator aSubIt = aSubs.begin();
////      for (++aSubIt; aSubIt != aSubs.end(); ++aSubIt)
////        myStorage->addCoincidentPoints(aMasterPoint, *aSubIt);
////    }
////    // add constraint to myConstraints
////    myConstraints.push_back(*anOtherCIt);
////  }
////
////  // Remove constraints from theConstraint
////  std::vector<Slvs_hConstraint>::iterator aCIter = theConstraint->mySlvsConstraints.begin();
////  for (; aCIter != theConstraint->mySlvsConstraints.end(); aCIter++)
////    theConstraint->myStorage->removeConstraint(*aCIter);
////
////  if (myStorage == theConstraint->myStorage) {
////    // Clean removed items
////    std::set<Slvs_hParam> aRemParams;
////    std::set<Slvs_hEntity> aRemEnts;
////    std::set<Slvs_hConstraint> aRemConstr;
////    theConstraint->myStorage->getRemoved(aRemParams, aRemEnts, aRemConstr);
////
////    if (!aRemEnts.empty()) {
////      std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = theConstraint->myFeatureMap.begin();
////      while (aFeatIt != theConstraint->myFeatureMap.end()) {
////        if (aRemEnts.find(aFeatIt->second) != aRemEnts.end()) {
////          // remove feature
////          std::map<FeaturePtr, Slvs_hEntity>::iterator aRemoveIt = aFeatIt++;
////          theConstraint->myFeatureMap.erase(aRemoveIt);
////        } else
////          ++aFeatIt;
////      }
////      std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIt = theConstraint->myAttributeMap.begin();
////      while (anAttrIt != theConstraint->myAttributeMap.end()) {
////        if (aRemEnts.find(anAttrIt->second) != aRemEnts.end()) {
////          // remove attribute
////          std::map<AttributePtr, Slvs_hEntity>::iterator aRemoveIt = anAttrIt++;
////          theConstraint->myAttributeMap.erase(aRemoveIt);
////        } else
////          ++anAttrIt;
////      }
////    }
////  }
////
////  // Copy data.
////  addConstraint(theConstraint->myBaseConstraint);
////  std::map<ConstraintPtr, Slvs_hConstraint>::iterator aConstrIter =
////      theConstraint->myExtraCoincidence.begin();
////  for (; aConstrIter != theConstraint->myExtraCoincidence.end(); aConstrIter++)
////    addConstraint(aConstrIter->first);
////  // Clear the lists to not remove the entities on destruction
////  theConstraint->mySlvsConstraints.clear();
////  theConstraint->myFeatureMap.clear();
////  theConstraint->myAttributeMap.clear();
////}

////Slvs_hConstraint SketchSolver_ConstraintCoincidence::addConstraint(
////    Slvs_hEntity thePoint1, Slvs_hEntity thePoint2)
////{
////  if (thePoint1 == thePoint2)
////    return SLVS_E_UNKNOWN;
////
////  bool hasDuplicated = myStorage->hasDuplicatedConstraint();
////  Slvs_Constraint aNewConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
////      SLVS_C_POINTS_COINCIDENT, myGroup->getWorkplaneId(), 0.0, thePoint1, thePoint2, 
////      SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////  Slvs_hConstraint aNewID = myStorage->addConstraint(aNewConstraint);
////  if (!hasDuplicated && myStorage->hasDuplicatedConstraint()) {
////    // the duplicated constraint appears
////    myStorage->removeConstraint(aNewID);
////    return SLVS_E_UNKNOWN;
////  }
////  mySlvsConstraints.push_back(aNewID);
////  return aNewID;
////}
////
////Slvs_hConstraint SketchSolver_ConstraintCoincidence::addPointOnEntity(
////    Slvs_hEntity thePoint, Slvs_hEntity theEntity)
////{
////  // Check the point is not coincident with boundaries of the entity
////  Slvs_Entity anEnt = myStorage->getEntity(theEntity);
////  int aPos = anEnt.type == SLVS_E_LINE_SEGMENT ? 0 : 1;
////  for (; anEnt.point[aPos] != SLVS_E_UNKNOWN; aPos++)
////    if (anEnt.point[aPos] == thePoint ||
////        myStorage->isCoincident(anEnt.point[aPos], thePoint))
////      return SLVS_E_UNKNOWN;
////
////  bool hasDuplicated = myStorage->hasDuplicatedConstraint();
////  Slvs_Constraint aBaseCoincidence = myStorage->getConstraint(mySlvsConstraints.front());
////  Slvs_hConstraint aType = anEnt.type == SLVS_E_LINE_SEGMENT ?
////      SLVS_C_PT_ON_LINE : SLVS_C_PT_ON_CIRCLE;
////  Slvs_Constraint aNewConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
////      aType, myGroup->getWorkplaneId(), 0.0, aBaseCoincidence.ptA, SLVS_E_UNKNOWN,
////      theEntity, SLVS_E_UNKNOWN);
////  Slvs_hConstraint aNewID = myStorage->addConstraint(aNewConstraint);
////  if (!hasDuplicated && myStorage->hasDuplicatedConstraint()) {
////    // the duplicated constraint appears
////    myStorage->removeConstraint(aNewID);
////    return SLVS_E_UNKNOWN;
////  }
////  mySlvsConstraints.push_back(aNewID);
////  return aNewID;
////}
////
////void SketchSolver_ConstraintCoincidence::addConstraint(ConstraintPtr theConstraint)
////{
////  if (mySlvsConstraints.empty()) {
////    // This constraint is empty, rebuild it from scratch
////    myBaseConstraint = theConstraint;
////    process();
////    return;
////  }
////
////  std::list<AttributePtr> anAttrList =
////      theConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
////  std::list<AttributePtr>::iterator anIter = anAttrList.begin();
////  std::vector<Slvs_hEntity> aPoints;
////  Slvs_hEntity anEntity = SLVS_E_UNKNOWN;
////  int anEntType;
////  for (; anIter != anAttrList.end(); anIter++) {
////    Slvs_hEntity aPointID = SLVS_E_UNKNOWN;
////    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
////    if (!aRefAttr)
////      continue;
////
////    AttributePtr aPointAttr;
////    if (aRefAttr->isObject()) {
////      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
////      std::map<FeaturePtr, Slvs_hEntity>::const_iterator aFeatFound = 
////          myFeatureMap.find(aFeature);
////      if (aFeatFound != myFeatureMap.end())
////        anEntity = aFeatFound->second;
////      else {
////        anEntity = myGroup->getFeatureId(aFeature);
////        if (anEntity == SLVS_E_UNKNOWN)
////          anEntity = changeEntity(aFeature, anEntType);
////        else {
////          myFeatureMap[aFeature] = anEntity;
////          // Obtain relations between attributes of the feature and SolveSpace entities
////          std::list<AttributePtr> anAttrList =
////              aFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
////          std::list<AttributePtr>::iterator anIt = anAttrList.begin();
////          for (; anIt != anAttrList.end(); ++anIt) {
////            Slvs_hEntity anAttrID = myGroup->getAttributeId(*anIt);
////            if (anAttrID != SLVS_E_UNKNOWN)
////              myAttributeMap[*anIt] = anAttrID;
////          }
////        }
////      }
////      // If the feature is a point, add it to the list of coincident points
////      if (aFeature->getKind() == SketchPlugin_Point::ID()) {
////        aPointID = anEntity;
////        anEntity = SLVS_E_UNKNOWN;
////        aPointAttr = aFeature->attribute(SketchPlugin_Point::COORD_ID());
////      }
////    } else {
////      aPointAttr = aRefAttr->attr();
////      std::map<AttributePtr, Slvs_hEntity>::const_iterator anAttrFound =
////          myAttributeMap.find(aPointAttr);
////      if (anAttrFound != myAttributeMap.end())
////        aPointID = anAttrFound->second;
////      else {
////        aPointID = myGroup->getAttributeId(aPointAttr);
////        if (aPointID == SLVS_E_UNKNOWN)
////          aPointID = changeEntity(aPointAttr, anEntType);
////      }
////    }
////
////    if (aPointAttr) { // the point is found
////      aPoints.push_back(aPointID);
////      myCoincidentPoints.insert(aPointAttr);
////      myAttributeMap[aPointAttr] = aPointID;
////    }
////  }
////
////  Slvs_hConstraint aNewConstr = SLVS_E_UNKNOWN;
////  if (anEntity != SLVS_E_UNKNOWN)
////    aNewConstr = addPointOnEntity(aPoints.front(), anEntity);
////  else { // coincidence between two points
////    Slvs_Constraint aBaseCoincidence = myStorage->getConstraint(mySlvsConstraints.front());
////    std::vector<Slvs_hEntity>::const_iterator aPtIter = aPoints.begin();
////    for (; aPtIter != aPoints.end(); aPtIter++) {
////      Slvs_hConstraint aC = addConstraint(aBaseCoincidence.ptA, *aPtIter);
////      if (aC != SLVS_E_UNKNOWN)
////        aNewConstr = aC;
////    }
////  }
////  myExtraCoincidence[theConstraint] = aNewConstr;
////}

////bool SketchSolver_ConstraintCoincidence::remove(ConstraintPtr theConstraint)
////{
////  cleanErrorMsg();
////  if (mySlvsConstraints.empty())
////    return true;
////  ConstraintPtr aConstraint = theConstraint ? theConstraint : myBaseConstraint;
////  int aPos = -1; // position of constraint in the list (-1 for base constraint)
////  std::map<ConstraintPtr, Slvs_hConstraint>::iterator anExtraIt;
////  if (aConstraint != myBaseConstraint) {
////    anExtraIt = myExtraCoincidence.find(aConstraint);
////    if (anExtraIt == myExtraCoincidence.end())
////      return false; // there is no constraint, which is specified to remove
////    else {
////      bool isEmpty = anExtraIt->second == SLVS_E_UNKNOWN;
////      if (!isEmpty) {
////        isEmpty = true;
////        for (aPos = 0; aPos < (int)mySlvsConstraints.size(); aPos++)
////          if (mySlvsConstraints[aPos] == anExtraIt->second) {
////            isEmpty = false;
////            break;
////          }
////        aPos -= 1;
////      }
////      myExtraCoincidence.erase(anExtraIt);
////      if (isEmpty)
////        return false;
////    }
////  }
////
////  bool isFullyRemoved = myStorage->removeConstraint(mySlvsConstraints[aPos+1]);
////  mySlvsConstraints.erase(mySlvsConstraints.begin() + (1+aPos));
////  if (aPos < 0 && !myExtraCoincidence.empty()) {
////    anExtraIt = myExtraCoincidence.begin();
////    // Remove invalid constraints
////    while (anExtraIt != myExtraCoincidence.end()) {
////      if (!anExtraIt->first->data() || !anExtraIt->first->data()->isValid()) {
////        std::map<ConstraintPtr, Slvs_hConstraint>::iterator aTempIt = anExtraIt++;
////        if (aTempIt->first != SLVS_E_UNKNOWN) {
////          myStorage->removeConstraint(aTempIt->second);
////          std::vector<Slvs_hConstraint>::iterator anIt = mySlvsConstraints.begin();
////          for (; anIt != mySlvsConstraints.end(); anIt++)
////            if (*anIt == aTempIt->second) {
////              mySlvsConstraints.erase(anIt);
////              break;
////            }
////        }
////        myExtraCoincidence.erase(aTempIt);
////        continue;
////      }
////      anExtraIt++;
////    }
////    // Find first non-extra conststraint
////    anExtraIt = myExtraCoincidence.begin();
////    while (anExtraIt != myExtraCoincidence.end() && anExtraIt->second == SLVS_E_UNKNOWN)
////      anExtraIt++;
////    if (anExtraIt != myExtraCoincidence.end()) {
////      // Need to specify another base coincidence constraint
////      myBaseConstraint = anExtraIt->first;
////      myExtraCoincidence.erase(anExtraIt);
////      if (mySlvsConstraints.empty()) {
////        std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
////        Slvs_Constraint aBase = myStorage->getConstraint(*aCIter);
////        for (++aCIter; aCIter != mySlvsConstraints.end(); aCIter++) {
////          Slvs_Constraint aConstr = myStorage->getConstraint(*aCIter);
////          aConstr.ptA = aBase.ptA;
////          myStorage->updateConstraint(aConstr);
////        }
////      }
////    }
////  }
////  // Clear removed attributes
////  std::set<Slvs_hParam> aParamRemoved;
////  std::set<Slvs_hEntity> anEntRemoved;
////  std::set<Slvs_hConstraint> aConstrRemoved;
////  myStorage->getRemoved(aParamRemoved, anEntRemoved, aConstrRemoved);
////  std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIter = myAttributeMap.begin();
////  while (anAttrIter != myAttributeMap.end()) {
////    if (anEntRemoved.find(anAttrIter->second) != anEntRemoved.end()) {
////      std::map<AttributePtr, Slvs_hEntity>::iterator aTempIt = anAttrIter++;
////      myCoincidentPoints.erase(aTempIt->first);
////      myAttributeMap.erase(aTempIt);
////      continue;
////    }
////    anAttrIter++;
////  }
////
////  // Go through remaining extra coincidence and try to add or remove them
////  anExtraIt = myExtraCoincidence.begin();
////  while (anExtraIt != myExtraCoincidence.end()) {
////    if (anExtraIt->first == SLVS_E_UNKNOWN) {
////      if (!anExtraIt->first->data() || !anExtraIt->first->data()->isValid()) {
////        std::map<ConstraintPtr, Slvs_hConstraint>::iterator aTempIt = anExtraIt++;
////        myExtraCoincidence.erase(aTempIt);
////        continue;
////      }
////      if (mySlvsConstraints.empty()) {
////        myBaseConstraint = anExtraIt->first;
////        std::map<ConstraintPtr, Slvs_hConstraint>::iterator aTempIt = anExtraIt++;
////        myExtraCoincidence.erase(aTempIt);
////        process();
////        continue;
////      } else
////        addConstraint(anExtraIt->first);
////    }
////    anExtraIt++;
////  }
////  return mySlvsConstraints.empty();
////}

