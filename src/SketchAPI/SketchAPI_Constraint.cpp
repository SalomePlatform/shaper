// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Constraint.cpp
// Purpose:
//
// History:
// 08/08/16 - Artem ZHIDKOV - Creation of the file

#include "SketchAPI_Constraint.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_SketchEntity.h>

#include <SketcherPrs_Tools.h>

SketchAPI_Constraint::SketchAPI_Constraint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstraint)
    initialize();
}

SketchAPI_Constraint::~SketchAPI_Constraint()
{
}

bool SketchAPI_Constraint::initialize()
{
  if (!feature()) {
    throwException("Constraint exception: The feature is NULL.");
    return false;
  }
  return true;
}

static const std::string& constraintTypeToSetter(const std::string& theType)
{
  if (theType == SketchPlugin_ConstraintCoincidence::ID()) {
    static const std::string COINCIDENCE_SETTER("setCoincident");
    return COINCIDENCE_SETTER;
  }
  if (theType == SketchPlugin_ConstraintAngle::ID()) {
    static const std::string ANGLE_SETTER("setAngle");
    return ANGLE_SETTER;
  }
  if (theType == SketchPlugin_ConstraintCollinear::ID()) {
    static const std::string COLLINEAR_SETTER("setCollinear");
    return COLLINEAR_SETTER;
  }
  if (theType == SketchPlugin_ConstraintDistance::ID()) {
    static const std::string DISTANCE_SETTER("setDistance");
    return DISTANCE_SETTER;
  }
  if (theType == SketchPlugin_ConstraintDistanceHorizontal::ID()) {
    static const std::string DISTANCE_SETTER("setHorizontalDistance");
    return DISTANCE_SETTER;
  }
  if (theType == SketchPlugin_ConstraintDistanceVertical::ID()) {
    static const std::string DISTANCE_SETTER("setVerticalDistance");
    return DISTANCE_SETTER;
  }
  if (theType == SketchPlugin_ConstraintEqual::ID()) {
    static const std::string EQUAL_SETTER("setEqual");
    return EQUAL_SETTER;
  }
  if (theType == SketchPlugin_ConstraintHorizontal::ID()) {
    static const std::string HORIZONTAL_SETTER("setHorizontal");
    return HORIZONTAL_SETTER;
  }
  if (theType == SketchPlugin_ConstraintLength::ID()) {
    static const std::string LENGTH_SETTER("setLength");
    return LENGTH_SETTER;
  }
  if (theType == SketchPlugin_ConstraintMiddle::ID()) {
    static const std::string MIDDLE_SETTER("setMiddlePoint");
    return MIDDLE_SETTER;
  }
  if (theType == SketchPlugin_ConstraintParallel::ID()) {
    static const std::string PARALLEL_SETTER("setParallel");
    return PARALLEL_SETTER;
  }
  if (theType == SketchPlugin_ConstraintPerpendicular::ID()) {
    static const std::string PERPENDICULAR_SETTER("setPerpendicular");
    return PERPENDICULAR_SETTER;
  }
  if (theType == SketchPlugin_ConstraintRadius::ID()) {
    static const std::string RADIUS_SETTER("setRadius");
    return RADIUS_SETTER;
  }
  if (theType == SketchPlugin_ConstraintRigid::ID()) {
    static const std::string FIXED_SETTER("setFixed");
    return FIXED_SETTER;
  }
  if (theType == SketchPlugin_ConstraintTangent::ID()) {
    static const std::string TANGENT_SETTER("setTangent");
    return TANGENT_SETTER;
  }
  if (theType == SketchPlugin_ConstraintVertical::ID()) {
    static const std::string VERTICAL_SETTER("setVertical");
    return VERTICAL_SETTER;
  }

  static const std::string DUMMY;
  return DUMMY;
}

static std::string angleTypeToString(int theAngleType)
{
  switch (theAngleType) {
  case SketcherPrs_Tools::ANGLE_COMPLEMENTARY:
    return std::string("Complementary");
  case SketcherPrs_Tools::ANGLE_BACKWARD:
    return std::string("Backward");
  default:
    break;
  }
  return std::string();
}

void SketchAPI_Constraint::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSetter = constraintTypeToSetter(aBase->getKind());
  if (aSetter.empty())
    return; // incorrect constraint type

  // do not need to dump "Fixed" constraint for external object
  if (aBase->getKind() == SketchPlugin_ConstraintRigid::ID()) {
    AttributeRefAttrPtr aRefAttr = aBase->refattr(SketchPlugin_Constraint::ENTITY_A());
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
    if (!aFeature)
      return;
    AttributeSelectionPtr aAttr =
      aFeature->data()->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
    if (aAttr && aAttr->context().get() != NULL && !aAttr->isInvalid())
      return;
  }

  bool isAngle = aBase->getKind() == SketchPlugin_ConstraintAngle::ID();
  std::string aSetterSuffix;
  if (isAngle)
    aSetterSuffix = angleTypeToString(aBase->integer(
                    SketchPlugin_ConstraintAngle::TYPE_ID())->value());

  const std::string& aSketchName = theDumper.parentName(aBase);
  theDumper << aBase << " = " << aSketchName << "." << aSetter << aSetterSuffix << "(";

  bool isFirstAttr = true;
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
    AttributeRefAttrPtr aRefAttr = aBase->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
    if (aRefAttr && aRefAttr->isInitialized()) {
      theDumper << (isFirstAttr ? "" : ", ") << aRefAttr;
      isFirstAttr = false;
    }
  }

  AttributeDoublePtr aValueAttr = aBase->real(
      isAngle ? SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID() :SketchPlugin_Constraint::VALUE());
  if (aValueAttr && aValueAttr->isInitialized())
    theDumper << ", " << aValueAttr;

  theDumper << ")" << std::endl;
}
