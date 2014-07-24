// File:    SketchSolver_ConstraintGroup.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintGroup.h"

#include <SketchSolver_Constraint.h>

#include <Events_Error.h>
#include <Events_Loop.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintLength.h>
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

/*
 * Collects all sketch solver error' codes
 * as inline static functions
 * TODO: Move this class into a separate file
 */
class SketchSolver_Error {
public:
  /// The value parameter for the constraint
  inline static const std::string& CONSTRAINTS()
  {
    static const std::string MY_ERROR_VALUE("Conflicting constraints");
    return MY_ERROR_VALUE;
  }
};

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
// =========  SketchSolver_ConstraintGroup  ===============
// ========================================================

SketchSolver_ConstraintGroup::
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
  
  myTempConstraints.clear();
  myTempPointWhereDragged.clear();
  myTempPointWDrgdID = 0;

  // Initialize workplane
  myWorkplane.h = SLVS_E_UNKNOWN;
#ifndef NDEBUG
  assert(addWorkplane(theWorkplane));
#else
  addWorkplane(theWorkplane);
#endif
}

SketchSolver_ConstraintGroup::~SketchSolver_ConstraintGroup()
{
  myParams.clear();
  myEntities.clear();
  myConstraints.clear();
  myConstraintMap.clear();
  myTempConstraints.clear();
  myTempPointWhereDragged.clear();

  // If the group with maximal identifier is deleted, decrease the indexer
  if (myID == myGroupIndexer)
    myGroupIndexer--;
}

// ============================================================================
//  Function: isBaseWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verify the group is based on the given workplane
// ============================================================================
bool SketchSolver_ConstraintGroup::isBaseWorkplane(
                boost::shared_ptr<SketchPlugin_Feature> theWorkplane) const
{
  return theWorkplane == mySketch;
}

// ============================================================================
//  Function: isInteract
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verify are there any entities in the group used by given constraint
// ============================================================================
bool SketchSolver_ConstraintGroup::isInteract(
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
        theConstraint->data()->attribute(SketchPlugin_Constraint::ATTRIBUTE(i))
      );
    if (!aCAttrRef) continue;
    if (!aCAttrRef->isObject() && 
        myEntityAttrMap.find(aCAttrRef->attr()) != myEntityAttrMap.end())
      return true;
    if (aCAttrRef->isObject())
    { // Obtain a base feature for the object
      ResultConstructionPtr aRC = 
        boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aCAttrRef->object());
      if (!aRC) continue;
      boost::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
      FeaturePtr aFeature = aDoc->feature(aRC);
      if (myEntityFeatMap.find(aFeature) != myEntityFeatMap.end())
        return true;
    }
  }

  // Entities did not found
  return false;
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_ConstraintGroup::changeConstraint(
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
  AttributeDoublePtr aDistAttr =
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theConstraint->data()->attribute(SketchPlugin_Constraint::VALUE()));
  if (aDistAttr)
  {
    aDistance = aDistAttr->value();
    // SketchPlugin circle defined by its radius, but SolveSpace uses constraint for diameter
    if (aConstrType == SLVS_C_DIAMETER)
      aDistance *= 2.0;
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

    // Convert the object of the attribute to the feature
    FeaturePtr aFeature;
    if (aConstrAttr->isObject() && aConstrAttr->object())
    {
      ResultConstructionPtr aRC = 
        boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aConstrAttr->object());
      if (!aRC) continue;
      boost::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
      aFeature = aDoc->feature(aRC);
    }

    // For the length constraint the start and end points of the line should be added to the entities list instead of line
    if (aConstrType == SLVS_C_PT_PT_DISTANCE && theConstraint->getKind().compare(SketchPlugin_ConstraintLength::ID()) == 0)
    {
      boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
      aConstrEnt[indAttr]   = changeEntity(aData->attribute(SketchPlugin_Line::START_ID()));
      aConstrEnt[indAttr+1] = changeEntity(aData->attribute(SketchPlugin_Line::END_ID()));
      // measured object is added into the map of objects to avoid problems with interaction between constraint and group
      myEntityFeatMap[aFeature] = 0;
      break; // there should be no other entities
    }
    else if (aConstrAttr->isObject())
      aConstrEnt[indAttr] = changeEntity(aFeature);
    else
      aConstrEnt[indAttr] = changeEntity(aConstrAttr->attr());
  }

  if (aConstrMapIter == myConstraintMap.end())
  {
    // Several points may be coincident, it is not necessary to store all constraints between them.
    // Try to find sequence of coincident points which connects the points of new constraint
    if (aConstrType == SLVS_C_POINTS_COINCIDENT)
    {
      if (aConstrEnt[0] == aConstrEnt[1]) // no need to add self coincidence
        return false;
      if (!addCoincidentPoints(aConstrEnt[0], aConstrEnt[1]))
      {
        myExtraCoincidence.insert(theConstraint); // the constraint is stored for further purposes
        return false;
      }
    }

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
Slvs_hEntity SketchSolver_ConstraintGroup::changeEntity(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // If the entity is already in the group, try to find it
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    aEntIter = myEntityAttrMap.find(theEntity);
  std::vector<Slvs_Param>::const_iterator aParamIter; // looks at first parameter of already existent entity or at the end of vector otherwise
  if (aEntIter == myEntityAttrMap.end()) // no such entity => should be created
    aParamIter = myParams.end();
  else
  { // the entity already exists
    int aEntPos = Search(aEntIter->second, myEntities);
    int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
    aParamIter = myParams.begin() + aParamPos;
  }
  const bool isEntExists = (aEntIter != myEntityAttrMap.end()); // defines that the entity already exists

  // Look over supported types of entities

  // Point in 3D
  boost::shared_ptr<GeomDataAPI_Point> aPoint =
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(theEntity);
  if (aPoint)
  {
    Slvs_hParam aX = changeParameter(aPoint->x(), aParamIter);
    Slvs_hParam aY = changeParameter(aPoint->y(), aParamIter);
    Slvs_hParam aZ = changeParameter(aPoint->z(), aParamIter);

    if (isEntExists)
      return aEntIter->second;

    // New entity
    Slvs_Entity aPtEntity = Slvs_MakePoint3d(++myEntityMaxID, myID, aX, aY, aZ);
    myEntities.push_back(aPtEntity);
    myEntityAttrMap[theEntity] = aPtEntity.h;
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

    if (isEntExists)
      return aEntIter->second;

    // New entity
    Slvs_Entity aPt2DEntity = Slvs_MakePoint2d(++myEntityMaxID, myID, myWorkplane.h, aU, aV);
    myEntities.push_back(aPt2DEntity);
    myEntityAttrMap[theEntity] = aPt2DEntity.h;
    return aPt2DEntity.h;
  }

  // Scalar value (used for the distance entities)
  AttributeDoublePtr aScalar = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
  if (aScalar)
  {
    Slvs_hParam aValue = changeParameter(aScalar->value(), aParamIter);

    if (isEntExists)
      return aEntIter->second;

    // New entity
    Slvs_Entity aDistance = Slvs_MakeDistance(++myEntityMaxID, myID, myWorkplane.h, aValue);
    myEntities.push_back(aDistance);
    myEntityAttrMap[theEntity] = aDistance.h;
    return aDistance.h;
  }

  /// \todo Other types of entities

  // Unsupported or wrong entity type
  return SLVS_E_UNKNOWN;
}


// ============================================================================
//  Function: changeEntity
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the element defined by the feature affected by any constraint
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintGroup::changeEntity(
                FeaturePtr theEntity)
{
  // If the entity is already in the group, try to find it
  std::map<FeaturePtr, Slvs_hEntity>::const_iterator
    aEntIter = myEntityFeatMap.find(theEntity);
  // defines that the entity already exists
  const bool isEntExists = (myEntityFeatMap.find(theEntity) != myEntityFeatMap.end());

  // SketchPlugin features
  boost::shared_ptr<SketchPlugin_Feature> aFeature =
    boost::dynamic_pointer_cast<SketchPlugin_Feature>(theEntity);
  if (aFeature)
  { // Verify the feature by its kind
    const std::string& aFeatureKind = aFeature->getKind();

    // Line
    if (aFeatureKind.compare(SketchPlugin_Line::ID()) == 0)
    {
      Slvs_hEntity aStart = changeEntity(aFeature->data()->attribute(SketchPlugin_Line::START_ID()));
      Slvs_hEntity aEnd   = changeEntity(aFeature->data()->attribute(SketchPlugin_Line::END_ID()));

      if (isEntExists)
        return aEntIter->second;

      // New entity
      Slvs_Entity aLineEntity = Slvs_MakeLineSegment(++myEntityMaxID, myID, myWorkplane.h, aStart, aEnd);
      myEntities.push_back(aLineEntity);
      myEntityFeatMap[theEntity] = aLineEntity.h;
      return aLineEntity.h;
    }
    // Circle
    else if (aFeatureKind.compare(SketchPlugin_Circle::ID()) == 0)
    {
      Slvs_hEntity aCenter = changeEntity(aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));
      Slvs_hEntity aRadius = changeEntity(aFeature->data()->attribute(SketchPlugin_Circle::RADIUS_ID()));

      if (isEntExists)
        return aEntIter->second;

      // New entity
      Slvs_Entity aCircleEntity = 
        Slvs_MakeCircle(++myEntityMaxID, myID, myWorkplane.h, aCenter, myWorkplane.normal, aRadius);
      myEntities.push_back(aCircleEntity);
      myEntityFeatMap[theEntity] = aCircleEntity.h;
      return aCircleEntity.h;
    }
    // Arc
    else if (aFeatureKind.compare(SketchPlugin_Arc::ID()) == 0)
    {
      Slvs_hEntity aCenter = changeEntity(aFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
      Slvs_hEntity aStart  = changeEntity(aFeature->data()->attribute(SketchPlugin_Arc::START_ID()));
      Slvs_hEntity aEnd    = changeEntity(aFeature->data()->attribute(SketchPlugin_Arc::END_ID()));

      if (isEntExists)
        return aEntIter->second;

      Slvs_Entity anArcEntity = Slvs_MakeArcOfCircle(++myEntityMaxID, myID, 
                                  myWorkplane.h, myWorkplane.normal, aCenter, aStart, aEnd);
      myEntities.push_back(anArcEntity);
      myEntityFeatMap[theEntity] = anArcEntity.h;
      return anArcEntity.h;
    }
    // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
    else if (aFeatureKind.compare(SketchPlugin_Point::ID()) == 0)
    {
      Slvs_hEntity aPoint = changeEntity(aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));

      if (isEntExists)
        return aEntIter->second;

      // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
      myEntityFeatMap[theEntity] = aPoint;
      return aPoint;
    }
  }

  /// \todo Other types of features

  // Unsupported or wrong entity type
  return SLVS_E_UNKNOWN;
}

// ============================================================================
//  Function: changeNormal
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the normal of workplane
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintGroup::changeNormal(
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
    aEntIter = myEntityAttrMap.find(theNorm);
  std::vector<Slvs_Param>::const_iterator aParamIter; // looks to the first parameter of already existent entity or to the end of vector otherwise
  if (aEntIter == myEntityAttrMap.end()) // no such entity => should be created
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

  if (aEntIter != myEntityAttrMap.end()) // the entity already exists
    return aEntIter->second;

  // Create a normal
  Slvs_Entity aNormal = Slvs_MakeNormal3d(++myEntityMaxID, myID,
                aNormParams[0], aNormParams[1], aNormParams[2], aNormParams[3]);
  myEntities.push_back(aNormal);
  myEntityAttrMap[theNorm] = aNormal.h;
  return aNormal.h;
}


// ============================================================================
//  Function: addWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create workplane for the group
// ============================================================================
bool SketchSolver_ConstraintGroup::addWorkplane(
                boost::shared_ptr<SketchPlugin_Feature> theSketch)
{
  if (myWorkplane.h || theSketch->getKind().compare(SketchPlugin_Sketch::ID()) != 0)
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
bool SketchSolver_ConstraintGroup::updateWorkplane()
{
  // Get parameters of workplane
  boost::shared_ptr<ModelAPI_Attribute> aDirX    = mySketch->data()->attribute(SketchPlugin_Sketch::DIRX_ID());
  boost::shared_ptr<ModelAPI_Attribute> aDirY    = mySketch->data()->attribute(SketchPlugin_Sketch::DIRY_ID());
  boost::shared_ptr<ModelAPI_Attribute> aNorm    = mySketch->data()->attribute(SketchPlugin_Sketch::NORM_ID());
  boost::shared_ptr<ModelAPI_Attribute> anOrigin = mySketch->data()->attribute(SketchPlugin_Sketch::ORIGIN_ID());
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
Slvs_hParam SketchSolver_ConstraintGroup::changeParameter(
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
void SketchSolver_ConstraintGroup::resolveConstraints()
{
  if (!myNeedToSolve)
    return;

  myConstrSolver.setGroupID(myID);
  myConstrSolver.setParameters(myParams);
  myConstrSolver.setEntities(myEntities);
  myConstrSolver.setConstraints(myConstraints);
  myConstrSolver.setDraggedParameters(myTempPointWhereDragged);

  int aResult = myConstrSolver.solve();
  if (aResult == SLVS_RESULT_OKAY)
  { // solution succeeded, store results into correspondent attributes
    // Obtain result into the same list of parameters
    if (!myConstrSolver.getResult(myParams))
      return;

    // We should go through the attributes map, because only attributes have valued parameters
    std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
      anEntIter = myEntityAttrMap.begin();
    for ( ; anEntIter != myEntityAttrMap.end(); anEntIter++)
      if (updateAttribute(anEntIter->first, anEntIter->second))
        updateRelatedConstraints(anEntIter->first);
  }
  else if (!myConstraints.empty())
    Events_Error::send(SketchSolver_Error::CONSTRAINTS(), this);

  removeTemporaryConstraints();
  myNeedToSolve = false;
}

// ============================================================================
//  Function: mergeGroups
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  append specified group to the current group
// ============================================================================
void SketchSolver_ConstraintGroup::mergeGroups(
                const SketchSolver_ConstraintGroup& theGroup)
{
  // If specified group is empty, no need to merge
  if (theGroup.myConstraintMap.empty())
    return ;

  // Map between old and new indexes of SolveSpace constraints
  std::map<Slvs_hConstraint, Slvs_hConstraint> aConstrMap;

  // Add all constraints from theGroup to the current group
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::const_iterator
    aConstrIter = theGroup.myConstraintMap.begin();
  for ( ; aConstrIter != theGroup.myConstraintMap.end(); aConstrIter++)
    if (changeConstraint(aConstrIter->first))
      aConstrMap[aConstrIter->second] = myConstrMaxID; // the constraint was added => store its ID

  // Add temporary constraints from theGroup
  std::list<Slvs_hConstraint>::const_iterator aTempConstrIter = theGroup.myTempConstraints.begin();
  for ( ; aTempConstrIter != theGroup.myTempConstraints.end(); aTempConstrIter++)
  {
    std::map<Slvs_hConstraint, Slvs_hConstraint>::iterator aFind = aConstrMap.find(*aTempConstrIter);
    if (aFind != aConstrMap.end())
      myTempConstraints.push_back(aFind->second);
  }

  if (myTempPointWhereDragged.empty())
    myTempPointWhereDragged = theGroup.myTempPointWhereDragged;
  else if (!theGroup.myTempPointWhereDragged.empty())
  { // Need to create additional transient constraint
    std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
      aFeatureIter = theGroup.myEntityAttrMap.begin();
    for (; aFeatureIter != theGroup.myEntityAttrMap.end(); aFeatureIter++)
      if (aFeatureIter->second == myTempPointWDrgdID)
      {
        addTemporaryConstraintWhereDragged(aFeatureIter->first);
        break;
      }
  }

  myNeedToSolve = myNeedToSolve || theGroup.myNeedToSolve;
}

// ============================================================================
//  Function: splitGroup
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  divide the group into several subgroups
// ============================================================================
void SketchSolver_ConstraintGroup::splitGroup(std::vector<SketchSolver_ConstraintGroup*>& theCuts)
{
  // Divide constraints and entities into several groups
  std::vector< std::set<Slvs_hEntity> >     aGroupsEntities;
  std::vector< std::set<Slvs_hConstraint> > aGroupsConstr;
  int aMaxNbEntities = 0; // index of the group with maximal nuber of elements (this group will be left in the current)
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for ( ; aConstrIter != myConstraints.end(); aConstrIter++)
  {
    Slvs_hEntity aConstrEnt[] = {
      aConstrIter->ptA,     aConstrIter->ptB,
      aConstrIter->entityA, aConstrIter->entityB};
    std::vector<int> anIndexes;
    // Go through the groupped entities and find even one of entities of current constraint
    std::vector< std::set<Slvs_hEntity> >::iterator aGrEntIter;
    for (aGrEntIter = aGroupsEntities.begin(); aGrEntIter != aGroupsEntities.end(); aGrEntIter++)
    {
      bool isFound = false;
      for (int i = 0; i < 4 && !isFound; i++)
        if (aConstrEnt[i] != 0)
          isFound = (aGrEntIter->find(aConstrEnt[i]) != aGrEntIter->end());
      if (isFound)
        anIndexes.push_back(aGrEntIter - aGroupsEntities.begin());
    }
    // Add new group if no one is found
    if (anIndexes.empty())
    {
      std::set<Slvs_hEntity> aNewGrEnt;
      for (int i = 0; i < 4; i++)
        if (aConstrEnt[i] != 0)
          aNewGrEnt.insert(aConstrEnt[i]);
      std::set<Slvs_hConstraint> aNewGrConstr;
      aNewGrConstr.insert(aConstrIter->h);

      aGroupsEntities.push_back(aNewGrEnt);
      aGroupsConstr.push_back(aNewGrConstr);
      if (aNewGrEnt.size() > aGroupsEntities[aMaxNbEntities].size())
        aMaxNbEntities = aGroupsEntities.size() - 1;
    }
    else if (anIndexes.size() == 1)
    { // Add entities indexes into the found group
      aGrEntIter = aGroupsEntities.begin() + anIndexes.front();
      for (int i = 0; i < 4; i++)
        if (aConstrEnt[i] != 0)
          aGrEntIter->insert(aConstrEnt[i]);
      aGroupsConstr[anIndexes.front()].insert(aConstrIter->h);
      if (aGrEntIter->size() > aGroupsEntities[aMaxNbEntities].size())
        aMaxNbEntities = aGrEntIter - aGroupsEntities.begin();
    }
    else 
    { // There are found several connected groups, merge them
      std::vector< std::set<Slvs_hEntity> >::iterator aFirstGroup = 
        aGroupsEntities.begin() + anIndexes.front();
      std::vector< std::set<Slvs_hConstraint> >::iterator aFirstConstr = 
        aGroupsConstr.begin() + anIndexes.front();
      std::vector<int>::iterator anInd = anIndexes.begin();
      for (++anInd; anInd != anIndexes.end(); anInd++)
      {
        aFirstGroup->insert(aGroupsEntities[*anInd].begin(), aGroupsEntities[*anInd].end());
        aFirstConstr->insert(aGroupsConstr[*anInd].begin(), aGroupsConstr[*anInd].end());
      }
      if (aFirstGroup->size() > aGroupsEntities[aMaxNbEntities].size())
        aMaxNbEntities = anIndexes.front();
      // Remove merged groups
      for (anInd = anIndexes.end() - 1; anInd != anIndexes.begin(); anInd--)
      {
        aGroupsEntities.erase(aGroupsEntities.begin() + (*anInd));
        aGroupsConstr.erase(aGroupsConstr.begin() + (*anInd));
      }
    }
  }

  if (aGroupsEntities.size() <= 1)
    return ;

  // Remove the group with maximum elements as it will be left in the current group
  aGroupsEntities.erase(aGroupsEntities.begin() + aMaxNbEntities);
  aGroupsConstr.erase(aGroupsConstr.begin() + aMaxNbEntities);

  // Add new groups of constraints and divide current group
  std::vector<SketchSolver_ConstraintGroup*> aNewGroups;
  for (int i = aGroupsEntities.size(); i > 0; i--)
  {
    SketchSolver_ConstraintGroup* aG = new SketchSolver_ConstraintGroup(mySketch);
    aNewGroups.push_back(aG);
  }
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::const_iterator
    aConstrMapIter = myConstraintMap.begin();
  int aConstrMapPos = 0; // position of iterator in the map (used to restore iterator after removing constraint)
  while (aConstrMapIter != myConstraintMap.end())
  {
    std::vector< std::set<Slvs_hConstraint> >::const_iterator aGIter = aGroupsConstr.begin();
    std::vector<SketchSolver_ConstraintGroup*>::iterator aGroup = aNewGroups.begin();
    for ( ; aGIter != aGroupsConstr.end(); aGIter++, aGroup++)
      if (aGIter->find(aConstrMapIter->second) != aGIter->end())
      {
        (*aGroup)->changeConstraint(aConstrMapIter->first);
        removeConstraint(aConstrMapIter->first);
        // restore iterator
        aConstrMapIter = myConstraintMap.begin();
        for (int i = 0; i < aConstrMapPos; i++)
          aConstrMapIter++;
        break;
      }
    if (aGIter == aGroupsConstr.end())
    {
      aConstrMapIter++;
      aConstrMapPos++;
    }
  }

  theCuts.insert(theCuts.end(), aNewGroups.begin(), aNewGroups.end());
}

// ============================================================================
//  Function: updateGroup
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  search removed entities and constraints
// ============================================================================
bool SketchSolver_ConstraintGroup::updateGroup()
{
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::reverse_iterator
    aConstrIter = myConstraintMap.rbegin();
  bool isAllValid = true;
  bool isCCRemoved = false; // indicates that at least one of coincidence constraints was removed
  while (isAllValid && aConstrIter != myConstraintMap.rend())
  {
    if (!aConstrIter->first->data()->isValid())
    {
      if (aConstrIter->first->getKind().compare(SketchPlugin_ConstraintCoincidence::ID()) == 0)
        isCCRemoved = true;
      std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::reverse_iterator
        aCopyIter = aConstrIter++;
      removeConstraint(aCopyIter->first);
      isAllValid = false;
    }
    else aConstrIter++;
  }

  // Probably, need to update coincidence constraints
  if (isCCRemoved && !myExtraCoincidence.empty())
  {
    // Make a copy, because the new list of unused constrtaints will be generated
    std::set< boost::shared_ptr<SketchPlugin_Constraint> > anExtraCopy = myExtraCoincidence;
    myExtraCoincidence.clear();

    std::set< boost::shared_ptr<SketchPlugin_Constraint> >::iterator
      aCIter = anExtraCopy.begin();
    for ( ; aCIter != anExtraCopy.end(); aCIter++)
      if ((*aCIter)->data()->isValid())
        changeConstraint(*aCIter);
  }

  return !isAllValid;
}

// ============================================================================
//  Function: updateAttribute
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  update features of sketch after resolving constraints
// ============================================================================
bool SketchSolver_ConstraintGroup::updateAttribute(
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
    if (fabs(aPoint->x() - myParams[aFirstParamPos].val) > tolerance ||
        fabs(aPoint->y() - myParams[aFirstParamPos+1].val) > tolerance ||
        fabs(aPoint->z() - myParams[aFirstParamPos+2].val) > tolerance)
    {
      aPoint->setValue(myParams[aFirstParamPos].val,
                       myParams[aFirstParamPos+1].val,
                       myParams[aFirstParamPos+2].val);
      return true;
    }
    return false;
  }

  // Point in 2D
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D)
  {
    if (fabs(aPoint2D->x() - myParams[aFirstParamPos].val) > tolerance ||
        fabs(aPoint2D->y() - myParams[aFirstParamPos+1].val) > tolerance)
    {
      aPoint2D->setValue(myParams[aFirstParamPos].val,
                         myParams[aFirstParamPos+1].val);
      return true;
    }
    return false;
  }

  // Scalar value
  AttributeDoublePtr aScalar = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aScalar)
  {
    if (fabs(aScalar->value() - myParams[aFirstParamPos].val) > tolerance)
    {
      aScalar->setValue(myParams[aFirstParamPos].val);
      return true;
    }
    return false;
  }

  /// \todo Support other types of entities
  return false;
}

// ============================================================================
//  Function: updateEntityIfPossible
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  search the entity in this group and update it
// ============================================================================
void SketchSolver_ConstraintGroup::updateEntityIfPossible(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  if (myEntityAttrMap.find(theEntity) != myEntityAttrMap.end())
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

    if (myNeedToSolve)
      updateRelatedConstraints(theEntity);
  }
}

// ============================================================================
//  Function: addTemporaryConstraintWhereDragged
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  add transient constraint SLVS_C_WHERE_DRAGGED for the entity, 
//            which was moved by user
// ============================================================================
void SketchSolver_ConstraintGroup::addTemporaryConstraintWhereDragged(
                boost::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // Find identifier of the entity
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator
    anEntIter = myEntityAttrMap.find(theEntity);
  if (anEntIter == myEntityAttrMap.end())
    return ;

  // If this is a first dragged point, its parameters should be placed 
  // into Slvs_System::dragged field to avoid system inconsistense
  if (myTempPointWhereDragged.empty())
  {
    int anEntPos = Search(anEntIter->second, myEntities);
    Slvs_hParam* aDraggedParam = myEntities[anEntPos].param;
    for (int i = 0; i < 4; i++, aDraggedParam++)
      if (*aDraggedParam != 0)
        myTempPointWhereDragged.push_back(*aDraggedParam);
    myTempPointWDrgdID = myEntities[anEntPos].h;
    return ;
  }

  // Get identifiers of all dragged points
  std::set<Slvs_hEntity> aDraggedPntID;
  aDraggedPntID.insert(myTempPointWDrgdID);
  std::list<Slvs_hConstraint>::iterator aTmpCoIter = myTempConstraints.begin();
  for ( ; aTmpCoIter != myTempConstraints.end(); aTmpCoIter++)
  {
    unsigned int aConstrPos = Search(*aTmpCoIter, myConstraints);
    if (aConstrPos < myConstraints.size())
      aDraggedPntID.insert(myConstraints[aConstrPos].ptA);
  }
  // Find whether there is a point coincident with theEntity, which already has SLVS_C_WHERE_DRAGGED
  std::vector< std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
  for ( ; aCoPtIter != myCoincidentPoints.end(); aCoPtIter++)
  {
    if (aCoPtIter->find(anEntIter->second) == aCoPtIter->end())
      continue; // the entity was not found in current set

    // Find one of already created SLVS_C_WHERE_DRAGGED constraints in current set of coincident points
    std::set<Slvs_hEntity>::const_iterator aDrgIter = aDraggedPntID.begin();
    for ( ; aDrgIter != aDraggedPntID.end(); aDrgIter++)
      if (aCoPtIter->find(*aDrgIter) != aCoPtIter->end())
        return ; // the SLVS_C_WHERE_DRAGGED constraint already exists
  }

  // Create additional SLVS_C_WHERE_DRAGGED constraint if myTempPointWhereDragged field is not empty
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
void SketchSolver_ConstraintGroup::removeTemporaryConstraints()
{
  std::list<Slvs_hConstraint>::reverse_iterator aTmpConstrIter;
  for (aTmpConstrIter = myTempConstraints.rbegin(); aTmpConstrIter != myTempConstraints.rend(); aTmpConstrIter++)
  {
    unsigned int aConstrPos = Search(*aTmpConstrIter, myConstraints);
    if (aConstrPos >= myConstraints.size())
      continue;
    myConstraints.erase(myConstraints.begin() + aConstrPos);

    // If the removing constraint has higher index, decrease the indexer
    if (*aTmpConstrIter == myConstrMaxID)
      myConstrMaxID--;
  }
  myTempConstraints.clear();

  // Clear basic dragged point
  myTempPointWhereDragged.clear();
}

// ============================================================================
//  Function: removeConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  remove constraint and all unused entities
// ============================================================================
void SketchSolver_ConstraintGroup::removeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::iterator
    anIterToRemove = myConstraintMap.find(theConstraint);
  if (anIterToRemove == myConstraintMap.end())
     return ;

  Slvs_hConstraint aCnstrToRemove = anIterToRemove->second;
  // Remove constraint from the map
  myConstraintMap.erase(anIterToRemove);

  // Find unused entities
  int aConstrPos = Search(aCnstrToRemove, myConstraints);
  std::set<Slvs_hEntity> anEntToRemove;
  Slvs_hEntity aCnstEnt[] = {myConstraints[aConstrPos].ptA,     myConstraints[aConstrPos].ptB, 
                             myConstraints[aConstrPos].entityA, myConstraints[aConstrPos].entityB};
  for (int i = 0; i < 4; i++)
    if (aCnstEnt[i] != 0)
      anEntToRemove.insert(aCnstEnt[i]);
  myConstraints.erase(myConstraints.begin() + aConstrPos);
  if (aCnstrToRemove == myConstrMaxID)
    myConstrMaxID--;
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for ( ; aConstrIter != myConstraints.end(); aConstrIter++)
  {
    Slvs_hEntity aEnts[] = {aConstrIter->ptA,     aConstrIter->ptB, 
                            aConstrIter->entityA, aConstrIter->entityB};
    for (int i = 0; i < 4; i++)
      if (aEnts[i] != 0 && anEntToRemove.find(aEnts[i]) != anEntToRemove.end())
        anEntToRemove.erase(aEnts[i]);
  }

  if (anEntToRemove.empty())
    return ;

  // Remove unused entities
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
    anEntAttrIter = myEntityAttrMap.begin();
  while (anEntAttrIter != myEntityAttrMap.end())
  {
    if (anEntToRemove.find(anEntAttrIter->second) != anEntToRemove.end())
    {
      std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
        aRemovedIter = anEntAttrIter;
      anEntAttrIter++;
      myEntityAttrMap.erase(aRemovedIter);
    }
    else anEntAttrIter++;
  }
  std::map<FeaturePtr, Slvs_hEntity>::iterator
    anEntFeatIter = myEntityFeatMap.begin();
  while (anEntFeatIter != myEntityFeatMap.end())
  {
    if (anEntToRemove.find(anEntFeatIter->second) != anEntToRemove.end())
    {
      std::map<FeaturePtr, Slvs_hEntity>::iterator
        aRemovedIter = anEntFeatIter;
      anEntFeatIter++;
      myEntityFeatMap.erase(aRemovedIter);
    }
    else anEntFeatIter++;
  }
  std::set<Slvs_hEntity>::const_reverse_iterator aRemIter = anEntToRemove.rbegin();
  for ( ; aRemIter != anEntToRemove.rend(); aRemIter++)
  {
    unsigned int anEntPos = Search(*aRemIter, myEntities);
    if (anEntPos >= myEntities.size())
      continue;
    unsigned int aParamPos = Search(myEntities[anEntPos].param[0], myParams);
    if (aParamPos >= myParams.size())
      continue;
    int aNbParams = 0;
    while (myEntities[anEntPos].param[aNbParams] != 0) 
      aNbParams++;
    if (myEntities[anEntPos].param[aNbParams-1] == myParamMaxID)
      myParamMaxID -= aNbParams;
    myParams.erase(myParams.begin() + aParamPos, myParams.begin() + aParamPos + aNbParams);
    if (*aRemIter == myEntityMaxID)
      myEntityMaxID--;
    myEntities.erase(myEntities.begin() + anEntPos);

    // Remove entity's ID from the lists of conincident points
    std::vector< std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
    for ( ; aCoPtIter != myCoincidentPoints.end(); aCoPtIter++)
      aCoPtIter->erase(*aRemIter);
  }
  if (myCoincidentPoints.size() == 1 && myCoincidentPoints.front().empty())
    myCoincidentPoints.clear();
}


// ============================================================================
//  Function: addCoincidentPoints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  add coincident point the appropriate list of such points
// ============================================================================
bool SketchSolver_ConstraintGroup::addCoincidentPoints(
                const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2)
{
  std::vector< std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
  std::vector< std::set<Slvs_hEntity> >::iterator aFirstFound = myCoincidentPoints.end();
  while (aCoPtIter != myCoincidentPoints.end())
  {
    bool isFound[2] = { // indicate which point ID was already in coincidence constraint
      aCoPtIter->find(thePoint1) != aCoPtIter->end(),
      aCoPtIter->find(thePoint2) != aCoPtIter->end(),
    };
    if (isFound[0] && isFound[1]) // points are already connected by coincidence constraints => no need additional one
      return false;
    if ((isFound[0] && !isFound[1]) || (!isFound[0] && isFound[1]))
    {
      if (aFirstFound != myCoincidentPoints.end())
      { // there are two groups of coincident points connected by created constraint => merge them
        int aFirstFoundShift = aFirstFound - myCoincidentPoints.begin();
        int aCurrentShift = aCoPtIter - myCoincidentPoints.begin();
        aFirstFound->insert(aCoPtIter->begin(), aCoPtIter->end());
        myCoincidentPoints.erase(aCoPtIter);
        aFirstFound = myCoincidentPoints.begin() + aFirstFoundShift;
        aCoPtIter = myCoincidentPoints.begin() + aCurrentShift;
        continue;
      }
      else
      {
        aCoPtIter->insert(isFound[0] ? thePoint2 : thePoint1);
        aFirstFound = aCoPtIter;
      }
    }
    aCoPtIter++;
  }
  // No points were found, need to create new set
  if (aFirstFound == myCoincidentPoints.end())
  {
    std::set<Slvs_hEntity> aNewSet;
    aNewSet.insert(thePoint1);
    aNewSet.insert(thePoint2);
    myCoincidentPoints.push_back(aNewSet);
  }

  return true;
}


// ============================================================================
//  Function: updateRelatedConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  emit the signal to update constraints
// ============================================================================
void SketchSolver_ConstraintGroup::updateRelatedConstraints(
                    boost::shared_ptr<ModelAPI_Attribute> theEntity) const
{
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::const_iterator
    aConstrIter = myConstraintMap.begin();
  for ( ; aConstrIter != myConstraintMap.end(); aConstrIter++)
  {
    std::list< boost::shared_ptr<ModelAPI_Attribute> > anAttributes = 
      aConstrIter->first->data()->attributes(std::string());

    std::list< boost::shared_ptr<ModelAPI_Attribute> >::iterator
      anAttrIter = anAttributes.begin();
    for ( ; anAttrIter != anAttributes.end(); anAttrIter++)
    {
      bool isUpd = (*anAttrIter == theEntity);
      boost::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIter);
      if (aRefAttr && !aRefAttr->isObject() && aRefAttr->attr() == theEntity)
        isUpd = true;

      if (isUpd)
      {
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(aConstrIter->first, anEvent);
        break;
      }
    }
  }
}

void SketchSolver_ConstraintGroup::updateRelatedConstraints(
                    boost::shared_ptr<ModelAPI_Feature> theFeature) const
{
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint>::const_iterator
    aConstrIter = myConstraintMap.begin();
  for ( ; aConstrIter != myConstraintMap.end(); aConstrIter++)
  {
    std::list< boost::shared_ptr<ModelAPI_Attribute> > anAttributes = 
      aConstrIter->first->data()->attributes(std::string());

    std::list< boost::shared_ptr<ModelAPI_Attribute> >::iterator
      anAttrIter = anAttributes.begin();
    for ( ; anAttrIter != anAttributes.end(); anAttrIter++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIter);
      if (aRefAttr && aRefAttr->isObject() && aRefAttr->object() == theFeature)
      {
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(aConstrIter->first, anEvent);
        break;
      }
    }
  }
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
  while (aResIndex < aVecSize && aResIndex >= 0 && theEntities[aResIndex].h < theEntityID)
    aResIndex++;
  if (aResIndex == -1)
    aResIndex = aVecSize;
  return aResIndex;
}
