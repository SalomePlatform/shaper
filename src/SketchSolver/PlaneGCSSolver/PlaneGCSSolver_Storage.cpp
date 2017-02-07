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

#include <PlaneGCSSolver_AttributeBuilder.h>
#include <PlaneGCSSolver_FeatureBuilder.h>
#include <PlaneGCSSolver_EntityDestroyer.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <SketchPlugin_Projection.h>

#include <cmath>


static void constraintsToSolver(const ConstraintWrapperPtr& theConstraint,
                                const SolverPtr& theSolver)
{
  std::shared_ptr<PlaneGCSSolver_Solver> aSolver =
      std::dynamic_pointer_cast<PlaneGCSSolver_Solver>(theSolver);
  if (!aSolver)
    return;

  const std::list<GCSConstraintPtr>& aConstraints =
      std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(theConstraint)->constraints();
  std::list<GCSConstraintPtr>::const_iterator anIt = aConstraints.begin();
  for (; anIt != aConstraints.end(); ++anIt)
    aSolver->addConstraint(*anIt);
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

static bool isCopyInMulti(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  if (!theFeature)
    return false;

  bool aResult = theFeature->isCopy();
  if (aResult) {
    const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
    for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
         aRefIt != aRefs.end() && aResult; ++aRefIt) {
      FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
      if (anOwner->getKind() == SketchPlugin_Projection::ID())
        aResult = false;
    }
  }
  return aResult;
}

bool PlaneGCSSolver_Storage::update(FeaturePtr theFeature, bool theForce)
{
  bool isUpdated = false;
  EntityWrapperPtr aRelated = entity(theFeature);
  if (aRelated) // send signal to subscribers
    notify(theFeature);
  else { // Feature is not exist, create it
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
    bool isCopy = isCopyInMulti(aSketchFeature);
    // the feature is a copy in "Multi" constraint and does not used in other constraints
    if (!theForce && isCopy && myFeatureMap.find(theFeature) == myFeatureMap.end())
      return false;

    // external feature processing
    bool isExternal = (aSketchFeature && (aSketchFeature->isExternal() || isCopy));

    PlaneGCSSolver_FeatureBuilder aBuilder(isExternal ? 0 : this);

    // Reserve the feature in the map of features
    // (do not want to add several copies of it while adding attributes)
    aRelated = createFeature(theFeature, &aBuilder);
    myFeatureMap[theFeature] = aRelated;

    const std::list<GCSConstraintPtr>& aConstraints = aBuilder.constraints();
    if (!aConstraints.empty()) { // the feature is arc
      /// TODO: avoid this workaround
      ConstraintWrapperPtr aWrapper(
          new PlaneGCSSolver_ConstraintWrapper(aConstraints, CONSTRAINT_UNKNOWN));
      aWrapper->setId(++myConstraintLastID);
      constraintsToSolver(aWrapper, mySketchSolver);

      myArcConstraintMap[myFeatureMap[theFeature]] = aWrapper;
    }
    isUpdated = true;
  }

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anAttrIt = anAttributes.begin();
  for (; anAttrIt != anAttributes.end(); ++anAttrIt)
    if ((*anAttrIt)->attributeType() == GeomDataAPI_Point2D::typeId() ||
        (*anAttrIt)->attributeType() == ModelAPI_AttributeDouble::typeId())
      isUpdated = update(*anAttrIt) || isUpdated;

  // update arc
  if (aRelated && aRelated->type() == ENTITY_ARC) {
    /// TODO: this code should be shared with FeatureBuilder somehow

    std::shared_ptr<PlaneGCSSolver_EntityWrapper> anEntity =
        std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(aRelated);
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
  if (!aRelated) { // Attribute does not exist, create it.
    // First of all check if the parent feature exists. If not, add it.
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttribute->owner());
    if (aFeature && myFeatureMap.find(aFeature) == myFeatureMap.end())
      return update(aFeature, theForce); // theAttribute has been processed while adding feature

    PlaneGCSSolver_AttributeBuilder aBuilder(this);
    aRelated = createAttribute(anAttribute, &aBuilder);
    return aRelated.get() != 0;
  }

  bool isUpdated = updateValues(anAttribute, aRelated);
  if (isUpdated)
    setNeedToResolve(true);
  return isUpdated;
}



bool PlaneGCSSolver_Storage::removeConstraint(ConstraintPtr theConstraint)
{
  std::map<ConstraintPtr, ConstraintWrapperPtr>::iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound != myConstraintMap.end()) {
    ConstraintID anID = aFound->second->id();
    // Remove solver's constraints
    std::shared_ptr<PlaneGCSSolver_Solver> aSolver =
        std::dynamic_pointer_cast<PlaneGCSSolver_Solver>(mySketchSolver);
    aSolver->removeConstraint(anID);
    // Remove constraint
    myConstraintMap.erase(aFound);

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
      aDestroyer.remove(aFIter->second);

      // remove invalid arc
      std::map<EntityWrapperPtr, ConstraintWrapperPtr>::iterator
          aFound = myArcConstraintMap.find(aFIter->second);
      if (aFound != myArcConstraintMap.end()) {
        std::dynamic_pointer_cast<PlaneGCSSolver_Solver>(
            mySketchSolver)->removeConstraint(aFound->second->id());
        myArcConstraintMap.erase(aFound);
      }
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
  double* aResult = new double(0);
  myParameters.push_back(aResult);
  return aResult;
}

void PlaneGCSSolver_Storage::removeParameters(const GCS::SET_pD& theParams)
{
  for (int i = (int)myParameters.size() - 1; i >= 0; --i)
    if (theParams.find(myParameters[i]) != theParams.end())
      myParameters.erase(myParameters.begin() + i);
}


bool PlaneGCSSolver_Storage::isRedundant(
    GCSConstraintPtr theCheckedConstraint,
    ConstraintWrapperPtr theParentConstraint,
    std::list<std::set<double*> >& theCoincidentPoints) const
{
  if (theParentConstraint->type() == CONSTRAINT_SYMMETRIC) {
    if (theCheckedConstraint->getTypeId() == GCS::Perpendicular) {
      BuilderPtr aBuilder = PlaneGCSSolver_Builder::getInstance();
      // check the initial point is placed on the mirror line
      std::list<EntityWrapperPtr> aSubs = theParentConstraint->entities();
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aBuilder->point(aSubs.front());
      std::shared_ptr<GeomAPI_Lin2d> aLine = aBuilder->line(aSubs.back());
      return aLine->distance(aPoint) < tolerance;
    }
  }
  else if (theParentConstraint->type() == CONSTRAINT_PT_PT_COINCIDENT) {
    // Verify that the coincidence between points is already added
    GCS::VEC_pD aParams = theCheckedConstraint->params();

    std::list<std::set<double*> >::iterator aCoincIt, aFound1, aFound2;
    aFound1 = aFound2 = theCoincidentPoints.end();
    for (aCoincIt = theCoincidentPoints.begin();
         aCoincIt != theCoincidentPoints.end(); ++aCoincIt) {
      if (aFound1 == theCoincidentPoints.end() && aCoincIt->find(aParams[0]) != aCoincIt->end())
        aFound1 = aCoincIt;
      if (aFound2 == theCoincidentPoints.end() && aCoincIt->find(aParams[1]) != aCoincIt->end())
        aFound2 = aCoincIt;
      if (aFound1 != theCoincidentPoints.end() && aFound2 != theCoincidentPoints.end())
        break;
    }
    if (aCoincIt != theCoincidentPoints.end()) { // both point are found
      if (aFound1 == aFound2)
        return true;
      // merge two groups of coincidence
      aFound1->insert(aFound2->begin(), aFound2->end());
      theCoincidentPoints.erase(aFound2);
    } else {
      if (aFound1 != theCoincidentPoints.end())
        aFound1->insert(aParams[1]);
      else if (aFound2 != theCoincidentPoints.end())
        aFound2->insert(aParams[0]);
      else {
        std::set<double*> aNewCoincidence;
        aNewCoincidence.insert(aParams[0]);
        aNewCoincidence.insert(aParams[1]);
        theCoincidentPoints.push_back(aNewCoincidence);
      }
    }
  }

  return false;
}

void PlaneGCSSolver_Storage::initializeSolver()
{
  std::shared_ptr<PlaneGCSSolver_Solver> aSolver =
      std::dynamic_pointer_cast<PlaneGCSSolver_Solver>(mySketchSolver);
  if (aSolver)
    aSolver->setParameters(myParameters);
}

// indicates attribute containing in the external feature
bool isExternalAttribute(const AttributePtr& theAttribute)
{
  if (!theAttribute)
    return false;
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());
  return aSketchFeature.get() && aSketchFeature->isExternal();
}

void PlaneGCSSolver_Storage::refresh() const
{
  const double aTol = 1000. * tolerance; // tolerance to prevent frequent updates

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anIt = myAttributeMap.begin();
  for (; anIt != myAttributeMap.end(); ++anIt) {
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
          fabs(aPoint2D->y() - (*aGCSPoint->y)) > aTol)
        aPoint2D->setValue(*aGCSPoint->x, *aGCSPoint->y);
      continue;
    }
    AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anIt->first);
    if (aScalar) {
      ScalarWrapperPtr aScalarWrapper =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      if (fabs(aScalar->value() - aScalarWrapper->value()) > aTol)
        aScalar->setValue(aScalarWrapper->value());
      continue;
    }
  }
}
