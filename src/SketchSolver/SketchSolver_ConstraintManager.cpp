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
#include <SketchPlugin_Sketch.h>

SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::_self = 0;

/// Global constaint manager object
SketchSolver_ConstraintManager* myManager = SketchSolver_ConstraintManager::Instance();

/// This value is used to give unique index to the groups
static Slvs_hGroup myGroupIndexer = 0;

/** \brief Makes transformation from ModelAPI_Attribute to the list of parameters' values
 *  \remark Convertion of normal in 3D needs two attributes (coordinate axis of transversal plane)
 *  \param[in,out] theParams        list of converted values which appended to incoming list
 *  \param[in]     theAttr          attribute to be converted
 *  \param[in]     theNormExtraAttr additional attribute for conversion of a normal
 */
static void ConvertAttributeToParamList(
        std::vector<double>&                  theParams, 
        boost::shared_ptr<ModelAPI_Attribute> theAttr, 
        boost::shared_ptr<ModelAPI_Attribute> theNormExtraAttr = boost::shared_ptr<ModelAPI_Attribute>());

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
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_CREATED))
  {
    const Model_FeatureUpdatedMessage* aCreateMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);

    // Only sketches and constraints can be added by Create event
    boost::shared_ptr<SketchPlugin_Sketch> aSketch = 
      boost::dynamic_pointer_cast<SketchPlugin_Sketch>(aCreateMsg->feature());
    if (aSketch)
    {
      addWorkplane(aSketch);
      return ;
    }
    boost::shared_ptr<SketchPlugin_Constraint> aConstraint = 
      boost::dynamic_pointer_cast<SketchPlugin_Constraint>(aCreateMsg->feature());
    if (aConstraint)
    {
      addConstraint(aConstraint);
      return ;
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_DELETED))
  {
    const Model_FeatureDeletedMessage* aDeleteMsg = dynamic_cast<const Model_FeatureDeletedMessage*>(theMessage);
    /// \todo Implement deleting objects on event
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_UPDATED))
  {
    const Model_FeatureUpdatedMessage* aUpdateMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);

    boost::shared_ptr<SketchPlugin_Sketch> aSketch = 
      boost::dynamic_pointer_cast<SketchPlugin_Sketch>(aUpdateMsg->feature());
    if (aSketch)
    {
      updateWorkplane(aSketch);
      return ;
    }

    boost::shared_ptr<SketchPlugin_Constraint> aConstraint = 
      boost::dynamic_pointer_cast<SketchPlugin_Constraint>(aUpdateMsg->feature());
    if (aConstraint)
    {
//      updateConstraint(aConstraint);
      return ;
    }

    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
      boost::dynamic_pointer_cast<SketchPlugin_Feature>(aUpdateMsg->feature());
//    if (aFeature)
//      updateEntity(aFeature);
  }
}


bool SketchSolver_ConstraintManager::addWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch)
{
  // Check the specified workplane is already used
  std::vector<SketchSolver_ConstraintGroup>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aGroupIter->isBaseWorkplane(theSketch))
      return true;
  // Create new group for workplane
  SketchSolver_ConstraintGroup aNewGroup(theSketch);
  // Verify that the group is created successfully
  if (!aNewGroup.isBaseWorkplane(theSketch))
    return false;
  myGroups.push_back(aNewGroup);
  return true;
}

bool SketchSolver_ConstraintManager::updateWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch)
{
  bool aResult = true; // changed when a workplane wrongly updated
  bool isUpdated = false;
  // Try to update specified workplane in all groups
  std::vector<SketchSolver_ConstraintGroup>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aGroupIter->isBaseWorkplane(theSketch))
    {
      isUpdated = true;
      if (!aGroupIter->updateWorkplane(theSketch))
        aResult = false;
    }
  // If the workplane is not updates, so this is a new workplane
  if (!isUpdated)
    return addWorkplane(theSketch);
  return aResult;
}

bool SketchSolver_ConstraintManager::addConstraint(
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
    SketchSolver_ConstraintGroup aGroup(aWP);
    aGroup.addConstraint(theConstraint);
    myGroups.push_back(aGroup);
    return true;
  }
  else if (aGroups.size() == 1)
  { // Only one group => add constraint into it
    Slvs_hGroup aGroupId = *(aGroups.begin());
    std::vector<SketchSolver_ConstraintGroup>::iterator aGroupIter;
    for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
      if (aGroupIter->getId() == aGroupId)
        return aGroupIter->addConstraint(theConstraint);
  }
  else if (aGroups.size() > 1)
  { // Several groups applicable for this constraint => need to merge them
    /// \todo Implement merging of groups
  }

  // Something goes wrong
  return false;
}


void SketchSolver_ConstraintManager::findGroups(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint, 
              std::vector<Slvs_hGroup>&                  theGroupIDs) const
{
  std::vector<SketchSolver_ConstraintGroup>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aGroupIter->isInteract(theConstraint))
      theGroupIDs.push_back(aGroupIter->getId());
}

boost::shared_ptr<SketchPlugin_Sketch> SketchSolver_ConstraintManager::findWorkplaneForConstraint(
              boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  std::vector<SketchSolver_ConstraintGroup>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
  {
    boost::shared_ptr<SketchPlugin_Sketch> aWP = aGroupIter->getWorkplane();
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



// ========================================================
// =========  SketchSolver_ConstraintGroup  ===============
// ========================================================

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::
  SketchSolver_ConstraintGroup(boost::shared_ptr<SketchPlugin_Sketch> theWorkplane)
  : myID(++myGroupIndexer),
    myParamMaxID(0),
    myEntityMaxID(0),
    myConstrMaxID(0),
    myConstraintMap()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();

  // Nullify all elements of the set of equations
  myConstrSet.param = 0;
  myConstrSet.entity = 0;
  myConstrSet.constraint = 0;
  myConstrSet.failed = 0;

  // Initialize workplane
  myWorkplane.h = 0;
  addWorkplane(theWorkplane);
}

SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::~SketchSolver_ConstraintGroup()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();
  myConstraintMap.clear();

  if (myConstrSet.param)
    delete [] myConstrSet.param;
  if (myConstrSet.entity)
    delete [] myConstrSet.entity;
  if (myConstrSet.constraint)
    delete [] myConstrSet.constraint;
  if (myConstrSet.failed)
    delete [] myConstrSet.failed;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isBaseWorkplane(
                boost::shared_ptr<SketchPlugin_Sketch> theWorkplane) const
{
  return theWorkplane == mySketch;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::isInteract(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const
{
  /// \todo Should be implemented
  return false;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addConstraint(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // There is no workplane yet, something wrong
  if (myWorkplane.h == 0)
    return false;

  // Get constraint type and verify the constraint parameters are correct
  int aConstrType = getConstraintType(theConstraint);
  if (aConstrType == SLVS_C_UNKNOWN)
    return false;

  // Create constraint parameters
  double aDistance = 0.0; // scalar value of the constraint
  boost::shared_ptr<ModelAPI_AttributeDouble> aDistAttr =
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theConstraint->data()->attribute(CONSTRAINT_ATTR_VALUE));
  if (aDistAttr)
    aDistance = aDistAttr->value();

  Slvs_hEntity aConstrEnt[CONSTRAINT_ATTR_SIZE]; // parameters of the constraint
  for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
  {
    boost::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
      );
    aConstrEnt[indAttr] = addEntity(aConstrAttr->attr());
  }

  // Create SolveSpace constraint structure
  Slvs_Constraint aConstraint = 
    Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType, myWorkplane.h, 
                        aDistance, aConstrEnt[0], aConstrEnt[1], aConstrEnt[2], aConstrEnt[3]);
  myConstraints.push_back(aConstraint);
  myConstraintMap[theConstraint] = *(myConstraints.rbegin());

  return true;
}

Slvs_hEntity SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addEntity(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // Look over supported types of entities

  // Point in 3D
  boost::shared_ptr<GeomDataAPI_Point> aPoint = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(theEntity);
  if (aPoint)
  {
    Slvs_hParam aX = addParameter(aPoint->x());
    Slvs_hParam aY = addParameter(aPoint->y());
    Slvs_hParam aZ = addParameter(aPoint->z());
    Slvs_Entity aPtEntity = Slvs_MakePoint3d(++myEntityMaxID, myID, aX, aY, aZ);
    myEntities.push_back(aPtEntity);
    return aPtEntity.h;
  }

  // Point in 2D
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
  if (aPoint2D)
  {
    // The 2D points are created on workplane. So, if there is no workplane yet, then error
    if (myWorkplane.h == 0)
      return 0;
    Slvs_hParam aU = addParameter(aPoint2D->x());
    Slvs_hParam aV = addParameter(aPoint2D->y());
    Slvs_Entity aPt2DEntity = Slvs_MakePoint2d(++myEntityMaxID, myID, myWorkplane.h, aU, aV);
    myEntities.push_back(aPt2DEntity);
    return aPt2DEntity.h;
  }

  /// \todo Other types of entities
  
  // Unsupported or wrong entity type
  return 0;
}

Slvs_hEntity SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addNormal(
                boost::shared_ptr<ModelAPI_Attribute> theDirX, 
                boost::shared_ptr<ModelAPI_Attribute> theDirY)
{
  // Convert axes to the coordinates of normal
  std::vector<double> aNormCoord;
  ConvertAttributeToParamList(aNormCoord, theDirX, theDirY);
  
  // Create a normal
  Slvs_hParam aNormParams[4];
  for (int i = 0; i < 4; i++)
    aNormParams[i] = addParameter(aNormCoord[i]);
  Slvs_Entity aNormal = Slvs_MakeNormal3d(++myEntityMaxID, myID, 
                aNormParams[0], aNormParams[1], aNormParams[2], aNormParams[3]);
  myEntities.push_back(aNormal);
  return aNormal.h;
}


bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addWorkplane(
                boost::shared_ptr<SketchPlugin_Sketch> theSketch)
{
  if (myWorkplane.h)
    return false; // the workplane already exists

  // Get parameters of workplane
  boost::shared_ptr<ModelAPI_Attribute> aDirX    = theSketch->data()->attribute(SKETCH_ATTR_DIRX);
  boost::shared_ptr<ModelAPI_Attribute> aDirY    = theSketch->data()->attribute(SKETCH_ATTR_DIRY);
  boost::shared_ptr<ModelAPI_Attribute> anOrigin = theSketch->data()->attribute(SKETCH_ATTR_ORIGIN);
  // Transform them into SolveSpace format
  Slvs_hEntity aNormalWP = addNormal(aDirX, aDirY);
  if (!aNormalWP) return false;
  Slvs_hEntity anOriginWP = addEntity(anOrigin);
  if (!anOriginWP) return false;
  // Create workplane
  myWorkplane = Slvs_MakeWorkplane(++myEntityMaxID, myID, anOriginWP, aNormalWP);
  mySketch = theSketch;
  // Workplane should be added to the list of entities
  myEntities.push_back(myWorkplane);
  return true;
}

bool SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::updateWorkplane(
                boost::shared_ptr<SketchPlugin_Sketch> theSketch)
{
  // Renew Sketch pointer
  mySketch = theSketch;

  // Get parameters of workplane
  boost::shared_ptr<ModelAPI_Attribute> aDirX  = theSketch->data()->attribute(SKETCH_ATTR_DIRX);
  boost::shared_ptr<ModelAPI_Attribute> aDirY  = theSketch->data()->attribute(SKETCH_ATTR_DIRY);
  boost::shared_ptr<ModelAPI_Attribute> anOrig = theSketch->data()->attribute(SKETCH_ATTR_ORIGIN);
  // Transform them to lists of coordinates
  std::vector<double> aNormal;
  ConvertAttributeToParamList(aNormal, aDirX, aDirY);
  std::vector<double> anOrigin;
  ConvertAttributeToParamList(anOrigin, anOrig);

  // Search the normal and the origin in the parameters list and update their values.
  // Remark: entities are sorted in the vector, so the most probable position 
  //         of the entity is equal to identifier the entity
  
  // search normal
  int aEntPos = Search(myWorkplane.normal, myEntities);
  if (aEntPos < 0) return false;
  // search first parameter of normal
  int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
  if (aParamPos < 0) return false;
  std::vector<Slvs_Param>::iterator aParamIter = myParams.begin() + aParamPos;
  // change normal parameters
  std::vector<double>::iterator anIter;
  for (anIter = aNormal.begin(); anIter != aNormal.end(); anIter++, aParamIter++)
    aParamIter->val = *anIter;

  // search origin
  aEntPos = Search(myWorkplane.point[0], myEntities);
  if (aEntPos < 0) return false;
  // search first parameter of origin
  aParamPos = Search(myEntities[aEntPos].param[0], myParams);
  if (aParamPos < 0) return false;
  aParamIter = myParams.begin() + aParamPos;
  // change origin's parameters
  for (anIter = anOrigin.begin(); anIter != anOrigin.end(); anIter++, aParamIter++)
    aParamIter->val = *anIter;

  return true;
}


Slvs_hParam SketchSolver_ConstraintManager::SketchSolver_ConstraintGroup::addParameter(double theParam)
{
  Slvs_Param aParam = Slvs_MakeParam(++myParamMaxID, myID, theParam);
  myParams.push_back(aParam);
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



// ========================================================
// =========      Auxiliary functions       ===============
// ========================================================

void ConvertAttributeToParamList(
                std::vector<double>&                  theParams, 
                boost::shared_ptr<ModelAPI_Attribute> theAttr, 
                boost::shared_ptr<ModelAPI_Attribute> theNormExtraAttr)
{
  // Point in 3D
  boost::shared_ptr<GeomDataAPI_Point> aPoint = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(theAttr);
  if (aPoint)
  {
    theParams.push_back(aPoint->x());
    theParams.push_back(aPoint->y());
    theParams.push_back(aPoint->z());
    return ;
  }

  // Point in 2D
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttr);
  if (aPoint2D)
  {
    theParams.push_back(aPoint2D->x());
    theParams.push_back(aPoint2D->y());
    return ;
  }

  // Normal in 3D
  boost::shared_ptr<GeomDataAPI_Dir> aDirX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(theAttr);
  boost::shared_ptr<GeomDataAPI_Dir> aDirY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(theNormExtraAttr);
  if (aDirX && aDirY)
  {
    // quaternion parameters of normal vector
    double qw, qx, qy, qz;
    Slvs_MakeQuaternion(aDirX->x(), aDirX->y(), aDirX->z(), 
                        aDirY->x(), aDirY->y(), aDirY->z(), 
                        &qw, &qx, &qy, &qz);
    theParams.push_back(qw);
    theParams.push_back(qx);
    theParams.push_back(qy);
    theParams.push_back(qz);
  }

  /// \todo Other types of entities
}

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

