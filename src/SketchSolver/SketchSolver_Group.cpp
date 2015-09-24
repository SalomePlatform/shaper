// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Group.h"

#include <SketchSolver_Builder.h>
#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_ConstraintMulti.h>
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
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Sketch.h>

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

Slvs_hGroup GroupIndexer::myGroupIndex = SLVS_G_OUTOFGROUP;


static void sendMessage(const char* theMessageName)
{
  std::shared_ptr<Events_Message> aMessage = std::shared_ptr<Events_Message>(
      new Events_Message(Events_Loop::eventByName(theMessageName)));
  Events_Loop::loop()->send(aMessage);
}



// ========================================================
// =========  SketchSolver_Group  ===============
// ========================================================

SketchSolver_Group::SketchSolver_Group(
    std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane)
    : myID(GroupIndexer::NEW_GROUP()),
      myPrevSolved(true)
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
  // Obtain regular constraints interacting with the feature and find its ID
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theFeature);
  if (aConstraints.empty())
    return aResult;
  std::set<ConstraintPtr>::iterator aConstrIter = aConstraints.begin();
  for (; aConstrIter != aConstraints.end(); ++aConstrIter) {
    ConstraintConstraintMap::const_iterator aCIter = myConstraints.find(*aConstrIter);
    if (aCIter == myConstraints.end())
      continue;
    aResult = aCIter->second->getId(theFeature);
    if (aResult != SLVS_E_UNKNOWN)
      return aResult;
  }
  // The feature is not found, check it in the temporary constraints
  std::set<SolverConstraintPtr>::iterator aTmpCIter = myTempConstraints.begin();
  for (; aTmpCIter != myTempConstraints.end() && aResult == SLVS_E_UNKNOWN; ++aTmpCIter)
    aResult = (*aTmpCIter)->getId(theFeature);
  return aResult;
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
  // Obtain regular constraints interacting with the attribute and find its ID
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theAttribute);
  std::set<ConstraintPtr>::iterator aConstrIter = aConstraints.begin();
  for (; aConstrIter != aConstraints.end(); aConstrIter++) {
    ConstraintConstraintMap::const_iterator aCIter = myConstraints.find(*aConstrIter);
    if (aCIter == myConstraints.end())
      continue;
    aResult = aCIter->second->getId(theAttribute);
    if (aResult != SLVS_E_UNKNOWN)
      return aResult;
  }
  // The attribute is not found, check it in the temporary constraints
  std::set<SolverConstraintPtr>::const_iterator aTmpCIter = myTempConstraints.begin();
  for (; aTmpCIter != myTempConstraints.end() && aResult == SLVS_E_UNKNOWN; ++aTmpCIter)
    aResult = (*aTmpCIter)->getId(theAttribute);
  // Last chance to find attribute in parametric constraints
  std::map<AttributePtr, SolverConstraintPtr>::const_iterator aParIter =
      myParametricConstraints.find(theAttribute);
  if (aParIter != myParametricConstraints.end())
    aResult = aParIter->second->getId(theAttribute);
  return aResult;
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

  if (!theConstraint || !theConstraint->data())
    return false;

  if (!checkFeatureValidity(theConstraint))
    return false;

  bool isNewConstraint = myConstraints.find(theConstraint) == myConstraints.end();
  if (isNewConstraint) {
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
      bool hasMultiCoincidence = false;
      ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
      for (; aCIter != myConstraints.end(); ++aCIter) {
        std::shared_ptr<SketchSolver_ConstraintCoincidence> aCoincidence =
          std::dynamic_pointer_cast<SketchSolver_ConstraintCoincidence>(aCIter->second);
        if (!aCoincidence)
          continue;
        std::shared_ptr<SketchSolver_ConstraintCoincidence> aCoinc2 =
          std::dynamic_pointer_cast<SketchSolver_ConstraintCoincidence>(aConstraint);
        if (aCoincidence != aCoinc2 && aCoincidence->isCoincide(aCoinc2)) {
          aCoinc2->attach(aCoincidence);
          // update other coincidences
          ConstraintConstraintMap::iterator anIt = aCIter;
          for (++anIt; anIt != myConstraints.end(); ++anIt)
            if (anIt->second == aCIter->second)
              anIt->second = aCoinc2;
          aCIter->second = aCoinc2;
          hasMultiCoincidence = true;
        }
      }

      if (hasMultiCoincidence)
        notifyMultiConstraints();
    }
    myConstraints[theConstraint] = aConstraint;
  }
  else
    myConstraints[theConstraint]->update();

  // Fix base features for fillet
  if (isNewConstraint && theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID()) {
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
  // Fix mirror line
  if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        theConstraint->attribute(SketchPlugin_ConstraintMirror::ENTITY_A()));
    if (aRefAttr && aRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      if (aFeature) {
        SolverConstraintPtr aConstraint =
            SketchSolver_Builder::getInstance()->createRigidConstraint(aFeature);
        if (aConstraint) {
          aConstraint->setGroup(this);
          aConstraint->setStorage(myStorage);
          setTemporary(aConstraint);
        }
      }
    }
  }

  if (!myFeatureStorage)
    myFeatureStorage = FeatureStoragePtr(new SketchSolver_FeatureStorage);
  myFeatureStorage->changeConstraint(theConstraint);

  // Check the attributes of constraint are given by parametric expression
  std::list<AttributePtr> anAttributes =
      theConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anAttrIt = anAttributes.begin();
  for (; anAttrIt != anAttributes.end(); ++anAttrIt) {
    AttributeRefAttrPtr aRefAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIt);
    if (!aRefAttr || aRefAttr->isObject())
      continue;
    std::shared_ptr<GeomDataAPI_Point2D> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttr->attr());
    if (!aPoint || (aPoint->textX().empty() && aPoint->textY().empty()))
      continue;

    std::map<AttributePtr, SolverConstraintPtr>::iterator aFound =
        myParametricConstraints.find(aRefAttr->attr());
    if (aFound == myParametricConstraints.end()) {
      SolverConstraintPtr aConstraint =
          SketchSolver_Builder::getInstance()->createParametricConstraint(aRefAttr->attr());
      if (!aConstraint)
        continue;
      aConstraint->setGroup(this);
      aConstraint->setStorage(myStorage);
      myParametricConstraints[aRefAttr->attr()] = aConstraint;
    } else
      aFound->second->update();
  }

  return true;
}


void SketchSolver_Group::updateConstraints()
{
  std::set<SolverConstraintPtr> aPostponed; // postponed constraints Multi-Rotation and Multi-Translation

  ConstraintConstraintMap::iterator anIt = myConstraints.begin();
  for (; anIt != myConstraints.end(); ++anIt) {
    if (myChangedConstraints.find(anIt->first) == myChangedConstraints.end())
      continue;
    if (anIt->first->getKind() == SketchPlugin_MultiRotation::ID() ||
        anIt->first->getKind() == SketchPlugin_MultiTranslation::ID())
      aPostponed.insert(anIt->second);
    else
      anIt->second->update();
  }

  // Update postponed constraints
  std::set<SolverConstraintPtr>::iterator aSCIter = aPostponed.begin();
  for (; aSCIter != aPostponed.end(); ++aSCIter)
    (*aSCIter)->update();

  myChangedConstraints.clear();
}

bool SketchSolver_Group::updateFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  if (!checkFeatureValidity(theFeature))
    return false;

  std::set<ConstraintPtr> aConstraints =
      myFeatureStorage->getConstraints(std::dynamic_pointer_cast<ModelAPI_Feature>(theFeature));
  if (aConstraints.empty())
    return false;
  std::set<ConstraintPtr>::iterator aCIter = aConstraints.begin();
  for (; aCIter != aConstraints.end(); aCIter++) {
    ConstraintConstraintMap::iterator aSolConIter = myConstraints.find(*aCIter);
    if (aSolConIter == myConstraints.end() || !aSolConIter->first->data() ||
        !aSolConIter->first->data()->isValid())
      continue;
    myFeatureStorage->changeFeature(theFeature, aSolConIter->first);

    aSolConIter->second->addFeature(theFeature);
    myChangedConstraints.insert(aSolConIter->first);
  }

  // Search attributes of the feature in the set of parametric constraints and update them
  std::list<AttributePtr> anAttrList =
      theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::iterator anAttrIt = anAttrList.begin();
  for (; anAttrIt != anAttrList.end(); ++anAttrIt) {
    std::map<AttributePtr, SolverConstraintPtr>::iterator aFound =
        myParametricConstraints.find(*anAttrIt);
    if (aFound != myParametricConstraints.end())
      aFound->second->update();
    else {
      std::shared_ptr<GeomDataAPI_Point2D> aPoint =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anAttrIt);
      if (aPoint && (!aPoint->textX().empty() || !aPoint->textY().empty())) {
        // Create new parametric constraint
        SolverConstraintPtr aConstraint =
            SketchSolver_Builder::getInstance()->createParametricConstraint(*anAttrIt);
        if (!aConstraint)
          continue;
        aConstraint->setGroup(this);
        aConstraint->setStorage(myStorage);
        myParametricConstraints[*anAttrIt] = aConstraint;
      }
    }
  }
  return true;
}

void SketchSolver_Group::moveFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  // Firstly, create temporary rigid constraint
  SolverConstraintPtr aConstraint =
      SketchSolver_Builder::getInstance()->createMovementConstraint(theFeature);
  if (!aConstraint)
    return;
  aConstraint->setGroup(this);
  aConstraint->setStorage(myStorage);
  if (aConstraint->error().empty())
    setTemporary(aConstraint);
  // Secondly, update the feature
  updateFeature(theFeature);
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
      aParIter->group = SLVS_G_OUTOFGROUP;
      aParIter->h = myStorage->addParameter(*aParIter);
    }
    std::vector<Slvs_Entity>::iterator anEntIter = anEntities.begin();
    for (; anEntIter != anEntities.end(); anEntIter++) {
      anEntIter->h = SLVS_E_UNKNOWN; // the ID should be generated by storage
      anEntIter->group = SLVS_G_OUTOFGROUP;
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
  if (!myChangedConstraints.empty())
    updateConstraints();

  bool aResolved = false;
  if (myStorage->isNeedToResolve() && !isEmpty()) {
    myConstrSolver.setGroupID(myID);
    myConstrSolver.calculateFailedConstraints(false);
    myStorage->initializeSolver(myConstrSolver);

    int aResult = SLVS_RESULT_OKAY;
    try {
      if (myStorage->hasDuplicatedConstraint())
        aResult = SLVS_RESULT_INCONSISTENT;
      else {
        // To avoid overconstraint situation, we will remove temporary constraints one-by-one
        // and try to find the case without overconstraint
        bool isLastChance = false;
        int aNbTemp = myStorage->numberTemporary();
        while (true) {
          aResult = myConstrSolver.solve();
          if (aResult == SLVS_RESULT_OKAY || isLastChance)
            break;
          if (aNbTemp <= 0) {
            // try to update parameters and resolve once again
            ConstraintConstraintMap::iterator aConstrIt = myConstraints.begin();
            for (; aConstrIt != myConstraints.end(); ++aConstrIt)
              aConstrIt->second->update();
            isLastChance = true;
          } else
            aNbTemp = myStorage->deleteTemporaryConstraint();
          myConstrSolver.calculateFailedConstraints(true); // something failed => need to find it
          myStorage->initializeSolver(myConstrSolver);
        }
      }
    } catch (...) {
//      Events_Error::send(SketchSolver_Error::SOLVESPACE_CRASH(), this);
      getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue(SketchSolver_Error::SOLVESPACE_CRASH());
      if (myPrevSolved) {
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_FAILED);
        myPrevSolved = false;
      }
      return false;
    }
    if (aResult == SLVS_RESULT_OKAY) {  // solution succeeded, store results into correspondent attributes
      myFeatureStorage->blockEvents(true);
      // First refresh parametric constraints to satisfy parameters
      std::map<AttributePtr, SolverConstraintPtr>::iterator aParIter = myParametricConstraints.begin();
      for (; aParIter != myParametricConstraints.end(); ++aParIter)
        aParIter->second->refresh();
      // Update all other constraints
      ConstraintConstraintMap::iterator aConstrIter = myConstraints.begin();
      for (; aConstrIter != myConstraints.end(); ++aConstrIter)
        aConstrIter->second->refresh();
      myFeatureStorage->blockEvents(false);
      if (!myPrevSolved) {
        getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue("");
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_REPAIRED);
        myPrevSolved = true;
      }
    } else if (!myConstraints.empty()) {
//      Events_Error::send(SketchSolver_Error::CONSTRAINTS(), this);
      getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue(SketchSolver_Error::CONSTRAINTS());
      if (myPrevSolved) {
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_FAILED);
        myPrevSolved = false;
      }
    }

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

  std::set<ObjectPtr> aConstraints;
  ConstraintConstraintMap::const_iterator aConstrIter = theGroup.myConstraints.begin();
  for (; aConstrIter != theGroup.myConstraints.end(); aConstrIter++)
    aConstraints.insert(aConstrIter->first);

  std::list<FeaturePtr> aSortedConstraints = selectApplicableFeatures(aConstraints);
  std::list<FeaturePtr>::iterator aSCIter = aSortedConstraints.begin();
  for (; aSCIter != aSortedConstraints.end(); ++aSCIter) {
    ConstraintPtr aConstr = std::dynamic_pointer_cast<SketchPlugin_Constraint>(*aSCIter);
    if (!aConstr)
      continue;
    changeConstraint(aConstr);
  }
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
    std::set<ConstraintPtr> anInvalidConstraints;
    ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
    for (; aCIter != myConstraints.end(); ++aCIter) {
      if (!aCIter->first->data() || !aCIter->first->data()->isValid())
        anInvalidConstraints.insert(aCIter->first);
    }
    std::set<ConstraintPtr>::const_iterator aRemoveIt = anInvalidConstraints.begin();
    for (; aRemoveIt != anInvalidConstraints.end(); ++aRemoveIt)
      removeConstraint(*aRemoveIt);
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
  std::set<SolverConstraintPtr>::iterator aTmpIt = myTempConstraints.begin();
  for (; aTmpIt != myTempConstraints.end(); ++aTmpIt)
    (*aTmpIt)->remove();
  myTempConstraints.clear();

  while (myStorage->numberTemporary())
    myStorage->deleteTemporaryConstraint();
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
  bool isFullyRemoved = true;
  myFeatureStorage->removeConstraint(theConstraint);
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (aCIter->second->hasConstraint(theConstraint)) {
      if (!aCIter->second->remove(theConstraint)) // the constraint is not fully removed
        isFullyRemoved = false;
      break;
    }
  if (aCIter == myConstraints.end())
    return;

  if (isFullyRemoved)
    myConstraints.erase(aCIter);
  else if (aCIter != myConstraints.end() &&
           aCIter->first->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
    // Update multicoincidence
    std::list<ConstraintPtr> aMultiCoinc;
    SolverConstraintPtr aCoincidence = aCIter->second;
    while (aCIter != myConstraints.end()) {
      if (aCIter->second != aCoincidence) {
        ++aCIter;
        continue;
      }
      if (aCIter->first != theConstraint)
        aMultiCoinc.push_back(aCIter->first);
      aCIter->second->remove(aCIter->first);
      ConstraintConstraintMap::iterator aRemoveIt = aCIter++;
      myConstraints.erase(aRemoveIt);
    }

    std::list<ConstraintPtr>::iterator anIt = aMultiCoinc.begin();
    for (; anIt != aMultiCoinc.end(); ++anIt)
      changeConstraint(*anIt);

    notifyMultiConstraints();
  }
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


// ============================================================================
//  Function: checkFeatureValidity
//  Class:    SketchSolver_Group
//  Purpose:  verifies is the feature valid
// ============================================================================
bool SketchSolver_Group::checkFeatureValidity(FeaturePtr theFeature)
{
  if (!theFeature || !theFeature->data()->isValid())
    return true;

  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  return aFactory->validate(theFeature);
}

// ============================================================================
//  Function: notifyMultiConstraints
//  Class:    SketchSolver_Group
//  Purpose:  Update Multi-Translation/-Rotation constraints due to multi coincidence appears/disappears
// ============================================================================
void SketchSolver_Group::notifyMultiConstraints()
{
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); ++aCIter) {
    if (aCIter->first->getKind() == SketchPlugin_MultiRotation::ID() ||
        aCIter->first->getKind() == SketchPlugin_MultiTranslation::ID()) {
      std::shared_ptr<SketchSolver_ConstraintMulti> aMulti = 
          std::dynamic_pointer_cast<SketchSolver_ConstraintMulti>(aCIter->second);
      aMulti->checkCoincidence();
    }
  }
}




// ===========   Auxiliary functions   ========================================
static double featureToVal(FeaturePtr theFeature)
{
  if (theFeature->getKind() == SketchPlugin_Sketch::ID())
    return 0.0; // sketch
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (!aConstraint)
    return 1.0; // features (arc, circle, line, point)

  const std::string& anID = aConstraint->getKind();
  if (anID == SketchPlugin_ConstraintCoincidence::ID()) {
    AttributeRefAttrPtr anAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeRefAttrPtr anAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    if (anAttrA && anAttrB && (anAttrA->isObject() || anAttrB->isObject()))
      return 2.0; // point-on-line and point-on-circle should go before points coincidence constraint
    return 2.5;
  }
  if (anID == SketchPlugin_ConstraintDistance::ID() ||
      anID == SketchPlugin_ConstraintLength::ID() ||
      anID == SketchPlugin_ConstraintRadius::ID())
    return 3.0;
  if (anID == SketchPlugin_ConstraintAngle::ID())
    return 3.5;
  if (anID == SketchPlugin_ConstraintHorizontal::ID() ||
      anID == SketchPlugin_ConstraintVertical::ID() ||
      anID == SketchPlugin_ConstraintParallel::ID() ||
      anID == SketchPlugin_ConstraintPerpendicular::ID())
    return 4.0;
  if (anID == SketchPlugin_ConstraintEqual::ID())
    return 5.0;
  if (anID == SketchPlugin_ConstraintTangent::ID() ||
      anID == SketchPlugin_ConstraintMirror::ID())
    return 6.0;
  if (anID == SketchPlugin_ConstraintRigid::ID())
    return 7.0;
  if (anID == SketchPlugin_MultiRotation::ID() ||
      anID == SketchPlugin_MultiTranslation::ID())
    return 8.0;

  // all other constraints are placed between Equal and Tangent constraints
  return 5.5;
}

static bool isLess(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  return featureToVal(theFeature1) < featureToVal(theFeature2);
}

std::list<FeaturePtr> SketchSolver_Group::selectApplicableFeatures(const std::set<ObjectPtr>& theObjects)
{
  std::list<FeaturePtr> aResult;
  std::list<FeaturePtr>::iterator aResIt;

  std::set<ObjectPtr>::const_iterator anObjIter = theObjects.begin();
  for (; anObjIter != theObjects.end(); ++anObjIter) {
    // Operate sketch itself and SketchPlugin features only.
    // Also, the Fillet need to be skipped, because there are several separated constraints composing it.
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIter);
    if (!aFeature)
      continue;
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature = 
        std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if ((aFeature->getKind() != SketchPlugin_Sketch::ID() && !aSketchFeature) ||
        aFeature->getKind() == SketchPlugin_ConstraintFillet::ID())
      continue;

    // Find the place where to insert a feature
    for (aResIt = aResult.begin(); aResIt != aResult.end(); ++aResIt)
      if (isLess(aFeature, *aResIt))
        break;
    aResult.insert(aResIt, aFeature);
  }

  return aResult;
}

