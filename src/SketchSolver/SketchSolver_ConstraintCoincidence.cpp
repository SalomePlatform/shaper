#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Group.h>

#include <map>

bool SketchSolver_ConstraintCoincidence::hasConstraint(ConstraintPtr theConstraint) const
{
  if (myBaseConstraint == theConstraint)
    return true;
  std::map<Slvs_hConstraint, ConstraintPtr>::const_iterator anIt = myExtraCoincidence.begin();
  for (; anIt != myExtraCoincidence.end(); anIt++)
    if (anIt->second == theConstraint)
      return true;
  return false;
}

std::list<ConstraintPtr> SketchSolver_ConstraintCoincidence::constraints() const
{
  std::list<ConstraintPtr> aConstraints;
  aConstraints.push_back(myBaseConstraint);
  std::map<Slvs_hConstraint, ConstraintPtr>::const_iterator anIt = myExtraCoincidence.begin();
  for (; anIt != myExtraCoincidence.end(); anIt++)
    aConstraints.push_back(anIt->second);
  return aConstraints;
}

bool SketchSolver_ConstraintCoincidence::isCoincide(
    std::shared_ptr<SketchSolver_ConstraintCoincidence> theConstraint) const
{
  std::set<AttributePtr>::const_iterator anAttrIter = theConstraint->myCoincidentPoints.begin();
  for (; anAttrIter != theConstraint->myCoincidentPoints.end(); anAttrIter++)
    if (myCoincidentPoints.find(*anAttrIter) != myCoincidentPoints.end())
      return true;
  return false;
}

void SketchSolver_ConstraintCoincidence::attach(
    std::shared_ptr<SketchSolver_ConstraintCoincidence> theConstraint)
{
  cleanErrorMsg();
  Slvs_Constraint aBaseCoincidence = myStorage->getConstraint(mySlvsConstraints.front());
  // Remove constraints from theConstraint
  std::vector<Slvs_hConstraint>::iterator aCIter = theConstraint->mySlvsConstraints.begin();
  for (; aCIter != theConstraint->mySlvsConstraints.end(); aCIter++)
    theConstraint->myStorage->removeConstraint(*aCIter);

  if (myStorage == theConstraint->myStorage) {
    // Clean removed items
    std::set<Slvs_hParam> aRemParams;
    std::set<Slvs_hEntity> aRemEnts;
    std::set<Slvs_hConstraint> aRemConstr;
    theConstraint->myStorage->getRemoved(aRemParams, aRemEnts, aRemConstr);
  }

  // Copy data.
  addConstraint(theConstraint->myBaseConstraint);
  std::map<Slvs_hConstraint, ConstraintPtr>::iterator aConstrIter =
      theConstraint->myExtraCoincidence.begin();
  for (; aConstrIter != theConstraint->myExtraCoincidence.end(); aConstrIter++)
    addConstraint(aConstrIter->second);
  // Clear the lists to not remove the entities on destruction
  theConstraint->mySlvsConstraints.clear();
  theConstraint->myFeatureMap.clear();
  theConstraint->myAttributeMap.clear();
}

Slvs_hConstraint SketchSolver_ConstraintCoincidence::addConstraint(
    Slvs_hEntity thePoint1, Slvs_hEntity thePoint2)
{
  bool hasDuplicated = myStorage->hasDuplicatedConstraint();
  Slvs_Constraint aNewConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
      SLVS_C_POINTS_COINCIDENT, myGroup->getWorkplaneId(), 0.0, thePoint1, thePoint2, 
      SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
  Slvs_hConstraint aNewID = myStorage->addConstraint(aNewConstraint);
  if (!hasDuplicated && myStorage->hasDuplicatedConstraint()) {
    // the duplicated constraint appears
    myStorage->removeConstraint(aNewID);
    return SLVS_E_UNKNOWN;
  }
  mySlvsConstraints.push_back(aNewID);
  return aNewID;
}

void SketchSolver_ConstraintCoincidence::addConstraint(ConstraintPtr theConstraint)
{
  std::list<AttributePtr> anAttrList =
      theConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anIter = anAttrList.begin();
  std::vector<Slvs_hEntity> anEntities;
  for (; anIter != anAttrList.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (!aRefAttr || aRefAttr->isObject() ||
        myAttributeMap.find(aRefAttr->attr()) != myAttributeMap.end())
      continue;
    int aType;
    Slvs_hEntity anEntityID = myGroup->getAttributeId(aRefAttr->attr());
    if (anEntityID == SLVS_E_UNKNOWN)
      anEntityID = changeEntity(aRefAttr->attr(), aType);
    anEntities.push_back(anEntityID);
    myCoincidentPoints.insert(aRefAttr->attr());
  }

  Slvs_Constraint aBaseCoincidence = myStorage->getConstraint(mySlvsConstraints.front());
  Slvs_hConstraint aNewConstr = SLVS_E_UNKNOWN;
  std::vector<Slvs_hEntity>::iterator anEntIter = anEntities.begin();
  for (; anEntIter != anEntities.end(); anEntIter++)
    aNewConstr = addConstraint(aBaseCoincidence.ptA, *anEntIter);
  myExtraCoincidence[aNewConstr] = theConstraint;
}

void SketchSolver_ConstraintCoincidence::process()
{
  SketchSolver_Constraint::process();

  // Fill the list of coincident points
  std::list<AttributePtr> anAttrList =
      myBaseConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anIt = anAttrList.begin();
  for (; anIt != anAttrList.end(); anIt++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIt);
    if (!aRefAttr || aRefAttr->isObject())
      continue;
    myCoincidentPoints.insert(aRefAttr->attr());
  }
}

bool SketchSolver_ConstraintCoincidence::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (mySlvsConstraints.empty())
    return true;
  ConstraintPtr aConstraint = theConstraint ? theConstraint : myBaseConstraint;
  int aPos = -1; // position of constraint in the list (-1 for base constraint)
  std::map<Slvs_hConstraint, ConstraintPtr>::iterator anExtraIt;
  if (aConstraint != myBaseConstraint) {
    anExtraIt = myExtraCoincidence.begin();
    for (aPos = 0; anExtraIt != myExtraCoincidence.end(); anExtraIt++, aPos++)
      if (anExtraIt->second == aConstraint)
        break;
    if (aPos >= (int)myExtraCoincidence.size())
      return false; // there is no constraint, which is specified to remove
    else {
      bool isEmpty = anExtraIt->first == SLVS_E_UNKNOWN;
      myExtraCoincidence.erase(anExtraIt);
      if (isEmpty)
        return false;
    }
  }

  bool isFullyRemoved = myStorage->removeConstraint(mySlvsConstraints[aPos+1]);
  mySlvsConstraints.erase(mySlvsConstraints.begin() + (1+aPos));
  if (aPos < 0 && !myExtraCoincidence.empty()) {
    anExtraIt = myExtraCoincidence.begin();
    // Remove invalid constraints
    while (anExtraIt != myExtraCoincidence.end()) {
      if (!anExtraIt->second->data() || !anExtraIt->second->data()->isValid()) {
        std::map<Slvs_hConstraint, ConstraintPtr>::iterator aTempIt = anExtraIt++;
        if (aTempIt->first != SLVS_E_UNKNOWN) {
          myStorage->removeConstraint(aTempIt->first);
          std::vector<Slvs_hConstraint>::iterator anIt = mySlvsConstraints.begin();
          for (; anIt != mySlvsConstraints.end(); anIt++)
            if (*anIt == aTempIt->first) {
              mySlvsConstraints.erase(anIt);
              break;
            }
        }
        myExtraCoincidence.erase(aTempIt);
        continue;
      }
      anExtraIt++;
    }
    // Find first non-extra conststraint
    while (anExtraIt != myExtraCoincidence.end() && anExtraIt->first == SLVS_E_UNKNOWN)
      anExtraIt++;
    if (anExtraIt != myExtraCoincidence.end()) {
      // Need to specify another base coincidence constraint
      myBaseConstraint = anExtraIt->second;
      myExtraCoincidence.erase(anExtraIt);
      std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
      Slvs_Constraint aBase = myStorage->getConstraint(*aCIter);
      for (++aCIter; aCIter != mySlvsConstraints.end(); aCIter++) {
        Slvs_Constraint aConstr = myStorage->getConstraint(*aCIter);
        aConstr.ptA = aBase.ptA;
        myStorage->updateConstraint(aConstr);
      }
    }
  }
  // Clear removed attributes
  std::set<Slvs_hParam> aParamRemoved;
  std::set<Slvs_hEntity> anEntRemoved;
  std::set<Slvs_hConstraint> aConstrRemoved;
  myStorage->getRemoved(aParamRemoved, anEntRemoved, aConstrRemoved);
  std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIter = myAttributeMap.begin();
  while (anAttrIter != myAttributeMap.end()) {
    if (anEntRemoved.find(anAttrIter->second) != anEntRemoved.end()) {
      std::map<AttributePtr, Slvs_hEntity>::iterator aTempIt = anAttrIter++;
      myCoincidentPoints.erase(aTempIt->first);
      myAttributeMap.erase(aTempIt);
      continue;
    }
    anAttrIter++;
  }

  // Go through remaining extra coincidence and try to add or remove them
  anExtraIt = myExtraCoincidence.begin();
  while (anExtraIt != myExtraCoincidence.end()) {
    if (anExtraIt->first == SLVS_E_UNKNOWN) {
      if (!anExtraIt->second->data() || !anExtraIt->second->data()->isValid()) {
        std::map<Slvs_hConstraint, ConstraintPtr>::iterator aTempIt = anExtraIt++;
        myExtraCoincidence.erase(aTempIt);
        continue;
      }
      if (mySlvsConstraints.empty()) {
        myBaseConstraint = anExtraIt->second;
        std::map<Slvs_hConstraint, ConstraintPtr>::iterator aTempIt = anExtraIt++;
        myExtraCoincidence.erase(aTempIt);
        process();
        continue;
      } else
        addConstraint(anExtraIt->second);
    }
    anExtraIt++;
  }
  return mySlvsConstraints.empty();
}

