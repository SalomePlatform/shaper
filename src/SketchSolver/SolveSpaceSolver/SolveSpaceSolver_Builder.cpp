// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_Builder.cpp
// Created: 25 Mar 2015
// Author:  Artem ZHIDKOV

#include <SolveSpaceSolver_Builder.h>
#include <SolveSpaceSolver_Solver.h>
#include <SolveSpaceSolver_Storage.h>
#include <SolveSpaceSolver_ParameterWrapper.h>
#include <SolveSpaceSolver_EntityWrapper.h>
#include <SolveSpaceSolver_ConstraintWrapper.h>
#include <SolveSpaceSolver_ConstraintType.h>

#include <SketchSolver_Manager.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_IntersectionPoint.h>

#include <math.h>


static EntityWrapperPtr createLine(FeaturePtr theFeature,
                                   const std::list<EntityWrapperPtr>& theAttributes,
                                   const GroupID& theGroupID,
                                   const EntityID& theSketchID);
static EntityWrapperPtr createCircle(FeaturePtr theFeature,
                                     const std::list<EntityWrapperPtr>& theAttributes,
                                     const GroupID& theGroupID,
                                     const EntityID& theSketchID);
static EntityWrapperPtr createArc(FeaturePtr theFeature,
                                  const std::list<EntityWrapperPtr>& theAttributes,
                                  const GroupID& theGroupID,
                                  const EntityID& theSketchID);

/// \brief Set flags of constraint to identify which points are coincident in the Tangency
///        (for more information, see SolveSpace documentation)
static void adjustTangency(ConstraintWrapperPtr theConstraint);
/// \brief Set flags for angle constraint
static void adjustAngle(ConstraintWrapperPtr theConstraint);
/// \brief Update mirror points
static void adjustMirror(ConstraintWrapperPtr theConstraint);
/// \brief Update a sign of the point-line distance constraint
static void adjustPtLineDistance(ConstraintWrapperPtr theConstraint);

/// \brief Transform points to be symmetric regarding to the mirror line
static void makeMirrorPoints(EntityWrapperPtr theOriginal,
                             EntityWrapperPtr theMirrored,
                             EntityWrapperPtr theMirrorLine);



// Initialization of constraint builder self pointer
BuilderPtr SolveSpaceSolver_Builder::mySelf = SolveSpaceSolver_Builder::getInstance();

BuilderPtr SolveSpaceSolver_Builder::getInstance()
{
  if (!mySelf) {
    mySelf = BuilderPtr(new SolveSpaceSolver_Builder);
    SketchSolver_Manager::instance()->setBuilder(mySelf);
  }
  return mySelf;
}

StoragePtr SolveSpaceSolver_Builder::createStorage(const GroupID& theGroup) const
{
  return StoragePtr(new SolveSpaceSolver_Storage(theGroup));
}

SolverPtr SolveSpaceSolver_Builder::createSolver() const
{
  return SolverPtr(new SolveSpaceSolver_Solver);
}


std::list<ConstraintWrapperPtr> SolveSpaceSolver_Builder::createConstraint(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const EntityID& theSketchID,
    const SketchSolver_ConstraintType& theType,
    const double& theValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2) const
{
  if (theType == CONSTRAINT_SYMMETRIC)
    return createMirror(theConstraint, theGroupID, theSketchID,
                        thePoint1, thePoint2, theEntity1);
  else if (theType == CONSTRAINT_TANGENT_CIRCLE_LINE) {
    // replace by distance from center of circle to the line
    const std::list<EntityWrapperPtr>& aSubs = theEntity1->subEntities();
    EntityWrapperPtr aCenter = aSubs.front();
    AttributeDoublePtr aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        aSubs.back()->baseAttribute());
    return createConstraint(theConstraint, theGroupID, theSketchID,
        CONSTRAINT_PT_LINE_DISTANCE, aRadius->value(), aCenter, EntityWrapperPtr(), theEntity2);
  }
  else if (theType == CONSTRAINT_COLLINEAR) {
    // replace by two constraints point-on-line
    std::list<ConstraintWrapperPtr> aConstraints;
    const std::list<EntityWrapperPtr>& aSubs1 = theEntity1->subEntities();
    const std::list<EntityWrapperPtr>& aSubs2 = theEntity2->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt1, anIt2;
    for (anIt2 = aSubs2.begin(); anIt2 != aSubs2.end(); ++anIt2) {
      for (anIt1 = aSubs1.begin(); anIt1 != aSubs1.end(); ++anIt1)
        if ((*anIt1)->id() == (*anIt2)->id())
          break;
      if (anIt1 != aSubs1.end())
        continue; // the lines have coincident point

      std::list<ConstraintWrapperPtr> aC = createConstraint(theConstraint, theGroupID,
          theSketchID, CONSTRAINT_PT_ON_LINE, theValue, *anIt2, EntityWrapperPtr(), theEntity1);
      aConstraints.insert(aConstraints.end(), aC.begin(), aC.end());
    }
    return aConstraints;
  }

  int aType = ConstraintType::toSolveSpace(theType);
  if (aType == SLVS_C_UNKNOWN)
    return std::list<ConstraintWrapperPtr>();

  Slvs_hEntity aSlvsEntities[4] = {SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN};
  EntityWrapperPtr anOriginal[4] = {thePoint1, thePoint2, theEntity1, theEntity2};
  std::list<EntityWrapperPtr> aConstrAttrList; // to be filled
  for (int i = 0; i < 4; ++i) {
    if (!anOriginal[i])
      continue;
    aSlvsEntities[i] = (Slvs_hEntity)anOriginal[i]->id();
    if (aSlvsEntities[i] == SLVS_E_UNKNOWN)
      return std::list<ConstraintWrapperPtr>(); // entity is not added into a storage, constraint can not be created
    aConstrAttrList.push_back(anOriginal[i]);
  }

  Slvs_Constraint aConstraint = Slvs_MakeConstraint(
      SLVS_C_UNKNOWN, (Slvs_hGroup)theGroupID, aType, (Slvs_hEntity)theSketchID,
      theValue, aSlvsEntities[0], aSlvsEntities[1], aSlvsEntities[2], aSlvsEntities[3]);
  ConstraintWrapperPtr aResult(new SolveSpaceSolver_ConstraintWrapper(theConstraint, aConstraint));
  aResult->setGroup(theGroupID);
  aResult->setValue(theValue);
  aResult->setEntities(aConstrAttrList);
  adjustConstraint(aResult);

  return std::list<ConstraintWrapperPtr>(1, aResult);
}

std::list<ConstraintWrapperPtr> SolveSpaceSolver_Builder::createConstraint(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const EntityID& theSketchID,
    const SketchSolver_ConstraintType& theType,
    const double& theValue,
    const bool theFullValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const std::list<EntityWrapperPtr>& theTrsfEnt) const
{
  if (theType != CONSTRAINT_MULTI_ROTATION && theType != CONSTRAINT_MULTI_TRANSLATION)
    return std::list<ConstraintWrapperPtr>();

  int aType = ConstraintType::toSolveSpace(theType);
  if (aType == SLVS_C_UNKNOWN)
    return std::list<ConstraintWrapperPtr>();

  Slvs_Constraint aConstraint =
      Slvs_MakeConstraint(SLVS_C_UNKNOWN, (Slvs_hGroup)theGroupID, aType, (Slvs_hEntity)theSketchID,
      theValue, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);

  std::list<EntityWrapperPtr> aConstrAttrList = theTrsfEnt;
  if (thePoint2)
    aConstrAttrList.push_front(thePoint2);
  aConstrAttrList.push_front(thePoint1);

  ConstraintWrapperPtr aResult(new SolveSpaceSolver_ConstraintWrapper(theConstraint, aConstraint));
  aResult->setGroup(theGroupID);
  aResult->setValue(theValue);
  aResult->setIsFullValue(theFullValue);
  aResult->setEntities(aConstrAttrList);
  return std::list<ConstraintWrapperPtr>(1, aResult);
}


std::list<ConstraintWrapperPtr> SolveSpaceSolver_Builder::createMirror(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const EntityID& theSketchID,
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2,
    const EntityWrapperPtr& theMirrorLine) const
{
  Slvs_Constraint aConstraint;
  std::list<ConstraintWrapperPtr> aResult;
  std::list<EntityWrapperPtr> aConstrAttrList;
  if (theEntity1->type() == ENTITY_POINT) {
    if (theEntity2->group() == theGroupID) // theEntity2 is not fixed
      makeMirrorPoints(theEntity1, theEntity2, theMirrorLine);

    aConstraint = Slvs_MakeConstraint(
        SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID, SLVS_C_SYMMETRIC_LINE, (Slvs_hEntity)theSketchID,
        0.0, (Slvs_hEntity)theEntity1->id(), (Slvs_hEntity)theEntity2->id(),
        (Slvs_hEntity)theMirrorLine->id(), SLVS_E_UNKNOWN);

    aConstrAttrList.push_back(theEntity1);
    aConstrAttrList.push_back(theEntity2);
    aConstrAttrList.push_back(theMirrorLine);

    ConstraintWrapperPtr aWrapper(new SolveSpaceSolver_ConstraintWrapper(
        theConstraint, aConstraint));
    aWrapper->setGroup(theGroupID);
    aWrapper->setEntities(aConstrAttrList);
    aResult.push_back(aWrapper);
  }
  else if (theEntity1->type() == ENTITY_LINE) {
    const std::list<EntityWrapperPtr>& aPoints1 = theEntity1->subEntities();
    const std::list<EntityWrapperPtr>& aPoints2 = theEntity2->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt1 = aPoints1.begin();
    std::list<EntityWrapperPtr>::const_iterator anIt2 = aPoints2.begin();
    for (; anIt1 != aPoints1.end() && anIt2 != aPoints2.end(); ++anIt1, ++anIt2) {
      std::list<ConstraintWrapperPtr> aMrrList =
          createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
      aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());
    }
  }
  else if (theEntity1->type() == ENTITY_CIRCLE) {
    const std::list<EntityWrapperPtr>& aPoints1 = theEntity1->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt1 = aPoints1.begin();
    for (; anIt1 != aPoints1.end(); ++anIt1)
      if ((*anIt1)->type() == ENTITY_POINT)
        break;
    const std::list<EntityWrapperPtr>& aPoints2 = theEntity2->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt2 = aPoints2.begin();
    for (; anIt2 != aPoints2.end(); ++anIt2)
      if ((*anIt2)->type() == ENTITY_POINT)
        break;

    std::list<ConstraintWrapperPtr> aMrrList =
        createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());

    // Additional constraint for equal radii
    aMrrList = createConstraint(theConstraint, theGroupID, theSketchID, CONSTRAINT_EQUAL_RADIUS,
        0.0, EntityWrapperPtr(), EntityWrapperPtr(), theEntity1, theEntity2);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());
  }
  else if (theEntity1->type() == ENTITY_ARC) {
    // Do not allow mirrored arc recalculate its position until coordinated of all points recalculated
    FeaturePtr aMirrArc = theEntity2->baseFeature();
    aMirrArc->data()->blockSendAttributeUpdated(true);

    std::list<ConstraintWrapperPtr> aMrrList;
    std::list<EntityWrapperPtr>::const_iterator anIt1 = theEntity1->subEntities().begin();
    std::list<EntityWrapperPtr>::const_iterator anIt2 = theEntity2->subEntities().begin();
    if ((*anIt2)->group() == theGroupID) // mirrored point is not fixed
      makeMirrorPoints(*anIt1, *anIt2, theMirrorLine);

    // Workaround to avoid problems in SolveSpace.
    // The symmetry of two arcs will be done using symmetry of three points on these arcs:
    // start point, end point, and any other point on the arc
    std::list<EntityWrapperPtr> aBaseArcPoints(++anIt1, theEntity1->subEntities().end());
    std::list<EntityWrapperPtr> aMirrorArcPoints(++anIt2, theEntity2->subEntities().end());
    // indices of points of arc, center corresponds center, first point corresponds last point
    aMirrorArcPoints.reverse();

    anIt1 = aBaseArcPoints.begin();
    anIt2 = aMirrorArcPoints.begin();
    for (; anIt1 != aBaseArcPoints.end(); ++anIt1, ++anIt2) {
      aMrrList = createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
      aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());
    }
    // Restore event sending
    aMirrArc->data()->blockSendAttributeUpdated(false);
  }
  return aResult;
}

void SolveSpaceSolver_Builder::adjustConstraint(ConstraintWrapperPtr theConstraint) const
{
  SketchSolver_ConstraintType aType = theConstraint->type();
  // Update flags in constraints
  if (aType == CONSTRAINT_TANGENT_ARC_ARC || aType == CONSTRAINT_TANGENT_ARC_LINE)
    adjustTangency(theConstraint);
  else if (aType == CONSTRAINT_ANGLE)
    adjustAngle(theConstraint);
  else if (aType == CONSTRAINT_SYMMETRIC)
    adjustMirror(theConstraint);
  else if (aType == CONSTRAINT_PT_LINE_DISTANCE)
    adjustPtLineDistance(theConstraint);
}

EntityWrapperPtr SolveSpaceSolver_Builder::createFeature(
    FeaturePtr theFeature,
    const std::list<EntityWrapperPtr>& theAttributes,
    const GroupID& theGroupID,
    const EntityID& theSketchID) const
{
  static EntityWrapperPtr aDummy;
  if (!theFeature->data()->isValid())
    return aDummy;

  // Sketch
  CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aSketch)
    return createSketchEntity(aSketch, theGroupID);

  // SketchPlugin features
  std::shared_ptr<SketchPlugin_Feature> aFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
  if (!aFeature)
    return aDummy;

  // Verify the feature by its kind
  const std::string& aFeatureKind = aFeature->getKind();
  // Line
  if (aFeatureKind == SketchPlugin_Line::ID())
    return createLine(theFeature, theAttributes, theGroupID, theSketchID);
  // Circle
  else if (aFeatureKind == SketchPlugin_Circle::ID())
    return createCircle(theFeature, theAttributes, theGroupID, theSketchID);
  // Arc
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    return createArc(theFeature, theAttributes, theGroupID, theSketchID);
  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
  else if (aFeatureKind == SketchPlugin_Point::ID() || 
           aFeatureKind == SketchPlugin_IntersectionPoint::ID()) {
    AttributePtr aPoint = theFeature->attribute(SketchPlugin_Point::COORD_ID());
    if (!aPoint->isInitialized())
      return aDummy;
    EntityWrapperPtr aSub = theAttributes.empty() ? createAttribute(aPoint, theGroupID, theSketchID) :
                            theAttributes.front();
    if (!aSub)
      return aDummy;

    const Slvs_Entity& aSubEnt =
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(aSub)->entity();
    EntityWrapperPtr aResult(new SolveSpaceSolver_EntityWrapper(theFeature, aPoint, aSubEnt));
    aResult->setSubEntities(theAttributes);
    return aResult;
  }

  // wrong entity
  return aDummy;
}

EntityWrapperPtr SolveSpaceSolver_Builder::createAttribute(
    AttributePtr theAttribute,
    const GroupID& theGroupID,
    const EntityID& theSketchID) const
{
  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject()) {
      // do not create features here
      return EntityWrapperPtr();
    } else
      anAttribute = aRefAttr->attr();
  }

  std::list<ParameterWrapperPtr> aParameters;
  Slvs_Entity anEntity;
  anEntity.type = 0;

  // Point in 3D
  std::shared_ptr<GeomDataAPI_Point> aPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
  if (aPoint) {
    aParameters.push_back(createParameter(theGroupID, aPoint->x(), !aPoint->textX().empty()));
    aParameters.push_back(createParameter(theGroupID, aPoint->y(), !aPoint->textY().empty()));
    aParameters.push_back(createParameter(theGroupID, aPoint->z(), !aPoint->textZ().empty()));
    // Create entity (parameters are not filled)
    anEntity = Slvs_MakePoint3d(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
        SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
  } else {
    // Point in 2D
    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
    if (aPoint2D) {
      aParameters.push_back(createParameter(theGroupID, aPoint2D->x(), !aPoint2D->textX().empty()));
      aParameters.push_back(createParameter(theGroupID, aPoint2D->y(), !aPoint2D->textY().empty()));
      // Create entity (parameters are not filled)
      anEntity = Slvs_MakePoint2d(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
          (Slvs_hEntity)theSketchID, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    } else {
      // Scalar value (used for the distance entities)
      AttributeDoublePtr aScalar =
          std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
      if (aScalar) {
        aParameters.push_back(createParameter(theGroupID, aScalar->value(), !aScalar->text().empty()));
        // Create entity (parameter is not filled)
        anEntity = Slvs_MakeDistance(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
          (Slvs_hEntity)theSketchID, SLVS_E_UNKNOWN);
      }
    }
  }

  if (anEntity.type == 0) {
    // unknown attribute type
    return EntityWrapperPtr();
  }

  EntityWrapperPtr aResult(new SolveSpaceSolver_EntityWrapper(theAttribute, anEntity));
  aResult->setParameters(aParameters);
  return aResult;
}



EntityWrapperPtr SolveSpaceSolver_Builder::createSketchEntity(
    CompositeFeaturePtr theSketch,
    const GroupID& theGroupID) const
{
  DataPtr aSketchData = theSketch->data();
  if (!aSketchData || !aSketchData->isValid())
    return EntityWrapperPtr(); // the sketch is incorrect

  // Get parameters of workplane
  AttributePtr aDirX    = aSketchData->attribute(SketchPlugin_Sketch::DIRX_ID());
  AttributePtr aNorm    = aSketchData->attribute(SketchPlugin_Sketch::NORM_ID());
  AttributePtr anOrigin = aSketchData->attribute(SketchPlugin_Sketch::ORIGIN_ID());
  if (!anOrigin->isInitialized() || !aNorm->isInitialized() || !aDirX->isInitialized())
    return EntityWrapperPtr();

  EntityWrapperPtr aNewEnt;
  std::list<EntityWrapperPtr> aSubs;

  // Create SolveSpace entity corresponding to the sketch origin
  aNewEnt = createAttribute(anOrigin, theGroupID);
  if (!aNewEnt)
    return EntityWrapperPtr();
  aSubs.push_back(aNewEnt);

  // Create SolveSpace entity corresponding the the sketch normal
  aNewEnt = createNormal(aNorm, aDirX, theGroupID);
  if (!aNewEnt)
    return EntityWrapperPtr();
  aSubs.push_back(aNewEnt);

  // Create workplane
  Slvs_Entity aWorkplane = Slvs_MakeWorkplane(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
      SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);

  aNewEnt = EntityWrapperPtr(
      new SolveSpaceSolver_EntityWrapper(FeaturePtr(theSketch), aWorkplane));
  aNewEnt->setSubEntities(aSubs);
  return aNewEnt;
}

EntityWrapperPtr SolveSpaceSolver_Builder::createNormal(
    AttributePtr theNormal,
    AttributePtr theDirX,
    const GroupID& theGroupID) const
{
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theNormal);
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theDirX);
  if (!aDirX || !aNorm ||
      (fabs(aDirX->x()) + fabs(aDirX->y()) + fabs(aDirX->z()) < tolerance) || 
      !aNorm->isInitialized())
    return EntityWrapperPtr();
  // calculate Y direction
  std::shared_ptr<GeomAPI_Dir> aDirY(new GeomAPI_Dir(aNorm->dir()->cross(aDirX->dir())));

  // quaternion parameters of normal vector
  double qw, qx, qy, qz;
  Slvs_MakeQuaternion(aDirX->x(), aDirX->y(), aDirX->z(), aDirY->x(), aDirY->y(), aDirY->z(), &qw,
                      &qx, &qy, &qz);
  double aNormCoord[4] = { qw, qx, qy, qz };

  // Create parameters of the normal
  std::list<ParameterWrapperPtr> aParameters;
  for (int i = 0; i < 4; i++)
    aParameters.push_back(createParameter(theGroupID, aNormCoord[i]));

  // Create a normal with empty parameters
  Slvs_Entity aNormalEnt = Slvs_MakeNormal3d(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
      SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
  EntityWrapperPtr aNormal(new SolveSpaceSolver_EntityWrapper(theNormal, aNormalEnt));
  aNormal->setParameters(aParameters);
  return aNormal;
}

ParameterWrapperPtr SolveSpaceSolver_Builder::createParameter(
    const GroupID& theGroup, const double theValue, const bool theExpr) const
{
  Slvs_Param aParam = Slvs_MakeParam(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroup, theValue);
  ParameterWrapperPtr aWrapper(new SolveSpaceSolver_ParameterWrapper(aParam));
  aWrapper->setIsParametric(theExpr);
  return aWrapper;
}





// ================   Auxiliary functions   ==========================
EntityWrapperPtr createLine(FeaturePtr theFeature,
                            const std::list<EntityWrapperPtr>& theAttributes,
                            const GroupID& theGroupID,
                            const EntityID& theSketchID)
{
  EntityWrapperPtr aNewEntity;
  std::list<EntityWrapperPtr> aSubs;

  AttributePtr aStart = theFeature->attribute(SketchPlugin_Line::START_ID());
  AttributePtr aEnd = theFeature->attribute(SketchPlugin_Line::END_ID());
  if (!aStart->isInitialized() || !aEnd->isInitialized())
    return aNewEntity;

  EntityWrapperPtr aStartEnt, aEndEnt;
  std::list<EntityWrapperPtr>::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aSlvsEntity = 
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(*anIt);
    if (aSlvsEntity->isBase(aStart))
      aStartEnt = aSlvsEntity;
    else if (aSlvsEntity->isBase(aEnd))
      aEndEnt = aSlvsEntity;
  }
  if (!aStartEnt || !aEndEnt)
    return aNewEntity;

  aSubs.push_back(aStartEnt);
  aSubs.push_back(aEndEnt);
  Slvs_Entity anEntity = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
      (Slvs_hEntity)theSketchID, (Slvs_hEntity)aStartEnt->id(), (Slvs_hEntity)aEndEnt->id());

  aNewEntity = EntityWrapperPtr(new SolveSpaceSolver_EntityWrapper(theFeature, anEntity));
  aNewEntity->setSubEntities(aSubs);
  return aNewEntity;
}

EntityWrapperPtr createCircle(FeaturePtr theFeature,
                              const std::list<EntityWrapperPtr>& theAttributes,
                              const GroupID& theGroupID,
                              const EntityID& theSketchID)
{
  EntityWrapperPtr aNewEntity;
  std::list<EntityWrapperPtr> aSubs;

  AttributePtr aCenter = theFeature->attribute(SketchPlugin_Circle::CENTER_ID());
  AttributePtr aRadius = theFeature->attribute(SketchPlugin_Circle::RADIUS_ID());
  if (!aCenter->isInitialized() || !aRadius->isInitialized())
    return aNewEntity;

  EntityWrapperPtr aCenterEnt, aRadiusEnt, aNormalEnt;
  std::list<EntityWrapperPtr>::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aSlvsEntity = 
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(*anIt);
    if (aSlvsEntity->isBase(aCenter))
      aCenterEnt = aSlvsEntity;
    else if (aSlvsEntity->isBase(aRadius))
      aRadiusEnt = aSlvsEntity;
    else if (aSlvsEntity->type() == ENTITY_NORMAL)
      aNormalEnt = aSlvsEntity;
  }
  if (!aCenterEnt || !aRadiusEnt || !aNormalEnt)
    return aNewEntity;

  aSubs.push_back(aCenterEnt);
  aSubs.push_back(aRadiusEnt);
  Slvs_Entity anEntity = Slvs_MakeCircle(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
      (Slvs_hEntity)theSketchID, (Slvs_hEntity)aCenterEnt->id(),
      (Slvs_hEntity)aNormalEnt->id(), (Slvs_hEntity)aRadiusEnt->id());

  aNewEntity = EntityWrapperPtr(new SolveSpaceSolver_EntityWrapper(theFeature, anEntity));
  aNewEntity->setSubEntities(aSubs);
  return aNewEntity;
}

EntityWrapperPtr createArc(FeaturePtr theFeature,
                           const std::list<EntityWrapperPtr>& theAttributes,
                           const GroupID& theGroupID,
                           const EntityID& theSketchID)
{
  EntityWrapperPtr aNewEntity;
  std::list<EntityWrapperPtr> aSubs;

  AttributePtr aCenter = theFeature->attribute(SketchPlugin_Arc::CENTER_ID());
  AttributePtr aStart = theFeature->attribute(SketchPlugin_Arc::START_ID());
  AttributePtr aEnd = theFeature->attribute(SketchPlugin_Arc::END_ID());
  if (!aCenter->isInitialized() || !aStart->isInitialized() || !aEnd->isInitialized())
    return aNewEntity;

  EntityWrapperPtr aCenterEnt, aStartEnt, aEndEnt, aNormalEnt;
  std::list<EntityWrapperPtr>::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aSlvsEntity = 
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(*anIt);
    if (aSlvsEntity->isBase(aCenter))
      aCenterEnt = aSlvsEntity;
    else if (aSlvsEntity->isBase(aStart))
      aStartEnt = aSlvsEntity;
    else if (aSlvsEntity->isBase(aEnd))
      aEndEnt = aSlvsEntity;
    else if (aSlvsEntity->type() == ENTITY_NORMAL)
      aNormalEnt = aSlvsEntity;
  }
  if (!aCenterEnt || !aStartEnt || !aEndEnt || !aNormalEnt)
    return aNewEntity;

  aSubs.push_back(aCenterEnt);
  aSubs.push_back(aStartEnt);
  aSubs.push_back(aEndEnt);
  Slvs_Entity anEntity = Slvs_MakeArcOfCircle(SLVS_E_UNKNOWN, (Slvs_hGroup)theGroupID,
      (Slvs_hEntity)theSketchID, (Slvs_hEntity)aNormalEnt->id(),
      (Slvs_hEntity)aCenterEnt->id(), (Slvs_hEntity)aStartEnt->id(), (Slvs_hEntity)aEndEnt->id());

  aNewEntity = EntityWrapperPtr(new SolveSpaceSolver_EntityWrapper(theFeature, anEntity));
  aNewEntity->setSubEntities(aSubs);
  return aNewEntity;
}


void adjustTangency(ConstraintWrapperPtr theConstraint)
{
  BuilderPtr aBuilder = SolveSpaceSolver_Builder::getInstance();

  std::shared_ptr<SolveSpaceSolver_ConstraintWrapper> aConstraint =
    std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(theConstraint);

  // Collect start, end points of entities
  std::shared_ptr<GeomAPI_Pnt2d> aStartEntPoints[2][2];
  bool isCoinc[2][2] = {false};
  const std::list<EntityWrapperPtr>& aSubs = aConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (int i = 0; aSIt != aSubs.end(); ++aSIt, ++i) {
    const std::list<EntityWrapperPtr>& aPoints = (*aSIt)->subEntities();
    std::list<EntityWrapperPtr>::const_iterator aPIt = aPoints.begin();
    if ((*aSIt)->type() == ENTITY_ARC)
      ++aPIt;
    for (int j = 0; aPIt != aPoints.end(); ++aPIt, ++j) {
      aStartEntPoints[i][j] = aBuilder->point(*aPIt);
      if (i > 0) { // check coincidence
        for (int k = 0; k < 2; ++k)
          if (aStartEntPoints[i][j]->distance(aStartEntPoints[0][k]) < tolerance)
            isCoinc[0][k] = isCoinc[i][j] = true;
      }
    }
  }

  Slvs_Constraint& aSlvsConstraint = aConstraint->changeConstraint();
  if (isCoinc[0][0] == false && isCoinc[0][1] == true)
    aSlvsConstraint.other = 1;
  else aSlvsConstraint.other = 0;
  if (isCoinc[1][0] == false && isCoinc[1][1] == true)
    aSlvsConstraint.other2 = 1;
  else aSlvsConstraint.other2 = 0;
}

void adjustAngle(ConstraintWrapperPtr theConstraint)
{
  BuilderPtr aBuilder = SolveSpaceSolver_Builder::getInstance();

  std::shared_ptr<SolveSpaceSolver_ConstraintWrapper> aConstraint =
    std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(theConstraint);

  bool isFixed[2][2];
  std::shared_ptr<GeomAPI_Pnt2d> aPoints[2][2]; // start and end points of lines
  const std::list<EntityWrapperPtr>& aConstrLines = aConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aCLIt = aConstrLines.begin();
  for (int i = 0; aCLIt != aConstrLines.end(); ++i, ++aCLIt) {
    const std::list<EntityWrapperPtr>& aLinePoints = (*aCLIt)->subEntities();
    std::list<EntityWrapperPtr>::const_iterator aLPIt = aLinePoints.begin();
    for (int j = 0; aLPIt != aLinePoints.end(); ++j, ++aLPIt) {
      isFixed[i][j] = ((*aLPIt)->group() != theConstraint->group());
      aPoints[i][j] = aBuilder->point(*aLPIt);
    }
  }

  if (isFixed[0][0] && isFixed[0][1] && isFixed[1][0] && isFixed[1][1])
    return; // both lines are fixed => no need to update them

  std::shared_ptr<GeomAPI_Lin2d> aLine[2] = {
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoints[0][0], aPoints[0][1])),
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPoints[1][0], aPoints[1][1]))
  };
  std::shared_ptr<GeomAPI_Pnt2d> anIntersection = aLine[0]->intersect(aLine[1]);
  if (!anIntersection)
    return;
  double aDist[2][2];
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      aDist[i][j] = anIntersection->distance(aPoints[i][j]);
      if (fabs(aDist[i][j]) <= tolerance)
        aDist[i][j] = 0.0;
    }
    if (aDist[i][0] > tolerance && aDist[i][1] > tolerance &&
        aDist[i][0] + aDist[i][1] < aPoints[i][0]->distance(aPoints[i][1]) + 2.0 * tolerance) {
      // the intersection point is an inner point of the line,
      // we change the sign of distance till start point to calculate correct coordinates
      // after rotation
      aDist[i][0] *= -1.0;
    }
  }
  std::shared_ptr<GeomAPI_Dir2d> aDir[2];
  for (int i = 0; i < 2; i++) {
    if (aDist[i][1] > fabs(aDist[i][0]))
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(
          aPoints[i][1]->xy()->decreased(anIntersection->xy())));
    else {
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(
          aPoints[i][0]->xy()->decreased(anIntersection->xy())));
      // main direction is opposite => change signs
      if (aDist[i][0] < 0.0) {
        aDist[i][0] *= -1.0;
        aDist[i][1] *= -1.0;
      }
    }
  }

  Slvs_Constraint& aSlvsConstraint = aConstraint->changeConstraint();
  aSlvsConstraint.other = false;
  for (int i = 0; i < 2; i++)
    if (aLine[i]->direction()->dot(aDir[i]) < 0.0)
      aSlvsConstraint.other = !aSlvsConstraint.other;

  // Recalculate positions of lines to avoid conflicting constraints
  // while changing angle value several times
  double cosA = cos(aConstraint->value() * PI / 180.0);
  double sinA = sin(aConstraint->value() * PI / 180.0);
  //if (aDir[0]->cross(aDir[1]) < 0.0)
  //  sinA *= -1.0;
  int aLineToUpd = 1;
  if (isFixed[1][0] && isFixed[1][1]) {
    sinA *= -1.0;
    aLineToUpd = 0;
  }
  double x = aDir[1-aLineToUpd]->x() * cosA - aDir[1-aLineToUpd]->y() * sinA;
  double y = aDir[1-aLineToUpd]->x() * sinA + aDir[1-aLineToUpd]->y() * cosA;

  std::shared_ptr<GeomAPI_Pnt2d> aNewPoints[2];
  for (int i = 0; i < 2; i++) {
    aNewPoints[i] = std::shared_ptr<GeomAPI_Pnt2d>(
        new GeomAPI_Pnt2d(anIntersection->x() + x * aDist[aLineToUpd][i],
                          anIntersection->y() + y * aDist[aLineToUpd][i]));
  }

  std::shared_ptr<GeomAPI_XY> aDelta;
  if (isFixed[aLineToUpd][0] && !isFixed[aLineToUpd][1])
    aDelta = aPoints[aLineToUpd][0]->xy()->decreased(aNewPoints[0]->xy());
  else if (!isFixed[aLineToUpd][0] && isFixed[aLineToUpd][1])
    aDelta = aPoints[aLineToUpd][1]->xy()->decreased(aNewPoints[1]->xy());
  if (aDelta) {
    for (int i = 0; i < 2; i++) {
      aNewPoints[i]->setX(aNewPoints[i]->x() + aDelta->x());
      aNewPoints[i]->setY(aNewPoints[i]->y() + aDelta->y());
    }
  }

  // Update positions of points
  std::list<EntityWrapperPtr>::const_iterator anUpdLine = aConstrLines.begin();
  if (aLineToUpd > 0) ++anUpdLine;
  const std::list<EntityWrapperPtr>& anUpdPoints = (*anUpdLine)->subEntities();
  std::list<EntityWrapperPtr>::const_iterator aPIt = anUpdPoints.begin();
  for (int i = 0; aPIt != anUpdPoints.end(); ++aPIt, ++i) {
    std::shared_ptr<GeomDataAPI_Point2D> aPnt2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>((*aPIt)->baseAttribute());
    aPnt2D->setValue(aNewPoints[i]);
  }
}

void adjustMirror(ConstraintWrapperPtr theConstraint)
{
  std::vector<EntityWrapperPtr> aPoints;
  EntityWrapperPtr aMirrorLine;

  const std::list<EntityWrapperPtr>& aSubs = theConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator anIt = aSubs.begin();
  for (; anIt != aSubs.end(); ++anIt) {
    if ((*anIt)->type() == ENTITY_POINT)
      aPoints.push_back(*anIt);
    else if ((*anIt)->type() == ENTITY_LINE)
      aMirrorLine = *anIt;
  }

  makeMirrorPoints(aPoints[0], aPoints[1], aMirrorLine);
}

void makeMirrorPoints(EntityWrapperPtr theOriginal,
                      EntityWrapperPtr theMirrored,
                      EntityWrapperPtr theMirrorLine)
{
  BuilderPtr aBuilder = SolveSpaceSolver_Builder::getInstance();

  std::shared_ptr<GeomAPI_Lin2d> aMirrorLine = aBuilder->line(theMirrorLine);
  std::shared_ptr<GeomAPI_Dir2d> aMLDir = aMirrorLine->direction();
  // orthogonal direction
  aMLDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aMLDir->y(), -aMLDir->x()));

  std::shared_ptr<GeomAPI_Pnt2d> aPoint = aBuilder->point(theOriginal);
  std::shared_ptr<GeomAPI_XY> aVec = aPoint->xy()->decreased(aMirrorLine->location()->xy());
  double aDist = aVec->dot(aMLDir->xy());
  aVec = aPoint->xy()->added(aMLDir->xy()->multiplied(-2.0 * aDist));
  double aCoord[2] = {aVec->x(), aVec->y()};
  std::list<ParameterWrapperPtr>::const_iterator aMIt = theMirrored->parameters().begin();
  for (int i = 0; aMIt != theMirrored->parameters().end(); ++aMIt, ++i)
    (*aMIt)->setValue(aCoord[i]);

  // update corresponding attribute
  AttributePtr anAttr = std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(theMirrored)->baseAttribute();
  if (anAttr) {
    std::shared_ptr<GeomDataAPI_Point2D> aMirroredPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
    aMirroredPnt->setValue(aCoord[0], aCoord[1]);
  }
}

void adjustPtLineDistance(ConstraintWrapperPtr theConstraint)
{
  BuilderPtr aBuilder = SolveSpaceSolver_Builder::getInstance();

  std::shared_ptr<GeomAPI_Pnt2d> aPoint;
  std::shared_ptr<GeomAPI_Lin2d> aLine;
  std::list<EntityWrapperPtr> aSubs = theConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++aSIt) {
    if ((*aSIt)->type() == ENTITY_POINT)
      aPoint = aBuilder->point(*aSIt);
    else if ((*aSIt)->type() == ENTITY_LINE)
      aLine = aBuilder->line(*aSIt);
  }

  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());
  if (aPtLineVec->cross(aLineVec) * theConstraint->value() < 0.0)
    theConstraint->setValue(theConstraint->value() * (-1.0));
}
