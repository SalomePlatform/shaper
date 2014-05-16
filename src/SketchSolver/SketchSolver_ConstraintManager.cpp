// File:    SketchSolver_ConstraintManager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintManager.h"

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
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <math.h>

/// Tolerance for value of parameters
const double tolerance = 1.e-10;

// Initialization of constraint manager self pointer
SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::_self = 0;

/// Global constaint manager object
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
}

SketchSolver_ConstraintManager::~SketchSolver_ConstraintManager()
{
  myGroups.clear();
}

void SketchSolver_ConstraintManager::processEvent(const Events_Message* theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_CREATED) ||
      theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_UPDATED))
  {
    const Model_FeatureUpdatedMessage* aUpdateMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);

    // Only sketches and constraints can be added by Create event
    boost::shared_ptr<SketchPlugin_Sketch> aSketch = 
      boost::dynamic_pointer_cast<SketchPlugin_Sketch>(aUpdateMsg->feature());
    if (aSketch)
    {
      changeWorkplane(aSketch);
      return ;
    }
    boost::shared_ptr<SketchPlugin_Constraint> aConstraint = 
      boost::dynamic_pointer_cast<SketchPlugin_Constraint>(aUpdateMsg->feature());
    if (aConstraint)
    {
      changeConstraint(aConstraint);

      // Solve the set of constraints
      ResolveConstraints();
      return ;
    }

    /// \todo Implement feature update handling
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
      boost::dynamic_pointer_cast<SketchPlugin_Feature>(aUpdateMsg->feature());
    if (aFeature)
    {
      updateEntity(aFeature);

      // Solve the set of constraints
      ResolveConstraints();
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_DELETED))
  {
    const Model_FeatureDeletedMessage* aDeleteMsg = dynamic_cast<const Model_FeatureDeletedMessage*>(theMessage);
    /// \todo Implement deleting objects on event
  }
}

bool SketchSolver_ConstraintManager::changeWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch)
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

bool SketchSolver_ConstraintManager::changeConstraint(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // Search the groups which this constraint touchs
  std::vector<Slvs_hGroup> aGroups;
  findGroups(theConstraint, aGroups);

  // Process the groups list
  if (aGroups.size() == 0)
  { // There are no groups applicable for this constraint => create new one
    boost::shared_ptr<SketchPlugin_Sketch> aWP = findWorkplaneForConstraint(theConstraint);
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
    /// \todo Implement merging of groups
  }

  // Something goes wrong
  return false;
}

void SketchSolver_ConstraintManager::updateEntity(boost::shared_ptr<SketchPlugin_Feature> theFeature)
{
  // Create list of attributes depending on type of the feature
  std::vector<std::string> anAttrList;
  // Point
  boost::shared_ptr<SketchPlugin_Point> aPoint = 
    boost::dynamic_pointer_cast<SketchPlugin_Point>(theFeature);
  if (aPoint)
    anAttrList.push_back(POINT_ATTR_COORD);
  // Line
  boost::shared_ptr<SketchPlugin_Line> aLine = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(theFeature);
  if (aLine)
  {
    anAttrList.push_back(LINE_ATTR_START);
    anAttrList.push_back(LINE_ATTR_END);
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


void SketchSolver_ConstraintManager::findGroups(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint, 
              std::vector<Slvs_hGroup>&                  theGroupIDs) const
{
  boost::shared_ptr<SketchPlugin_Sketch> aWP = findWorkplaneForConstraint(theConstraint);

  std::vector<SketchSolver_ConstraintGroup*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aWP == (*aGroupIter)->getWorkplane() && (*aGroupIter)->isInteract(theConstraint))
      theGroupIDs.push_back((*aGroupIter)->getId());
}

boost::shared_ptr<SketchPlugin_Sketch> SketchSolver_ConstraintManager::findWorkplaneForConstraint(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  std::vector<SketchSolver_ConstraintGroup*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
  {
    boost::shared_ptr<SketchPlugin_Sketch> aWP = (*aGroupIter)->getWorkplane();
    boost::shared_ptr<ModelAPI_AttributeRefList> aWPFeatures = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aWP->data()->attribute(SKETCH_ATTR_FEATURES));
    std::list< boost::shared_ptr<ModelAPI_Feature> > aFeaturesList = aWPFeatures->list();
    std::list< boost::shared_ptr<ModelAPI_Feature> >::const_iterator anIter;
    for (anIter = aFeaturesList.begin(); anIter != aFeaturesList.end(); anIter++)
      if (*anIter == theConstraint)
        return aWP; // workplane is found
  }

  return boost::shared_ptr<SketchPlugin_Sketch>();
}

void SketchSolver_ConstraintManager::ResolveConstraints()
{
  std::vector<SketchSolver_ConstraintGroup*>::iterator aGroupIter; 
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    (*aGroupIter)->ResolveConstraints();
}



// ========================================================
// =========  SketchSolver_ConstraintGroup  ===============
// ========================================================

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::
  SketchSolver_ConstraintGroup(boost::shared_ptr<SketchPlugin_Sketch> theWorkplane)
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
  addWorkplane(theWorkplane);
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

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isBaseWorkplane(
                boost::shared_ptr<SketchPlugin_Sketch> theWorkplane) const
{
  return theWorkplane == mySketch;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isInteract(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  // Check the group is empty
  if (myWorkplane.h != SLVS_E_UNKNOWN && myConstraints.empty())
    return true;

  /// \todo Should be implemented
  return false;
}

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
  int aConstrType = getConstraintType(theConstraint);
  if (aConstrType == SLVS_C_UNKNOWN || 
     (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
    return false;

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
        theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
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

  // Point in 2D
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
  if (aPoint2D)
  {
    // The 2D points are created on workplane. So, if there is no workplane yet, then error
    if (myWorkplane.h == SLVS_E_UNKNOWN)
      return SLVS_E_UNKNOWN;
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

  /// \todo Other types of entities
  
  // Unsupported or wrong entity type
  return SLVS_E_UNKNOWN;
}

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
  std::vector<Slvs_Param>::const_iterator aParamIter; // looks at first parameter of already existent entity or at the end of vector otherwise
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


bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addWorkplane(
                boost::shared_ptr<SketchPlugin_Sketch> theSketch)
{
  if (myWorkplane.h)
    return false; // the workplane already exists

  mySketch = theSketch;
  return updateWorkplane();
}

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

int SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::getConstraintType(
                const boost::shared_ptr<SketchPlugin_Constraint>& theConstraint) const
{
  // Constraint for coincidence of two points
  boost::shared_ptr<SketchPlugin_ConstraintCoincidence> aPtEquiv = 
    boost::dynamic_pointer_cast<SketchPlugin_ConstraintCoincidence>(theConstraint);
  if (aPtEquiv)
  {
    // Verify the constraint has only two attributes and they are points
    int aPt2d = 0; // bit-mapped field, each bit indicates whether the attribute is 2D point 
    int aPt3d = 0; // bit-mapped field, the same information for 3D points
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
        );
      if (!anAttr) continue;
      // Verify the attribute is a 2D point
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D = 
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
      if (aPoint2D)
      {
        aPt2d |= (1 << indAttr);
        continue;
      }
      // Verify the attribute is a 3D point
      boost::shared_ptr<GeomDataAPI_Point> aPoint3D = 
        boost::dynamic_pointer_cast<GeomDataAPI_Point>(anAttr->attr());
      if (aPoint3D)
      {
        aPt3d |= (1 << indAttr);
        continue;
      }
      // Attribute neither 2D nor 3D point is not supported by this type of constraint
      return SLVS_C_UNKNOWN;
    }
    // The constrained points should be in first and second positions,
    // so the expected value of aPt2d or aPt3d is 3
    if ((aPt2d == 3 && aPt3d == 0) || (aPt2d == 0 && aPt3d == 3))
      return SLVS_C_POINTS_COINCIDENT;
    // Constraint parameters are wrong
    return SLVS_C_UNKNOWN;
  }

  /// \todo Implement other kind of constrtaints

  return SLVS_C_UNKNOWN;
}

void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::ResolveConstraints()
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

  /// \todo Support other types of entities
}

void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::updateEntityIfPossible(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  if (myEntityMap.find(theEntity) != myEntityMap.end())
  {
    // If the attribute is a point and it is changed (the group needs to rebuild), 
    // probably user has dragged this point into this position, 
    // so it is necessary to add constraint which will quarantee the point will not change

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

void SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addTemporaryConstraintWhereDragged(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // Find identifier of the entity
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    anEntIter = myEntityMap.find(theEntity);

  // Create WHERE_DRAGGED constraint
  Slvs_Constraint aWDConstr = Slvs_MakeConstraint(++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED, 
                                                  myWorkplane.h, 0.0, anEntIter->second, 0, 0, 0);
  myConstraints.push_back(aWDConstr);
  myTempConstraints.push_back(aWDConstr.h);
}

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
  std::vector<T>::const_iterator aEntIter = theEntities.begin() + theEntityID - 1;
  while (aEntIter != theEntities.end() && aEntIter->h > theEntityID)
    aEntIter--;
  while (aEntIter != theEntities.end() && aEntIter->h < theEntityID)
    aEntIter++;
  if (aEntIter == theEntities.end())
    return -1;
  return aEntIter - theEntities.begin();
}
