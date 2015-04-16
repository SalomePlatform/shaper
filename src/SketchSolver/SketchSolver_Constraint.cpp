#include <SketchSolver_Constraint.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>

#include <math.h>

SketchSolver_Constraint::SketchSolver_Constraint(
    ConstraintPtr  theConstraint)
  : myBaseConstraint(theConstraint),
    myGroup(0)
{
}

SketchSolver_Constraint::~SketchSolver_Constraint()
{
  std::map<AttributePtr, Slvs_hParam>::const_iterator anIt1 = myValueMap.begin();
  for (; anIt1 != myValueMap.end(); anIt1++)
    myStorage->removeParameter(anIt1->second);
  myValueMap.clear();

  std::map<AttributePtr, Slvs_hEntity>::const_iterator anIt2 = myAttributeMap.begin();
  for (; anIt2 != myAttributeMap.end(); anIt2++)
    myStorage->removeEntity(anIt2->second);
  myAttributeMap.clear();

  std::map<FeaturePtr, Slvs_hEntity>::iterator anIt3 =  myFeatureMap.begin();
  while (!myFeatureMap.empty()) {
    std::shared_ptr<SketchPlugin_Feature> aFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(anIt3->first);
    Slvs_hEntity anEnt = anIt3->second;
    std::map<FeaturePtr, Slvs_hEntity>::iterator aRemIt = anIt3++;
    myFeatureMap.erase(aRemIt);
    if (!myGroup->isInteract(aFeature))
      myStorage->removeEntity(anEnt);
  }

  std::vector<Slvs_hConstraint>::const_iterator anIt4 = mySlvsConstraints.begin();
  for (; anIt4 != mySlvsConstraints.end(); anIt4++)
    myStorage->removeConstraint(*anIt4);
  mySlvsConstraints.clear();
}

void SketchSolver_Constraint::setStorage(StoragePtr theStorage)
{
  myStorage = theStorage;
  process();
}

void SketchSolver_Constraint::setGroup(SketchSolver_Group* theGroup)
{
  myGroup = theGroup;
  process();
}

void SketchSolver_Constraint::addFeature(FeaturePtr theFeature)
{
  int aType;
  changeEntity(theFeature, aType);
}


void SketchSolver_Constraint::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  int aConstrType = getType();
  double aValue = 0.0;
  std::vector<Slvs_hEntity> anAttributes;
  getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;

  Slvs_hGroup aGroupID = myGroup->getId();
  Slvs_hEntity aWorkplaneID = myGroup->getWorkplaneId();
  Slvs_Constraint aConstraint;
  if (mySlvsConstraints.empty())
    aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, aGroupID, aConstrType, aWorkplaneID,
        aValue, anAttributes[0], anAttributes[1], anAttributes[2], anAttributes[3]);
  else {
    aConstraint = myStorage->getConstraint(mySlvsConstraints[0]);
    aConstraint.valA = aValue;
    static const int aNbAttrs = 6;
    Slvs_hEntity* aConstrAttrs[aNbAttrs] = {
        &aConstraint.ptA, &aConstraint.ptB,
        &aConstraint.entityA, &aConstraint.entityB,
        &aConstraint.entityC, &aConstraint.entityD};
    std::vector<Slvs_hEntity>::const_iterator anIter = anAttributes.begin();
    for (int i = 0; i < aNbAttrs && anIter != anAttributes.end(); i++, anIter++)
      *(aConstrAttrs[i]) = *anIter;
  }

  Slvs_hConstraint anID = myStorage->addConstraint(aConstraint);
  if (mySlvsConstraints.empty())
    mySlvsConstraints.push_back(anID);
  else
    mySlvsConstraints[0] = anID;
  adjustConstraint();
}

void SketchSolver_Constraint::update(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  bool needToRebuild = (theConstraint && theConstraint != myBaseConstraint);
  if (!needToRebuild) {
    // Check the attrbutes of constraint are changed
    ConstraintPtr aConstraint = theConstraint ? theConstraint : myBaseConstraint;
    std::list<AttributePtr> anAttrList = aConstraint->data()->attributes(std::string());
    std::list<AttributePtr>::iterator anAttrIter = anAttrList.begin();
    for (; anAttrIter != anAttrList.end(); anAttrIter++) {
      AttributeRefAttrPtr aRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIter);
      if (aRefAttr) {
        if (aRefAttr->isObject()) {
          FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
          if (aFeature && myFeatureMap.find(aFeature) == myFeatureMap.end()) {
            needToRebuild = true;
            break;
          }
        } else if (aRefAttr->attr() &&
                    myAttributeMap.find(aRefAttr->attr()) == myAttributeMap.end()) {
          needToRebuild = true;
          break;
        }
      }
      AttributeRefListPtr aRefList =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anAttrIter);
      if (aRefList) {
        std::list<ObjectPtr> anItems = aRefList->list();
        std::list<ObjectPtr>::iterator anIt = anItems.begin();
        for (; anIt != anItems.end(); anIt++) {
          FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
          if (aFeature && myFeatureMap.find(aFeature) == myFeatureMap.end()) {
            needToRebuild = true;
            break;
          }
        }
        if (needToRebuild)
          break;
      }
    }
  }
  if (needToRebuild) {
    if (theConstraint && theConstraint->getKind() != myBaseConstraint->getKind())
      return;
    remove(myBaseConstraint);
    if (theConstraint)
      myBaseConstraint = theConstraint;
    process();
    return;
  }

  // Update all attributes
  int aType;
  std::map<Slvs_hEntity, Slvs_hEntity> aRelocationMap;
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIter = myFeatureMap.begin();
  for (; aFeatIter != myFeatureMap.end(); aFeatIter++) {
    Slvs_hEntity aPrevID = aFeatIter->second;
    aFeatIter->second = changeEntity(aFeatIter->first, aType);
    if (aFeatIter->second != aPrevID)
      aRelocationMap[aPrevID] = aFeatIter->second;
  }
  std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIter = myAttributeMap.begin();
  for (; anAttrIter != myAttributeMap.end(); anAttrIter++) {
    Slvs_hEntity aPrevID = anAttrIter->second;
    anAttrIter->second = changeEntity(anAttrIter->first, aType);
    if (anAttrIter->second != aPrevID)
      aRelocationMap[aPrevID] = anAttrIter->second;
  }

  // Value if exists
  DataPtr aData = myBaseConstraint->data();
  if (!aData) return;
  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    myBaseConstraint->attribute(SketchPlugin_Constraint::VALUE()));
  double aValue = aValueAttr ? aValueAttr->value() : 0.0;

  // Update constraint
  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++) {
    Slvs_Constraint aConstraint = myStorage->getConstraint(*aCIter);
    if (aValueAttr)
      aConstraint.valA = aValue;
    Slvs_hEntity* aCoeffs[6] = {
        &aConstraint.ptA, &aConstraint.ptB,
        &aConstraint.entityA, &aConstraint.entityB,
        &aConstraint.entityC, &aConstraint.entityD};
    for (int i = 0; i < 6; i++) {
      if (*(aCoeffs[i]) == SLVS_E_UNKNOWN)
        continue;
      std::map<Slvs_hEntity, Slvs_hEntity>::iterator aFound = aRelocationMap.find(*(aCoeffs[i]));
      if (aFound != aRelocationMap.end())
        *(aCoeffs[i]) = aFound->second;
    }
    *aCIter = myStorage->addConstraint(aConstraint);
  }
  adjustConstraint();
}

bool SketchSolver_Constraint::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  if (mySlvsConstraints.empty())
    return true;
  bool isFullyRemoved = myStorage->removeConstraint(mySlvsConstraints.front());
  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();
  mySlvsConstraints.clear();
  return true;
}

void SketchSolver_Constraint::cleanRemovedEntities()
{
  std::set<Slvs_hParam> aRemovedParams;
  std::set<Slvs_hEntity> aRemovedEntities;
  std::set<Slvs_hConstraint> aRemovedConstraints;
  myStorage->getRemoved(aRemovedParams, aRemovedEntities, aRemovedConstraints);
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.begin();
  while (aFeatIt != myFeatureMap.end()) {
    if (aRemovedEntities.find(aFeatIt->second) == aRemovedEntities.end()) {
      aFeatIt++;
      continue;
    }
    std::map<FeaturePtr, Slvs_hEntity>::iterator aTmpIter = aFeatIt++;
    myFeatureMap.erase(aTmpIter);
  }
  std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIt = myAttributeMap.begin();
  while (anAttrIt != myAttributeMap.end()) {
    if (aRemovedEntities.find(anAttrIt->second) == aRemovedEntities.end()) {
      anAttrIt++;
      continue;
    }
    std::map<AttributePtr, Slvs_hEntity>::iterator aTmpIter = anAttrIt++;
    myAttributeMap.erase(aTmpIter);
  }
  std::map<AttributePtr, Slvs_hParam>::iterator aValIt = myValueMap.begin();
  while (aValIt != myValueMap.end()) {
    if (aRemovedParams.find(aValIt->second) == aRemovedParams.end()) {
      aValIt++;
      continue;
    }
    std::map<AttributePtr, Slvs_hParam>::iterator aTmpIter = aValIt++;
    myValueMap.erase(aTmpIter);
  }
}

void SketchSolver_Constraint::getAttributes(
    double& theValue,
    std::vector<Slvs_hEntity>& theAttributes)
{
  static const int anInitNbOfAttr = 4;
  theAttributes.assign(anInitNbOfAttr, SLVS_E_UNKNOWN);

  DataPtr aData = myBaseConstraint->data();

  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    aData->attribute(SketchPlugin_Constraint::VALUE()));
  theValue = aValueAttr ? aValueAttr->value() : 0.0;

  int aPtInd = 0; // index of first point in the list of attributes
  int aEntInd = 2; // index of first antity in the list of attributes
  std::list<AttributePtr> aConstrAttrs = aData->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anIter = aConstrAttrs.begin();
  for (; anIter != aConstrAttrs.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (!aRefAttr || !aRefAttr->isInitialized()) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }

    int aType = SLVS_E_UNKNOWN; // type of created entity
    Slvs_hEntity anEntity = myGroup->getAttributeId(aRefAttr);
    if (anEntity == SLVS_E_UNKNOWN)
      anEntity = changeEntity(aRefAttr, aType);
    else {
      Slvs_Entity anEnt = myStorage->getEntity(anEntity);
      aType = anEnt.type;
    }

    if (aType == SLVS_E_UNKNOWN)
      continue;
    else if (aType == SLVS_E_POINT_IN_2D || aType == SLVS_E_POINT_IN_3D)
      theAttributes[aPtInd++] = anEntity; // the point is created
    else { // another entity (not a point) is created
      if (aEntInd < anInitNbOfAttr)
        theAttributes[aEntInd] = anEntity;
      else
        theAttributes.push_back(anEntity);
      aEntInd++;
    }
  }
}

Slvs_hEntity SketchSolver_Constraint::changeEntity(AttributeRefAttrPtr theAttribute, int& theType)
{
  // Convert the object of the attribute to the feature
  FeaturePtr aFeature;
  if (theAttribute->isObject() && theAttribute->object()) {
    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
        theAttribute->object());
    if (!aRC) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return SLVS_E_UNKNOWN;
    }
    std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
    aFeature = aDoc->feature(aRC);

    return changeEntity(aFeature, theType);
  }

  return changeEntity(theAttribute->attr(), theType);
}

Slvs_hEntity SketchSolver_Constraint::changeEntity(AttributePtr theEntity, int& theType)
{
  Slvs_hEntity aResult = SLVS_E_UNKNOWN;
  if (!theEntity || !isInitialized(theEntity))
    return SLVS_E_UNKNOWN;

  // If the entity is already in the group, try to find it
  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator anEntIter =
      myAttributeMap.find(theEntity);
  Slvs_Entity aCurrentEntity;
  aCurrentEntity.h = SLVS_E_UNKNOWN;
  if (anEntIter != myAttributeMap.end())
    aCurrentEntity = myStorage->getEntity(anEntIter->second);
  else {
    aResult = myGroup->getAttributeId(theEntity);
    if (aResult != SLVS_E_UNKNOWN) {
      Slvs_Entity anEnt = myStorage->getEntity(aResult);
      theType = anEnt.type;
      myAttributeMap[theEntity] = aResult;
      return aResult;
    }
  }

  Slvs_hGroup aGroupID = myGroup->getId();
  // Point in 3D
  std::shared_ptr<GeomDataAPI_Point> aPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point>(theEntity);
  if (aPoint) {
    double aXYZ[3] = {aPoint->x(), aPoint->y(), aPoint->z()};
    Slvs_hParam aParams[3];
    for (int i = 0; i < 3; i++) {
      Slvs_Param aPar = aCurrentEntity.h != SLVS_E_UNKNOWN ?
          myStorage->getParameter(aCurrentEntity.param[i]) :
          Slvs_MakeParam(SLVS_E_UNKNOWN, aGroupID, 0.0);
      aPar.val = aXYZ[i];
      aParams[i] = myStorage->addParameter(aPar);
    }

    if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
      aCurrentEntity = Slvs_MakePoint3d(SLVS_E_UNKNOWN, aGroupID, aParams[0], aParams[1], aParams[2]);
    else { // update entity data
      for (int i = 0; i < 3; i++)
        aCurrentEntity.param[i] = aParams[i];
    }
    aResult = myStorage->addEntity(aCurrentEntity);
  } else {
    // All entities except 3D points are created on workplane. So, if there is no workplane yet, then error
    Slvs_hEntity aWorkplaneID = myGroup->getWorkplaneId();
    if (aWorkplaneID == SLVS_E_UNKNOWN)
      return SLVS_E_UNKNOWN;

    // Point in 2D
    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
    if (aPoint2D) {
      double aXY[2] = {aPoint2D->x(), aPoint2D->y()};
      Slvs_hParam aParams[2];
      for (int i = 0; i < 2; i++) {
        Slvs_Param aPar = aCurrentEntity.h != SLVS_E_UNKNOWN ?
            myStorage->getParameter(aCurrentEntity.param[i]) :
            Slvs_MakeParam(SLVS_E_UNKNOWN, aGroupID, 0.0);
        aPar.val = aXY[i];
        aParams[i] = myStorage->addParameter(aPar);
      }

      if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
        aCurrentEntity = Slvs_MakePoint2d(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID, aParams[0], aParams[1]);
      else { // update entity data
        for (int i = 0; i < 2; i++)
          aCurrentEntity.param[i] = aParams[i];
      }
      aResult = myStorage->addEntity(aCurrentEntity);
    } else {
      // Scalar value (used for the distance entities)
      AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
      if (aScalar) {
        Slvs_Param aParam = aCurrentEntity.h != SLVS_E_UNKNOWN ?
            myStorage->getParameter(aCurrentEntity.param[0]) :
            Slvs_MakeParam(SLVS_E_UNKNOWN, aGroupID, 0.0);
        aParam.val = aScalar->value();
        Slvs_hParam aValue = myStorage->addParameter(aParam);

        if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
          aCurrentEntity = Slvs_MakeDistance(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID, aValue);
        else
          aCurrentEntity.param[0] = aValue;
        aResult = myStorage->addEntity(aCurrentEntity);
      }
    }
  }

  myAttributeMap[theEntity] = aResult;
  theType = aCurrentEntity.type;
  return aResult;
}

Slvs_hEntity SketchSolver_Constraint::changeEntity(FeaturePtr theEntity, int& theType)
{
  Slvs_hEntity aResult = SLVS_E_UNKNOWN;
  if (!theEntity || !theEntity->data() || !theEntity->data()->isValid())
    return SLVS_E_UNKNOWN;
  // If the entity is already in the group, try to find it
  std::map<FeaturePtr, Slvs_hEntity>::const_iterator anEntIter = myFeatureMap.find(theEntity);
  Slvs_Entity aCurrentEntity;
  aCurrentEntity.h = SLVS_E_UNKNOWN;
  if (anEntIter != myFeatureMap.end())
    aCurrentEntity = myStorage->getEntity(anEntIter->second);
  else {
    aResult = myGroup->getFeatureId(theEntity);
    if (aResult != SLVS_E_UNKNOWN) {
      Slvs_Entity anEnt = myStorage->getEntity(aResult);
      theType = anEnt.type;
      myFeatureMap[theEntity] = aResult;
      return aResult;
    }
  }

  Slvs_hGroup aGroupID = myGroup->getId();
  Slvs_hEntity aWorkplaneID = myGroup->getWorkplaneId();
  DataPtr aData = theEntity->data();

  // SketchPlugin features
  const std::string& aFeatureKind = theEntity->getKind();
  AttributePtr anAttribute;
  int anAttrType;
  // Line
  if (aFeatureKind == SketchPlugin_Line::ID()) {
    anAttribute = aData->attribute(SketchPlugin_Line::START_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aStart = changeEntity(anAttribute, anAttrType);

    anAttribute = aData->attribute(SketchPlugin_Line::END_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aEnd = changeEntity(anAttribute, anAttrType);

    if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
      aCurrentEntity = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID, aStart, aEnd);
    else {
      aCurrentEntity.point[0] = aStart;
      aCurrentEntity.point[1] = aEnd;
    }
    aResult = myStorage->addEntity(aCurrentEntity);
  }
  // Circle
  else if (aFeatureKind == SketchPlugin_Circle::ID()) {
    anAttribute = aData->attribute(SketchPlugin_Circle::CENTER_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aCenter = changeEntity(anAttribute, anAttrType);

    anAttribute = aData->attribute(SketchPlugin_Circle::RADIUS_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aRadius = changeEntity(anAttribute, anAttrType);

    if (aCurrentEntity.h == SLVS_E_UNKNOWN) { // New entity
      Slvs_Entity aWorkplane = myStorage->getEntity(aWorkplaneID);
      aCurrentEntity = Slvs_MakeCircle(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID,
                                        aCenter, aWorkplane.normal, aRadius);
    } else {
      aCurrentEntity.point[0] = aCenter;
      aCurrentEntity.distance = aRadius;
    }
    aResult = myStorage->addEntity(aCurrentEntity);
  }
  // Arc
  else if (aFeatureKind == SketchPlugin_Arc::ID()) {
    anAttribute = aData->attribute(SketchPlugin_Arc::CENTER_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aCenter = changeEntity(anAttribute, anAttrType);

    anAttribute = aData->attribute(SketchPlugin_Arc::START_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aStart = changeEntity(anAttribute, anAttrType);

    anAttribute = aData->attribute(SketchPlugin_Arc::END_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    Slvs_hEntity aEnd = changeEntity(anAttribute, anAttrType);

    if (aCurrentEntity.h == SLVS_E_UNKNOWN) { // New entity
      Slvs_Entity aWorkplane = myStorage->getEntity(aWorkplaneID);
      aCurrentEntity = Slvs_MakeArcOfCircle(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID,
                                            aWorkplane.normal, aCenter, aStart, aEnd);
    } else {
      aCurrentEntity.point[0] = aCenter;
      aCurrentEntity.point[1] = aStart;
      aCurrentEntity.point[2] = aEnd;
    }
    aResult = myStorage->addEntity(aCurrentEntity);
  }
  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
  else if (aFeatureKind == SketchPlugin_Point::ID()) {
    anAttribute = aData->attribute(SketchPlugin_Point::COORD_ID());
    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
    // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
    aResult = changeEntity(anAttribute, anAttrType);
    aCurrentEntity.type = SLVS_E_POINT_IN_3D;
  }

  if (aResult != SLVS_E_UNKNOWN) {
    myFeatureMap[theEntity] = aResult;
    theType = aCurrentEntity.type;
  }
  return aResult;
}

std::list<ConstraintPtr> SketchSolver_Constraint::constraints() const
{
  std::list<ConstraintPtr> aConstraints;
  aConstraints.push_back(myBaseConstraint);
  return aConstraints;
}

void SketchSolver_Constraint::refresh()
{
  cleanErrorMsg();
  std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIter = myAttributeMap.begin();
  for (; anAttrIter != myAttributeMap.end(); anAttrIter++) {
    std::shared_ptr<GeomDataAPI_Point> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(anAttrIter->first);
    if (aPoint) {
      Slvs_Entity anEntity = myStorage->getEntity(anAttrIter->second);
      double aXYZ[3];
      for (int i = 0; i < 3; i++) {
        Slvs_Param aPar = myStorage->getParameter(anEntity.param[i]);
        aXYZ[i] = aPar.val;
      }
      if (fabs(aPoint->x() - aXYZ[0]) > tolerance ||
          fabs(aPoint->y() - aXYZ[1]) > tolerance ||
          fabs(aPoint->z() - aXYZ[2]) > tolerance)
        aPoint->setValue(aXYZ[0], aXYZ[1], aXYZ[2]);
    } else {
      // Point in 2D
      std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrIter->first);
      if (aPoint2D) {
        Slvs_Entity anEntity = myStorage->getEntity(anAttrIter->second);
        double aXY[2];
        for (int i = 0; i < 2; i++) {
          Slvs_Param aPar = myStorage->getParameter(anEntity.param[i]);
          aXY[i] = aPar.val;
        }
        if (fabs(aPoint2D->x() - aXY[0]) > tolerance ||
            fabs(aPoint2D->y() - aXY[1]) > tolerance)
          aPoint2D->setValue(aXY[0], aXY[1]);
      } else {
        // Scalar value (used for the distance entities)
        AttributeDoublePtr aScalar =
            std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttrIter->first);
        if (aScalar) {
          Slvs_Entity anEntity = myStorage->getEntity(anAttrIter->second);
          Slvs_Param aPar = myStorage->getParameter(anEntity.param[0]);
          if (fabs(aScalar->value() - aPar.val) > tolerance)
            aScalar->setValue(aPar.val);
        }
      }
    }
  }

  std::map<AttributePtr, Slvs_hParam>::iterator aValIter = myValueMap.begin();
  for (; aValIter != myValueMap.end(); aValIter++) {
    AttributeDoublePtr aScalar =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttrIter->first);
    if (aScalar) {
      Slvs_Param aPar = myStorage->getParameter(anAttrIter->second);
      aScalar->setValue(aPar.val);
    }
  }
}

Slvs_hEntity SketchSolver_Constraint::getId(FeaturePtr theFeature) const
{
  std::map<FeaturePtr, Slvs_hEntity>::const_iterator aFIter = myFeatureMap.find(theFeature);
  if (aFIter == myFeatureMap.end())
    return SLVS_E_UNKNOWN;
  // check the Feature is really in the storage
  Slvs_Entity anEntity = myStorage->getEntity(aFIter->second);
  if (anEntity.h == SLVS_E_UNKNOWN) {
    // rebuild feature
    int aType;
    anEntity.h = const_cast<SketchSolver_Constraint*>(this)->changeEntity(aFIter->first, aType);
    const_cast<SketchSolver_Constraint*>(this)->myFeatureMap[theFeature] = anEntity.h;
  }
  return anEntity.h;
}

Slvs_hEntity SketchSolver_Constraint::getId(AttributePtr theAttribute) const
{
  std::map<AttributePtr, Slvs_hEntity>::const_iterator anAttrIter = myAttributeMap.find(theAttribute);
  if (anAttrIter == myAttributeMap.end())
    return SLVS_E_UNKNOWN;
  return anAttrIter->second;
}

bool SketchSolver_Constraint::isInitialized(AttributePtr theAttribute)
{
  if (theAttribute->isInitialized())
    return true;
  myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
  return false;
}


void SketchSolver_Constraint::calculateMiddlePoint(
    const Slvs_Entity& theEntity, double theCoeff, double& theX, double& theY) const
{
  if (theEntity.type == SLVS_E_LINE_SEGMENT) {
    double aStartEndXY[2][2];
    Slvs_Entity aPoint;
    for (int i = 0; i < 2; i++) {
      aPoint = myStorage->getEntity(theEntity.point[i]);
      for (int j = 0; j < 2; j++)
        aStartEndXY[i][j] = myStorage->getParameter(aPoint.param[j]).val;
    }
    theX = (1.0 - theCoeff) * aStartEndXY[0][0] + theCoeff * aStartEndXY[1][0];
    theY = (1.0 - theCoeff) * aStartEndXY[0][1] + theCoeff * aStartEndXY[1][1];
  } else if (theEntity.type == SLVS_E_ARC_OF_CIRCLE) {
    double anArcPoint[3][2];
    Slvs_Entity aPoint;
    for (int i = 0; i < 3; i++) {
      aPoint = myStorage->getEntity(theEntity.point[i]);
      for (int j = 0; j < 2; j++)
        anArcPoint[i][j] = myStorage->getParameter(aPoint.param[j]).val;
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
      return;
    } else if (1 - theCoeff < tolerance) {
      theX = anArcPoint[0][0] + anArcPoint[2][0];
      theY = anArcPoint[0][1] + anArcPoint[2][1];
      return;
    }

    double xStart = anArcPoint[1][0] / aRad, xEnd = anArcPoint[2][0] / aRad;
    double yStart = anArcPoint[1][1] / aRad, yEnd = anArcPoint[2][1] / aRad;
    double aTanStart = abs(xStart) < tolerance ? yStart : yStart / xStart;
    double aTanEnd   = abs(xEnd) < tolerance   ? yEnd   : yEnd / xEnd;
    double aCotStart = abs(yStart) < tolerance ? xStart : xStart / yStart;
    double aCotEnd   = abs(yEnd) < tolerance   ? xEnd   : xEnd / yEnd;
    if (anArcPoint[1][0] * anArcPoint[2][0] < 0.0) {
      if (anArcPoint[1][0] > 0.0)
        yEnd = 2.0 - yEnd;
      else
        yStart = -2.0 - yStart;
    } else {
      if (aTanStart > aTanEnd) {
        if (yStart > yEnd) {
          yStart = 2.0 - yStart;
          yEnd = -2.0 - yEnd;
        } else {
          yStart = -2.0 - yStart;
          yEnd = 2.0 - yEnd;
        }
      }
    }
    if (anArcPoint[1][1] * anArcPoint[2][1] < 0.0) {
      if (anArcPoint[1][1] > 0.0)
        xEnd = 2.0 - xEnd;
      else
        xStart = -2.0 - xStart;
    } else {
      if (aCotStart < aCotEnd) {
        if (xStart > xEnd) {
          xStart = 2.0 - xStart;
          xEnd = -2.0 - xEnd;
        } else {
          xStart = -2.0 - xStart;
          xEnd = 2.0 - xEnd;
        }
      }
    }
    x = (1.0 - theCoeff) * xStart + theCoeff * xEnd;
    y = (1.0 - theCoeff) * yStart + theCoeff * yEnd;
    if (x > 1.0) x = 2.0 - x;
    if (x < -1.0) x = -2.0 - x;
    if (y > 1.0) y = 2.0 - y;
    if (y < -1.0) y = -2.0 - y;

    aNorm = sqrt(x*x + y*y);
    if (aNorm >= tolerance) {
      x *= aRad / aNorm;
      y *= aRad / aNorm;
    } else {
      x = -0.5 * (anArcPoint[2][1] + anArcPoint[1][1]);
      y = -0.5 * (anArcPoint[2][0] + anArcPoint[1][0]);
    }
    theX = anArcPoint[0][0] + x;
    theY = anArcPoint[0][1] + y;
  }
}

void SketchSolver_Constraint::makeTemporary() const
{
  std::vector<Slvs_hConstraint>::const_iterator anIt = mySlvsConstraints.begin();
  for (; anIt != mySlvsConstraints.end(); anIt++)
    myStorage->addTemporaryConstraint(*anIt);
}

