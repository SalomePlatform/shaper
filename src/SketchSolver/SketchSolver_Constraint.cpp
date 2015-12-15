#include <SketchSolver_Constraint.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>

#include <GeomAPI_Dir2d.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>

#include <math.h>

SketchSolver_Constraint::SketchSolver_Constraint(
    ConstraintPtr  theConstraint)
  : myBaseConstraint(theConstraint),
    myGroupID(GID_UNKNOWN),
    myType(CONSTRAINT_UNKNOWN)
{
}

void SketchSolver_Constraint::process(StoragePtr theStorage,
                                      const GroupID& theGroupID,
                                      const EntityID& theSketchID)
{
  myStorage = theStorage;
  myGroupID = theGroupID;
  mySketchID = theSketchID;
  // Process constraint according to its type
  process();
}


SketchSolver_ConstraintType SketchSolver_Constraint::TYPE(ConstraintPtr theConstraint)
{
  const std::string& aType = theConstraint->getKind();
  if (aType == SketchPlugin_ConstraintCoincidence::ID())
    return CONSTRAINT_COINCIDENCE;
  else if (aType == SketchPlugin_ConstraintRigid::ID())
    return CONSTRAINT_FIXED;
  else if (aType == SketchPlugin_ConstraintHorizontal::ID())
    return CONSTRAINT_HORIZONTAL;
  else if (aType == SketchPlugin_ConstraintVertical::ID())
    return CONSTRAINT_VERTICAL;
  else if (aType == SketchPlugin_ConstraintAngle::ID())
    return CONSTRAINT_ANGLE;
  else if (aType == SketchPlugin_ConstraintDistance::ID())
    return CONSTRAINT_DISTANCE;
  else if (aType == SketchPlugin_ConstraintEqual::ID())
    return CONSTRAINT_EQUAL;
  else if (aType == SketchPlugin_ConstraintLength::ID())
    return CONSTRAINT_PT_PT_DISTANCE;
  else if (aType == SketchPlugin_ConstraintMirror::ID())
    return CONSTRAINT_SYMMETRIC;
  else if (aType == SketchPlugin_ConstraintParallel::ID())
    return CONSTRAINT_PARALLEL;
  else if (aType == SketchPlugin_ConstraintPerpendicular::ID())
    return CONSTRAINT_PERPENDICULAR;
  else if (aType == SketchPlugin_ConstraintRadius::ID())
    return CONSTRAINT_RADIUS;
  else if (aType == SketchPlugin_ConstraintTangent::ID())
    return CONSTRAINT_TANGENT;
  return CONSTRAINT_UNKNOWN;
}

void SketchSolver_Constraint::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    // Not enough parameters are assigned
    return;
  }
////  if (!myConstraints.empty()) // some data is changed, update constraint
////    update();

  SketchSolver_ConstraintType aConstrType = getType();
  double aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;
  if (anAttributes.empty()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }
  if (aConstrType == CONSTRAINT_UNKNOWN)
    aConstrType = getType();

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aNewConstraints = aBuilder->createConstraint(
      myBaseConstraint, myGroupID, mySketchID, aConstrType,
      aValue, anAttributes[0], anAttributes[1], anAttributes[2], anAttributes[3]);
  myStorage->addConstraint(myBaseConstraint, aNewConstraints);

  adjustConstraint();
}

////bool SketchSolver_Constraint::checkAttributesChanged(ConstraintPtr theConstraint)
////{
////  std::set<Slvs_hEntity> aCurAttrs; // list of currently used attributes
////  std::vector<Slvs_hConstraint>::const_iterator aConstrIter = mySlvsConstraints.begin();
////  for (; aConstrIter != mySlvsConstraints.end(); aConstrIter++) {
////    Slvs_Constraint aConstr = myStorage->getConstraint(*aConstrIter);
////    if (aConstr.ptA != SLVS_E_UNKNOWN) aCurAttrs.insert(aConstr.ptA);
////    if (aConstr.ptB != SLVS_E_UNKNOWN) aCurAttrs.insert(aConstr.ptB);
////    if (aConstr.entityA != SLVS_E_UNKNOWN) aCurAttrs.insert(aConstr.entityA);
////    if (aConstr.entityB != SLVS_E_UNKNOWN) aCurAttrs.insert(aConstr.entityB);
////    if (aConstr.entityC != SLVS_E_UNKNOWN) aCurAttrs.insert(aConstr.entityC);
////    if (aConstr.entityD != SLVS_E_UNKNOWN) aCurAttrs.insert(aConstr.entityD);
////  }
////  // Check the attrbutes of constraint are changed
////  ConstraintPtr aConstraint = theConstraint ? theConstraint : myBaseConstraint;
////  std::list<AttributePtr> anAttrList = aConstraint->data()->attributes(std::string());
////  std::list<AttributePtr>::iterator anAttrIter = anAttrList.begin();
////  for (; anAttrIter != anAttrList.end(); anAttrIter++) {
////    AttributeRefAttrPtr aRefAttr =
////        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIter);
////    if (aRefAttr) {
////      if (aRefAttr->isObject()) {
////        FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
////        std::map<FeaturePtr, Slvs_hEntity>::iterator aFIt = myFeatureMap.find(aFeature);
////        if (aFeature) {
////          if (aFIt == myFeatureMap.end())
////            return true;
////          // Additional check the points of entity
////          if (aCurAttrs.find(aFIt->second) == aCurAttrs.end()) {
////            Slvs_Entity anEntity = myStorage->getEntity(aFIt->second);
////            bool isFound = false;
////            for (int i = 0; i < 4 && !isFound; i++)
////              if (anEntity.point[i] != SLVS_E_UNKNOWN && 
////                  aCurAttrs.find(anEntity.point[i]) != aCurAttrs.end())
////                isFound = true;
////            if (!isFound)
////              return true;
////          }
////        }
////      } else if (aRefAttr->attr()) {
////        std::map<AttributePtr, Slvs_hEntity>::iterator anAIt = myAttributeMap.find(aRefAttr->attr());
////        if (anAIt == myAttributeMap.end() || aCurAttrs.find(anAIt->second) == aCurAttrs.end())
////          return true;
////      }
////    }
////    AttributeRefListPtr aRefList =
////        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anAttrIter);
////    if (aRefList) {
////      std::list<ObjectPtr> anItems = aRefList->list();
////      std::list<ObjectPtr>::iterator anIt = anItems.begin();
////      for (; anIt != anItems.end(); anIt++) {
////        FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
////        if (aFeature && myFeatureMap.find(aFeature) == myFeatureMap.end())
////          return true;
////      }
////    }
////  }
////  return false;
////}

void SketchSolver_Constraint::update()
{
  cleanErrorMsg();

  std::list<ConstraintWrapperPtr> aWrapper = myStorage->constraint(myBaseConstraint);
  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::VALUE()));
  if (aValueAttr) {
    std::list<ConstraintWrapperPtr>::iterator aWIt = aWrapper.begin();
    for (; aWIt != aWrapper.end(); ++aWIt)
      (*aWIt)->setValue(aValueAttr->value());
  }
  myStorage->addConstraint(myBaseConstraint, aWrapper);

  adjustConstraint();
}

bool SketchSolver_Constraint::remove()
{
  cleanErrorMsg();
  return myStorage->removeConstraint(myBaseConstraint);
}

void SketchSolver_Constraint::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  static const int anInitNbOfAttr = 4;
  theAttributes.assign(anInitNbOfAttr, EntityWrapperPtr());

  DataPtr aData = myBaseConstraint->data();
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  myType = TYPE(myBaseConstraint);

  AttributeDoublePtr aValueAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()));
  theValue = aValueAttr ? aValueAttr->value() : 0.0;

  int aPtInd = 0; // index of first point in the list of attributes
  int aEntInd = 2; // index of first entity in the list of attributes
  std::list<AttributePtr> aConstrAttrs = aData->attributes(ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::iterator anIter = aConstrAttrs.begin();
  for (; anIter != aConstrAttrs.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (!aRefAttr || !aRefAttr->isInitialized()) {
      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
      return;
    }

    myStorage->update(aRefAttr, myGroupID);
    EntityWrapperPtr anEntity = myStorage->entity(aRefAttr);

    SketchSolver_EntityType aType = anEntity->type();
    if (aType == ENTITY_UNKNOWN)
      continue;
    else if (aType == ENTITY_POINT)
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

bool SketchSolver_Constraint::isUsed(FeaturePtr theFeature) const
{
  const std::list<ConstraintWrapperPtr>& aCList = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aCList.begin();
  for (; aCIt != aCList.end(); ++aCIt)
    if ((*aCIt)->isUsed(theFeature))
      return true;

  std::list<AttributePtr> anAttrList = theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::const_iterator anAttrIt = anAttrList.begin();
  for (; anAttrIt != anAttrList.end(); ++ anAttrIt)
    if (isUsed(*anAttrIt))
      return true;

  return false;
}

bool SketchSolver_Constraint::isUsed(AttributePtr theAttribute) const
{
  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject())
      return isUsed(ModelAPI_Feature::feature(aRefAttr->object()));
    else
      anAttribute = aRefAttr->attr();
  }

  const std::list<ConstraintWrapperPtr>& aCList = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aCList.begin();
  for (; aCIt != aCList.end(); ++aCIt)
    if ((*aCIt)->isUsed(theAttribute))
      return true;
  return false;
}

////Slvs_hEntity SketchSolver_Constraint::changeEntity(AttributeRefAttrPtr theAttribute, int& theType)
////{
////  // Convert the object of the attribute to the feature
////  FeaturePtr aFeature;
////  if (theAttribute->isObject() && theAttribute->object()) {
////    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
////        theAttribute->object());
////    if (!aRC) {
////      myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
////      return SLVS_E_UNKNOWN;
////    }
////    std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
////    aFeature = aDoc->feature(aRC);
////
////    return changeEntity(aFeature, theType);
////  }
////
////  return changeEntity(theAttribute->attr(), theType);
////}
////
////Slvs_hEntity SketchSolver_Constraint::changeEntity(AttributePtr theEntity, int& theType)
////{
////  Slvs_hEntity aResult = SLVS_E_UNKNOWN;
////  if (!theEntity || !isInitialized(theEntity)) {
////    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
////    return SLVS_E_UNKNOWN;
////  }
////
////  // If the entity is already in the group, try to find it
////  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator anEntIter =
////      myAttributeMap.find(theEntity);
////  Slvs_Entity aCurrentEntity;
////  aCurrentEntity.h = SLVS_E_UNKNOWN;
////  if (anEntIter != myAttributeMap.end())
////    aCurrentEntity = myStorage->getEntity(anEntIter->second);
////  else {
////    aResult = myGroup->getAttributeId(theEntity);
////    if (aResult != SLVS_E_UNKNOWN) {
////      Slvs_Entity anEnt = myStorage->getEntity(aResult);
////      theType = anEnt.type;
////      myAttributeMap[theEntity] = aResult;
////      return aResult;
////    }
////  }
////
////  Slvs_hGroup aGroupID = myGroup->getId();
////  // do not update entity from another group
////  if (aCurrentEntity.h != SLVS_E_UNKNOWN && aGroupID != aCurrentEntity.group)
////    return aCurrentEntity.h;
////
////  // Point in 3D
////  std::shared_ptr<GeomDataAPI_Point> aPoint =
////      std::dynamic_pointer_cast<GeomDataAPI_Point>(theEntity);
////  if (aPoint) {
////    double aXYZ[3] = {aPoint->x(), aPoint->y(), aPoint->z()};
////    Slvs_hParam aParams[3];
////    for (int i = 0; i < 3; i++) {
////      Slvs_Param aPar = aCurrentEntity.h != SLVS_E_UNKNOWN ?
////          myStorage->getParameter(aCurrentEntity.param[i]) :
////          Slvs_MakeParam(SLVS_E_UNKNOWN, aGroupID, 0.0);
////      aPar.val = aXYZ[i];
////      aParams[i] = myStorage->addParameter(aPar);
////    }
////
////    if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
////      aCurrentEntity = Slvs_MakePoint3d(SLVS_E_UNKNOWN, aGroupID, aParams[0], aParams[1], aParams[2]);
////    else { // update entity data
////      for (int i = 0; i < 3; i++)
////        aCurrentEntity.param[i] = aParams[i];
////    }
////    aResult = myStorage->addEntity(aCurrentEntity);
////  } else {
////    // All entities except 3D points are created on workplane. So, if there is no workplane yet, then error
////    Slvs_hEntity aWorkplaneID = myGroup->getWorkplaneId();
////    if (aWorkplaneID == SLVS_E_UNKNOWN)
////      return SLVS_E_UNKNOWN;
////
////    // Point in 2D
////    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
////        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
////    if (aPoint2D) {
////      double aXY[2] = {aPoint2D->x(), aPoint2D->y()};
////      Slvs_hParam aParams[2];
////      for (int i = 0; i < 2; i++) {
////        Slvs_Param aPar = aCurrentEntity.h != SLVS_E_UNKNOWN ?
////            myStorage->getParameter(aCurrentEntity.param[i]) :
////            Slvs_MakeParam(SLVS_E_UNKNOWN, aGroupID, 0.0);
////        aPar.val = aXY[i];
////        aParams[i] = myStorage->addParameter(aPar);
////      }
////
////      if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
////        aCurrentEntity = Slvs_MakePoint2d(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID, aParams[0], aParams[1]);
////      else { // update entity data
////        for (int i = 0; i < 2; i++)
////          aCurrentEntity.param[i] = aParams[i];
////      }
////      aResult = myStorage->addEntity(aCurrentEntity);
////    } else {
////      // Scalar value (used for the distance entities)
////      AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
////      if (aScalar) {
////        Slvs_Param aParam = aCurrentEntity.h != SLVS_E_UNKNOWN ?
////            myStorage->getParameter(aCurrentEntity.param[0]) :
////            Slvs_MakeParam(SLVS_E_UNKNOWN, aGroupID, 0.0);
////        aParam.val = aScalar->value();
////        Slvs_hParam aValue = myStorage->addParameter(aParam);
////
////        if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
////          aCurrentEntity = Slvs_MakeDistance(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID, aValue);
////        else
////          aCurrentEntity.param[0] = aValue;
////        aResult = myStorage->addEntity(aCurrentEntity);
////      }
////    }
////  }
////
////  myAttributeMap[theEntity] = aResult;
////  theType = aCurrentEntity.type;
////  return aResult;
////}
////
////Slvs_hEntity SketchSolver_Constraint::changeEntity(FeaturePtr theEntity, int& theType)
////{
////  Slvs_hEntity aResult = SLVS_E_UNKNOWN;
////  if (!theEntity || !theEntity->data() || !theEntity->data()->isValid())
////    return SLVS_E_UNKNOWN;
////  // If the entity is already in the group, try to find it
////  std::map<FeaturePtr, Slvs_hEntity>::const_iterator anEntIter = myFeatureMap.find(theEntity);
////  Slvs_Entity aCurrentEntity;
////  aCurrentEntity.h = SLVS_E_UNKNOWN;
////  if (anEntIter != myFeatureMap.end())
////    aCurrentEntity = myStorage->getEntity(anEntIter->second);
////  else {
////    aResult = myGroup->getFeatureId(theEntity);
////    if (aResult != SLVS_E_UNKNOWN) {
////      Slvs_Entity anEnt = myStorage->getEntity(aResult);
////      theType = anEnt.type;
////      myFeatureMap[theEntity] = aResult;
////      return aResult;
////    }
////  }
////
////  Slvs_hGroup aGroupID = myGroup->getId();
////  // do not update entity from another group
////  if (aCurrentEntity.h != SLVS_E_UNKNOWN && aGroupID != aCurrentEntity.group)
////    return aCurrentEntity.h;
////
////  Slvs_hEntity aWorkplaneID = myGroup->getWorkplaneId();
////  DataPtr aData = theEntity->data();
////
////  // SketchPlugin features
////  const std::string& aFeatureKind = theEntity->getKind();
////  AttributePtr anAttribute;
////  int anAttrType;
////  // Line
////  if (aFeatureKind == SketchPlugin_Line::ID()) {
////    anAttribute = aData->attribute(SketchPlugin_Line::START_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aStart = changeEntity(anAttribute, anAttrType);
////
////    anAttribute = aData->attribute(SketchPlugin_Line::END_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aEnd = changeEntity(anAttribute, anAttrType);
////
////    if (aCurrentEntity.h == SLVS_E_UNKNOWN) // New entity
////      aCurrentEntity = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID, aStart, aEnd);
////    else {
////      aCurrentEntity.point[0] = aStart;
////      aCurrentEntity.point[1] = aEnd;
////    }
////    aResult = myStorage->addEntity(aCurrentEntity);
////  }
////  // Circle
////  else if (aFeatureKind == SketchPlugin_Circle::ID()) {
////    anAttribute = aData->attribute(SketchPlugin_Circle::CENTER_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aCenter = changeEntity(anAttribute, anAttrType);
////
////    anAttribute = aData->attribute(SketchPlugin_Circle::RADIUS_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aRadius = changeEntity(anAttribute, anAttrType);
////
////    if (aCurrentEntity.h == SLVS_E_UNKNOWN) { // New entity
////      Slvs_Entity aWorkplane = myStorage->getEntity(aWorkplaneID);
////      aCurrentEntity = Slvs_MakeCircle(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID,
////                                        aCenter, aWorkplane.normal, aRadius);
////    } else {
////      aCurrentEntity.point[0] = aCenter;
////      aCurrentEntity.distance = aRadius;
////    }
////    aResult = myStorage->addEntity(aCurrentEntity);
////  }
////  // Arc
////  else if (aFeatureKind == SketchPlugin_Arc::ID()) {
////    anAttribute = aData->attribute(SketchPlugin_Arc::CENTER_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aCenter = changeEntity(anAttribute, anAttrType);
////
////    anAttribute = aData->attribute(SketchPlugin_Arc::START_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aStart = changeEntity(anAttribute, anAttrType);
////
////    anAttribute = aData->attribute(SketchPlugin_Arc::END_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    Slvs_hEntity aEnd = changeEntity(anAttribute, anAttrType);
////
////    if (aCurrentEntity.h == SLVS_E_UNKNOWN) { // New entity
////      Slvs_Entity aWorkplane = myStorage->getEntity(aWorkplaneID);
////      aCurrentEntity = Slvs_MakeArcOfCircle(SLVS_E_UNKNOWN, aGroupID, aWorkplaneID,
////                                            aWorkplane.normal, aCenter, aStart, aEnd);
////    } else {
////      aCurrentEntity.point[0] = aCenter;
////      aCurrentEntity.point[1] = aStart;
////      aCurrentEntity.point[2] = aEnd;
////    }
////    aResult = myStorage->addEntity(aCurrentEntity);
////  }
////  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
////  else if (aFeatureKind == SketchPlugin_Point::ID()) {
////    anAttribute = aData->attribute(SketchPlugin_Point::COORD_ID());
////    if (!isInitialized(anAttribute)) return SLVS_E_UNKNOWN;
////    // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
////    aResult = changeEntity(anAttribute, anAttrType);
////    aCurrentEntity.type = SLVS_E_POINT_IN_3D;
////  }
////
////  if (aResult != SLVS_E_UNKNOWN) {
////    myFeatureMap[theEntity] = aResult;
////    theType = aCurrentEntity.type;
////  }
////  return aResult;
////}
////
////bool SketchSolver_Constraint::hasConstraint(ConstraintPtr theConstraint) const
////{
////  std::list<ConstraintPtr>::const_iterator anIt = myConstraints.begin();
////  for (; anIt != myConstraints.end(); ++anIt)
////    if (*anIt == theConstraint)
////      return true;
////  return false;
////}
////
////void SketchSolver_Constraint::refresh()
////{
////  cleanErrorMsg();
////  std::map<AttributePtr, Slvs_hEntity>::iterator anAttrIter = myAttributeMap.begin();
////  while (anAttrIter != myAttributeMap.end()) {
////    std::shared_ptr<GeomDataAPI_Point> aPoint =
////        std::dynamic_pointer_cast<GeomDataAPI_Point>(anAttrIter->first);
////    Slvs_Entity anEntity = myStorage->getEntity(anAttrIter->second);
////    if (anEntity.h == SLVS_E_UNKNOWN) {
////      std::map<AttributePtr, Slvs_hEntity>::iterator aTmpIter = anAttrIter++;
////      myAttributeMap.erase(aTmpIter);
////      continue;
////    }
////    if (aPoint) {
////      double aXYZ[3];
////      for (int i = 0; i < 3; i++) {
////        Slvs_Param aPar = myStorage->getParameter(anEntity.param[i]);
////        aXYZ[i] = aPar.val;
////      }
////      if (fabs(aPoint->x() - aXYZ[0]) > tolerance ||
////          fabs(aPoint->y() - aXYZ[1]) > tolerance ||
////          fabs(aPoint->z() - aXYZ[2]) > tolerance)
////        aPoint->setValue(aXYZ[0], aXYZ[1], aXYZ[2]);
////    } else {
////      // Point in 2D
////      std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
////          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrIter->first);
////      if (aPoint2D) {
////        double aXY[2];
////        for (int i = 0; i < 2; i++) {
////          Slvs_Param aPar = myStorage->getParameter(anEntity.param[i]);
////          aXY[i] = aPar.val;
////        }
////        if (fabs(aPoint2D->x() - aXY[0]) > tolerance ||
////            fabs(aPoint2D->y() - aXY[1]) > tolerance)
////          aPoint2D->setValue(aXY[0], aXY[1]);
////      } else {
////        // Scalar value (used for the distance entities)
////        AttributeDoublePtr aScalar =
////            std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttrIter->first);
////        if (aScalar) {
////          Slvs_Param aPar = myStorage->getParameter(anEntity.param[0]);
////          if (fabs(aScalar->value() - aPar.val) > tolerance)
////            aScalar->setValue(aPar.val);
////        }
////      }
////    }
////    anAttrIter++;
////  }
////
////  std::map<AttributePtr, Slvs_hParam>::iterator aValIter = myValueMap.begin();
////  for (; aValIter != myValueMap.end(); aValIter++) {
////    AttributeDoublePtr aScalar =
////        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttrIter->first);
////    if (aScalar) {
////      Slvs_Param aPar = myStorage->getParameter(anAttrIter->second);
////      aScalar->setValue(aPar.val);
////    }
////  }
////}

////bool SketchSolver_Constraint::isInitialized(AttributePtr theAttribute)
////{
////  if (theAttribute->isInitialized())
////    return true;
////  myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
////  return false;
////}
////
////
////void SketchSolver_Constraint::calculateMiddlePoint(
////    const Slvs_Entity& theEntity, double theCoeff, double& theX, double& theY) const
////{
////  if (theEntity.type == SLVS_E_LINE_SEGMENT) {
////    double aStartEndXY[2][2];
////    Slvs_Entity aPoint;
////    for (int i = 0; i < 2; i++) {
////      aPoint = myStorage->getEntity(theEntity.point[i]);
////      for (int j = 0; j < 2; j++)
////        aStartEndXY[i][j] = myStorage->getParameter(aPoint.param[j]).val;
////    }
////    theX = (1.0 - theCoeff) * aStartEndXY[0][0] + theCoeff * aStartEndXY[1][0];
////    theY = (1.0 - theCoeff) * aStartEndXY[0][1] + theCoeff * aStartEndXY[1][1];
////  } else if (theEntity.type == SLVS_E_ARC_OF_CIRCLE) {
////    double anArcPoint[3][2];
////    Slvs_Entity aPoint;
////    for (int i = 0; i < 3; i++) {
////      aPoint = myStorage->getEntity(theEntity.point[i]);
////      for (int j = 0; j < 2; j++)
////        anArcPoint[i][j] = myStorage->getParameter(aPoint.param[j]).val;
////    }
////    // project last point of arc on the arc
////    double x = anArcPoint[1][0] - anArcPoint[0][0];
////    double y = anArcPoint[1][1] - anArcPoint[0][1];
////    double aRad = sqrt(x*x + y*y);
////    x = anArcPoint[2][0] - anArcPoint[0][0];
////    y = anArcPoint[2][1] - anArcPoint[0][1];
////    double aNorm = sqrt(x*x + y*y);
////    if (aNorm >= tolerance) {
////      anArcPoint[2][0] = x * aRad / aNorm;
////      anArcPoint[2][1] = y * aRad / aNorm;
////    }
////    anArcPoint[1][0] -= anArcPoint[0][0];
////    anArcPoint[1][1] -= anArcPoint[0][1];
////    if (theCoeff < tolerance) {
////      theX = anArcPoint[0][0] + anArcPoint[1][0];
////      theY = anArcPoint[0][1] + anArcPoint[1][1];
////      return;
////    } else if (1 - theCoeff < tolerance) {
////      theX = anArcPoint[0][0] + anArcPoint[2][0];
////      theY = anArcPoint[0][1] + anArcPoint[2][1];
////      return;
////    }
////
////    std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(anArcPoint[1][0], anArcPoint[1][1]));
////    std::shared_ptr<GeomAPI_Dir2d> aEndDir(new GeomAPI_Dir2d(anArcPoint[2][0], anArcPoint[2][1]));
////    double anAngle = aStartDir->angle(aEndDir);
////    if (anAngle < 0)
////      anAngle += 2.0 * PI;
////    anAngle *= theCoeff;
////    double aCos = cos(anAngle);
////    double aSin = sin(anAngle);
////    theX = anArcPoint[0][0] + anArcPoint[1][0] * aCos - anArcPoint[1][1] * aSin;
////    theY = anArcPoint[0][1] + anArcPoint[1][0] * aSin + anArcPoint[1][1] * aCos;
////  }
////}

void SketchSolver_Constraint::makeTemporary() const
{
  myStorage->setTemporary(myBaseConstraint);
}

