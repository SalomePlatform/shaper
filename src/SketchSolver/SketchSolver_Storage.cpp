// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Storage.cpp
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#include <SketchSolver_Storage.h>
#include <SketchSolver_Manager.h>

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>


/// \brief Verify two vectors of constraints are equal.
///        Vectors differ by the order of elements are equal.
static bool isEqual(const std::list<ConstraintWrapperPtr>& theCVec1,
                    const std::list<ConstraintWrapperPtr>& theCVec2);


void SketchSolver_Storage::addConstraint(ConstraintPtr        theConstraint,
                                         ConstraintWrapperPtr theSolverConstraint)
{
  if (theSolverConstraint) {
    std::list<ConstraintWrapperPtr> aConstrList(1, theSolverConstraint);
    addConstraint(theConstraint, aConstrList);
  } else
    addConstraint(theConstraint, std::list<ConstraintWrapperPtr>());
}

void SketchSolver_Storage::addConstraint(
    ConstraintPtr                   theConstraint,
    std::list<ConstraintWrapperPtr> theSolverConstraints)
{
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound == myConstraintMap.end() || !isEqual(aFound->second, theSolverConstraints))
    setNeedToResolve(true);

  if (theSolverConstraints.empty()) {
    // constraint links to the empty list, add its attributes linked to the empty entities
    std::list<AttributePtr> aRefAttrs =
        theConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
    std::list<AttributePtr>::const_iterator anAttrIt = aRefAttrs.begin();
    for (; anAttrIt != aRefAttrs.end(); ++anAttrIt) {
      AttributeRefAttrPtr aRef = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIt);
      if (aRef->isObject()) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
        if (aFeature) addEntity(aFeature, EntityWrapperPtr());
      } else
        addEntity(aRef->attr(), EntityWrapperPtr());
    }
    std::list<AttributePtr> aRefLists =
        theConstraint->data()->attributes(ModelAPI_AttributeRefList::typeId());
    for (anAttrIt = aRefLists.begin(); anAttrIt != aRefLists.end(); ++anAttrIt) {
      AttributeRefListPtr aRef = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anAttrIt);
      std::list<ObjectPtr> anObj = aRef->list();
      std::list<ObjectPtr>::iterator anIt = anObj.begin();
      for (; anIt != anObj.end(); ++anIt) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
        if (aFeature) addEntity(aFeature, EntityWrapperPtr());
      }
    }
  }
  else if (theSolverConstraints.front()->type() != CONSTRAINT_PT_PT_COINCIDENT) {
    // Do not add point-point coincidence, because it is already made by setting
    // the same parameters for both points
    std::list<ConstraintWrapperPtr>::iterator aCIt = theSolverConstraints.begin();
    for (; aCIt != theSolverConstraints.end(); ++aCIt)
      update(*aCIt);
  }

  if (!theSolverConstraints.empty() || aFound == myConstraintMap.end())
    myConstraintMap[theConstraint] = theSolverConstraints;
  // block events if necessary
  if (myEventsBlocked && theConstraint->data() && theConstraint->data()->isValid())
    theConstraint->data()->blockSendAttributeUpdated(myEventsBlocked);
}

void SketchSolver_Storage::addEntity(FeaturePtr       theFeature,
                                     EntityWrapperPtr theSolverEntity)
{
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFound = myFeatureMap.find(theFeature);
  if (aFound == myFeatureMap.end() || !aFound->second ||
     (theSolverEntity && !aFound->second->isEqual(theSolverEntity)))
    setNeedToResolve(true); // the entity is new or modified

  if (!theSolverEntity) {
    // feature links to the empty entity, add its attributes
    std::list<AttributePtr> aPntAttrs =
        theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<AttributePtr>::const_iterator anAttrIt = aPntAttrs.begin();
    for (; anAttrIt != aPntAttrs.end(); ++anAttrIt)
      addEntity(*anAttrIt, EntityWrapperPtr());
    if (aFound == myFeatureMap.end())
      myFeatureMap[theFeature] = theSolverEntity;
  } else
    myFeatureMap[theFeature] = theSolverEntity;

  // block events if necessary
  if (myEventsBlocked && theFeature->data() && theFeature->data()->isValid())
    theFeature->data()->blockSendAttributeUpdated(myEventsBlocked);
}

void SketchSolver_Storage::addEntity(AttributePtr     theAttribute,
                                     EntityWrapperPtr theSolverEntity)
{
  std::map<AttributePtr, EntityWrapperPtr>::const_iterator aFound = myAttributeMap.find(theAttribute);
  if (aFound == myAttributeMap.end() || !aFound->second ||
     (theSolverEntity && !aFound->second->isEqual(theSolverEntity)))
    setNeedToResolve(true); // the entity is new or modified

  if (theSolverEntity || aFound == myAttributeMap.end())
    myAttributeMap[theAttribute] = theSolverEntity;
  // block events if necessary
  if (myEventsBlocked && theAttribute->owner() &&
      theAttribute->owner()->data() && theAttribute->owner()->data()->isValid())
    theAttribute->owner()->data()->blockSendAttributeUpdated(myEventsBlocked);
}


bool SketchSolver_Storage::update(FeaturePtr theFeature, const GroupID& theGroup)
{
  bool isUpdated = false;
  EntityWrapperPtr aRelated = entity(theFeature);
  if (!aRelated) { // Feature is not exist, create it
    std::list<EntityWrapperPtr> aSubs;
    // Reserve the feature in the map of features (do not want to add several copies of it)
    myFeatureMap[theFeature] = aRelated;
    // Firstly, create/update its attributes
    std::list<AttributePtr> anAttrs =
        theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<AttributePtr>::const_iterator anIt = anAttrs.begin();
    for (; anIt != anAttrs.end(); ++anIt) {
      isUpdated = update(*anIt, theGroup) || isUpdated;
      aSubs.push_back(entity(*anIt));
    }
    // If the feature is a circle, add its radius as a sub
    if (theFeature->getKind() == SketchPlugin_Circle::ID()) {
      AttributePtr aRadius = theFeature->attribute(SketchPlugin_Circle::RADIUS_ID());
      isUpdated = update(aRadius, theGroup) || isUpdated;
      aSubs.push_back(entity(aRadius));
    }
    // If the feature if circle or arc, we need to add normal of the sketch to the list of subs
    if (theFeature->getKind() == SketchPlugin_Arc::ID() ||
        theFeature->getKind() == SketchPlugin_Circle::ID()) {
      EntityWrapperPtr aNormal = getNormal();
      if (aNormal) aSubs.push_back(aNormal);
    }
    // Secondly, convert feature
    BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
    GroupID aGroup = theGroup != GID_UNKNOWN ? theGroup : myGroupID;
    aRelated = aBuilder->createFeature(theFeature, aSubs, aGroup);
    if (!aRelated)
      return false;
    addEntity(theFeature, aRelated);
  } else if (theGroup != GID_UNKNOWN)
    changeGroup(aRelated, theGroup);
  return update(aRelated) || isUpdated;
}

bool SketchSolver_Storage::update(AttributePtr theAttribute, const GroupID& theGroup)
{
  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      return update(aFeature, theGroup);
    } else
      anAttribute = aRefAttr->attr();
  }

  EntityWrapperPtr aRelated = entity(anAttribute);
  if (!aRelated) { // Attribute is not exist, create it
    // verify the attribute is a point of arc and add whole arc
    if (anAttribute->owner()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(anAttribute->owner());
      if (aFeature->getKind() == SketchPlugin_Arc::ID() &&
          myFeatureMap.find(aFeature) == myFeatureMap.end()) {
        // Additional checking that all attributes are initialized
        if (aFeature->attribute(SketchPlugin_Arc::CENTER_ID())->isInitialized() && 
            aFeature->attribute(SketchPlugin_Arc::START_ID())->isInitialized() && 
            aFeature->attribute(SketchPlugin_Arc::END_ID())->isInitialized()) {
////          myFeatureMap[aFeature] = EntityWrapperPtr();
          return SketchSolver_Storage::update(aFeature);
        }
      }
    }
    BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
    GroupID aGroup = theGroup != GID_UNKNOWN ? theGroup : myGroupID;
    aRelated = aBuilder->createAttribute(anAttribute, aGroup);
    if (!aRelated)
      return false;
    addEntity(anAttribute, aRelated);
  } else if (theGroup != GID_UNKNOWN)
    changeGroup(aRelated, theGroup);
  return update(aRelated);
}



const std::list<ConstraintWrapperPtr>& SketchSolver_Storage::constraint(
    const ConstraintPtr& theConstraint) const
{
  static std::list<ConstraintWrapperPtr> aDummy;

  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr>>::const_iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound != myConstraintMap.end())
    return aFound->second;
  return aDummy;
}

const EntityWrapperPtr& SketchSolver_Storage::entity(const FeaturePtr& theFeature) const
{
  static EntityWrapperPtr aDummy;

  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFound = myFeatureMap.find(theFeature);
  if (aFound != myFeatureMap.end())
    return aFound->second;
  return aDummy;
}

const EntityWrapperPtr& SketchSolver_Storage::entity(const AttributePtr& theAttribute) const
{
  static EntityWrapperPtr aDummy;

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator
      aFound = myAttributeMap.find(theAttribute);
  if (aFound != myAttributeMap.end())
    return aFound->second;

  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      return entity(aFeature);
    } else
      return entity(aRefAttr->attr());
  }
  return aDummy;
}

bool SketchSolver_Storage::removeConstraint(ConstraintPtr theConstraint)
{
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound == myConstraintMap.end())
    return true; // no constraint, already deleted

  // Remove constraint
  std::list<ConstraintWrapperPtr> aConstrList = aFound->second;
  myConstraintMap.erase(aFound);
  // Remove SolveSpace constraints
  bool isFullyRemoved = true;
  std::list<ConstraintWrapperPtr>::iterator anIt = aConstrList.begin();
  while (anIt != aConstrList.end()) {
    if (remove(*anIt)) {
      std::list<ConstraintWrapperPtr>::iterator aRemoveIt = anIt++;
      aConstrList.erase(aRemoveIt);
    } else {
      isFullyRemoved = false;
      ++anIt;
    }
  }
  return isFullyRemoved;
}

template <class ENT_TYPE>
static bool isUsed(ConstraintWrapperPtr theConstraint, ENT_TYPE theEntity)
{
  std::list<EntityWrapperPtr>::const_iterator anEntIt = theConstraint->entities().begin();
  for (; anEntIt != theConstraint->entities().end(); ++anEntIt)
    if ((*anEntIt)->isBase(theEntity))
      return true;
  return false;
}

static bool isUsed(EntityWrapperPtr theFeature, AttributePtr theSubEntity)
{
  std::list<EntityWrapperPtr>::const_iterator aSubIt = theFeature->subEntities().begin();
  for (; aSubIt != theFeature->subEntities().end(); ++aSubIt)
    if ((*aSubIt)->isBase(theSubEntity))
      return true;
  return false;
}

bool SketchSolver_Storage::isUsed(FeaturePtr theFeature) const
{
  if (myFeatureMap.find(theFeature) != myFeatureMap.end())
    return true;
  // check constraints
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIt = myConstraintMap.begin();
  std::list<ConstraintWrapperPtr>::const_iterator aCWIt;
  for (; aCIt != myConstraintMap.end(); ++aCIt)
    for (aCWIt = aCIt->second.begin(); aCWIt != aCIt->second.end(); ++aCWIt)
      if (::isUsed(*aCWIt, theFeature))
        return true;
  // check attributes
  std::list<AttributePtr> anAttrList = theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::const_iterator anIt = anAttrList.begin();
  for (; anIt != anAttrList.end(); ++anIt)
    if (isUsed(*anIt))
      return true;
  return false;
}

bool SketchSolver_Storage::isUsed(AttributePtr theAttribute) const
{
  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject())
      return isUsed(ModelAPI_Feature::feature(aRefAttr->object()));
    else
      anAttribute = aRefAttr->attr();
  }

  if (myAttributeMap.find(theAttribute) != myAttributeMap.end())
    return true;
  // check in constraints
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIt = myConstraintMap.begin();
  std::list<ConstraintWrapperPtr>::const_iterator aCWIt;
  for (; aCIt != myConstraintMap.end(); ++aCIt)
    for (aCWIt = aCIt->second.begin(); aCWIt != aCIt->second.end(); ++aCWIt)
      if (::isUsed(*aCWIt, anAttribute))
        return true;
  // check in features
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIt = myFeatureMap.begin();
  for (; aFIt != myFeatureMap.end(); ++aFIt)
    if (::isUsed(aFIt->second, anAttribute))
      return true;
  return false;
}


bool SketchSolver_Storage::removeEntity(FeaturePtr theFeature)
{
  std::map<FeaturePtr, EntityWrapperPtr>::iterator aFound = myFeatureMap.find(theFeature);
  if (aFound == myFeatureMap.end())
    return false; // feature not found, nothing to delete

  EntityWrapperPtr anEntity = aFound->second;
  myFeatureMap.erase(aFound);

  // Check if the feature is not used by constraints, remove it
  if (!isUsed(theFeature) && remove(anEntity))
    return true;

  // feature is not removed, revert operation
  myFeatureMap[theFeature] = anEntity;
  update(anEntity);
  return false;
}

bool SketchSolver_Storage::removeEntity(AttributePtr theAttribute)
{
  std::map<AttributePtr, EntityWrapperPtr>::iterator aFound = myAttributeMap.find(theAttribute);
  if (aFound == myAttributeMap.end())
    return false; // attribute not found, nothing to delete

  EntityWrapperPtr anEntity = aFound->second;
  myAttributeMap.erase(aFound);

  // Check if the attribute is not used by constraints and features, remove it
  if (!isUsed(theAttribute) && remove(anEntity))
    return true;

  // attribute is not removed, revert operation
  myAttributeMap[theAttribute] = anEntity;
  update(anEntity);
  return false;
}


bool SketchSolver_Storage::remove(ConstraintWrapperPtr theConstraint)
{
  bool isFullyRemoved = true;
  std::list<EntityWrapperPtr>::const_iterator anIt = theConstraint->entities().begin();
  for (; anIt != theConstraint->entities().end(); ++anIt) {
    FeaturePtr aBaseFeature = (*anIt)->baseFeature();
    if (aBaseFeature)
      isFullyRemoved = SketchSolver_Storage::removeEntity(aBaseFeature) && isFullyRemoved;
    else
      isFullyRemoved = SketchSolver_Storage::removeEntity((*anIt)->baseAttribute()) && isFullyRemoved;
  }
  return isFullyRemoved;
}

bool SketchSolver_Storage::remove(EntityWrapperPtr theEntity)
{
  bool isFullyRemoved = true;
  std::list<EntityWrapperPtr>::const_iterator anEntIt = theEntity->subEntities().begin();
  for (; anEntIt != theEntity->subEntities().end(); ++anEntIt) {
    FeaturePtr aBaseFeature = (*anEntIt)->baseFeature();
    if (aBaseFeature)
      isFullyRemoved = SketchSolver_Storage::removeEntity(aBaseFeature) && isFullyRemoved;
    else
      isFullyRemoved = SketchSolver_Storage::removeEntity((*anEntIt)->baseAttribute()) && isFullyRemoved;
  }

  std::list<ParameterWrapperPtr>::const_iterator aParIt = theEntity->parameters().begin();
  for (; aParIt != theEntity->parameters().end(); ++aParIt)
    isFullyRemoved = remove(*aParIt) && isFullyRemoved;
  return isFullyRemoved;
}


bool SketchSolver_Storage::isInteract(const FeaturePtr& theFeature) const
{
  if (!theFeature)
    return false;
  if (myConstraintMap.empty())
    return true; // empty storage interacts with each feature

  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstraint) {
    if (myConstraintMap.find(aConstraint) != myConstraintMap.end())
      return true;
  } else if (myFeatureMap.find(theFeature) != myFeatureMap.end())
    return true;

  std::list<AttributePtr> anAttrList = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::const_iterator anIt = anAttrList.begin();
  for (; anIt != anAttrList.end(); ++anIt)
    if (isInteract(*anIt))
      return true;

  return false;
}

bool SketchSolver_Storage::isInteract(const AttributePtr& theAttribute) const
{
  if (!theAttribute)
    return false;

  AttributeRefListPtr aRefList = 
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
  if (aRefList) {
    std::list<ObjectPtr> anObjects = aRefList->list();
    std::list<ObjectPtr>::iterator anObjIt = anObjects.begin();
    for (; anObjIt != anObjects.end(); ++anObjIt) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*anObjIt);
      if (isInteract(aFeature))
        return true;
    }
    return false;
  }

  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
    return myAttributeMap.find(theAttribute) != myAttributeMap.end();
  if (!aRefAttr->isObject())
    return myAttributeMap.find(aRefAttr->attr()) != myAttributeMap.end();

  FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
  return isInteract(aFeature);
}

bool SketchSolver_Storage::isConsistent() const
{
  // Check the constraints are valid
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIter = myConstraintMap.begin();
  for (; aCIter != myConstraintMap.end(); ++aCIter)
    if (!aCIter->first->data() || !aCIter->first->data()->isValid())
      return false;
  // Check the features are valid
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIter = myFeatureMap.begin();
  for (; aFIter != myFeatureMap.end(); aFIter++)
    if (!aFIter->first->data() || !aFIter->first->data()->isValid())
      return false;
  return true;
}

bool SketchSolver_Storage::isFixed(EntityWrapperPtr theEntity) const
{
  if (theEntity->group() != myGroupID)
    return true;
  // no need additional checking for entities differ than point
  if (theEntity->type() != ENTITY_POINT)
    return false;

  CoincidentPointsMap::const_iterator anIt = myCoincidentPoints.begin();
  for (; anIt != myCoincidentPoints.end(); ++anIt)
    if (anIt->first == theEntity || anIt->second.find(theEntity) != anIt->second.end()) {
      if (anIt->first->group() != myGroupID)
        return true;
      std::set<EntityWrapperPtr>::const_iterator anEntIt = anIt->second.begin();
      for (; anEntIt != anIt->second.end(); ++anEntIt)
        if ((*anEntIt)->group() != myGroupID)
          return true;
    }

  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator aCIt = myConstraintMap.begin();
  std::list<ConstraintWrapperPtr>::const_iterator aCWIt;
  for (; aCIt != myConstraintMap.end(); ++aCIt) {
    if (aCIt->second.empty())
      continue;
    aCWIt = aCIt->second.begin();
    if ((*aCWIt)->type() != CONSTRAINT_FIXED)
      continue;
    for (; aCWIt != aCIt->second.end(); ++aCIt)
      if ((theEntity->baseAttribute() && (*aCWIt)->isUsed(theEntity->baseAttribute())) ||
          (theEntity->baseFeature() && (*aCWIt)->isUsed(theEntity->baseFeature())))
        return true;
  }

  return false;
}

void SketchSolver_Storage::removeInvalidEntities()
{
  // Remove invalid constraints
  std::list<ConstraintPtr> anInvalidConstraints;
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIter = myConstraintMap.begin();
  for (; aCIter != myConstraintMap.end(); ++aCIter)
    if (!aCIter->first->data() || !aCIter->first->data()->isValid())
      anInvalidConstraints.push_back(aCIter->first);
  std::list<ConstraintPtr>::const_iterator anInvCIt = anInvalidConstraints.begin();
  for (; anInvCIt != anInvalidConstraints.end(); ++anInvCIt)
    removeConstraint(*anInvCIt);
  // Remove invalid features
  std::list<FeaturePtr> anInvalidFeatures;
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIter = myFeatureMap.begin();
  for (; aFIter != myFeatureMap.end(); aFIter++)
    if (!aFIter->first->data() || !aFIter->first->data()->isValid())
      anInvalidFeatures.push_back(aFIter->first);
  std::list<FeaturePtr>::const_iterator anInvFIt = anInvalidFeatures.begin();
  for (; anInvFIt != anInvalidFeatures.end(); ++anInvFIt)
    removeEntity(*anInvFIt);
}

EntityWrapperPtr SketchSolver_Storage::getNormal() const
{
  EntityWrapperPtr aSketch = sketch();
  if (!aSketch)
    return aSketch;

  // Find normal entity
  const std::list<EntityWrapperPtr>& aSketchSubs = aSketch->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSketchSubs.begin();
  for (; aSIt != aSketchSubs.end(); ++aSIt)
    if ((*aSIt)->type() == ENTITY_NORMAL)
      return *aSIt;
  return EntityWrapperPtr();
}

const EntityWrapperPtr& SketchSolver_Storage::sketch() const
{
  static EntityWrapperPtr aDummySketch;

  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIt = myFeatureMap.begin();
  for (; aFIt != myFeatureMap.end(); ++aFIt)
    if (aFIt->second && aFIt->second->type() == ENTITY_SKETCH)
      break;
  if (aFIt == myFeatureMap.end())
    return aDummySketch;
  return aFIt->second;
}

void SketchSolver_Storage::setSketch(const EntityWrapperPtr& theSketch)
{
  if (sketch())
    return;
  addEntity(FeaturePtr(), theSketch);
}

void SketchSolver_Storage::blockEvents(bool isBlocked)
{
  if (isBlocked == myEventsBlocked)
    return;

  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIter = myConstraintMap.begin();
  for (; aCIter != myConstraintMap.end(); aCIter++)
    if (aCIter->first->data() && aCIter->first->data()->isValid())
      aCIter->first->data()->blockSendAttributeUpdated(isBlocked);

  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIter = myFeatureMap.begin();
  for (; aFIter != myFeatureMap.end(); aFIter++)
    if (aFIter->first->data() && aFIter->first->data()->isValid())
      aFIter->first->data()->blockSendAttributeUpdated(isBlocked);

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anAtIter = myAttributeMap.begin();
  for (; anAtIter != myAttributeMap.end(); anAtIter++)
    if (anAtIter->first->owner() && anAtIter->first->owner()->data() &&
        anAtIter->first->owner()->data()->isValid())
      anAtIter->first->owner()->data()->blockSendAttributeUpdated(isBlocked);
  myEventsBlocked = isBlocked;
}






// ==============   Auxiliary functions   ====================================
bool isEqual(const std::list<ConstraintWrapperPtr>& theCVec1,
             const std::list<ConstraintWrapperPtr>& theCVec2)
{
  if (theCVec1.size() != theCVec2.size())
    return false;

  std::list<bool> aChecked(theCVec2.size(), false);
  std::list<ConstraintWrapperPtr>::const_iterator anIt1 = theCVec1.begin();
  for (; anIt1 != theCVec1.end(); ++anIt1) {
    std::list<ConstraintWrapperPtr>::const_iterator anIt2 = theCVec2.begin();
    std::list<bool>::iterator aCheckIt = aChecked.begin();
    while (aCheckIt != aChecked.end() && *aCheckIt) {
      ++aCheckIt;
      ++anIt2;
    }
    for (; anIt2 != theCVec2.end(); ++anIt2, ++aCheckIt)
      if (!(*aCheckIt) && (*anIt1)->isEqual(*anIt2)) {
        *aCheckIt = true;
        break;
      }
    // the same constraint is not found
    if (anIt2 == theCVec2.end())
      return false;
  }
  return true;
}
