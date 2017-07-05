// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Solver.h>
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
#include <SketchPlugin_Projection.h>

#include <cmath>


static void constraintsToSolver(const ConstraintWrapperPtr& theConstraint,
                                const SolverPtr& theSolver)
{
  const std::list<GCSConstraintPtr>& aConstraints =
      std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(theConstraint)->constraints();
  std::list<GCSConstraintPtr>::const_iterator anIt = aConstraints.begin();
  for (; anIt != aConstraints.end(); ++anIt)
    theSolver->addConstraint(*anIt);
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

void PlaneGCSSolver_Storage::addTemporaryConstraint(
    const ConstraintWrapperPtr& theSolverConstraint)
{
  if (myConstraintMap.empty())
    return; // no need to process temporary constraints if there is no active constraint

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
  static const double aTol = 1000. * tolerance;
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
     if (anOwner->getKind() == theFeatureKind)
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
  EntityWrapperPtr aRelated = entity(theFeature);
  if (aRelated) // send signal to subscribers
    sendNotify = true;
  else { // Feature is not exist, create it
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
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
    createArcConstraints(aRelated);
    isUpdated = true;
  }

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anAttrIt = anAttributes.begin();
  for (; anAttrIt != anAttributes.end(); ++anAttrIt)
    if ((*anAttrIt)->attributeType() == GeomDataAPI_Point2D::typeId() ||
        (*anAttrIt)->attributeType() == ModelAPI_AttributeDouble::typeId())
      isUpdated = update(*anAttrIt) || isUpdated;

  // send notification to listeners due to at least one attribute is changed
  if (sendNotify && isUpdated)
    notify(theFeature);

  // update arc
  if (aRelated && aRelated->type() == ENTITY_ARC) {
    /// TODO: this code should be shared with FeatureBuilder somehow

    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
        std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(aRelated);
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEntity->entity());

    static std::shared_ptr<GeomAPI_Dir2d> OX(new GeomAPI_Dir2d(1.0, 0.0));
    std::shared_ptr<GeomAPI_Pnt2d> aCenter(
        new GeomAPI_Pnt2d(*anArc->center.x, *anArc->center.y));
    std::shared_ptr<GeomAPI_Pnt2d> aStart(
        new GeomAPI_Pnt2d(*anArc->start.x, *anArc->start.y));

    *anArc->rad = aStart->distance(aCenter);

    std::shared_ptr<GeomAPI_Dir2d> aDir(new GeomAPI_Dir2d(aStart->xy()->decreased(aCenter->xy())));
    *anArc->startAngle = OX->angle(aDir);

    aDir = std::shared_ptr<GeomAPI_Dir2d>(
        new GeomAPI_Dir2d((*anArc->end.x) - aCenter->x(), (*anArc->end.y) - aCenter->y()));
    *anArc->endAngle = OX->angle(aDir);
  }

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

  removeArcConstraints(theEntity);

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

  createArcConstraints(theEntity);

  myNeedToResolve = true;
}


void PlaneGCSSolver_Storage::createArcConstraints(const EntityWrapperPtr& theArc)
{
  if (!theArc || theArc->type() != ENTITY_ARC || theArc->isExternal())
    return;

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
  aWrapper->setId(++myConstraintLastID);
  constraintsToSolver(aWrapper, mySketchSolver);

  myArcConstraintMap[theArc] = aWrapper;
}

void PlaneGCSSolver_Storage::removeArcConstraints(const EntityWrapperPtr& theArc)
{
  std::map<EntityWrapperPtr, ConstraintWrapperPtr>::iterator
      aFound = myArcConstraintMap.find(theArc);
  if (aFound != myArcConstraintMap.end()) {
    mySketchSolver->removeConstraint(aFound->second->id());
    myArcConstraintMap.erase(aFound);
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
      removeArcConstraints(aFIter->second);
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
