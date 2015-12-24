// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Storage.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Builder.h>
#include <PlaneGCSSolver_Solver.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EntityWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ParameterWrapper.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
////#include <ModelAPI_AttributeRefAttr.h>
////#include <ModelAPI_Feature.h>
////#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Arc.h>
////#include <SketchPlugin_Circle.h>
////#include <SketchPlugin_Line.h>
////#include <SketchPlugin_Point.h>
////
////#ifndef OBSOLETE
////#include <SketchPlugin_ConstraintAngle.h>
////#endif

#include <cmath>

////#ifdef OBSOLETE
/////** \brief Search the entity/parameter with specified ID in the list of elements
//// *  \param[in] theEntityID unique ID of the element
//// *  \param[in] theEntities list of elements
//// *  \return position of the found element or -1 if the element is not found
//// */
////template<typename T>
////static int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities);
////
/////// \brief Compare two parameters to be different
////static bool IsNotEqual(const Slvs_Param& theParam1, const Slvs_Param& theParam2);
/////// \brief Compare two entities to be different
////static bool IsNotEqual(const Slvs_Entity& theEntity1, const Slvs_Entity& theEntity2);
/////// \brief Compare two constriants to be different
////static bool IsNotEqual(const Slvs_Constraint& theConstraint1, const Slvs_Constraint& theConstraint2);
////#endif


PlaneGCSSolver_Storage::PlaneGCSSolver_Storage(const GroupID& theGroup)
  : SketchSolver_Storage(theGroup),
    myEntityLastID(EID_SKETCH)
{
}

void PlaneGCSSolver_Storage::addConstraint(
    ConstraintPtr                   theConstraint,
    std::list<ConstraintWrapperPtr> theSolverConstraints)
{
  SketchSolver_Storage::addConstraint(theConstraint, theSolverConstraints);

  // update point-point coincidence
  if (!theSolverConstraints.empty() &&
      theSolverConstraints.front()->type() == CONSTRAINT_PT_PT_COINCIDENT) {
    std::list<ConstraintWrapperPtr>::iterator aCIt = theSolverConstraints.begin();
    for (; aCIt != theSolverConstraints.end(); ++aCIt)
      update(*aCIt);
  }
}


bool PlaneGCSSolver_Storage::update(ConstraintWrapperPtr theConstraint)
{
  bool isUpdated = false;
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aConstraint =
      std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(theConstraint);

  // point-Line distance should be positive
  if (aConstraint->type() == CONSTRAINT_PT_LINE_DISTANCE && aConstraint->value() < 0.0)
    aConstraint->setValue(-aConstraint->value());

  // make value of constraint unchangeable
  ParameterWrapperPtr aValue = aConstraint->valueParameter();
  if (aValue)
    isUpdated = update(aValue) || isUpdated;

  // update constrained entities
  std::list<EntityWrapperPtr> anEntities = theConstraint->entities();
  std::list<EntityWrapperPtr>::iterator anIt = anEntities.begin();
  for (; anIt != anEntities.end(); ++anIt)
    isUpdated = update(*anIt) || isUpdated;

  return isUpdated;
}

/// \brief Update coordinates of the point or scalar using its base attribute
static bool updateValues(EntityWrapperPtr& theEntity)
{
  bool isUpdated = false;
  AttributePtr anAttr = theEntity->baseAttribute();
  const std::list<ParameterWrapperPtr> aParams = theEntity->parameters();

  double aCoord[2];

  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
  if (aPoint2D) {
    aCoord[0] = aPoint2D->x();
    aCoord[1] = aPoint2D->y();
  } else {
    AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
    if (aScalar)
      aCoord[0] = aScalar->value();
  }

  std::list<ParameterWrapperPtr>::const_iterator anIt = aParams.begin();
  for (int i = 0; anIt != aParams.end(); ++anIt, ++i)
    if (fabs((*anIt)->value() - aCoord[i]) > tolerance) {
      (*anIt)->setValue(aCoord[i]);
      isUpdated = true;
    }
  return isUpdated;
}

bool PlaneGCSSolver_Storage::update(EntityWrapperPtr theEntity)
{
  if (theEntity->type() == ENTITY_SKETCH)
    return true; // sketch is not necessary for PlaneGCS, so it is always says true

  bool isUpdated = false;

  if (theEntity->baseAttribute()) {
    isUpdated = updateValues(theEntity);
    if (isUpdated)
      setNeedToResolve(true);
  }

  // update parameters
  std::list<ParameterWrapperPtr> aParams = theEntity->parameters();
  std::list<ParameterWrapperPtr>::iterator aPIt = aParams.begin();
  for (; aPIt != aParams.end(); ++aPIt)
    isUpdated = update(*aPIt) || isUpdated;

  // update sub-entities
  std::list<EntityWrapperPtr> aSubEntities = theEntity->subEntities();
  std::list<EntityWrapperPtr>::iterator aSIt = aSubEntities.begin();
  for (; aSIt != aSubEntities.end(); ++aSIt)
    isUpdated = update(*aSIt) || isUpdated;

  // additional constraints for the arc processing
  if (theEntity->type() == ENTITY_ARC)
    processArc(theEntity);

  // Change entity's ID, if necessary
  if (theEntity->id() == EID_UNKNOWN) {
    if (theEntity->type() == ENTITY_POINT) {
      std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
      if (!aPoint) {
        aPoint = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(
            theEntity->subEntities().front());
      }
      aPoint->setId(++myEntityLastID);
    } else if (theEntity->type() == ENTITY_SCALAR) {
      std::shared_ptr<PlaneGCSSolver_ScalarWrapper> aScalar =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(theEntity);
      aScalar->setId(++myEntityLastID);
    } else {
      std::shared_ptr<PlaneGCSSolver_EntityWrapper> aGCSEnt =
          std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theEntity);
      aGCSEnt->setId(++myEntityLastID);
    }
  }
  return isUpdated;
}

bool PlaneGCSSolver_Storage::update(ParameterWrapperPtr theParameter)
{
  std::shared_ptr<PlaneGCSSolver_ParameterWrapper> aParam =
      std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(theParameter);
  if (aParam->isProcessed())
    return false;
  if (theParameter->group() != myGroupID || theParameter->isParametric())
    myConst.push_back(aParam->parameter());
  else
    myParameters.push_back(aParam->parameter());
  aParam->setProcessed(true);
  return true;
}


bool PlaneGCSSolver_Storage::remove(ConstraintWrapperPtr theConstraint)
{
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aConstraint =
    std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(theConstraint);

  bool isFullyRemoved = true;
  const std::list<EntityWrapperPtr>& aSubs = aConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++ aSIt)
    isFullyRemoved = remove(*aSIt) && isFullyRemoved;

  if (aConstraint->valueParameter())
    isFullyRemoved = remove(aConstraint->valueParameter()) && isFullyRemoved;
  if (!isFullyRemoved &&
     (!aConstraint->baseConstraint()->data() || !aConstraint->baseConstraint()->data()->isValid()))
    isFullyRemoved = true;
  if (isFullyRemoved) {
    setNeedToResolve(true);
    myRemovedConstraints.insert(myRemovedConstraints.end(),
        aConstraint->constraints().begin(), aConstraint->constraints().end());
  }
  return isFullyRemoved;
}

bool PlaneGCSSolver_Storage::remove(EntityWrapperPtr theEntity)
{
  if ((theEntity->baseAttribute() && isUsed(theEntity->baseAttribute())) ||
      (theEntity->baseFeature() && isUsed(theEntity->baseFeature())))
    return false;

  bool isFullyRemoved = SketchSolver_Storage::remove(theEntity);
  if (isFullyRemoved && theEntity->id() == myEntityLastID)
    --myEntityLastID;
  return isFullyRemoved;
}

bool PlaneGCSSolver_Storage::remove(ParameterWrapperPtr theParameter)
{
  std::shared_ptr<PlaneGCSSolver_ParameterWrapper> aParam =
      std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(theParameter);
  if (aParam->isProcessed()) {
    double* aValPtr = aParam->parameter();
    GCS::VEC_pD::iterator anIt =  myParameters.begin();
    for (; anIt != myParameters.end(); ++anIt)
      if (*anIt == aValPtr)
        break;
    if (anIt != myParameters.end()) {
      myParameters.erase(anIt);
      setNeedToResolve(true);
    }
    else {
      for (anIt = myConst.begin(); anIt != myConst.end(); ++anIt)
        if (*anIt == aValPtr)
          break;
      if (anIt != myConst.end()) {
        myConst.erase(anIt);
        setNeedToResolve(true);
      }
    }
  }
  aParam->setProcessed(false);
  return true;
}


void PlaneGCSSolver_Storage::addCoincidentPoints(
    EntityWrapperPtr theMaster, EntityWrapperPtr theSlave)
{
  if (theMaster->type() != ENTITY_POINT || theSlave->type() != ENTITY_POINT)
    return;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aMaster =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theMaster);
  if (!aMaster)
    aMaster = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(
      std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theMaster)->subEntities().front());
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aSlave =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theSlave);
  if (!aSlave)
    aSlave = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(
      std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theSlave)->subEntities().front());

  // Search available coincidence
  CoincidentPointsMap::iterator aMasterFound = myCoincidentPoints.find(aMaster);
  CoincidentPointsMap::iterator aSlaveFound = myCoincidentPoints.find(aSlave);
  if (aMasterFound == myCoincidentPoints.end() &&  aSlaveFound == myCoincidentPoints.end()) {
    // try to find master and slave points in the lists of slaves of already existent coincidences
    CoincidentPointsMap::iterator anIt = myCoincidentPoints.begin();
    for (; anIt != myCoincidentPoints.end(); ++anIt) {
      if (anIt->second.find(aMaster) != anIt->second.end())
        aMasterFound = anIt;
      else if (anIt->second.find(aSlave) != anIt->second.end())
        aSlaveFound = anIt;

      if (aMasterFound != myCoincidentPoints.end() &&  aSlaveFound != myCoincidentPoints.end())
        break;
    }
  }

  if (aMasterFound == myCoincidentPoints.end()) {
    // create new group
    myCoincidentPoints[aMaster] = std::set<EntityWrapperPtr>();
    aMasterFound = myCoincidentPoints.find(aMaster);
  } else if (aMasterFound == aSlaveFound)
    return; // already coincident

  if (aSlaveFound != myCoincidentPoints.end()) {
    // A slave has been found, we need to attach all points coincident with it to the new master
    std::set<EntityWrapperPtr> aNewSlaves = aSlaveFound->second;
    aNewSlaves.insert(aSlaveFound->first);
    myCoincidentPoints.erase(aSlaveFound);

    std::set<EntityWrapperPtr>::const_iterator aSlIt = aNewSlaves.begin();
    for (; aSlIt != aNewSlaves.end(); ++aSlIt)
      addCoincidentPoints(aMaster, *aSlIt);
  } else {
    //std::list<ParameterWrapperPtr> aSlaveParams = aSlave->parameters();
    //aSlave->setParameters(aMaster->parameters());

    //// Remove slave's parameters
    //std::list<ParameterWrapperPtr>::iterator aParIt = aSlaveParams.begin();
    //for (; aParIt != aSlaveParams.end(); ++aParIt)
    //  remove(*aParIt);

    aMasterFound->second.insert(aSlave);
  }
}


void PlaneGCSSolver_Storage::changeGroup(EntityWrapperPtr theEntity, const GroupID& theGroup)
{
  theEntity->setGroup(theGroup);
  if (theGroup == myGroupID)
    makeVariable(theEntity);
  else
    makeConstant(theEntity);
}

void PlaneGCSSolver_Storage::changeGroup(ParameterWrapperPtr theParam, const GroupID& theGroup)
{
////  // TODO
}

void PlaneGCSSolver_Storage::verifyFixed()
{
////  // TODO
}


#ifndef OBSOLETE
////
////void PlaneGCSSolver_Storage::changeConstraint(
////    const ConstraintPtr& theConstraint,
////    const std::vector<GCSConstraintPtr>& theGCSConstraints)
////{
////  myConstraintsMap[theConstraint] = theGCSConstraints;
////}

////void PlaneGCSSolver_Storage::addConstraint(GCSConstraintPtr theConstraint)
////{
////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >::iterator
////      aFound = myConstraintsMap.find(ConstraintPtr());
////  if (aFound != myConstraintsMap.end())
////    aFound->second.push_back(theConstraint);
////  else {
////    std::vector<GCSConstraintPtr> aConstrVec(1, theConstraint);
////    myConstraintsMap[ConstraintPtr()] = aConstrVec;
////  }
////}

////EntityID PlaneGCSSolver_Storage::changeEntity(const FeaturePtr& theEntity)
////{
////  if (!theEntity || !theEntity->data() || !theEntity->data()->isValid())
////    return ENTITY_UNKNOWN;
////
////  // Firstly, try to find entity
////  std::map<FeaturePtr, EntityID>::const_iterator aFound = myFeatureEntityMap.find(theEntity);
////
////  EntityID aNewEntID;
////  if (aFound != myFeatureEntityMap.end())
////    aNewEntID = aFound->second;
////
////  const std::string& aFeatureKind = theEntity->getKind();
////  // SketchPlugin features
////  if (aFeatureKind == SketchPlugin_Line::ID())
////    updateLine(theEntity, aNewEntID);
////  // Circle
////  else if (aFeatureKind == SketchPlugin_Circle::ID())
////    updateCircle(theEntity, aNewEntID);
////  // Arc
////  else if (aFeatureKind == SketchPlugin_Arc::ID())
////    updateArc(theEntity, aNewEntID);
////  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
////  else if (aFeatureKind == SketchPlugin_Point::ID()) {
////    AttributePtr anAttribute = theEntity->attribute(SketchPlugin_Point::COORD_ID());
////    if (!anAttribute->isInitialized())
////      return ENTITY_UNKNOWN;
////    // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
////    aNewEntID = changeEntity(anAttribute);
////  }
////  else // NOTE: Other types of entities are unsupported
////    aNewEntID = ENTITY_UNKNOWN;
////
////  if (aNewEntID != ENTITY_UNKNOWN)
////    myFeatureEntityMap[theEntity] = aNewEntID;
////  return aNewEntID;
////}
////
////EntityID PlaneGCSSolver_Storage::changeEntity(const AttributePtr& theEntity)
////{
////  if (!theEntity)
////    return ENTITY_UNKNOWN;
////
////  AttributeRefAttrPtr aRefAttr =
////      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theEntity);
////  if (aRefAttr) {
////    if (aRefAttr->isObject()) {
////      ResultConstructionPtr aRC =
////          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aRefAttr->object());
////      if (!aRC)
////        return ENTITY_UNKNOWN;
////      return changeEntity(ModelAPI_Feature::feature(aRC));
////    }
////    else
////      return changeEntity(aRefAttr->attr());
////  }
////
////  EntityID aNewEntID = ENTITY_UNKNOWN;
////
////  // Firstly, try to find entity
////  std::map<AttributePtr, EntityID>::const_iterator aFound = myAttributeEntityMap.find(theEntity);
////  // Check type of attribute and create corresponding entity
////  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
////  if (aPoint2D) {
////    if (aFound != myAttributeEntityMap.end())
////      aNewEntID = aFound->second;
////    updatePoint(aPoint2D, aNewEntID);
////  } else {
////    AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
////    if (aScalar) {
////      // Check the scalar represents an angular value
////      bool isAngular = false;
////      FeaturePtr anOwner = ModelAPI_Feature::feature(aScalar->owner());
////      if (anOwner && anOwner->getKind() == SketchPlugin_ConstraintAngle::ID())
////        isAngular = true;
////
////      if (aFound != myAttributeEntityMap.end())
////        aNewEntID = aFound->second;
////      if (isAngular)
////        updateAngle(aScalar, aNewEntID);
////      else
////        updateScalar(aScalar, aNewEntID);
////    }
////  }
////  // NOTE: Other types of attributes are not supported
////
////  // Map attribute and new entity ID
////  if (aFound == myAttributeEntityMap.end() && aNewEntID != ENTITY_UNKNOWN)
////    myAttributeEntityMap[theEntity] = aNewEntID;
////  return aNewEntID;
////}


////bool PlaneGCSSolver_Storage::removeConstraint(const ConstraintPtr& theConstraint)
////{
////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >::iterator
////      aFound = myConstraintsMap.find(theConstraint);
////  if (aFound == myConstraintsMap.end())
////    return true; // no constraint - nothing to remove
////
////  std::vector<GCSConstraintPtr> aConstrList = aFound->second;
////  myConstraintsMap.erase(aFound);
////  bool isFullyRemoved = removeEntity(theConstraint->attribute(SketchPlugin_Constraint::VALUE()));
////  for (int ind = 0; ind < CONSTRAINT_ATTR_SIZE; ++ind)
////    isFullyRemoved = removeEntity(theConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(ind)))
////                   && isFullyRemoved;
////
////  myRemovedConstraints.insert(myRemovedConstraints.end(), aConstrList.begin(), aConstrList.end());
////
////  return isFullyRemoved;
////}

////bool PlaneGCSSolver_Storage::removeConstraint(GCSConstraintPtr theConstraint)
////{
////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >::iterator
////      aFound = myConstraintsMap.find(ConstraintPtr());
////  if (aFound != myConstraintsMap.end()) {
////    std::vector<GCSConstraintPtr>::iterator anIt = aFound->second.begin();
////    for (; anIt != aFound->second.end(); ++anIt)
////      if (*anIt == theConstraint) {
////        aFound->second.erase(anIt);
////        break;
////      }
////  }
////
////  myRemovedConstraints.push_back(theConstraint);
////  return true;
////}
////
////bool PlaneGCSSolver_Storage::removeEntity(const FeaturePtr& theFeature)
////{
////  if (!theFeature)
////    return true;
////  if (isUsed(theFeature))
////    return false;
////
////  // remove feature and corresponding entity
////  std::map<FeaturePtr, EntityID>::iterator aFound = myFeatureEntityMap.find(theFeature);
////  if (aFound != myFeatureEntityMap.end()) {
////    if (aFound->second.type == ARC) {
////      // remove additional arc constraints
////      std::map<EntityID, std::vector<GCSConstraintPtr> >::iterator aFoundArc =
////          myArcConstraintMap.find(aFound->second);
////      if (aFoundArc != myArcConstraintMap.end()) {
////        myRemovedConstraints.insert(myRemovedConstraints.end(),
////            aFoundArc->second.begin(), aFoundArc->second.end());
////        myArcConstraintMap.erase(aFoundArc);
////      }
////    }
////    myGCSEntities.erase(aFound->second);
////    myFeatureEntityMap.erase(aFound);
////  }
////
////  // remove feature's attributes
////  bool isFullyRemoved = true;
////  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
////  std::list<AttributePtr>::iterator anAttrIt = anAttributes.begin();
////  for (; anAttrIt != anAttributes.end(); ++anAttrIt)
////    isFullyRemoved = removeEntity(*anAttrIt) && isFullyRemoved;
////  return isFullyRemoved;
////}
////
////bool PlaneGCSSolver_Storage::removeEntity(const AttributePtr& theAttribute)
////{
////  if (!theAttribute)
////    return true;
////  if (isUsed(theAttribute))
////    return false;
////
////  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
////  if (aRefAttr) {
////    if (aRefAttr->isObject()) {
////      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
////      return removeEntity(aFeature);
////    } else
////      return removeEntity(aRefAttr->attr());
////  }
////
////  // remove attribute and corresponding entity
////  std::map<AttributePtr, EntityID>::iterator aFound = myAttributeEntityMap.find(theAttribute);
////  if (aFound == myAttributeEntityMap.end())
////    return true; // no attribute, nothing to remove
////
////  if (aFound->second.type == SCALAR || aFound->second.type == ANGLE) {
////    std::map<EntityID, double*>::iterator aFoundScalar = myGCSScalars.find(aFound->second);
////    if (aFoundScalar != myGCSScalars.end()) {
////      removeParameters(aFoundScalar->second);
////      myGCSScalars.erase(aFoundScalar);
////    }
////  }
////  else if (aFound->second.type == POINT) {
////    std::map<EntityID, GCSPointPtr>::iterator aFoundPoint = myGCSPoints.find(aFound->second);
////    if (aFoundPoint != myGCSPoints.end()) {
////      removeParameters(aFoundPoint->second->x, aFoundPoint->second->y);
////      myGCSPoints.erase(aFoundPoint);
////    }
////  }
////
////  myAttributeEntityMap.erase(aFound);
////  return true;
////}

////void PlaneGCSSolver_Storage::removeParameters(double* theParam1, double* theParam2)
////{
////  int aNbRemoved = 2;
////  for (int ind = 0; ind < 2 && aNbRemoved > 0; ++ind) {
////    GCS::VEC_pD& aList = ind ==0 ? myParameters : myConst;
////
////    GCS::VEC_pD::iterator aParIt = aList.begin();
////    while (aNbRemoved > 0 && aParIt != aList.end()) {
////      if (*aParIt != theParam1 && *aParIt != theParam2) {
////        ++aParIt;
////        continue;
////      }
////
////      myRemovedParameters.push_back(*aParIt);
////      int aShift = aParIt - aList.begin();
////      aList.erase(aParIt);
////      aParIt = aList.begin() + aShift;
////      --aNbRemoved;
////    }
////  }
////}


////double* PlaneGCSSolver_Storage::createScalar(const AttributeDoublePtr& theScalar)
////{
////  double* aResult = new double;
////  if (theScalar)
////    *aResult = theScalar->value();
////  myParameters.push_back(aResult);
////  myNeedToResolve = true;
////  return aResult;
////}
////
////void PlaneGCSSolver_Storage::updateScalar(const AttributeDoublePtr& theScalar, EntityID& theID)
////{
////  std::map<EntityID, double*>::const_iterator aFound = myGCSScalars.find(theID);
////  if (aFound == myGCSScalars.end()) {
////    // new scalar
////    theID = ++myEntityLastID;
////    theID.type = SCALAR;
////    myGCSScalars[theID] = createScalar(theScalar);
////  }
////  else if (fabs(*(myGCSScalars[theID]) - theScalar->value()) > tolerance) {
////    *(myGCSScalars[theID]) = theScalar->value();
////    myNeedToResolve = true;
////  }
////}
////
////void PlaneGCSSolver_Storage::updateAngle(const AttributeDoublePtr& theAngle, EntityID& theID)
////{
////  std::map<EntityID, double*>::const_iterator aFound = myGCSScalars.find(theID);
////  if (aFound == myGCSScalars.end()) {
////    // new scalar
////    theID = ++myEntityLastID;
////    theID.type = ANGLE;
////    myGCSScalars[theID] = createScalar(theAngle);
////  }
////  else if (fabs(*(myGCSScalars[theID]) - theAngle->value()) > tolerance) {
////    *(myGCSScalars[theID]) = theAngle->value();
////    myNeedToResolve = true;
////  }
////  // Convert degrees to radians
////  *(myGCSScalars[theID]) *= M_PI / 180.0;
////}
////
////GCS::Point PlaneGCSSolver_Storage::createPoint(const std::shared_ptr<GeomDataAPI_Point2D>& thePoint)
////{
////  GCS::Point aResult;
////  aResult.x = new double(thePoint->x());
////  aResult.y = new double(thePoint->y());
////
////  myParameters.push_back(aResult.x);
////  myParameters.push_back(aResult.y);
////
////  myNeedToResolve = true;
////  return aResult;
////}
////
////void PlaneGCSSolver_Storage::updatePoint(const std::shared_ptr<GeomDataAPI_Point2D>& thePoint,
////                                             EntityID&                             theID)
////{
////  std::map<EntityID, GCSPointPtr>::iterator aFound = myGCSPoints.find(theID);
////  if (aFound == myGCSPoints.end()) {
////    // new point
////    theID = ++myEntityLastID;
////    theID.type = POINT;
////    myGCSPoints[theID] = GCSPointPtr(new GCS::Point(createPoint(thePoint)));
////  }
////  else if (fabs(*(aFound->second->x) - thePoint->x()) > tolerance ||
////           fabs(*(aFound->second->y) - thePoint->y()) > tolerance) {
////    // update point
////    *(aFound->second->x) = thePoint->x();
////    *(aFound->second->y) = thePoint->y();
////    myNeedToResolve = true;
////  }
////}
////
////void PlaneGCSSolver_Storage::updateLine(const FeaturePtr& theLine, EntityID& theID)
////{
////  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////      theLine->attribute(SketchPlugin_Line::START_ID()));
////  std::shared_ptr<GeomDataAPI_Point2D> aEndAttr =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////      theLine->attribute(SketchPlugin_Line::END_ID()));
////  if (!aStartAttr->isInitialized() || !aEndAttr->isInitialized()) {
////    theID = ENTITY_UNKNOWN;
////    return;
////  }
////
////  std::map<EntityID, GCSCurvePtr>::iterator aFound = myGCSEntities.find(theID);
////  if (aFound == myGCSEntities.end()) {
////////    // new line
////////    theID = ++myEntityLastID;
////////    theID.type = LINE;
////////
////////    EntityID aStartID = changeEntity(aStartAttr);
////////    EntityID aEndID   = changeEntity(aEndAttr);
////////    GCS::Point* aStart = getPoint(aStartID);
////////    GCS::Point* aEnd   = getPoint(aEndID);
////////
////////    std::shared_ptr<GCS::Line> aLine(new GCS::Line);
////////    aLine->p1 = *aStart;
////////    aLine->p2 = *aEnd;
////////    myGCSEntities[theID] = aLine;
////  }
////  else {
////    // update line parameters
////    std::map<AttributePtr, EntityID>::iterator aFoundAttr = myAttributeEntityMap.find(aStartAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updatePoint(aStartAttr, aFoundAttr->second);
////    aFoundAttr = myAttributeEntityMap.find(aEndAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updatePoint(aEndAttr, aFoundAttr->second);
////  }
////}
////
////void PlaneGCSSolver_Storage::updateCircle(const FeaturePtr& theCircle, EntityID& theID)
////{
////  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////      theCircle->attribute(SketchPlugin_Circle::CENTER_ID()));
////  AttributeDoublePtr aRadiusAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
////      theCircle->attribute(SketchPlugin_Circle::RADIUS_ID()));
////  if (!aCenterAttr->isInitialized() || !aRadiusAttr->isInitialized()) {
////    theID = ENTITY_UNKNOWN;
////    return;
////  }
////
////  std::map<EntityID, GCSCurvePtr>::iterator aFound = myGCSEntities.find(theID);
////  if (aFound == myGCSEntities.end()) {
////////    // new circle
////////    theID = ++myEntityLastID;
////////    theID.type = CIRCLE;
////////
////////    EntityID aCenterID = changeEntity(aCenterAttr);
////////    EntityID aRadiusID = changeEntity(aRadiusAttr);
////////    GCS::Point* aCenter = getPoint(aCenterID);
////////    double* aRadius = getScalar(aRadiusID);
////////
////////    std::shared_ptr<GCS::Circle> aCircle(new GCS::Circle);
////////    aCircle->center = *aCenter;
////////    aCircle->rad = aRadius;
////////    myGCSEntities[theID] = aCircle;
////  }
////  else {
////    // update circle parameters
////    std::map<AttributePtr, EntityID>::iterator aFoundAttr = myAttributeEntityMap.find(aCenterAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updatePoint(aCenterAttr, aFoundAttr->second);
////    aFoundAttr = myAttributeEntityMap.find(aRadiusAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updateScalar(aRadiusAttr, aFoundAttr->second);
////  }
////}
////
////void PlaneGCSSolver_Storage::updateArc(const FeaturePtr& theArc, EntityID& theID)
////{
////  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////      theArc->attribute(SketchPlugin_Arc::CENTER_ID()));
////  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////      theArc->attribute(SketchPlugin_Arc::START_ID()));
////  std::shared_ptr<GeomDataAPI_Point2D> aEndAttr =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////      theArc->attribute(SketchPlugin_Arc::END_ID()));
////  if (!aCenterAttr->isInitialized() ||
////      !aStartAttr->isInitialized()  ||
////      !aEndAttr->isInitialized()) {
////    theID = ENTITY_UNKNOWN;
////    return;
////  }
////
////  std::shared_ptr<GCS::Arc> anArc;
////  std::map<EntityID, GCSCurvePtr>::iterator aFound = myGCSEntities.find(theID);
////  if (aFound == myGCSEntities.end()) {
////    // new arc
////    theID = ++myEntityLastID;
////    theID.type = ARC;
////
////    EntityID aCenterID = changeEntity(aCenterAttr);
////    EntityID aStartID  = changeEntity(aStartAttr);
////    EntityID aEndID    = changeEntity(aEndAttr);
////    GCS::Point* aCenter = getPoint(aCenterID);
////    GCS::Point* aStart  = getPoint(aStartID);
////    GCS::Point* aEnd    = getPoint(aEndID);
////
////    anArc = std::shared_ptr<GCS::Arc>(new GCS::Arc);
////    anArc->center = *aCenter;
////    anArc->start  = *aStart;
////    anArc->end    = *aEnd;
////    anArc->startAngle = createScalar();
////    anArc->endAngle   = createScalar();
////    anArc->rad        = createScalar();
////    myGCSEntities[theID] = anArc;
////
////    // Additional constraints for new arc
////    processArc(theID);
////  }
////  else {
////    // update arc parameters
////    anArc = std::dynamic_pointer_cast<GCS::Arc>(aFound->second);
////    std::map<AttributePtr, EntityID>::iterator aFoundAttr = myAttributeEntityMap.find(aCenterAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updatePoint(aCenterAttr, aFoundAttr->second);
////    aFoundAttr = myAttributeEntityMap.find(aStartAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updatePoint(aStartAttr, aFoundAttr->second);
////    aFoundAttr = myAttributeEntityMap.find(aEndAttr);
////    if (aFoundAttr != myAttributeEntityMap.end())
////      updatePoint(aEndAttr, aFoundAttr->second);
////  }
////
////  // Calculate additional parameters necessary for FreeGCS
////  std::shared_ptr<GeomAPI_Pnt2d> aCenterPnt = aCenterAttr->pnt();
////  std::shared_ptr<GeomAPI_Pnt2d> aStartPnt  = aStartAttr->pnt();
////  std::shared_ptr<GeomAPI_Pnt2d> aEndPnt    = aEndAttr->pnt();
////
////  *(anArc->rad) = aCenterPnt->distance(aStartPnt);
////  std::shared_ptr<GeomAPI_Edge> anArcEdge =
////      std::dynamic_pointer_cast<GeomAPI_Edge>(theArc->lastResult()->shape());
////  if (!anArcEdge) {
////    theID = ENTITY_UNKNOWN;
////    return;
////  }
////  anArcEdge->getRange(*(anArc->startAngle), *(anArc->endAngle));
////}


////double* PlaneGCSSolver_Storage::getScalar(const EntityID& theID) const
////{
////  std::map<EntityID, double*>::const_iterator aFound = myGCSScalars.find(theID);
////  if (aFound == myGCSScalars.end())
////    return 0;
////  return aFound->second;
////}
////
////GCS::Point* PlaneGCSSolver_Storage::getPoint(const EntityID& theID) const
////{
////  std::map<EntityID, GCSPointPtr>::const_iterator aFound = myGCSPoints.find(theID);
////  if (aFound == myGCSPoints.end())
////    return 0;
////  return aFound->second.get();
////}
////
////GCS::Line* PlaneGCSSolver_Storage::getLine(const EntityID& theID) const
////{
////  std::map<EntityID, GCSCurvePtr>::const_iterator aFound = myGCSEntities.find(theID);
////  if (aFound == myGCSEntities.end())
////    return 0;
////  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(aFound->second);
////  if (!aLine)
////    return 0;
////  return aLine.get();
////}
////
////GCS::Circle* PlaneGCSSolver_Storage::getCircle(const EntityID& theID) const
////{
////  std::map<EntityID, GCSCurvePtr>::const_iterator aFound = myGCSEntities.find(theID);
////  if (aFound == myGCSEntities.end())
////    return 0;
////  std::shared_ptr<GCS::Circle> aCircle = std::dynamic_pointer_cast<GCS::Circle>(aFound->second);
////  if (!aCircle)
////    return 0;
////  return aCircle.get();
////}
////
////GCS::Arc* PlaneGCSSolver_Storage::getArc(const EntityID& theID) const
////{
////  std::map<EntityID, GCSCurvePtr>::const_iterator aFound = myGCSEntities.find(theID);
////  if (aFound == myGCSEntities.end())
////    return 0;
////  std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(aFound->second);
////  if (!anArc)
////    return 0;
////  return anArc.get();
////}

void PlaneGCSSolver_Storage::processArc(const EntityWrapperPtr& theArc)
{
  // Calculate additional parameters necessary for PlaneGCS
  const std::list<EntityWrapperPtr>& aSubs = theArc->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aSubIt = aSubs.begin();
  while ((*aSubIt)->type() == ENTITY_POINT) // search scalar entities
    ++aSubIt;
  double* aStartAngle = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(*aSubIt++)->scalar();
  double* aEndAngle   = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(*aSubIt++)->scalar();
  double* aRadius     = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(*aSubIt)->scalar();

  FeaturePtr anArcFeature = theArc->baseFeature();
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::END_ID()));
  std::shared_ptr<GeomAPI_Pnt2d> aCenterPnt = aCenterAttr->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aStartPnt  = aStartAttr->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aEndPnt    = aEndAttr->pnt();

  *aRadius = aCenterPnt->distance(aStartPnt);
  if (!anArcFeature->lastResult())
    return;
  std::shared_ptr<GeomAPI_Edge> anArcEdge =
      std::dynamic_pointer_cast<GeomAPI_Edge>(anArcFeature->lastResult()->shape());
  if (!anArcEdge)
    return;
  anArcEdge->getRange(*aStartAngle, *aEndAngle);

  // No need to add constraints if they are already exist
  std::map<EntityWrapperPtr, std::vector<GCSConstraintPtr> >::const_iterator
      aFound = myArcConstraintMap.find(theArc);
  if (aFound != myArcConstraintMap.end())
    return;

  // Prepare additional constraints to produce the arc
  std::vector<GCSConstraintPtr> anArcConstraints;
  std::shared_ptr<PlaneGCSSolver_EntityWrapper> anArcEnt =
      std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theArc);
  std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anArcEnt->entity());
  // Distances from center till start and end points are equal to radius
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
      anArc->center, anArc->start, anArc->rad)));
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
      anArc->center, anArc->end, anArc->rad)));
  // Angles of start and end points should be equal to given angles
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PAngle(
      anArc->center, anArc->start, anArc->startAngle)));
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PAngle(
      anArc->center, anArc->end, anArc->endAngle)));

  myArcConstraintMap[theArc] = anArcConstraints;
}

////const std::vector<GCSConstraintPtr>& PlaneGCSSolver_Storage::getConstraint(
////    const ConstraintPtr& theConstraint) const
////{
////  static const std::vector<GCSConstraintPtr> aDummy;
////
////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >::const_iterator
////      aFound = myConstraintsMap.find(theConstraint);
////  if (aFound == myConstraintsMap.end())
////    return aDummy;
////  return aFound->second;
////}


void PlaneGCSSolver_Storage::makeConstant(const EntityWrapperPtr& theEntity)
{
  toggleEntity(theEntity, myParameters, myConst);
}

void PlaneGCSSolver_Storage::makeVariable(const EntityWrapperPtr& theEntity)
{
  toggleEntity(theEntity, myConst, myParameters);
}

static void getParametersToMove(const EntityWrapperPtr& theEntity, std::set<double*>& theParamList)
{
  const std::list<ParameterWrapperPtr> aParams = theEntity->parameters();
  std::list<ParameterWrapperPtr>::const_iterator aPIt = aParams.begin();
  for (; aPIt != aParams.end(); ++aPIt)
    theParamList.insert(
        std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(*aPIt)->parameter());

  const std::list<EntityWrapperPtr> aSubs = theEntity->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++aSIt)
    getParametersToMove(*aSIt, theParamList);
}

void PlaneGCSSolver_Storage::toggleEntity(
    const EntityWrapperPtr& theEntity, GCS::VEC_pD& theFrom, GCS::VEC_pD& theTo)
{
  std::set<double*> aParamsToMove;
  getParametersToMove(theEntity, aParamsToMove);

  GCS::VEC_pD::iterator anIt = theFrom.begin();
  while (anIt != theFrom.end()) {
    if (aParamsToMove.find(*anIt) == aParamsToMove.end()) {
      ++anIt;
      continue;
    }

    theTo.push_back(*anIt);
    int aShift = anIt - theFrom.begin();
    theFrom.erase(anIt);
    anIt = theFrom.begin() + aShift;
  }
}


void PlaneGCSSolver_Storage::initializeSolver(SolverPtr theSolver)
{
  std::shared_ptr<PlaneGCSSolver_Solver> aSolver =
      std::dynamic_pointer_cast<PlaneGCSSolver_Solver>(theSolver);
  if (!aSolver)
    return;

  if (myExistArc)
    processArcs();

  // initialize constraints
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIt = myConstraintMap.begin();
  for (; aCIt != myConstraintMap.end(); ++aCIt) {
    std::list<ConstraintWrapperPtr>::const_iterator aCWIt = aCIt->second.begin();
    for (; aCWIt != aCIt->second.end(); ++ aCWIt) {
      std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aGCS =
          std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(*aCWIt);
      std::list<GCSConstraintPtr>::const_iterator anIt = aGCS->constraints().begin();
      for (; anIt != aGCS->constraints().end(); ++anIt)
        aSolver->addConstraint(*anIt);
    }
  }
  // additional constraints for arcs
  std::map<EntityWrapperPtr, std::vector<GCSConstraintPtr> >::const_iterator
      anArcIt = myArcConstraintMap.begin();
  for (; anArcIt != myArcConstraintMap.end(); ++anArcIt) {
    std::vector<GCSConstraintPtr>::const_iterator anIt = anArcIt->second.begin();
    for (; anIt != anArcIt->second.end(); ++anIt)
      aSolver->addConstraint(*anIt);
  }
  // removed waste constraints
  std::list<GCSConstraintPtr>::const_iterator aRemIt = myRemovedConstraints.begin();
  for (; aRemIt != myRemovedConstraints.end(); ++aRemIt)
    aSolver->removeConstraint(*aRemIt);
  myRemovedConstraints.clear();
////  // remove waste parameters
////  std::list<double*>::const_iterator aRemParIt = myRemovedParameters.begin();
////  for (; aRemParIt != myRemovedParameters.end(); ++aRemParIt)
////    delete *aRemParIt;
////  myRemovedParameters.clear();
  // initialize unknowns
  aSolver->setParameters(myParameters);
}

void PlaneGCSSolver_Storage::refresh(bool theFixedOnly) const
{
  //blockEvents(true);

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anIt = myAttributeMap.begin();
  std::list<ParameterWrapperPtr> aParams;
  std::list<ParameterWrapperPtr>::const_iterator aParIt;
  for (; anIt != myAttributeMap.end(); ++anIt) {
    // the external feature always should keep the up to date values, so, 
    // refresh from the solver is never needed
    bool isExternal = false;
    if (anIt->first.get()) {
      std::shared_ptr<SketchPlugin_Feature> aSketchFeature = 
        std::dynamic_pointer_cast<SketchPlugin_Feature>(anIt->first->owner());
      if (aSketchFeature.get() && aSketchFeature->isExternal())
        isExternal = true;
    }

    // update parameter wrappers and obtain values of attributes
    aParams = anIt->second->parameters();
    double aCoords[3];
    bool isUpd[3] = {false};
    int i = 0;
    for (aParIt = aParams.begin(); i < 3 && aParIt != aParams.end(); ++aParIt, ++i) {
      if (!theFixedOnly || isExternal ||
          (*aParIt)->group() == GID_OUTOFGROUP || (*aParIt)->isParametric()) {
        aCoords[i] = (*aParIt)->value();
        isUpd[i] = true;
      }
    }
    if (!isUpd[0] && !isUpd[1] && !isUpd[2])
      continue; // nothing is updated

    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anIt->first);
    if (aPoint2D) {
      if ((isUpd[0] && fabs(aPoint2D->x() - aCoords[0]) > tolerance) ||
          (isUpd[1] && fabs(aPoint2D->y() - aCoords[1]) > tolerance) || isExternal) {
        if (!isUpd[0] || isExternal) aCoords[0] = aPoint2D->x();
        if (!isUpd[1] || isExternal) aCoords[1] = aPoint2D->y();
        aPoint2D->setValue(aCoords[0], aCoords[1]);
        // Find points coincident with this one (probably not in GID_OUTOFGROUP)
        CoincidentPointsMap::const_iterator aCoincIt = myCoincidentPoints.begin();
        for (; aCoincIt != myCoincidentPoints.end(); ++aCoincIt)
          if (aCoincIt->first == anIt->second ||
              aCoincIt->second.find(anIt->second) != aCoincIt->second.end())
            break;
        if (aCoincIt != myCoincidentPoints.end()) {
          aPoint2D = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
              aCoincIt->first->baseAttribute());
          if (aPoint2D)
            aPoint2D->setValue(aCoords[0], aCoords[1]);
          std::set<EntityWrapperPtr>::const_iterator aSlaveIt = aCoincIt->second.begin();
          for (; aSlaveIt != aCoincIt->second.end(); ++aSlaveIt) {
            aPoint2D = std::dynamic_pointer_cast<GeomDataAPI_Point2D>((*aSlaveIt)->baseAttribute());
            if (aPoint2D)
              aPoint2D->setValue(aCoords[0], aCoords[1]);
          }
        }
      }
      continue;
    }
    AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anIt->first);
    if (aScalar && !isExternal) {
      if (isUpd[0] && fabs(aScalar->value() - aCoords[0]) > tolerance)
        aScalar->setValue(aCoords[0]);
      continue;
    }
  }

  //blockEvents(false);
}

EntityWrapperPtr PlaneGCSSolver_Storage::calculateMiddlePoint(
    EntityWrapperPtr theBase, double theCoeff)
{
  std::shared_ptr<PlaneGCSSolver_Builder> aBuilder =
      std::dynamic_pointer_cast<PlaneGCSSolver_Builder>(PlaneGCSSolver_Builder::getInstance());

  std::shared_ptr<GeomAPI_XY> aMidPoint;
  if (theBase->type() == ENTITY_LINE) {
    std::shared_ptr<GeomAPI_Pnt2d> aPoints[2];
    const std::list<EntityWrapperPtr>& aSubs = theBase->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt = aSubs.begin();
    for (int i = 0; i < 2; ++i, ++anIt)
      aPoints[i] = aBuilder->point(*anIt);
    aMidPoint = aPoints[0]->xy()->multiplied(1.0 - theCoeff)->added(
        aPoints[1]->xy()->multiplied(theCoeff));
  }
  else if (theBase->type() == ENTITY_ARC) {
    double theX, theY;
    double anArcPoint[3][2];
    const std::list<EntityWrapperPtr>& aSubs = theBase->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt = aSubs.begin();
    for (int i = 0; i < 3; ++i, ++anIt) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aBuilder->point(*anIt);
      anArcPoint[i][0] = aPoint->x();
      anArcPoint[i][1] = aPoint->y();
    }
    // project last point of arc on the arc
    double x = anArcPoint[1][0] - anArcPoint[0][0];
    double y = anArcPoint[1][1] - anArcPoint[0][1];
    double aRad = sqrt(x*x + y*y);
    x = anArcPoint[2][0] - anArcPoint[0][0];
    y = anArcPoint[2][1] - anArcPoint[0][1];
    double aNorm = sqrt(x*x + y*y);
    if (aNorm >= tolerance) {
      anArcPoint[2][0] = x * aRad / aNorm;
      anArcPoint[2][1] = y * aRad / aNorm;
    }
    anArcPoint[1][0] -= anArcPoint[0][0];
    anArcPoint[1][1] -= anArcPoint[0][1];
    if (theCoeff < tolerance) {
      theX = anArcPoint[0][0] + anArcPoint[1][0];
      theY = anArcPoint[0][1] + anArcPoint[1][1];
    } else if (1 - theCoeff < tolerance) {
      theX = anArcPoint[0][0] + anArcPoint[2][0];
      theY = anArcPoint[0][1] + anArcPoint[2][1];
    } else {
      std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(anArcPoint[1][0], anArcPoint[1][1]));
      std::shared_ptr<GeomAPI_Dir2d> aEndDir(new GeomAPI_Dir2d(anArcPoint[2][0], anArcPoint[2][1]));
      double anAngle = aStartDir->angle(aEndDir);
      if (anAngle < 0)
        anAngle += 2.0 * PI;
      anAngle *= theCoeff;
      double aCos = cos(anAngle);
      double aSin = sin(anAngle);
      theX = anArcPoint[0][0] + anArcPoint[1][0] * aCos - anArcPoint[1][1] * aSin;
      theY = anArcPoint[0][1] + anArcPoint[1][0] * aSin + anArcPoint[1][1] * aCos;
    }
    aMidPoint = std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(theX, theY));
  }

  if (!aMidPoint)
    return EntityWrapperPtr();

  std::list<ParameterWrapperPtr> aParameters;
  aParameters.push_back(aBuilder->createParameter(myGroupID, aMidPoint->x()));
  aParameters.push_back(aBuilder->createParameter(myGroupID, aMidPoint->y()));
  // Create entity (parameters are not filled)
  GCSPointPtr aPnt(new GCS::Point);
  aPnt->x = std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(aParameters.front())->parameter();
  aPnt->y = std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(aParameters.back())->parameter();

  EntityWrapperPtr aResult(new PlaneGCSSolver_PointWrapper(AttributePtr(), aPnt));
  aResult->setGroup(myGroupID);
  aResult->setParameters(aParameters);

  update(aResult);
  return aResult;
}

////bool PlaneGCSSolver_Storage::isInteract(AttributePtr theAttribute) const
////{
////  std::map<AttributePtr, EntityID>::const_iterator aFound = myAttributeEntityMap.find(theAttribute);
////  if (aFound != myAttributeEntityMap.end())
////    return true;
////
////  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
////  if (!aRefAttr)
////    return false;
////  if (aRefAttr->isObject()) {
////    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
////    if (!aFeature)
////      return false;
////    return isInteract(aFeature);
////  }
////
////  return isInteract(aRefAttr->attr());
////}
////
////bool PlaneGCSSolver_Storage::isInteract(FeaturePtr theFeature) const
////{
////  if (myFeatureEntityMap.find(theFeature) != myFeatureEntityMap.end())
////    return true;
////  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
////  std::list<AttributePtr>::iterator anIt = anAttributes.begin();
////  for (; anIt != anAttributes.end(); ++anIt)
////    if (isInteract(*anIt))
////      return true;
////  return false;
////}


////bool PlaneGCSSolver_Storage::isUsed(const FeaturePtr& theFeature) const
////{
////  // There is checked only the feature itself, not its attributes.
////  // The attributes should be checked separately.
////
////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >::const_iterator
////      aCIt = myConstraintsMap.begin();
////  for (; aCIt != myConstraintsMap.end(); ++aCIt) {
////    if (!aCIt->first)
////      continue;
////    for (int ind = 0; ind < CONSTRAINT_ATTR_SIZE; ++ind) {
////      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////          aCIt->first->attribute(SketchPlugin_Constraint::ATTRIBUTE(ind)));
////      if (aRefAttr && aRefAttr->isObject()) {
////        FeaturePtr aFeat = ModelAPI_Feature::feature(aRefAttr->object());
////        if (aFeat == theFeature)
////          return true;
////      }
////    }
////  }
////  return false;
////}
////
////bool PlaneGCSSolver_Storage::isUsed(const AttributePtr& theAttribute) const
////{
////  // Check whether the attribute is used by constraints
////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >::const_iterator
////      aCIt = myConstraintsMap.begin();
////  for (; aCIt != myConstraintsMap.end(); ++aCIt) {
////    if (!aCIt->first)
////      continue;
////    if (aCIt->first->attribute(SketchPlugin_Constraint::VALUE()) == theAttribute)
////      return true;
////    for (int ind = 0; ind < CONSTRAINT_ATTR_SIZE; ++ind) {
////      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////          aCIt->first->attribute(SketchPlugin_Constraint::ATTRIBUTE(ind)));
////      if (!aRefAttr)
////        continue;
////      if (aRefAttr == theAttribute ||
////         (!aRefAttr->isObject() && aRefAttr->attr() == theAttribute))
////        return true;
////    }
////  }
////
////  // Check whether the attribute is used by features
////  std::map<FeaturePtr, EntityID>::const_iterator aFIt = myFeatureEntityMap.begin();
////  for (; aFIt != myFeatureEntityMap.end(); ++aFIt) {
////    std::list<AttributePtr> anAttrs =
////        aFIt->first->data()->attributes(GeomDataAPI_Point2D::typeId());
////    std::list<AttributePtr>::const_iterator anIt = anAttrs.begin();
////    for (; anIt != anAttrs.end(); ++anIt)
////      if (*anIt == theAttribute)
////        return true;
////  }
////
////  return false;
////}


////void PlaneGCSSolver_Storage::blockEvents(bool isBlocked)
////{
////  std::map<FeaturePtr, EntityID>::iterator aFIt = myFeatureEntityMap.begin();
////  for (; aFIt != myFeatureEntityMap.end(); ++aFIt)
////    if (aFIt->first->data() && aFIt->first->data()->isValid())
////      aFIt->first->data()->blockSendAttributeUpdated(isBlocked);
////
////  std::map<AttributePtr, EntityID>::iterator anAIt = myAttributeEntityMap.begin();
////  for (; anAIt != myAttributeEntityMap.end(); ++anAIt)
////    if (anAIt->first->owner() && anAIt->first->owner()->data() &&
////        anAIt->first->owner()->data()->isValid())
////      anAIt->first->owner()->data()->blockSendAttributeUpdated(isBlocked);
////}


#else

#endif
