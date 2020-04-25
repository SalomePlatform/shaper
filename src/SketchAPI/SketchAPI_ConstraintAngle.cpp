// Copyright (C) 2019  CEA/DEN, EDF R&D
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

#include "SketchAPI_ConstraintAngle.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

#include <GeomAPI_Angle2d.h>
#include <GeomAPI_Lin2d.h>

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_Line.h>

#include <SketcherPrs_Tools.h>

#include <cmath>

SketchAPI_ConstraintAngle::SketchAPI_ConstraintAngle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_Constraint(theFeature)
{
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstraint)
    initialize();
}

static FeaturePtr getFeatureLine(DataPtr theData, const std::string& theAttribute)
{
  FeaturePtr aLine;
  if (!theData.get() || !theData->isValid()) /// essential check as it is called in openGl thread)
    return aLine;

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (aFeature && aFeature->getKind() == SketchPlugin_Line::ID()) {
      return aFeature;
    }
  }
  return aLine;
}

static void calculatePossibleValuesOfAngle(FeaturePtr theFeature,
    double& theAngleDirect, double& theAngleComplementary, double& theAngleBackward)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  FeaturePtr aLineA = getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomDataAPI_Point2D> aStartA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineA->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineA->attribute(SketchPlugin_Line::END_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartB = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineB->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndB = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    aLineB->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Angle2d> anAng(new GeomAPI_Angle2d(
      aStartA->pnt(), aEndA->pnt(), aStartB->pnt(), aEndB->pnt()));
  theAngleDirect = fabs(anAng->angleDegree());
  theAngleComplementary = 180.0 - theAngleDirect;
  theAngleBackward = 360.0 - theAngleDirect;
}

static std::string angleTypeToString(FeaturePtr theFeature)
{
  static const double TOLERANCE = 1.e-7;
  static const std::string THE_ANGLE_DIRECT("Direct");
  static const std::string THE_ANGLE_SUPPLEMENTARY("Supplementary");
  static const std::string THE_ANGLE_BACKWARD("Backward");

  double anAngleDirect, anAngleComplmentary, anAngleBackward;
  calculatePossibleValuesOfAngle(theFeature, anAngleDirect, anAngleComplmentary, anAngleBackward);

  AttributeDoublePtr aValueAttr = theFeature->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID());
  double aDirectDiff = fabs(aValueAttr->value() - anAngleDirect);
  double aComplementaryDiff = fabs(aValueAttr->value() - anAngleComplmentary);
  double aBackwardDiff = fabs(aValueAttr->value() - anAngleBackward);

  AttributeIntegerPtr aTypeAttr = theFeature->integer(SketchPlugin_ConstraintAngle::TYPE_ID());
  bool isDirect = aTypeAttr->value() == SketcherPrs_Tools::ANGLE_DIRECT;
  bool isComplementary = aTypeAttr->value() == SketcherPrs_Tools::ANGLE_COMPLEMENTARY;
  bool isBackward = aTypeAttr->value() == SketcherPrs_Tools::ANGLE_BACKWARD;

  // find the minimal difference
  std::string aType;
  if (isDirect && aDirectDiff < TOLERANCE) {
    aType = THE_ANGLE_DIRECT;
  }
  else if (isComplementary && aComplementaryDiff < TOLERANCE)
    aType = THE_ANGLE_SUPPLEMENTARY;
  else if (isBackward && aBackwardDiff < TOLERANCE)
    aType = THE_ANGLE_BACKWARD;
  else {
    if (aComplementaryDiff < aDirectDiff && aComplementaryDiff < aBackwardDiff)
      aType = THE_ANGLE_SUPPLEMENTARY;
    else if (aBackwardDiff < aDirectDiff && aBackwardDiff < aComplementaryDiff)
      aType = THE_ANGLE_BACKWARD;
  }
  return aType;
}

void SketchAPI_ConstraintAngle::dump(ModelHighAPI_Dumper& theDumper) const
{
  // postpone constraint until all its attributes be dumped
  if (!areAllAttributesDumped(theDumper))
    return;

  // calculate angle value as it was just applied to the attributes
  FeaturePtr aBase = feature();

  const std::string& aSketchName = theDumper.parentName(aBase);
  theDumper << aSketchName << "." << "setAngle(";

  bool isFirstAttr = true;
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
    AttributeRefAttrPtr aRefAttr = aBase->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
    if (aRefAttr && aRefAttr->isInitialized()) {
      theDumper << (isFirstAttr ? "" : ", ") << aRefAttr;
      isFirstAttr = false;
    }
  }

  AttributeDoublePtr aValueAttr = aBase->real(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID());
  if (aValueAttr && aValueAttr->isInitialized())
    theDumper << ", " << aValueAttr;

  std::string aType = angleTypeToString(aBase);
  theDumper << ", type = \"" << aType << "\")" << std::endl;
}
