// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Group.h"

#include <SketchSolver_Builder.h>
#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>

#include <Events_Error.h>
#include <Events_Loop.h>
#include <GeomAPI_XY.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <math.h>
#include <assert.h>


/// \brief This class is used to give unique index to the groups
class GroupIndexer
{
public:
  /// \brief Return vacant index
  static Slvs_hGroup NEW_GROUP() { return ++myGroupIndex; }
  /// \brief Removes the index
  static void REMOVE_GROUP(const Slvs_hGroup& theIndex) {
    if (myGroupIndex == theIndex)
      myGroupIndex--;
  }

private:
  GroupIndexer() {};

  static Slvs_hGroup myGroupIndex; ///< index of the group
};

Slvs_hGroup GroupIndexer::myGroupIndex = 0;



// ========================================================
// =========  SketchSolver_Group  ===============
// ========================================================

SketchSolver_Group::SketchSolver_Group(
    std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane)
    : myID(GroupIndexer::NEW_GROUP())
{
  // Initialize workplane
  myWorkplaneID = SLVS_E_UNKNOWN;
#ifndef NDEBUG
  assert(addWorkplane(theWorkplane));
#else
  addWorkplane(theWorkplane);
#endif
}

SketchSolver_Group::~SketchSolver_Group()
{
  myConstraints.clear();
  GroupIndexer::REMOVE_GROUP(myID);
}

// ============================================================================
//  Function: isBaseWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  verify the group is based on the given workplane
// ============================================================================
bool SketchSolver_Group::isBaseWorkplane(CompositeFeaturePtr theWorkplane) const
{
  return theWorkplane == mySketch;
}

// ============================================================================
//  Function: isInteract
//  Class:    SketchSolver_Group
//  Purpose:  verify are there any entities in the group used by given constraint
// ============================================================================
bool SketchSolver_Group::isInteract(
    std::shared_ptr<SketchPlugin_Feature> theFeature) const
{
  // Empty group interacts with everything
  if (isEmpty()) return true;
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstraint)
    return myFeatureStorage->isInteract(aConstraint);
  return myFeatureStorage->isInteract(std::dynamic_pointer_cast<ModelAPI_Feature>(theFeature));
}

// ============================================================================
//  Function: getFeatureId
//  Class:    SketchSolver_Group
//  Purpose:  Find the identifier of the feature, if it already exists in the group
// ============================================================================
Slvs_hEntity SketchSolver_Group::getFeatureId(FeaturePtr theFeature) const
{
  Slvs_hEntity aResult = SLVS_E_UNKNOWN;
  if (!myFeatureStorage)
    return aResult;
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theFeature);
  if (aConstraints.empty())
    return aResult;
  std::set<ConstraintPtr>::iterator aConstrIter = aConstraints.begin();
  for (; aConstrIter != aConstraints.end(); aConstrIter++) {
    ConstraintConstraintMap::const_iterator aCIter = myConstraints.find(*aConstrIter);
    if (aCIter == myConstraints.end())
      continue;
    aResult = aCIter->second->getId(theFeature);
    if (aResult != SLVS_E_UNKNOWN)
      return aResult;
  }
  return SLVS_E_UNKNOWN;
}

// ============================================================================
//  Function: getAttributeId
//  Class:    SketchSolver_Group
//  Purpose:  Find the identifier of the attribute, if it already exists in the group
// ============================================================================
Slvs_hEntity SketchSolver_Group::getAttributeId(AttributePtr theAttribute) const
{
  Slvs_hEntity aResult = SLVS_E_UNKNOWN;
  if (!myFeatureStorage)
    return aResult;
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theAttribute);
  if (aConstraints.empty())
    return aResult;
  std::set<ConstraintPtr>::iterator aConstrIter = aConstraints.begin();
  for (; aConstrIter != aConstraints.end(); aConstrIter++) {
    ConstraintConstraintMap::const_iterator aCIter = myConstraints.find(*aConstrIter);
    if (aCIter == myConstraints.end())
      continue;
    aResult = aCIter->second->getId(theAttribute);
    if (aResult != SLVS_E_UNKNOWN)
      return aResult;
  }
  return SLVS_E_UNKNOWN;
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_Group::changeConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // There is no workplane yet, something wrong
  if (myWorkplaneID == SLVS_E_UNKNOWN)
    return false;

  if (!theConstraint)
    return false;

  if (myConstraints.find(theConstraint) == myConstraints.end()) {
    // Add constraint to the current group
    SolverConstraintPtr aConstraint =
        SketchSolver_Builder::getInstance()->createConstraint(theConstraint);
    if (!aConstraint)
      return false;
    aConstraint->setGroup(this);
    aConstraint->setStorage(myStorage);
    if (!aConstraint->error().empty()) {
      if (aConstraint->error() == SketchSolver_Error::NOT_INITIALIZED())
        return false; // some attribute are not initialized yet, don't show message
      Events_Error::send(aConstraint->error(), this);
    }

    // Additional verification of coincidence of several points
    if (theConstraint->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
      for (; aCIter != myConstraints.end(); aCIter++) {
        std::shared_ptr<SketchSolver_ConstraintCoincidence> aCoincidence =
          std::dynamic_pointer_cast<SketchSolver_ConstraintCoincidence>(aCIter->second);
        if (!aCoincidence)
          continue;
        std::shared_ptr<SketchSolver_ConstraintCoincidence> aCoinc2 =
          std::dynamic_pointer_cast<SketchSolver_ConstraintCoincidence>(aConstraint);
        if (aCoincidence != aCoinc2 && aCoincidence->isCoincide(aCoinc2)) {
          aCoincidence->attach(aCoinc2);
          aConstraint = aCoincidence;
        }
      }
    }
    myConstraints[theConstraint] = aConstraint;
  }
  else
    myConstraints[theConstraint]->update();

  // Fix base features for fillet
  if (theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID()) {
    std::list<AttributePtr> anAttrList =
        theConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
    std::list<AttributePtr>::iterator anAttrIter = anAttrList.begin();
    for (; anAttrIter != anAttrList.end(); anAttrIter++) {
      AttributeRefAttrPtr aRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIter);
      if (!aRefAttr || !aRefAttr->isObject())
        continue;
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      SolverConstraintPtr aConstraint =
          SketchSolver_Builder::getInstance()->createRigidConstraint(aFeature);
      if (!aConstraint)
        continue;
      aConstraint->setGroup(this);
      aConstraint->setStorage(myStorage);
      setTemporary(aConstraint);
    }
  }
  //// Fix base features for mirror
  //if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
  //  AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
  //      theConstraint->attribute(SketchPlugin_ConstraintMirror::ENTITY_B()));
  //  fixFeaturesList(aRefList);
  //}

  if (!myFeatureStorage)
    myFeatureStorage = FeatureStoragePtr(new SketchSolver_FeatureStorage);
  myFeatureStorage->changeConstraint(theConstraint);

  return true;
}


bool SketchSolver_Group::updateFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  std::set<ConstraintPtr> aConstraints =
      myFeatureStorage->getConstraints(std::dynamic_pointer_cast<ModelAPI_Feature>(theFeature));
  if (aConstraints.empty())
    return false;
  std::set<ConstraintPtr>::iterator aCIter = aConstraints.begin();
  for (; aCIter != aConstraints.end(); aCIter++) {
    ConstraintConstraintMap::iterator aSolConIter = myConstraints.find(*aCIter);
    if (aSolConIter == myConstraints.end())
      continue;
    myFeatureStorage->changeFeature(theFeature, aSolConIter->first);
    aSolConIter->second->addFeature(theFeature);
    aSolConIter->second->update();
  }
  return true;
}

void SketchSolver_Group::moveFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  updateFeature(theFeature);
  // Temporary rigid constraint
  SolverConstraintPtr aConstraint =
      SketchSolver_Builder::getInstance()->createRigidConstraint(theFeature);
  if (!aConstraint)
    return;
  aConstraint->setGroup(this);
  aConstraint->setStorage(myStorage);
  if (aConstraint->error().empty())
    setTemporary(aConstraint);
}

// ============================================================================
//  Function: fixFeaturesList
//  Class:    SketchSolver_Group
//  Purpose:  Apply temporary rigid constraints for the list of features
// ============================================================================
void SketchSolver_Group::fixFeaturesList(AttributeRefListPtr theList)
{
  std::list<ObjectPtr> aList = theList->list();
  std::list<ObjectPtr>::iterator anIt = aList.begin();
  std::list<FeaturePtr> aFeatures;
  // Sort features, at begining there are features used by Equal constraint
  for (; anIt != aList.end(); anIt++) {
    if (!(*anIt))
      continue;
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(aFeature);
    std::set<ConstraintPtr>::iterator aCIter = aConstraints.begin();
    for (; aCIter != aConstraints.end(); aCIter++)
      if ((*aCIter)->getKind() == SketchPlugin_ConstraintEqual::ID())
        break;
    if (aCIter != aConstraints.end())
      aFeatures.push_front(aFeature);
    else
      aFeatures.push_back(aFeature);
  }

  std::list<FeaturePtr>::iterator aFeatIter = aFeatures.begin();
  for (; aFeatIter != aFeatures.end(); aFeatIter++) {
    SolverConstraintPtr aConstraint =
        SketchSolver_Builder::getInstance()->createRigidConstraint(*aFeatIter);
    if (!aConstraint)
      continue;
    aConstraint->setGroup(this);
    aConstraint->setStorage(myStorage);
    setTemporary(aConstraint);
  }
}

// ============================================================================
//  Function: addWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  create workplane for the group
// ============================================================================
bool SketchSolver_Group::addWorkplane(CompositeFeaturePtr theSketch)
{
  if (myWorkplaneID != SLVS_E_UNKNOWN || theSketch->getKind() != SketchPlugin_Sketch::ID())
    return false;  // the workplane already exists or the function parameter is not Sketch

  mySketch = theSketch;
  updateWorkplane();
  return true;
}

// ============================================================================
//  Function: updateWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  update parameters of workplane
// ============================================================================
bool SketchSolver_Group::updateWorkplane()
{
  if (!myStorage) // Create storage if not exists
    myStorage = StoragePtr(new SketchSolver_Storage);
  SketchSolver_Builder* aBuilder = SketchSolver_Builder::getInstance();

  std::vector<Slvs_Entity> anEntities;
  std::vector<Slvs_Param> aParams;
  if (!aBuilder->createWorkplane(mySketch, anEntities, aParams))
    return false;

  if (myWorkplaneID == SLVS_E_UNKNOWN) {
    myWorkplaneID = anEntities.back().h;
    // Add new workplane elements
    std::vector<Slvs_Param>::iterator aParIter = aParams.begin();
    for (; aParIter != aParams.end(); aParIter++) {
      aParIter->h = SLVS_E_UNKNOWN; // the ID should be generated by storage
      aParIter->group = myID;
      aParIter->h = myStorage->addParameter(*aParIter);
    }
    std::vector<Slvs_Entity>::iterator anEntIter = anEntities.begin();
    for (; anEntIter != anEntities.end(); anEntIter++) {
      anEntIter->h = SLVS_E_UNKNOWN; // the ID should be generated by storage
      anEntIter->group = myID;
      anEntIter->wrkpl = myWorkplaneID;
      for (int i = 0; i < 4; i++)
        if (anEntIter->param[i] != SLVS_E_UNKNOWN)
          anEntIter->param[i] = aParams[anEntIter->param[i]-1].h;
      for (int i = 0; i < 4; i++)
        if (anEntIter->point[i] != SLVS_E_UNKNOWN)
          anEntIter->point[i] = anEntities[anEntIter->point[i]-1].h;
      anEntIter->h = myStorage->addEntity(*anEntIter);
    }
  } else {
    // Update existent workplane
    const Slvs_Entity& aWP = myStorage->getEntity(myWorkplaneID);
    const Slvs_Entity& anOrigin = myStorage->getEntity(aWP.point[0]);
    const Slvs_Entity& aNormal = myStorage->getEntity(aWP.normal);
    // Get parameters and update them
    Slvs_hParam aWPParams[7] = {
        anOrigin.param[0], anOrigin.param[1], anOrigin.param[2],
        aNormal.param[0], aNormal.param[1], aNormal.param[2], aNormal.param[3]
      };
    std::vector<Slvs_Param>::iterator aParIter = aParams.begin();
    for (int i = 0; aParIter != aParams.end(); aParIter++, i++) {
      Slvs_Param aParam = myStorage->getParameter(aWPParams[i]);
      aParam.val = aParIter->val;
      myStorage->updateParameter(aParam);
    }
  }
  return myWorkplaneID > 0;
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_Group
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
bool SketchSolver_Group::resolveConstraints()
{
  bool aResolved = false;
  if (myStorage->isNeedToResolve() && !isEmpty()) {
    myConstrSolver.setGroupID(myID);
    myStorage->initializeSolver(myConstrSolver);

    int aResult = SLVS_RESULT_OKAY;
    try {
      aResult = myConstrSolver.solve();
    } catch (...) {
      Events_Error::send(SketchSolver_Error::SOLVESPACE_CRASH(), this);
      return false;
    }
    if (aResult == SLVS_RESULT_OKAY) {  // solution succeeded, store results into correspondent attributes
      myFeatureStorage->blockEvents(true);
      ConstraintConstraintMap::iterator aConstrIter = myConstraints.begin();
      for (; aConstrIter != myConstraints.end(); aConstrIter++)
        aConstrIter->second->refresh();
      myFeatureStorage->blockEvents(false);
    } else if (!myConstraints.empty())
      Events_Error::send(SketchSolver_Error::CONSTRAINTS(), this);

    aResolved = true;
  }
  removeTemporaryConstraints();
  myStorage->setNeedToResolve(false);
  return aResolved;
}

// ============================================================================
//  Function: mergeGroups
//  Class:    SketchSolver_Group
//  Purpose:  append specified group to the current group
// ============================================================================
void SketchSolver_Group::mergeGroups(const SketchSolver_Group& theGroup)
{
  // If specified group is empty, no need to merge
  if (theGroup.isEmpty())
    return;
  if (!myFeatureStorage)
    myFeatureStorage = FeatureStoragePtr(new SketchSolver_FeatureStorage);

  std::vector<ConstraintPtr> aComplexConstraints;
  ConstraintConstraintMap::const_iterator aConstrIter = theGroup.myConstraints.begin();
  // append simple constraints
  for (; aConstrIter != theGroup.myConstraints.end(); aConstrIter++)
    if (isComplexConstraint(aConstrIter->first))
      aComplexConstraints.push_back(aConstrIter->first);
    else
      changeConstraint(aConstrIter->first);
  // append complex constraints
  std::vector<ConstraintPtr>::iterator aComplexIter = aComplexConstraints.begin();
  for (; aComplexIter != aComplexConstraints.end(); aComplexIter++)
      changeConstraint(*aComplexIter);
}

// ============================================================================
//  Function: splitGroup
//  Class:    SketchSolver_Group
//  Purpose:  divide the group into several subgroups
// ============================================================================
void SketchSolver_Group::splitGroup(std::vector<SketchSolver_Group*>& theCuts)
{
  // Obtain constraints, which should be separated
  FeatureStoragePtr aNewFeatStorage(new SketchSolver_FeatureStorage);
  std::vector<ConstraintPtr> anUnusedConstraints;
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for ( ; aCIter != myConstraints.end(); aCIter++) {
    std::list<ConstraintPtr> aBaseConstraints = aCIter->second->constraints();
    std::list<ConstraintPtr>::iterator anIter = aBaseConstraints.begin();
    for (; anIter != aBaseConstraints.end(); anIter++)
      if (aNewFeatStorage->isInteract(*anIter)) {
        aNewFeatStorage->changeConstraint(*anIter);
      } else
        anUnusedConstraints.push_back(*anIter);
  }

  // Check the unused constraints once again, because they may become interacted with new storage since adding constraints
  std::vector<ConstraintPtr>::iterator aUnuseIt = anUnusedConstraints.begin();
  while (aUnuseIt != anUnusedConstraints.end()) {
    if (aNewFeatStorage->isInteract(*aUnuseIt)) {
      size_t aShift = aUnuseIt - anUnusedConstraints.begin();
      anUnusedConstraints.erase(aUnuseIt);
      aUnuseIt = anUnusedConstraints.begin() + aShift;
      continue;
    }
    aUnuseIt++;
  }

  std::vector<SketchSolver_Group*>::iterator aCutsIter;
  aUnuseIt = anUnusedConstraints.begin();
  for ( ; aUnuseIt != anUnusedConstraints.end(); aUnuseIt++) {
    // Remove unused constraints
    removeConstraint(*aUnuseIt);
    // Try to append constraint to already existent group
    for (aCutsIter = theCuts.begin(); aCutsIter != theCuts.end(); aCutsIter++)
      if ((*aCutsIter)->isInteract(*aUnuseIt)) {
        (*aCutsIter)->changeConstraint(*aUnuseIt);
        break;
      }
    if (aCutsIter == theCuts.end()) {
      // Add new group
      SketchSolver_Group* aGroup = new SketchSolver_Group(mySketch);
      aGroup->changeConstraint(*aUnuseIt);
      theCuts.push_back(aGroup);
    }
  }
}

// ============================================================================
//  Function: isConsistent
//  Class:    SketchSolver_Group
//  Purpose:  search removed entities and constraints
// ============================================================================
bool SketchSolver_Group::isConsistent()
{
  if (!myFeatureStorage) // no one constraint is initialized yet
    return true;

  bool aResult = myFeatureStorage->isConsistent();
  if (!aResult) {
    // remove invalid entities
    ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
    while (aCIter != myConstraints.end()) {
      std::list<ConstraintPtr> aConstraints = aCIter->second->constraints();
      std::list<ConstraintPtr>::iterator anIt = aConstraints.begin();
      for (; anIt != aConstraints.end(); anIt++)
        if (!(*anIt)->data() || !(*anIt)->data()->isValid())
          if (aCIter->second->remove(*anIt)) {
            // the constraint is fully removed, detach it from the list
            ConstraintConstraintMap::iterator aTmpIt = aCIter++;
            myFeatureStorage->removeConstraint(aTmpIt->first);
            myConstraints.erase(aTmpIt);
            break;
          }
      if (anIt == aConstraints.end())
        aCIter++;
    }
  }
  return aResult;
}

// ============================================================================
//  Function: removeTemporaryConstraints
//  Class:    SketchSolver_Group
//  Purpose:  remove all transient SLVS_C_WHERE_DRAGGED constraints after
//            resolving the set of constraints
// ============================================================================
void SketchSolver_Group::removeTemporaryConstraints()
{
  myTempConstraints.clear();
  myStorage->removeTemporaryConstraints();
  // Clean lists of removed entities in the storage
  std::set<Slvs_hParam> aRemPar;
  std::set<Slvs_hEntity> aRemEnt;
  std::set<Slvs_hConstraint> aRemCon;
  myStorage->getRemoved(aRemPar, aRemEnt, aRemCon);
  myStorage->setNeedToResolve(false);
}

// ============================================================================
//  Function: removeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  remove constraint and all unused entities
// ============================================================================
void SketchSolver_Group::removeConstraint(ConstraintPtr theConstraint)
{
  myFeatureStorage->removeConstraint(theConstraint);
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (aCIter->second->hasConstraint(theConstraint)) {
      if (!aCIter->second->remove(theConstraint)) // the constraint is not fully removed
        aCIter = myConstraints.end();
      break;
    }
  if (aCIter != myConstraints.end())
    myConstraints.erase(aCIter);
}

// ============================================================================
//  Function: isComplexConstraint
//  Class:    SketchSolver_Group
//  Purpose:  verifies the constraint is complex, i.e. it needs another constraints to be created before
// ============================================================================
bool SketchSolver_Group::isComplexConstraint(FeaturePtr theConstraint)
{
  return theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID() ||
         theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID() ||
         theConstraint->getKind() == SketchPlugin_ConstraintTangent::ID();
}

// ============================================================================
//  Function: setTemporary
//  Class:    SketchSolver_Group
//  Purpose:  append given constraint to th group of temporary constraints
// ============================================================================
void SketchSolver_Group::setTemporary(SolverConstraintPtr theConstraint)
{
  theConstraint->makeTemporary();
  myTempConstraints.insert(theConstraint);
}

