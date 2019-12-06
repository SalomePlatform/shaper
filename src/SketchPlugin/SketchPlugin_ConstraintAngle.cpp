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

#include "SketchPlugin_ConstraintAngle.h"
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Tools.h>
#include <SketcherPrs_Tools.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Angle2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <SketcherPrs_Factory.h>
#include <SketcherPrs_Tools.h>

#include <cmath>
#include <regex>
#include <sstream>

const double tolerance = 1.e-7;
#define PI 3.1415926535897932

/// \brief Calculate intersection point of two lines
static std::shared_ptr<GeomAPI_Pnt2d> intersect(FeaturePtr theLine1, FeaturePtr theLine2);


SketchPlugin_ConstraintAngle::SketchPlugin_ConstraintAngle()
{
  myFlyoutUpdate = false;
}

void SketchPlugin_ConstraintAngle::initAttributes()
{
  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(ANGLE_VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(TYPE_ID(), ModelAPI_AttributeInteger::typeId());

  data()->addAttribute(ANGLE_REVERSED_FIRST_LINE_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(ANGLE_REVERSED_SECOND_LINE_ID(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(LOCATION_TYPE_ID(), ModelAPI_AttributeInteger::typeId());
  aValidators->registerNotObligatory(getKind(), LOCATION_TYPE_ID());

  data()->addAttribute(SELECTED_FIRST_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->attribute(SELECTED_FIRST_POINT_ID())->setIsArgument(false);
  aValidators->registerNotObligatory(getKind(), SELECTED_FIRST_POINT_ID());

  data()->addAttribute(SELECTED_SECOND_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->attribute(SELECTED_SECOND_POINT_ID())->setIsArgument(false);
  aValidators->registerNotObligatory(getKind(), SELECTED_SECOND_POINT_ID());

  if (attribute(TYPE_ID())->isInitialized())
    myPrevAngleType = integer(TYPE_ID())->value();
  else
    myPrevAngleType = (int)SketcherPrs_Tools::ANGLE_DIRECT;
}

void SketchPlugin_ConstraintAngle::colorConfigInfo(std::string& theSection, std::string& theName,
                                                   std::string& theDefault)
{
  theSection = "Visualization";
  theName = "sketch_dimension_color";
  theDefault = SKETCH_DIMENSION_COLOR;
}

void SketchPlugin_ConstraintAngle::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  AttributeRefAttrPtr anAttrA = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributeRefAttrPtr anAttrB = aData->refattr(SketchPlugin_Constraint::ENTITY_B());
  if (!anAttrA->isInitialized() || !anAttrB->isInitialized())
    return;

  AttributeDoublePtr anAttrValue = real(ANGLE_VALUE_ID());
  if (!anAttrValue->isInitialized())
    calculateAngle();

  // the value should to be computed here, not in the
  // getAISObject in order to change the model value
  // inside the object transaction. This is important for creating a constraint by preselection.
  // The display of the presentation in this case happens after the transaction commit
  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if(!aFlyOutAttr->isInitialized())
    compute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
}

AISObjectPtr SketchPlugin_ConstraintAngle::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::angleConstraint(this, sketch(),
                                                            thePrevious);
  if (anAIS.get() && !thePrevious.get())
    SketchPlugin_Tools::setDimensionColor(anAIS);
  return anAIS;
}

void SketchPlugin_ConstraintAngle::attributeChanged(const std::string& theID)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData)
    return;

  if (theID == TYPE_ID())
    updateAngleValue();

  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_B());
  if (!aLineA || !aLineB)
    return;

  if (theID == ENTITY_A() || theID == ENTITY_B() ||
      theID == TYPE_ID() || theID == ANGLE_VALUE_ID()) {
    calculateAngle();
  } else if (theID == FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    // Recalculate flyout point in local coordinates
    // coordinates are calculated according to the center of shapes intersection
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(FLYOUT_VALUE_PNT()));

    std::shared_ptr<ModelAPI_Data> aData = data();
    std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());

    // Intersection of lines
    std::shared_ptr<GeomAPI_Pnt2d> anInter = intersect(aLineA, aLineB);
    if (!anInter)
      return;

    myFlyoutUpdate = true;
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutAttr->pnt()->xy()->decreased(anInter->xy());
    if (aFlyoutDir->dot(aFlyoutDir) < tolerance * tolerance)
      aFlyoutAttr->setValue(aFlyoutAttr->x() + tolerance, aFlyoutAttr->y());
    myFlyoutUpdate = false;
  }
}

void SketchPlugin_ConstraintAngle::calculateAngle()
{
  // update *_REVERSED_* flags
  calculateAnglePosition();

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_B());

  GeomPnt2dPtr aStartA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::END_ID());
  GeomPnt2dPtr aStartB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::END_ID());

  std::shared_ptr<GeomAPI_Lin2d> aLine1(new GeomAPI_Lin2d(aStartA, aEndA));
  std::shared_ptr<GeomAPI_Lin2d> aLine2(new GeomAPI_Lin2d(aStartB, aEndB));

  bool isReversed1 = boolean(ANGLE_REVERSED_FIRST_LINE_ID())->value();
  bool isReversed2 = boolean(ANGLE_REVERSED_SECOND_LINE_ID())->value();

  std::shared_ptr<GeomAPI_Angle2d> anAng(
      new GeomAPI_Angle2d(aLine1, isReversed1, aLine2, isReversed2));
  double anAngle = anAng->angleDegree();

  AttributeDoublePtr anAngleValueAttr = real(ANGLE_VALUE_ID());
  if (!anAngleValueAttr->isInitialized())
    anAngleValueAttr->setValue(getAngleForType(fabs(anAngle)));

  anAngle /= fabs(anAngle);
  anAngle *= getAngleForType(anAngleValueAttr->value());

  // update value of the constraint to be passed to the solver
  real(SketchPlugin_Constraint::VALUE())->setValue(anAngle);
}

void SketchPlugin_ConstraintAngle::calculateAnglePosition()
{
  if (attribute(ANGLE_REVERSED_FIRST_LINE_ID())->isInitialized() &&
      attribute(ANGLE_REVERSED_SECOND_LINE_ID())->isInitialized())
    return; // already calculated

  DataPtr aData = data();
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_B());

  GeomPnt2dPtr aStartA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::END_ID());
  GeomPnt2dPtr aStartB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::END_ID());

  bool isReversed1 = false;
  bool isReversed2 = false;

  GeomPnt2dPtr aSelected1 = SketcherPrs_Tools::getPoint(this, SELECTED_FIRST_POINT_ID());
  GeomPnt2dPtr aSelected2 = SketcherPrs_Tools::getPoint(this, SELECTED_SECOND_POINT_ID());
  if (aSelected1 && aSelected2) {
    GeomPnt2dPtr anInterPnt = intersect(aLineA, aLineB);
    if (!anInterPnt)
      return;
    std::shared_ptr<GeomAPI_XY> anInterXY = anInterPnt->xy();
    isReversed1 = aSelected1->xy()->decreased(anInterXY)->dot(
                  aEndA->xy()->decreased(aStartA->xy())) < -tolerance;
    isReversed2 = aSelected2->xy()->decreased(anInterXY)->dot(
                  aEndB->xy()->decreased(aStartB->xy())) < -tolerance;
  }
  else {
    // no point is selected (document opened or Python script is loaded),
    // calculate basing on the value
    std::shared_ptr<GeomAPI_Angle2d> anAng(new GeomAPI_Angle2d(aStartA, aEndA, aStartB, aEndB));
    isReversed1 = anAng->isReversed(0);
    isReversed2 = anAng->isReversed(1);
  }

  // adjust reversed flags according to the angle type
  AttributeIntegerPtr aTypeAttr = integer(TYPE_ID());
  if (aTypeAttr && aTypeAttr->isInitialized() &&
     (SketcherPrs_Tools::AngleType)(aTypeAttr->value()) == SketcherPrs_Tools::ANGLE_COMPLEMENTARY)
    isReversed1 = !isReversed1;

  boolean(ANGLE_REVERSED_FIRST_LINE_ID())->setValue(isReversed1);
  boolean(ANGLE_REVERSED_SECOND_LINE_ID())->setValue(isReversed2);
}

static double angleForType(const double theAngle, const int theType)
{
  double anAngle = theAngle;
  switch ((SketcherPrs_Tools::AngleType)theType) {
    case SketcherPrs_Tools::ANGLE_DIRECT:
      anAngle = theAngle;
      break;
    case SketcherPrs_Tools::ANGLE_COMPLEMENTARY:
      anAngle = 180.0 - theAngle;
      break;
    case SketcherPrs_Tools::ANGLE_BACKWARD:
      anAngle = 360.0 - theAngle;
      break;
    default:
      break;
  }
  return anAngle;
}

double SketchPlugin_ConstraintAngle::getAngleForType(double theAngle)
{
  return angleForType(theAngle, integer(TYPE_ID())->value());
}

void SketchPlugin_ConstraintAngle::updateAngleValue()
{
  AttributeIntegerPtr anAngleType = integer(TYPE_ID());
  AttributeDoublePtr anAngleValueAttr = real(ANGLE_VALUE_ID());
  if (anAngleValueAttr->isInitialized()) {
    if (anAngleValueAttr->text().empty()) {
      // calculate value related to the type twice:
      // the first time - to return to direct angle,
      // the second time - to apply new type
      double aValue = angleForType(anAngleValueAttr->value(), myPrevAngleType);
      aValue = angleForType(aValue, anAngleType->value());
      anAngleValueAttr->setValue(aValue);
    }
    else {
      // process the parametric value
      std::string anAngleText = anAngleValueAttr->text();
      std::regex anAngleRegex("\\s*([-+]?[0-9]*\\.?[0-9]*)\\s*([-+])\\s*\\((.*)\\)",
                              std::regex_constants::ECMAScript);

      double anAnglePrefix = 0.0;
      static const char aSignPrefix[2] = { '-', '+' };
      int aSignInd = 1;

      std::smatch aResult;
      if (std::regex_search(anAngleText, aResult, anAngleRegex)) {
        anAnglePrefix = std::atof(aResult[1].str().c_str());
        aSignInd = aResult[2].str()[0] == aSignPrefix[0] ? 0 : 1;
        anAngleText = aResult[3].str();
      }

      if (myPrevAngleType != SketcherPrs_Tools::ANGLE_DIRECT)
        aSignInd = 1 - aSignInd;
      anAnglePrefix = angleForType(anAnglePrefix, myPrevAngleType);

      if (anAngleType->value() != SketcherPrs_Tools::ANGLE_DIRECT)
        aSignInd = 1 - aSignInd;
      anAnglePrefix = angleForType(anAnglePrefix, anAngleType->value());

      std::ostringstream aText;
      bool isPrintSign = true;
      if (fabs(anAnglePrefix) > tolerance)
        aText << anAnglePrefix;
      else
        isPrintSign = aSignInd == 0;
      if (isPrintSign)
        aText << " " << aSignPrefix[aSignInd] << " (";
      aText << anAngleText << (isPrintSign ? ")" : "");
      anAngleValueAttr->setText(aText.str());
    }
  }
  myPrevAngleType = anAngleType->value();
}

bool SketchPlugin_ConstraintAngle::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;
  if (!sketch())
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(attribute(theAttributeId));
  if (aFlyOutAttr->isInitialized() &&
      (fabs(aFlyOutAttr->x()) >= tolerance || fabs(aFlyOutAttr->y()) >= tolerance))
    return false;

  DataPtr aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

  if ((aLineA.get() == NULL) || (aLineB.get() == NULL))
    return false;

  // Start and end points of lines
  GeomPnt2dPtr aStartA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndA   = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::END_ID());
  if (aStartA->distance(aEndA) < tolerance)
    return false;

  GeomPnt2dPtr aStartB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndB   = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::END_ID());
  if (aStartB->distance(aEndB) < tolerance)
    return false;

  myFlyoutUpdate = true;
  double aX = (aStartA->x() + aEndA->x() + aStartB->x() + aEndB->x()) / 4.;
  double aY = (aStartA->y() + aEndA->y() + aStartB->y() + aEndB->y()) / 4.;

  aFlyOutAttr->setValue(aX, aY);
  myFlyoutUpdate = false;

  return true;
}


// ===============   Auxiliary functions   ==================================
std::shared_ptr<GeomAPI_Pnt2d> intersect(FeaturePtr theLine1, FeaturePtr theLine2)
{
  // Start and end points of lines
  const std::string& aLineStartAttr = SketchPlugin_Line::START_ID();
  const std::string& aLineEndAttr = SketchPlugin_Line::END_ID();
  GeomPnt2dPtr aStartA = SketcherPrs_Tools::getPoint(theLine1.get(), aLineStartAttr);
  GeomPnt2dPtr aEndA   = SketcherPrs_Tools::getPoint(theLine1.get(), aLineEndAttr);
  GeomPnt2dPtr aStartB = SketcherPrs_Tools::getPoint(theLine2.get(), aLineStartAttr);
  GeomPnt2dPtr aEndB   = SketcherPrs_Tools::getPoint(theLine2.get(), aLineEndAttr);
  if (aStartA->distance(aEndA) < tolerance || aStartB->distance(aEndB) < tolerance)
    std::shared_ptr<GeomAPI_Pnt2d>();

  // Lines and their intersection point
  std::shared_ptr<GeomAPI_Lin2d> aLA(new GeomAPI_Lin2d(aStartA, aEndA));
  std::shared_ptr<GeomAPI_Lin2d> aLB(new GeomAPI_Lin2d(aStartB, aEndB));
  return aLA->intersect(aLB);
}
