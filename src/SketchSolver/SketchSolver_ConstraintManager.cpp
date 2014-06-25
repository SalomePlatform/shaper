// File:    SketchSolver_ConstraintManager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintManager.h"

#include <Events_Loop.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <Model_Events.h>

#include <SketchPlugin_Constraint.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>


// Initialization of constraint manager self pointer
SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::_self = 0;

/// Global constraint manager object
SketchSolver_ConstraintManager* myManager = SketchSolver_ConstraintManager::Instance();


// ========================================================
// ========= SketchSolver_ConstraintManager ===============
// ========================================================
SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::Instance()
{
  if (!_self)
    _self = new SketchSolver_ConstraintManager();
  return _self;
}

SketchSolver_ConstraintManager::SketchSolver_ConstraintManager()
{
  myGroups.clear();

  // Register in event loop
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_CREATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_DELETED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_MOVED));
}

SketchSolver_ConstraintManager::~SketchSolver_ConstraintManager()
{
  myGroups.clear();
}

// ============================================================================
//  Function: processEvent
//  Class:    SketchSolver_PluginManager
//  Purpose:  listen the event loop and process the message
// ============================================================================
void SketchSolver_ConstraintManager::processEvent(const Events_Message* theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_CREATED) ||
      theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_UPDATED) || 
      theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_MOVED))
  {
    const Model_FeatureUpdatedMessage* anUpdateMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);
    std::set< FeaturePtr > aFeatures = anUpdateMsg->features();

    bool isModifiedEvt = theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_MOVED);
    if (!isModifiedEvt)
    {
      std::set< FeaturePtr >::iterator aFeatIter;
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++)
      {
        // Only sketches and constraints can be added by Create event
        const std::string& aFeatureKind = (*aFeatIter)->getKind();
        if (aFeatureKind.compare(SKETCH_KIND) == 0)
        {
          boost::shared_ptr<SketchPlugin_Feature> aSketch =
            boost::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
          if (aSketch)
            changeWorkplane(aSketch);
          continue;
        }
        boost::shared_ptr<SketchPlugin_Constraint> aConstraint =
          boost::dynamic_pointer_cast<SketchPlugin_Constraint>(*aFeatIter);
        if (aConstraint)
          changeConstraint(aConstraint);
        else
        {
          // Sketch plugin features can be only updated
          boost::shared_ptr<SketchPlugin_Feature> aFeature =
            boost::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
          if (aFeature)
            updateEntity(aFeature);
        }
      }
    }

    // Solve the set of constraints
    resolveConstraints();
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_DELETED))
  {
    const Model_FeatureDeletedMessage* aDeleteMsg = dynamic_cast<const Model_FeatureDeletedMessage*>(theMessage);
    const std::set<std::string>& aFeatureGroups = aDeleteMsg->groups();

    // Find SKETCH_KIND in groups. The constraint groups should be updated when an object removed from Sketch
    std::set<std::string>::const_iterator aFGrIter;
    for (aFGrIter = aFeatureGroups.begin(); aFGrIter != aFeatureGroups.end(); aFGrIter++)
      if (aFGrIter->compare(SKETCH_KIND) == 0)
        break;
    
    if (aFGrIter != aFeatureGroups.end())
    {
      std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter = myGroups.begin();
      std::vector<SketchSolver_ConstraintGroup*> aSeparatedGroups;
      while (aGroupIter != myGroups.end())
      {
        if (!(*aGroupIter)->isWorkplaneValid())
        { // the group should be removed
          delete *aGroupIter;
          int aShift = aGroupIter - myGroups.begin();
          myGroups.erase(aGroupIter);
          aGroupIter = myGroups.begin() + aShift;
          continue;
        }
        if ((*aGroupIter)->updateGroup())
        { // some constraints were removed, try to split the group
          (*aGroupIter)->splitGroup(aSeparatedGroups);
        }
        aGroupIter++;
      }
      if (aSeparatedGroups.size() > 0)
        myGroups.insert(myGroups.end(), aSeparatedGroups.begin(), aSeparatedGroups.end());
    }
  }
}

// ============================================================================
//  Function: changeWorkplane
//  Class:    SketchSolver_PluginManager
//  Purpose:  update workplane by given parameters of the sketch
// ============================================================================
bool SketchSolver_ConstraintManager::changeWorkplane(boost::shared_ptr<SketchPlugin_Feature> theSketch)
{
  bool aResult = true; // changed when a workplane wrongly updated
  bool isUpdated = false;
  // Try to update specified workplane in all groups
  std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if ((*aGroupIter)->isBaseWorkplane(theSketch))
    {
      isUpdated = true;
      if (!(*aGroupIter)->updateWorkplane())
        aResult = false;
    }
  // If the workplane is not updated, so this is a new workplane
  if (!isUpdated)
  {
    SketchSolver_ConstraintGroup* aNewGroup = new SketchSolver_ConstraintGroup(theSketch);
    // Verify that the group is created successfully
    if (!aNewGroup->isBaseWorkplane(theSketch))
    {
      delete aNewGroup;
      return false;
    }
    myGroups.push_back(aNewGroup);
  }
  return aResult;
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_PluginManager
//  Purpose:  create/update the constraint and place it into appropriate group
// ============================================================================
bool SketchSolver_ConstraintManager::changeConstraint(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // Search the groups which this constraint touches
  std::set<Slvs_hGroup> aGroups;
  findGroups(theConstraint, aGroups);

  // Process the groups list
  if (aGroups.size() == 0)
  { // There are no groups applicable for this constraint => create new one
    boost::shared_ptr<SketchPlugin_Feature> aWP = findWorkplaneForConstraint(theConstraint);
    if (!aWP) return false;
    SketchSolver_ConstraintGroup* aGroup = new SketchSolver_ConstraintGroup(aWP);
    if (!aGroup->changeConstraint(theConstraint))
    {
      delete aGroup;
      return false;
    }
    myGroups.push_back(aGroup);
    return true;
  }
  else if (aGroups.size() == 1)
  { // Only one group => add constraint into it
    Slvs_hGroup aGroupId = *(aGroups.begin());
    std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter;
    for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
      if ((*aGroupIter)->getId() == aGroupId)
        return (*aGroupIter)->changeConstraint(theConstraint);
  }
  else if (aGroups.size() > 1)
  { // Several groups applicable for this constraint => need to merge them
    std::set<Slvs_hGroup>::const_iterator aGroupsIter = aGroups.begin();

    // Search first group
    std::vector<SketchSolver_ConstraintGroup*>::iterator aFirstGroupIter;
    for (aFirstGroupIter = myGroups.begin(); aFirstGroupIter != myGroups.end(); aFirstGroupIter++)
      if ((*aFirstGroupIter)->getId() == *aGroupsIter)
        break;
    if (aFirstGroupIter == myGroups.end())
      return false;

    // Append other groups to the first one
    std::vector<SketchSolver_ConstraintGroup*>::iterator anOtherGroupIter = aFirstGroupIter + 1;
    for (aGroupsIter++; aGroupsIter != aGroups.end(); aGroupsIter++)
    {
      for ( ; anOtherGroupIter != myGroups.end(); anOtherGroupIter++)
        if ((*anOtherGroupIter)->getId() == *aGroupsIter)
          break;
      if (anOtherGroupIter == myGroups.end())
      { // Group disappears
        anOtherGroupIter = aFirstGroupIter + 1;
        continue;
      }

      (*aFirstGroupIter)->mergeGroups(**anOtherGroupIter);
      int aShiftFirst = aFirstGroupIter - myGroups.begin();
      int aShiftOther = anOtherGroupIter - myGroups.begin();
      delete *anOtherGroupIter;
      myGroups.erase(anOtherGroupIter);
      aFirstGroupIter  = myGroups.begin() + aShiftFirst;
      anOtherGroupIter = myGroups.begin() + aShiftOther;
    }

    return (*aFirstGroupIter)->changeConstraint(theConstraint);
  }

  // Something goes wrong
  return false;
}

// ============================================================================
//  Function: updateEntity
//  Class:    SketchSolver_PluginManager
//  Purpose:  update any element on the sketch, which is used by constraints
// ============================================================================
void SketchSolver_ConstraintManager::updateEntity(boost::shared_ptr<SketchPlugin_Feature> theFeature)
{
  // Create list of attributes depending on type of the feature
  std::vector<std::string> anAttrList;
  const std::string& aFeatureKind = theFeature->getKind();
  // Point
  if (aFeatureKind.compare(SKETCH_POINT_KIND) == 0)
    anAttrList.push_back(POINT_ATTR_COORD);
  // Line
  else if (aFeatureKind.compare(SKETCH_LINE_KIND) == 0)
  {
    anAttrList.push_back(LINE_ATTR_START);
    anAttrList.push_back(LINE_ATTR_END);
  }
  // Circle
  else if (aFeatureKind.compare(SKETCH_CIRCLE_KIND) == 0)
  {
    anAttrList.push_back(CIRCLE_ATTR_CENTER);
    anAttrList.push_back(CIRCLE_ATTR_RADIUS);
  }
  // Arc
  else if (aFeatureKind.compare(SKETCH_ARC_KIND) == 0)
  {
    anAttrList.push_back(ARC_ATTR_CENTER);
    anAttrList.push_back(ARC_ATTR_START);
    anAttrList.push_back(ARC_ATTR_END);
  }
  /// \todo Other types of features should be implemented

  // Check changing of feature's attributes (go through the groups and search usage of the attributes)
  std::vector<std::string>::const_iterator anAttrIter;
  for (anAttrIter = anAttrList.begin(); anAttrIter != anAttrList.end(); anAttrIter++)
  {
    std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter;
    for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    {
      if ((*aGroupIter)->isEmpty()) 
        continue;
      boost::shared_ptr<ModelAPI_Attribute> anAttribute =
        boost::dynamic_pointer_cast<ModelAPI_Attribute>(theFeature->data()->attribute(*anAttrIter));
      (*aGroupIter)->updateEntityIfPossible(anAttribute);
    }
  }

  std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (!(*aGroupIter)->isEmpty())
      (*aGroupIter)->updateRelatedConstraints(theFeature);
}


// ============================================================================
//  Function: findGroups
//  Class:    SketchSolver_PluginManager
//  Purpose:  search groups of entities interacting with given constraint
// ============================================================================
void SketchSolver_ConstraintManager::findGroups(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint,
              std::set<Slvs_hGroup>&                     theGroupIDs) const
{
  boost::shared_ptr<SketchPlugin_Feature> aWP = findWorkplaneForConstraint(theConstraint);

  SketchSolver_ConstraintGroup* anEmptyGroup = 0; // appropriate empty group for specified constraint
  std::vector<SketchSolver_ConstraintGroup*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aWP == (*aGroupIter)->getWorkplane() && (*aGroupIter)->isInteract(theConstraint))
    {
      if (!(*aGroupIter)->isEmpty())
        theGroupIDs.insert((*aGroupIter)->getId());
      else if (!anEmptyGroup)
        anEmptyGroup = *aGroupIter;
    }

  // When only empty group is found, use it
  if (anEmptyGroup && theGroupIDs.empty())
    theGroupIDs.insert(anEmptyGroup->getId());
}

// ============================================================================
//  Function: findWorkplaneForConstraint
//  Class:    SketchSolver_PluginManager
//  Purpose:  search workplane containing given constraint
// ============================================================================
boost::shared_ptr<SketchPlugin_Feature> SketchSolver_ConstraintManager::findWorkplaneForConstraint(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  // Already verified workplanes
  std::set< boost::shared_ptr<SketchPlugin_Feature> > aVerified;

  std::vector<SketchSolver_ConstraintGroup*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
  {
    boost::shared_ptr<SketchPlugin_Feature> aWP = (*aGroupIter)->getWorkplane();
    if (aVerified.find(aWP) != aVerified.end())
      continue;

    boost::shared_ptr<ModelAPI_AttributeRefList> aWPFeatures =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aWP->data()->attribute(SKETCH_ATTR_FEATURES));
    std::list< FeaturePtr > aFeaturesList = aWPFeatures->list();
    std::list< FeaturePtr >::const_iterator anIter;
    for (anIter = aFeaturesList.begin(); anIter != aFeaturesList.end(); anIter++)
      if (*anIter == theConstraint)
        return aWP; // workplane is found
    aVerified.insert(aWP);
  }

  return boost::shared_ptr<SketchPlugin_Feature>();
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_PluginManager
//  Purpose:  change entities according to available constraints
// ============================================================================
void SketchSolver_ConstraintManager::resolveConstraints()
{
  std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    (*aGroupIter)->resolveConstraints();

  // Features may be updated => send events
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
}

