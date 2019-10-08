// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Solver.h>
#include <PlaneGCSSolver_BooleanWrapper.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Tools.h>

#include <PlaneGCSSolver_AttributeBuilder.h>
#include <PlaneGCSSolver_FeatureBuilder.h>
#include <PlaneGCSSolver_EntityDestroyer.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_Projection.h>

#include <cmath>


static void constraintsToSolver(const ConstraintWrapperPtr& theConstraint,
                                const SolverPtr& theSolver)
{
  const std::list<GCSConstraintPtr>& aConstraints =
      std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(theConstraint)->constraints();
  theSolver->addConstraint(theConstraint->id(), aConstraints);
}


PlaneGCSSolver_Storage::PlaneGCSSolver_Storage(const SolverPtr& theSolver)
  : SketchSolver_Storage(theSolver),
    myConstraintLastID(CID_UNKNOWN)
{
}

void PlaneGCSSolver_Storage::addConstraint(
    ConstraintPtr        theConstraint,
    ConstraintWrapperPtr theSolverConstraint)
{
  SketchSolver_Storage::addConstraint(theConstraint, theSolverConstraint);

  theSolverConstraint->setId(++myConstraintLastID);
  constraintsToSolver(theSolverConstraint, mySketchSolver);
}

void PlaneGCSSolver_Storage::addMovementConstraint(
    const ConstraintWrapperPtr& theSolverConstraint)
{
  // before adding movement constraint to solver, re-check its DOF
  if (mySketchSolver->dof() == 0)
    mySketchSolver->diagnose();

  theSolverConstraint->setId(CID_MOVEMENT);
  constraintsToSolver(theSolverConstraint, mySketchSolver);
}


EntityWrapperPtr PlaneGCSSolver_Storage::createFeature(
    const FeaturePtr&             theFeature,
    PlaneGCSSolver_EntityBuilder* theBuilder)
{
  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin();
  for (; anIt != anAttributes.end(); ++anIt)
    createAttribute(*anIt, theBuilder);

  EntityWrapperPtr aResult = theBuilder->createFeature(theFeature);
  if (aResult)
    addEntity(theFeature, aResult);
  return aResult;
}

EntityWrapperPtr PlaneGCSSolver_Storage::createAttribute(
    const AttributePtr&           theAttribute,
    PlaneGCSSolver_EntityBuilder* theBuilder)
{
  EntityWrapperPtr aResult = theBuilder->createAttribute(theAttribute);
  if (aResult)
    addEntity(theAttribute, aResult);
  return aResult;
}

/// \brief Update value
static bool updateValue(const double& theSource, double& theDest)
{
  static const double aTol = 1.e4 * tolerance;
  bool isUpdated = fabs(theSource - theDest) > aTol;
  if (isUpdated)
    theDest = theSource;
  return isUpdated;
}

/// \brief Update coordinates of the point or scalar using its base attribute
static bool updateValues(AttributePtr& theAttribute, EntityWrapperPtr& theEntity)
{
  bool isUpdated = false;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D) {
    const GCSPointPtr& aGCSPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity)->point();
    isUpdated = updateValue(aPoint2D->x(), *(aGCSPoint->x)) || isUpdated;
    isUpdated = updateValue(aPoint2D->y(), *(aGCSPoint->y)) || isUpdated;
  } else {
    AttributeDoublePtr aScalar =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (aScalar) {
      ScalarWrapperPtr aWrapper =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(theEntity);
      // There is possible angular value, which is converted between degrees and radians.
      // So, we use its value instead of using direct pointer to value.
      double aValue = aWrapper->value();
      isUpdated = updateValue(aScalar->value(), aValue);
      if (isUpdated)
        aWrapper->setValue(aValue);
    } else {
      AttributeBooleanPtr aBoolean =
          std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(theAttribute);
      if (aBoolean) {
        BooleanWrapperPtr aWrapper =
            std::dynamic_pointer_cast<PlaneGCSSolver_BooleanWrapper>(theEntity);
        isUpdated = aWrapper->value() != aBoolean->value();
        aWrapper->setValue(aBoolean->value());
      }
    }
  }

  return isUpdated;
}

static bool hasReference(std::shared_ptr<SketchPlugin_Feature> theFeature,
                         const std::string& theFeatureKind)
{
  const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
       aRefIt != aRefs.end(); ++aRefIt) {
     FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
     if (anOwner && anOwner->getKind() == theFeatureKind)
       return true;
  }
  return false;
}

static bool isCopyFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  return theFeature && theFeature->isCopy();
}

bool PlaneGCSSolver_Storage::update(FeaturePtr theFeature, bool theForce)
{
  bool sendNotify = false;
  bool isUpdated = false;
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
  EntityWrapperPtr aRelated = entity(theFeature);
  if (aRelated) // send signal to subscribers
    sendNotify = true;
  else { // Feature is not exist, create it
    bool isCopy = isCopyFeature(aSketchFeature);
    bool isProjReferred = hasReference(aSketchFeature, SketchPlugin_Projection::ID());
    // the feature is a copy in "Multi" constraint and does not used in other constraints
    if (!theForce && (isCopy && !isProjReferred) &&
        myFeatureMap.find(theFeature) == myFeatureMap.end())
      return false;

    // external feature processing
    bool isExternal =
        (aSketchFeature && (aSketchFeature->isExternal() || isCopy || isProjReferred));

    PlaneGCSSolver_FeatureBuilder aBuilder(isExternal ? 0 : this);

    // Reserve the feature in the map of features
    // (do not want to add several copies of it while adding attributes)
    aRelated = createFeature(theFeature, &aBuilder);
    myFeatureMap[theFeature] = aRelated;
    createAuxiliaryConstraints(aRelated);
    isUpdated = true;
  }

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anAttrIt = anAttributes.begin();
  for (; anAttrIt != anAttributes.end(); ++anAttrIt)
    if ((*anAttrIt)->attributeType() == GeomDataAPI_Point2D::typeId() ||
        (*anAttrIt)->attributeType() == ModelAPI_AttributeDouble::typeId() ||
        (*anAttrIt)->attributeType() == ModelAPI_AttributeBoolean::typeId())
      isUpdated = update(*anAttrIt) || isUpdated;

  // check external attribute is changed
  bool isExternal = aSketchFeature &&
                   (aSketchFeature->isExternal() || isCopyFeature(aSketchFeature));
  if (aRelated && isExternal != aRelated->isExternal()) {
    if (isExternal)
      makeExternal(aRelated);
    else
      makeNonExternal(aRelated);
    isUpdated = true;
  }

  // send notification to listeners due to at least one attribute is changed
  if (sendNotify && isUpdated)
    notify(theFeature);

  // update arc
  if (aRelated)
    PlaneGCSSolver_Tools::recalculateArcParameters(aRelated);

  return isUpdated;
}

bool PlaneGCSSolver_Storage::update(AttributePtr theAttribute, bool theForce)
{
  if (!theAttribute->isInitialized())
    return false;

  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject()) {
      FeaturePtr aFeature;
      /// TODO: Check resultToFeatureOrAttribute() precisely.
      resultToFeatureOrAttribute(aRefAttr->object(), aFeature, anAttribute);
      if (aFeature)
        return update(aFeature, theForce);
    } else
      anAttribute = aRefAttr->attr();
  }

  EntityWrapperPtr aRelated = entity(anAttribute);
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttribute->owner());
  if (!aRelated) { // Attribute does not exist, create it.
    // First of all check if the parent feature exists. If not, add it.
    if (aFeature && myFeatureMap.find(aFeature) == myFeatureMap.end())
      return update(aFeature, theForce); // theAttribute has been processed while adding feature
    return aRelated.get() != 0;
  }

  bool isUpdated = updateValues(anAttribute, aRelated);
  if (isUpdated) {
    setNeedToResolve(true);
    notify(aFeature);
  }
  return isUpdated;
}

void PlaneGCSSolver_Storage::makeExternal(const EntityWrapperPtr& theEntity)
{
  if (theEntity->isExternal())
    return;

  removeAuxiliaryConstraints(theEntity);

  GCS::SET_pD aParameters = PlaneGCSSolver_Tools::parameters(theEntity);
  mySketchSolver->removeParameters(aParameters);
  theEntity->setExternal(true);
  myNeedToResolve = true;
}

void PlaneGCSSolver_Storage::makeNonExternal(const EntityWrapperPtr& theEntity)
{
  if (!theEntity->isExternal())
    return;

  GCS::SET_pD aParameters = PlaneGCSSolver_Tools::parameters(theEntity);
  mySketchSolver->addParameters(aParameters);
  theEntity->setExternal(false);

  createAuxiliaryConstraints(theEntity);

  myNeedToResolve = true;
}


static void createArcConstraints(const EntityWrapperPtr& theArc,
                                 const SolverPtr& theSolver,
                                 const ConstraintID theConstraintID,
                                 std::map<EntityWrapperPtr, ConstraintWrapperPtr>& theConstraints)
{
  EdgeWrapperPtr anEdge = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theArc);
  std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEdge->entity());

  // Additional constaints to fix arc's extra DoF (if the arc is not external):
  std::list<GCSConstraintPtr> anArcConstraints;
  // 1. distances from center till start and end points are equal to radius
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
      anArc->center, anArc->start, anArc->rad)));
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
      anArc->center, anArc->end, anArc->rad)));
  // 2. angles of start and end points should be equal to the arc angles
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PAngle(
      anArc->center, anArc->start, anArc->startAngle)));
  anArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PAngle(
      anArc->center, anArc->end, anArc->endAngle)));

  ConstraintWrapperPtr aWrapper(
    new PlaneGCSSolver_ConstraintWrapper(anArcConstraints, CONSTRAINT_UNKNOWN));
  aWrapper->setId(theConstraintID);
  constraintsToSolver(aWrapper, theSolver);

  theConstraints[theArc] = aWrapper;
}

static void createEllipseConstraints(
    const EntityWrapperPtr& theEllipse,
    const SolverPtr& theSolver,
    const ConstraintID theConstraintID,
    std::map<EntityWrapperPtr, ConstraintWrapperPtr>& theConstraints)
{
  EdgeWrapperPtr anEdge = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEllipse);
  std::shared_ptr<GCS::Ellipse> anEllipse =
      std::dynamic_pointer_cast<GCS::Ellipse>(anEdge->entity());

  // Additional constaints to fix ellipse's extra points
  std::list<GCSConstraintPtr> anEllipseConstraints;

  const std::map<std::string, EntityWrapperPtr>& anAttributes = theEllipse->additionalAttributes();
  for (std::map<std::string, EntityWrapperPtr>::const_iterator anIt = anAttributes.begin();
       anIt != anAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (!aPoint)
      continue;

    GCS::InternalAlignmentType anAlignmentX, anAlignmentY;
    if (anIt->first == SketchPlugin_Ellipse::SECOND_FOCUS_ID())
      anAlignmentX = GCS::EllipseFocus2X;
    else if (anIt->first == SketchPlugin_Ellipse::MAJOR_AXIS_START_ID())
      anAlignmentX = GCS::EllipseNegativeMajorX;
    else if (anIt->first == SketchPlugin_Ellipse::MAJOR_AXIS_END_ID())
      anAlignmentX = GCS::EllipsePositiveMajorX;
    else if (anIt->first == SketchPlugin_Ellipse::MINOR_AXIS_START_ID())
      anAlignmentX = GCS::EllipseNegativeMinorX;
    else if (anIt->first == SketchPlugin_Ellipse::MINOR_AXIS_END_ID())
      anAlignmentX = GCS::EllipsePositiveMinorX;

    anEllipseConstraints.push_back(GCSConstraintPtr(
        new GCS::ConstraintInternalAlignmentPoint2Ellipse(
        *anEllipse, *(aPoint->point()), anAlignmentX)));
    anAlignmentY = (GCS::InternalAlignmentType)((int)anAlignmentX + 1);
    anEllipseConstraints.push_back(GCSConstraintPtr(
        new GCS::ConstraintInternalAlignmentPoint2Ellipse(
        *anEllipse, *(aPoint->point()), anAlignmentY)));
  }

  // constraint to bind the major radius value
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aMajorAxisStart =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(
      anAttributes.at(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID()));
  ScalarWrapperPtr aMajorRadius =
      std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(
      anAttributes.at(SketchPlugin_Ellipse::MAJOR_RADIUS_ID()));
  anEllipseConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
      anEllipse->center, *(aMajorAxisStart->point()), aMajorRadius->scalar())));

  ConstraintWrapperPtr aWrapper(
    new PlaneGCSSolver_ConstraintWrapper(anEllipseConstraints, CONSTRAINT_UNKNOWN));
  aWrapper->setId(theConstraintID);
  if (theSolver)
    constraintsToSolver(aWrapper, theSolver);

  theConstraints[theEllipse] = aWrapper;
}

static void createEllipticArcConstraints(
    const EntityWrapperPtr& theEllipticArc,
    const SolverPtr& theSolver,
    const ConstraintID theConstraintID,
    std::map<EntityWrapperPtr, ConstraintWrapperPtr>& theConstraints)
{
  // create base constraints for the ellipse without adding them to solver
  createEllipseConstraints(theEllipticArc, SolverPtr(), theConstraintID, theConstraints);

  ConstraintWrapperPtr& aConstraint = theConstraints[theEllipticArc];
  std::list<GCSConstraintPtr> anEllArcConstraints = aConstraint->constraints();

  // constrain extremities of the elliptic arc
  EdgeWrapperPtr anEdge = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEllipticArc);
  std::shared_ptr<GCS::ArcOfEllipse> anArc =
      std::dynamic_pointer_cast<GCS::ArcOfEllipse>(anEdge->entity());

  anEllArcConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintCurveValue(anArc->start, anArc->start.x, *anArc, anArc->startAngle)));
  anEllArcConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintCurveValue(anArc->start, anArc->start.y, *anArc, anArc->startAngle)));
  anEllArcConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintCurveValue(anArc->end, anArc->end.x, *anArc, anArc->endAngle)));
  anEllArcConstraints.push_back(GCSConstraintPtr(
      new GCS::ConstraintCurveValue(anArc->end, anArc->end.y, *anArc, anArc->endAngle)));

  aConstraint->setConstraints(anEllArcConstraints);
  constraintsToSolver(aConstraint, theSolver);
}

void PlaneGCSSolver_Storage::createAuxiliaryConstraints(const EntityWrapperPtr& theEntity)
{
  if (!theEntity || theEntity->isExternal())
    return;

  if (theEntity->type() == ENTITY_ARC)
    createArcConstraints(theEntity, mySketchSolver, ++myConstraintLastID, myAuxConstraintMap);
  else if (theEntity->type() == ENTITY_ELLIPSE)
    createEllipseConstraints(theEntity, mySketchSolver, ++myConstraintLastID, myAuxConstraintMap);
  else if (theEntity->type() == ENTITY_ELLIPTIC_ARC) {
    createEllipticArcConstraints(theEntity, mySketchSolver,
                                 ++myConstraintLastID, myAuxConstraintMap);
  }
}

void PlaneGCSSolver_Storage::removeAuxiliaryConstraints(const EntityWrapperPtr& theEntity)
{
  std::map<EntityWrapperPtr, ConstraintWrapperPtr>::iterator
      aFound = myAuxConstraintMap.find(theEntity);
  if (aFound != myAuxConstraintMap.end()) {
    mySketchSolver->removeConstraint(aFound->second->id());
    myAuxConstraintMap.erase(aFound);
  }
}

template <typename ARCTYPE>
void adjustArcParametrization(ARCTYPE& theArc, bool theReversed)
{
  // tune start angle of the arc to be in [0, 2PI]
  while (*theArc.startAngle < -PI)
    *theArc.startAngle += 2.0 * PI;
  while (*theArc.startAngle >= PI)
    *theArc.startAngle -= 2.0 * PI;
  // adjust end angle of the arc
  if (theReversed) {
    while (*theArc.endAngle > *theArc.startAngle)
      *theArc.endAngle -= 2.0 * PI;
    while (*theArc.endAngle + 2 * PI < *theArc.startAngle)
      *theArc.endAngle += 2.0 * PI;
  }
  else {
    while (*theArc.endAngle < *theArc.startAngle)
      *theArc.endAngle += 2.0 * PI;
    while (*theArc.endAngle > *theArc.startAngle + 2 * PI)
      *theArc.endAngle -= 2.0 * PI;
  }
}

void PlaneGCSSolver_Storage::adjustParametrizationOfArcs()
{
  std::map<EntityWrapperPtr, ConstraintWrapperPtr>::iterator anIt = myAuxConstraintMap.begin();
  for (; anIt != myAuxConstraintMap.end(); ++anIt) {
    EdgeWrapperPtr anEdge = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(anIt->first);
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEdge->entity());
    if (anArc)
      adjustArcParametrization(*anArc, anEdge->isReversed());
    else {
      std::shared_ptr<GCS::ArcOfEllipse> aEllArc =
          std::dynamic_pointer_cast<GCS::ArcOfEllipse>(anEdge->entity());
      if (aEllArc)
        adjustArcParametrization(*aEllArc, anEdge->isReversed());
    }
  }

  // update parameters of Middle point constraint for point on arc
  std::map<ConstraintPtr, ConstraintWrapperPtr>::iterator aCIt = myConstraintMap.begin();
  for (; aCIt != myConstraintMap.end(); ++aCIt)
    if (aCIt->second->type() == CONSTRAINT_MIDDLE_POINT) {
      notify(aCIt->first);
    }
}


bool PlaneGCSSolver_Storage::removeConstraint(ConstraintPtr theConstraint)
{
  std::map<ConstraintPtr, ConstraintWrapperPtr>::iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound != myConstraintMap.end()) {
    ConstraintWrapperPtr aCW = aFound->second;
    ConstraintID anID = aCW->id();

    // Remove solver's constraints
    mySketchSolver->removeConstraint(anID);

    // Remove value if exists
    const ScalarWrapperPtr& aValue = aCW->valueParameter();
    if (aValue) {
      GCS::SET_pD aParToRemove;
      aParToRemove.insert(aValue->scalar());
      removeParameters(aParToRemove);
    }

    // Remove constraint
    myConstraintMap.erase(aFound);

    if (anID != CID_MOVEMENT)
      myNeedToResolve = true;

    // notify subscibers
    notify(theConstraint);
  }
  return true;
}

void PlaneGCSSolver_Storage::removeInvalidEntities()
{
  PlaneGCSSolver_EntityDestroyer aDestroyer;

  // Remove invalid constraints
  std::list<ConstraintPtr> anInvalidConstraints;
  std::map<ConstraintPtr, ConstraintWrapperPtr>::const_iterator
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
    if (!aFIter->first->data() || !aFIter->first->data()->isValid()) {
      anInvalidFeatures.push_back(aFIter->first);
      if (aFIter->second)
        aDestroyer.remove(aFIter->second);

      // remove invalid arc
      removeAuxiliaryConstraints(aFIter->second);
    }
  std::list<FeaturePtr>::const_iterator anInvFIt = anInvalidFeatures.begin();
  for (; anInvFIt != anInvalidFeatures.end(); ++anInvFIt)
    removeFeature(*anInvFIt);

  // Remove invalid attributes
  std::list<AttributePtr> anInvalidAttributes;
  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anAttrIt = myAttributeMap.begin();
  for (; anAttrIt != myAttributeMap.end(); ++anAttrIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature(anAttrIt->first->owner());
    if (!anOwner || !anOwner->data() || !anOwner->data()->isValid()) {
      anInvalidAttributes.push_back(anAttrIt->first);
      aDestroyer.remove(anAttrIt->second);
    }
  }
  std::list<AttributePtr>::const_iterator anInvAtIt = anInvalidAttributes.begin();
  for (; anInvAtIt != anInvalidAttributes.end(); ++anInvAtIt)
    removeAttribute(*anInvAtIt);

  // free memory occupied by parameters
  removeParameters(aDestroyer.parametersToRemove());

  /// TODO: Think on optimization of checking invalid features and attributes
}



double* PlaneGCSSolver_Storage::createParameter()
{
  return mySketchSolver->createParameter();
}

void PlaneGCSSolver_Storage::removeParameters(const GCS::SET_pD& theParams)
{
  mySketchSolver->removeParameters(theParams);
}

// indicates attribute containing in the external feature
static bool isExternalAttribute(const AttributePtr& theAttribute)
{
  if (!theAttribute)
    return false;
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());
  return aSketchFeature.get() && aSketchFeature->isExternal();
}

static void addOwnerToSet(const AttributePtr& theAttribute, std::set<FeaturePtr>& theFeatures)
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  if (anOwner)
    theFeatures.insert(anOwner);
}

void PlaneGCSSolver_Storage::refresh() const
{
  const double aTol = 1000. * tolerance; // tolerance to prevent frequent updates

  std::set<FeaturePtr> anUpdatedFeatures;

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anIt = myAttributeMap.begin();
  for (; anIt != myAttributeMap.end(); ++anIt) {
    if (!anIt->first->isInitialized())
      continue;

    // the external feature always should keep the up to date values, so,
    // refresh from the solver is never needed
    if (isExternalAttribute(anIt->first))
      continue;

    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anIt->first);
    if (aPoint2D) {
      std::shared_ptr<PlaneGCSSolver_PointWrapper> aPointWrapper =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
      GCSPointPtr aGCSPoint = aPointWrapper->point();
      if (fabs(aPoint2D->x() - (*aGCSPoint->x)) > aTol ||
          fabs(aPoint2D->y() - (*aGCSPoint->y)) > aTol) {
        aPoint2D->setValue(*aGCSPoint->x, *aGCSPoint->y);
        addOwnerToSet(anIt->first, anUpdatedFeatures);
      }
      continue;
    }
    AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anIt->first);
    if (aScalar) {
      ScalarWrapperPtr aScalarWrapper =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      if (fabs(aScalar->value() - aScalarWrapper->value()) > aTol) {
        aScalar->setValue(aScalarWrapper->value());
        addOwnerToSet(anIt->first, anUpdatedFeatures);
      }
      continue;
    }
  }

  // notify listeners about features update
  std::set<FeaturePtr>::const_iterator aFIt = anUpdatedFeatures.begin();
  for (; aFIt != anUpdatedFeatures.end(); ++aFIt)
    notify(*aFIt);
}

PlaneGCSSolver_Solver::SolveStatus PlaneGCSSolver_Storage::checkDegeneratedGeometry() const
{
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIt = myFeatureMap.begin();
  for (; aFIt != myFeatureMap.end(); ++aFIt) {
    EdgeWrapperPtr anEdge = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(aFIt->second);
    if (anEdge && anEdge->isDegenerated())
      return PlaneGCSSolver_Solver::STATUS_DEGENERATED;
  }
  return PlaneGCSSolver_Solver::STATUS_OK;
}


void PlaneGCSSolver_Storage::getUnderconstrainedGeometry(std::set<ObjectPtr>& theFeatures) const
{
  std::vector<double*> aFreeParams;
  mySketchSolver->getFreeParameters(aFreeParams);
  if (aFreeParams.empty())
    return;

  std::map<double*, FeaturePtr> aParamOfFeatures;
  for (std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIt = myFeatureMap.begin();
       aFIt != myFeatureMap.end(); ++aFIt) {
    GCS::SET_pD aParams = PlaneGCSSolver_Tools::parameters(aFIt->second);
    for (GCS::SET_pD::iterator aPIt = aParams.begin(); aPIt != aParams.end(); ++aPIt)
      aParamOfFeatures[*aPIt] = aFIt->first;
  }

  for (std::vector<double*>::iterator anIt = aFreeParams.begin();
       anIt != aFreeParams.end(); ++anIt) {
    std::map<double*, FeaturePtr>::iterator aFound = aParamOfFeatures.find(*anIt);
    if (aFound != aParamOfFeatures.end())
      theFeatures.insert(aFound->second);
  }
}
