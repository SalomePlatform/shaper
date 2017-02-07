// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_AttributeBuilder.cpp
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_AngleWrapper.h>
#include <PlaneGCSSolver_AttributeBuilder.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_MultiRotation.h>

PlaneGCSSolver_AttributeBuilder::PlaneGCSSolver_AttributeBuilder(
    PlaneGCSSolver_Storage* theStorage)
  : PlaneGCSSolver_EntityBuilder(theStorage)
{
}

PlaneGCSSolver_AttributeBuilder::PlaneGCSSolver_AttributeBuilder(const StoragePtr& theStorage)
  : PlaneGCSSolver_EntityBuilder(
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(theStorage).get())
{
}

static double* createParameter(PlaneGCSSolver_Storage* theStorage)
{
  return theStorage ? theStorage->createParameter() : (new double(0));
}

static EntityWrapperPtr createScalar(const AttributePtr&     theAttribute,
                                     PlaneGCSSolver_Storage* theStorage)
{
  AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (!aScalar)
    return EntityWrapperPtr();

  ScalarWrapperPtr aWrapper;
  // following attributes should be converted from degrees to radians
  //  - value of the Angle constraint
  //  - angle of the Multi-Rotation constraint
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  if ((theAttribute->id() == SketchPlugin_Constraint::VALUE() &&
      anOwner->getKind() == SketchPlugin_ConstraintAngle::ID()) ||
     (theAttribute->id() == SketchPlugin_MultiRotation::ANGLE_ID() &&
      anOwner->getKind() == SketchPlugin_MultiRotation::ID()))
    aWrapper = ScalarWrapperPtr(new PlaneGCSSolver_AngleWrapper(createParameter(theStorage)));
  else
    aWrapper = ScalarWrapperPtr(new PlaneGCSSolver_ScalarWrapper(createParameter(theStorage)));

  aWrapper->setValue(aScalar->value());
  return aWrapper;
}

static EntityWrapperPtr createPoint(const AttributePtr&     theAttribute,
                                    PlaneGCSSolver_Storage* theStorage)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (!aPoint2D)
    return EntityWrapperPtr();

  GCSPointPtr aNewPoint(new GCS::Point);

  aNewPoint->x = createParameter(theStorage);
  *(aNewPoint->x) = aPoint2D->x();
  aNewPoint->y = createParameter(theStorage);
  *(aNewPoint->y) = aPoint2D->y();

  return EntityWrapperPtr(new PlaneGCSSolver_PointWrapper(aNewPoint));
}

EntityWrapperPtr PlaneGCSSolver_AttributeBuilder::createAttribute(
    AttributePtr theAttribute)
{
  EntityWrapperPtr aResult;
  if (myStorage)
    aResult = myStorage->entity(theAttribute);
  if (!aResult)
    aResult = createPoint(theAttribute, myStorage);
  if (!aResult)
    aResult = createScalar(theAttribute, myStorage);
  if (aResult && !myStorage)
    aResult->setExternal(true);
  return aResult;
}

const std::list<GCSConstraintPtr>& PlaneGCSSolver_AttributeBuilder::constraints() const
{
  static std::list<GCSConstraintPtr> aList;
  return aList;
}
