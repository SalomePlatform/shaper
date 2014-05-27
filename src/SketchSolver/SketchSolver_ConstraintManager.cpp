// File:    SketchSolver_ConstraintManager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintManager.h"

#include <SketchSolver_Constraint.h>

#include <Events_Loop.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <Model_Events.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintCoincidence.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <math.h>
#include <assert.h>

/// Tolerance for value of parameters
const double tolerance = 1.e-10;

// Initialization of constraint manager self pointer
SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::_self = 0;

/// Global constraint manager object
SketchSolver_ConstraintManager* myManager = SketchSolver_ConstraintManager::Instance();

/// This value is used to give unique index to the groups
static Slvs_hGroup myGroupIndexer = 0;

/** \brief Search the entity/parameter with specified ID in the list of elements
 *  \param[in] theEntityID unique ID of the element
 *  \param[in] theEntities list of elements
 *  \return position of the found element or -1 if the element is not found
 */
template <typename T>
static int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities);



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
    std::set< boost::shared_ptr<ModelAPI_Feature> > aFeatures = anUpdateMsg->features();

    bool isModifiedEvt = theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_MOVED);
    if (!isModifiedEvt)
    {
      std::set< boost::shared_ptr<ModelAPI_Feature> >::iterator aFeatIter;
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++)
      {
        // Only sketches and constraints can be added by Create event
        const std::string& aFeatureKind = (*aFeatIter)->getKind();
        if (aFeatureKind.compare("Sketch") == 0)
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

    // Find "Sketch" in groups. The constraint groups should be updated when an object removed from Sketch
    std::set<std::string>::const_iterator aFGrIter;
    for (aFGrIter = aFeatureGroups.begin(); aFGrIter != aFeatureGroups.end(); aFGrIter++)
      if (aFGrIter->compare("Sketch") == 0)
        break;
    
    if (aFGrIter != aFeatureGroups.end())
    {
      std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter = myGroups.begin();
      while (aGroupIter != myGroups.end())
      {
        if ((*aGroupIter)->updateGroup())
        { // the group should be removed
          delete *aGroupIter;
          int aShift = aGroupIter - myGroups.begin();
          myGroups.erase(aGroupIter);
          aGroupIter = myGroups.begin() + aShift;
        }
        else aGroupIter++;
      }
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
  if (aFeatureKind.compare("SketchPoint") == 0)
    anAttrList.push_back(POINT_ATTR_COORD);
  // Line
  else if (aFeatureKind.compare("SketchLine") == 0)
  {
    anAttrList.push_back(LINE_ATTR_START);
    anAttrList.push_back(LINE_ATTR_END);
  }
  // Circle
  else if (aFeatureKind.compare("SketchCircle") == 0)
  {
    anAttrList.push_back(CIRCLE_ATTR_CENTER);
    anAttrList.push_back(CIRCLE_ATTR_RADIUS);
  }
  // Arc
  else if (aFeatureKind.compare("SketchArc") == 0)
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
      boost::shared_ptr<ModelAPI_Attribute> anAttribute =
        boost::dynamic_pointer_cast<ModelAPI_Attribute>(theFeature->data()->attribute(*anAttrIter));
      (*aGroupIter)->updateEntityIfPossible(anAttribute);
    }
  }
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
    std::list< boost::shared_ptr<ModelAPI_Feature> > aFeaturesList = aWPFeatures->list();
    std::list< boost::shared_ptr<ModelAPI_Feature> >::const_iterator anIter;
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



// ========================================================
// =========  SketchSolver_ConstraintGroup  ===============
// ========================================================

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::
  SketchSolver_ConstraintGroup(boost::shared_ptr<SketchPlugin_Feature> theWorkplane)
  : myID(++myGroupIndexer),
    myParamMaxID(0),
    myEntityMaxID(0),
    myConstrMaxID(0),
    myConstraintMap(),
    myNeedToSolve(false),
    myConstrSolver()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();

  // Initialize workplane
  myWorkplane.h = SLVS_E_UNKNOWN;
#ifndef NDEBUG
  assert(addWorkplane(theWorkplane));
#else
  addWorkplane(theWorkplane);
#endif
}

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::~SketchSolver_ConstraintGroup()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();
  myConstraintMap.clear();

  // If the group with maximal identifier is deleted, decrease the indexer
  if (myID == myGroupIndexer)
    myGroupIndexer--;
}

// ============================================================================
//  Function: isBaseWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verify the group is based on the given workplane
// ============================================================================
bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isBaseWorkplane(
                boost::shared_ptr<SketchPlugin_Feature> theWorkplane) const
{
  return theWorkplane == mySketch;
}

// ============================================================================
//  Function: isInteract
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verify are there any entities in the group used by given constraint
// ============================================================================
bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isInteract(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  // Check the group is empty
  if (myWorkplane.h != SLVS_E_UNKNOWN && myConstraints.empty())
    return true;

  // Go through constraint entities and verify if some of them already in the group
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; i++)
  {
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aCAttrRef =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[i])
      );
    if (!aCAttrRef) continue;
    if (myEntityMap.find(aCAttrRef->attr()) != myEntityMap.end())
      return true;
  }

  // Entities did not found
  return false;
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::changeConstraint(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // There is no workplane yet, something wrong
  if (myWorkplane.h == SLVS_E_UNKNOWN)
    return false;

  // Search this constraint in the current group to update it
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::const_iterator
    aConstrMapIter = myConstraintMap.find(theConstraint);
  std::vector<Slvs_Constraint>::iterator aConstrIter;
  if (aConstrMapIter != myConstraintMap.end())
  {
    int aConstrPos = Search(aConstrMapIter->second, myConstraints);
    aConstrIter = myConstraints.begin() + aConstrPos;
  }

  // Get constraint type and verify the constraint parameters are correct
  SketchSolver_Constraint aConstraint(theConstraint);
  int aConstrType = aConstraint.getType();
  if (aConstrType == SLVS_C_UNKNOWN ||
     (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
    return false;
  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();

  // Create constraint parameters
  double aDistance = 0.0; // scalar value of the constraint
  boost::shared_ptr<ModelAPI_AttributeDouble> aDistAttr =
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theConstraint->data()->attribute(CONSTRAINT_ATTR_VALUE));
  if (aDistAttr)
  {
    aDistance = aDistAttr->value();
    if (aConstrMapIter != myConstraintMap.end() && fabs(aConstrIter->valA - aDistance) > tolerance)
    {
      myNeedToSolve = true;
      aConstrIter->valA = aDistance;
    }
  }

  Slvs_hEntity aConstrEnt[CONSTRAINT_ATTR_SIZE]; // parameters of the constraint
  for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
  {
    aConstrEnt[indAttr] = SLVS_E_UNKNOWN;
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        theConstraint->data()->attribute(aConstraintAttributes[indAttr])
      );
    if (!aConstrAttr) continue;
    aConstrEnt[indAttr] = changeEntity(aConstrAttr->attr());
  }

  if (aConstrMapIter == myConstraintMap.end())
  {
    // Create SolveSpace constraint structure
    Slvs_Constraint aConstraint =
      Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType, myWorkplane.h,
                          aDistance, aConstrEnt[0], aConstrEnt[1], aConstrEnt[2], aConstrEnt[3]);
    myConstraints.push_back(aConstraint);
    myConstraintMap[theConstraint] = aConstraint.h;
  }
  return true;
}

// ============================================================================
//  Function: changeEntity
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the element affected by any constraint
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::changeEntity(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // If the entity is already in the group, try to find it
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    aEntIter = myEntityMap.find(theEntity);
  std::vector<Slvs_Param>::const_iterator aParamIter; // looks at first parameter of already existent entity or at the end of vector otherwise
  if (aEntIter == myEntityMap.end()) // no such entity => should be created
    aParamIter = myParams.end();
  else
  { // the entity already exists
    int aEntPos = Search(aEntIter->second, myEntities);
    int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
    aParamIter = myParams.begin() + aParamPos;
  }

  // Look over supported types of entities

  // Point in 3D
  boost::shared_ptr<GeomDataAPI_Point> aPoint =
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(theEntity);
  if (aPoint)
  {
    Slvs_hParam aX = changeParameter(aPoint->x(), aParamIter);
    Slvs_hParam aY = changeParameter(aPoint->y(), aParamIter);
    Slvs_hParam aZ = changeParameter(aPoint->z(), aParamIter);

    if (aEntIter != myEntityMap.end()) // the entity already exists
      return aEntIter->second;

    // New entity
    Slvs_Entity aPtEntity = Slvs_MakePoint3d(++myEntityMaxID, myID, aX, aY, aZ);
    myEntities.push_back(aPtEntity);
    myEntityMap[theEntity] = aPtEntity.h;
    return aPtEntity.h;
  }

  // All entities except 3D points are created on workplane. So, if there is no workplane yet, then error
  if (myWorkplane.h == SLVS_E_UNKNOWN)
    return SLVS_E_UNKNOWN;

  // Point in 2D
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
  if (aPoint2D)
  {
    Slvs_hParam aU = changeParameter(aPoint2D->x(), aParamIter);
    Slvs_hParam aV = changeParameter(aPoint2D->y(), aParamIter);

    if (aEntIter != myEntityMap.end()) // the entity already exists
      return aEntIter->second;

    // New entity
    Slvs_Entity aPt2DEntity = Slvs_MakePoint2d(++myEntityMaxID, myID, myWorkplane.h, aU, aV);
    myEntities.push_back(aPt2DEntity);
    myEntityMap[theEntity] = aPt2DEntity.h;
    return aPt2DEntity.h;
  }

  // Scalar value (used for the distance entities)
  boost::shared_ptr<ModelAPI_AttributeDouble> aScalar = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
  if (aScalar)
  {
    Slvs_hParam aValue = changeParameter(aScalar->value(), aParamIter);

    if (aEntIter != myEntityMap.end()) // the entity already exists
      return aEntIter->second;

    // New entity
    Slvs_Entity aDistance = Slvs_MakeDistance(++myEntityMaxID, myID, myWorkplane.h, aValue);
    myEntities.push_back(aDistance);
    myEntityMap[theEntity] = aDistance.h;
    return aDistance.h;
  }

  // SketchPlugin features
  boost::shared_ptr<SketchPlugin_Feature> aFeature =
    boost::dynamic_pointer_cast<SketchPlugin_Feature>(theEntity);
  if (aFeature)
  { // Verify the feature by its kind
    const std::string& aFeatureKind = aFeature->getKind();

    // Line
    if (aFeatureKind.compare("SketchLine") == 0)
    {
      Slvs_hEntity aStart = changeEntity(aFeature->data()->attribute(LINE_ATTR_START));
      Slvs_hEntity aEnd   = changeEntity(aFeature->data()->attribute(LINE_ATTR_END));

      if (aEntIter != myEntityMap.end()) // the entity already exists
        return aEntIter->second;

      // New entity
      Slvs_Entity aLineEntity = Slvs_MakeLineSegment(++myEntityMaxID, myID, myWorkplane.h, aStart, aEnd);
      myEntities.push_back(aLineEntity);
      myEntityMap[theEntity] = aLineEntity.h;
      return aLineEntity.h;
    }
    // Circle
    else if (aFeatureKind.compare("SketchCircle") == 0)
    {
      Slvs_hEntity aCenter = changeEntity(aFeature->data()->attribute(CIRCLE_ATTR_CENTER));
      Slvs_hEntity aRadius = changeEntity(aFeature->data()->attribute(CIRCLE_ATTR_RADIUS));

      if (aEntIter != myEntityMap.end()) // the entity already exists
        return aEntIter->second;

      // New entity
      Slvs_Entity aCircleEntity = 
        Slvs_MakeCircle(++myEntityMaxID, myID, myWorkplane.h, aCenter, myWorkplane.normal, aRadius);
      myEntities.push_back(aCircleEntity);
      myEntityMap[theEntity] = aCircleEntity.h;
      return aCircleEntity.h;
    }
    // Arc
    else if (aFeatureKind.compare("SketchArc") == 0)
    {
      Slvs_hEntity aCenter = changeEntity(aFeature->data()->attribute(ARC_ATTR_CENTER));
      Slvs_hEntity aStart  = changeEntity(aFeature->data()->attribute(ARC_ATTR_START));
      Slvs_hEntity aEnd    = changeEntity(aFeature->data()->attribute(ARC_ATTR_END));

      if (aEntIter != myEntityMap.end()) // the entity already exists
        return aEntIter->second;

      Slvs_Entity anArcEntity = Slvs_MakeArcOfCircle(++myEntityMaxID, myID, 
                                  myWorkplane.h, myWorkplane.normal, aCenter, aStart, aEnd);
      myEntities.push_back(anArcEntity);
      myEntityMap[theEntity] = anArcEntity.h;
      return anArcEntity.h;
    }
    // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
    else if (aFeatureKind.compare("SketchPoint") == 0)
    {
      Slvs_hEntity aPoint = changeEntity(aFeature->data()->attribute(POINT_ATTR_COORD));

      if (aEntIter != myEntityMap.end()) // the entity already exists
        return aEntIter->second;

      // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
      myEntityMap[theEntity] = aPoint;
      return aPoint;
    }
  }

  /// \todo Other types of entities

  // Unsupported or wrong entity type
  return SLVS_E_UNKNOWN;
}

// ============================================================================
//  Function: changeNormal
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the normal of workplane
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::changeNormal(
                boost::shared_ptr<ModelAPI_Attribute> theDirX,
                boost::shared_ptr<ModelAPI_Attribute> theDirY,
                boost::shared_ptr<ModelAPI_Attribute> theNorm)
{
  boost::shared_ptr<GeomDataAPI_Dir> aDirX =
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(theDirX);
  boost::shared_ptr<GeomDataAPI_Dir> aDirY =
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(theDirY);
  if (!aDirX || !aDirY ||
     (fabs(aDirX->x()) + fabs(aDirX->y()) + fabs(aDirX->z()) < tolerance) ||
     (fabs(aDirY->x()) + fabs(aDirY->y()) + fabs(aDirY->z()) < tolerance))
    return SLVS_E_UNKNOWN;

  // quaternion parameters of normal vector
  double qw, qx, qy, qz;
  Slvs_MakeQuaternion(aDirX->x(), aDirX->y(), aDirX->z(),
                      aDirY->x(), aDirY->y(), aDirY->z(),
                      &qw, &qx, &qy, &qz);
  double aNormCoord[4] = {qw, qx, qy, qz};

  // Try to find existent normal
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    aEntIter = myEntityMap.find(theNorm);
  std::vector<Slvs_Param>::const_iterator aParamIter; // looks to the first parameter of already existent entity or to the end of vector otherwise
  if (aEntIter == myEntityMap.end()) // no such entity => should be created
    aParamIter = myParams.end();
  else
  { // the entity already exists, update it
    int aEntPos = Search(aEntIter->second, myEntities);
    int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
    aParamIter = myParams.begin() + aParamPos;
  }

  // Change parameters of the normal
  Slvs_hParam aNormParams[4];
  for (int i = 0; i < 4; i++)
    aNormParams[i] = changeParameter(aNormCoord[i], aParamIter);

  if (aEntIter != myEntityMap.end()) // the entity already exists
    return aEntIter->second;

  // Create a normal
  Slvs_Entity aNormal = Slvs_MakeNormal3d(++myEntityMaxID, myID,
                aNormParams[0], aNormParams[1], aNormParams[2], aNormParams[3]);
  myEntities.push_back(aNormal);
  myEntityMap[theNorm] = aNormal.h;
  return aNormal.h;
}


// ============================================================================
//  Function: addWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create workplane for the group
// ============================================================================
bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addWorkplane(
                boost::shared_ptr<SketchPlugin_Feature> theSketch)
{
  if (myWorkplane.h || theSketch->getKind().compare("Sketch") != 0)
    return false; // the workplane already exists or the function parameter is not Sketch

  mySketch = theSketch;
  updateWorkplane();
  return true;
}

// ============================================================================
//  Function: updateWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  update parameters of workplane
// ============================================================================
bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::updateWorkplane()
{
  // Get parameters of workplane
  boost::shared_ptr<ModelAPI_Attribute> aDirX    = mySketch->data()->attribute(SKETCH_ATTR_DIRX);
  boost::shared_ptr<ModelAPI_Attribute> aDirY    = mySketch->data()->attribute(SKETCH_ATTR_DIRY);
  boost::shared_ptr<ModelAPI_Attribute> aNorm    = mySketch->data()->attribute(SKETCH_ATTR_NORM);
  boost::shared_ptr<ModelAPI_Attribute> anOrigin = mySketch->data()->attribute(SKETCH_ATTR_ORIGIN);
  // Transform them into SolveSpace format
  Slvs_hEntity aNormalWP = changeNormal(aDirX, aDirY, aNorm);
  if (!aNormalWP) return false;
  Slvs_hEntity anOriginWP = changeEntity(anOrigin);
  if (!anOriginWP) return false;

  if (!myWorkplane.h)
  {
    // Create workplane
    myWorkplane = Slvs_MakeWorkplane(++myEntityMaxID, myID, anOriginWP, aNormalWP);
    // Workplane should be added to the list of entities
    myEntities.push_back(myWorkplane);
  }
  return true;
}

// ============================================================================
//  Function: changeParameter
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update value of parameter
// ============================================================================
Slvs_hParam SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::changeParameter(
                const double&                            theParam,
                std::vector<Slvs_Param>::const_iterator& thePrmIter)
{
  if (thePrmIter != myParams.end())
  { // Parameter should be updated
    int aParamPos = thePrmIter - myParams.begin();
    if (fabs(thePrmIter->val - theParam) > tolerance)
    {
      myNeedToSolve = true; // parameter is changed, need to resolve constraints
      myParams[aParamPos].val = theParam;
    }
    thePrmIter++;
    return myParams[aParamPos].h;
  }

  // Newly created parameter
  Slvs_Param aParam = Slvs_MakeParam(++myParamMaxID, myID, theParam);
  myParams.push_back(aParam);
  myNeedToSolve = true;
  // The list of parameters is changed, move iterator to the end of the list to avoid problems
  thePrmIter = myParams.end();
  return aParam.h;
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::resolveConstraints()
{
  if (!myNeedToSolve)
    return;

  myConstrSolver.setGroupID(myID);
  myConstrSolver.setParameters(myParams);
  myConstrSolver.setEntities(myEntities);
  myConstrSolver.setConstraints(myConstraints);

  if (myConstrSolver.solve() == SLVS_RESULT_OKAY)
  { // solution succeeded, store results into correspondent attributes
    // Obtain result into the same list of parameters
    if (!myConstrSolver.getResult(myParams))
      return;

    std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
      anEntIter = myEntityMap.begin();
    for ( ; anEntIter != myEntityMap.end(); anEntIter++)
      updateAttribute(anEntIter->first, anEntIter->second);
  }
  /// \todo Implement error handling

  removeTemporaryConstraints();
  myNeedToSolve = false;
}

// ============================================================================
//  Function: mergeGroups
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  append specified group to the current group
// ============================================================================
void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::mergeGroups(
                const SketchSolver_ConstraintGroup& theGroup)
{
  // If specified group is empty, no need to merge
  if (theGroup.myConstraintMap.empty())
    return ;

  // NOTE: The possibility, that some elements are placed into both groups, is around 0, 
  // so the objects should be copied with changing the indexes

  // Maps between old and new indexes of SolveSpace elements:
  std::map<Slvs_hParam, Slvs_hParam>           aParamMap;
  std::map<Slvs_hEntity, Slvs_hEntity>         anEntityMap;
  std::map<Slvs_hConstraint, Slvs_hConstraint> aConstrMap;

  // Go through copying constraints
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = theGroup.myConstraints.begin();
  for ( ; aConstrIter != theGroup.myConstraints.end(); aConstrIter++)
  {
    Slvs_Constraint aConstraintCopy = *aConstrIter;
    // Go through constraint entities
    Slvs_hEntity* anEntities[CONSTRAINT_ATTR_SIZE] = {
      &(aConstraintCopy.ptA),     &(aConstraintCopy.ptB), 
      &(aConstraintCopy.entityA), &(aConstraintCopy.entityB)
    };
    for (int indEnt = 0; indEnt < CONSTRAINT_ATTR_SIZE; indEnt++)
    {
      if (*(anEntities[indEnt]) == 0)
        continue;
      if (anEntityMap.find(*(anEntities[indEnt])) != anEntityMap.end())
      { // entity is already copied
        *(anEntities[indEnt]) = anEntityMap[*(anEntities[indEnt])];
        continue;
      }

      // Copy entity
      Slvs_Entity anEntityCopy = theGroup.myEntities[Search(*(anEntities[indEnt]), theGroup.myEntities)];
      // Go through entity parameters
      const int aNbEntParams = 4; // maximal number of entity parameters
      for (int indPrm = 0; indPrm < aNbEntParams; indPrm++)
      {
        if (anEntityCopy.param[indPrm] == 0)
          continue;
        if (aParamMap.find(anEntityCopy.param[indPrm]) != aParamMap.end())
        {
          anEntityCopy.param[indPrm] = aParamMap[anEntityCopy.param[indPrm]];
          continue;
        }

        Slvs_Param aParamCopy = theGroup.myParams[Search(anEntityCopy.param[indPrm], theGroup.myParams)];
        aParamMap[aParamCopy.h] = ++myParamMaxID;
        aParamCopy.h = myParamMaxID;
        myParams.push_back(aParamCopy);
      }

      anEntityMap[anEntityCopy.h] = ++myEntityMaxID;
      anEntityCopy.h = myEntityMaxID;
      myEntities.push_back(anEntityCopy);
      *(anEntities[indEnt]) = anEntityCopy.h;
    }

    aConstraintCopy.h = ++myConstrMaxID;
    myConstraints.push_back(aConstraintCopy);
    aConstrMap[aConstrIter->h] = aConstraintCopy.h;
  }

  // Append maps of SketchPlugin to SolveSpace parameters
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::const_iterator
    aSPConstrMapIter = theGroup.myConstraintMap.begin();
  for ( ; aSPConstrMapIter!= theGroup.myConstraintMap.end(); aSPConstrMapIter++)
  {
    std::map<Slvs_hConstraint, Slvs_hConstraint>::iterator aFind = aConstrMap.find(aSPConstrMapIter->second);
    if (aFind != aConstrMap.end())
      myConstraintMap[aSPConstrMapIter->first] = aFind->second;
  }

  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    aSPEntMapIter = theGroup.myEntityMap.begin();
  for ( ; aSPEntMapIter != theGroup.myEntityMap.end(); aSPEntMapIter++) {
    std::map<Slvs_hEntity, Slvs_hEntity>::iterator aFind = anEntityMap.find(aSPEntMapIter->second);
    if (aFind != anEntityMap.end())
      myEntityMap[aSPEntMapIter->first] = aFind->second;
  }

  // Add temporary constraints
  std::list<Slvs_hConstraint>::const_iterator aTempConstrIter = theGroup.myTempConstraints.begin();
  for ( ; aTempConstrIter != theGroup.myTempConstraints.end(); aTempConstrIter++)
  {
    std::map<Slvs_hConstraint, Slvs_hConstraint>::iterator aFind = aConstrMap.find(*aTempConstrIter);
    if (aFind != aConstrMap.end())
      myTempConstraints.push_back(aFind->second);
  }
  myTempConstraints.sort();

  myNeedToSolve = myNeedToSolve || theGroup.myNeedToSolve;
}

// ============================================================================
//  Function: updateGroup
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  search removed entities and constraints
// ============================================================================
bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::updateGroup()
{
  // Check for valid sketch
  if (!mySketch->data()->isValid())
    return true;

  // Fast check for constraint validity. If all constraints are valid, no need to update the group
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::reverse_iterator
    aConstrIter = myConstraintMap.rbegin();
  bool isAllValid = true;
  for ( ; isAllValid && aConstrIter != myConstraintMap.rend(); aConstrIter++)
    if (!aConstrIter->first->data()->isValid())
      isAllValid = false;
  if (isAllValid)
    return false;

  // Remove invalid constraints.
  // There only constraint will be deleted (parameters and entities) will be removed below
  std::list< boost::shared_ptr<SketchPlugin_Constraint> > aConstrToDelete;
  std::map<Slvs_hEntity, bool> anEntToDelete; // entities will be removed if no valid constraints use them
  for (aConstrIter = myConstraintMap.rbegin(); aConstrIter != myConstraintMap.rend(); aConstrIter++)
  {
    bool isValid = aConstrIter->first->data()->isValid();

    int aConstrPos = Search(aConstrIter->second, myConstraints);
    if (aConstrPos < (int)myConstraints.size())
    {
      Slvs_hEntity aConstrEnt[] = {
        myConstraints[aConstrPos].ptA,     myConstraints[aConstrPos].ptB,
        myConstraints[aConstrPos].entityA, myConstraints[aConstrPos].entityB};
      for (int i = 0; i < 4; i++)
        if (aConstrEnt[i] != SLVS_E_UNKNOWN)
        {
          if (anEntToDelete.find(aConstrEnt[i]) == anEntToDelete.end())
            anEntToDelete[aConstrEnt[i]] = !isValid;
          else if (isValid) // constraint is valid => no need to remove its entities
            anEntToDelete[aConstrEnt[i]] = false;
        }
      if (!isValid)
      {
        myConstraints.erase(myConstraints.begin() + aConstrPos);
        if (aConstrIter->second == myConstrMaxID) // When the constraint with highest ID is removed, decrease indexer
          myConstrMaxID--;
        aConstrToDelete.push_front(aConstrIter->first);
      }
    }
  }
  std::list< boost::shared_ptr<SketchPlugin_Constraint> >::iterator aDelIter;
  for (aDelIter = aConstrToDelete.begin(); aDelIter != aConstrToDelete.end(); aDelIter++)
    myConstraintMap.erase(*aDelIter);

  // Remove invalid and unused entities
  std::map<Slvs_hEntity, bool>::reverse_iterator aEDelIter;
  for (aEDelIter = anEntToDelete.rbegin(); aEDelIter != anEntToDelete.rend(); aEDelIter++)
    if (aEDelIter->second)
    {
      int anEntPos = Search(aEDelIter->first, myEntities);
      std::vector<Slvs_Entity>::iterator aEntIter = myEntities.begin() + anEntPos;
      // Number of parameters for the entity
      int aNbParams = 0;
      while (aEntIter->param[aNbParams]) aNbParams++;
      if (aNbParams == 0) continue;
      // Decrease parameter indexer if there are deleted parameter with higher IDs
      if (aEntIter->param[aNbParams-1] == myParamMaxID)
        myParamMaxID -= aNbParams;
      // Remove parameters of the entity
      int aParamPos = Search(aEntIter->param[0], myParams);
      myParams.erase(myParams.begin() + aParamPos,
                     myParams.begin() + aParamPos + aNbParams);

      // Remove entity
      if (aEDelIter->first == myEntityMaxID)
        myEntityMaxID--;
      myEntities.erase(myEntities.begin() + anEntPos);
      // Remove such entity from myEntityMap
      std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
        anEntMapIter = myEntityMap.begin();
      for ( ; anEntMapIter != myEntityMap.end(); anEntMapIter++)
        if (anEntMapIter->second == aEDelIter->first)
          break;
      if (anEntMapIter != myEntityMap.end())
        myEntityMap.erase(anEntMapIter);
    }

  return false;
}

// ============================================================================
//  Function: updateAttribute
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  update features of sketch after resolving constraints
// ============================================================================
void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::updateAttribute(
                boost::shared_ptr<ModelAPI_Attribute> theAttribute,
                const Slvs_hEntity&                   theEntityID)
{
  // Search the position of the first parameter of the entity
  int anEntPos = Search(theEntityID, myEntities);
  int aFirstParamPos = Search(myEntities[anEntPos].param[0], myParams);

  // Look over supported types of entities

  // Point in 3D
  boost::shared_ptr<GeomDataAPI_Point> aPoint =
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
  if (aPoint)
  {
    aPoint->setValue(myParams[aFirstParamPos].val,
                     myParams[aFirstParamPos+1].val,
                     myParams[aFirstParamPos+2].val);
    return ;
  }

  // Point in 2D
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D)
  {
    aPoint2D->setValue(myParams[aFirstParamPos].val,
                       myParams[aFirstParamPos+1].val);
    return ;
  }

  // Scalar value
  boost::shared_ptr<ModelAPI_AttributeDouble> aScalar = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aScalar)
  {
    aScalar->setValue(myParams[aFirstParamPos].val);
    return ;
  }

  /// \todo Support other types of entities
}

// ============================================================================
//  Function: updateEntityIfPossible
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  search the entity in this group and update it
// ============================================================================
void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::updateEntityIfPossible(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  if (myEntityMap.find(theEntity) != myEntityMap.end())
  {
    // If the attribute is a point and it is changed (the group needs to rebuild),
    // probably user has dragged this point into this position,
    // so it is necessary to add constraint which will guarantee the point will not change

    // Store myNeedToSolve flag to verify the entity is really changed
    bool aNeedToSolveCopy = myNeedToSolve;
    myNeedToSolve = false;

    changeEntity(theEntity);

    if (myNeedToSolve) // the entity is changed
    {
      // Verify the entity is a point and add temporary constraint of permanency
      boost::shared_ptr<GeomDataAPI_Point> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point>(theEntity);
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
      if (aPoint || aPoint2D)
        addTemporaryConstraintWhereDragged(theEntity);
    }

    // Restore flag of changes
    myNeedToSolve = myNeedToSolve || aNeedToSolveCopy;
  }
}

// ============================================================================
//  Function: addTemporaryConstraintWhereDragged
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  add transient constraint SLVS_C_WHERE_DRAGGED for the entity, 
//            which was moved by user
// ============================================================================
void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addTemporaryConstraintWhereDragged(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // Find identifier of the entity
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    anEntIter = myEntityMap.find(theEntity);

  // Create SLVS_C_WHERE_DRAGGED constraint
  Slvs_Constraint aWDConstr = Slvs_MakeConstraint(++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED,
                                                  myWorkplane.h, 0.0, anEntIter->second, 0, 0, 0);
  myConstraints.push_back(aWDConstr);
  myTempConstraints.push_back(aWDConstr.h);
}

// ============================================================================
//  Function: removeTemporaryConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  remove all transient SLVS_C_WHERE_DRAGGED constraints after
//            resolving the set of constraints
// ============================================================================
void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::removeTemporaryConstraints()
{
  std::list<Slvs_hConstraint>::reverse_iterator aTmpConstrIter;
  for (aTmpConstrIter = myTempConstraints.rbegin(); aTmpConstrIter != myTempConstraints.rend(); aTmpConstrIter++)
  {
    int aConstrPos = Search(*aTmpConstrIter, myConstraints);
    myConstraints.erase(myConstraints.begin() + aConstrPos);

    // If the removing constraint has higher index, decrease the indexer
    if (*aTmpConstrIter == myConstrMaxID)
      myConstrMaxID--;
  }
  myTempConstraints.clear();
}



// ========================================================
// =========      Auxiliary functions       ===============
// ========================================================

template <typename T>
int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities)
{
  int aResIndex = theEntityID <= theEntities.size() ? theEntityID - 1 : 0;
  int aVecSize = theEntities.size();
  while (aResIndex >= 0 && theEntities[aResIndex].h > theEntityID)
    aResIndex--;
  while (aResIndex < aVecSize && theEntities[aResIndex].h < theEntityID)
    aResIndex++;
  if (aResIndex == -1)
    aResIndex = aVecSize;
  return aResIndex;
}
