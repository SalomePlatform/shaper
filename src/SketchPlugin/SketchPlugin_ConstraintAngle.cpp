// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
#include <ModelAPI_EventReentrantMessage.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Locale_Convert.h>

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
#include <vector>

const double tolerance = 1.e-7;
#define PI 3.1415926535897932

// To support old types of GCC (less than 4.9), check the regular expressions are working
#if (__cplusplus >= 201103L || _MSVC_LANG >= 201103L)  && \
    (__cplusplus >= 201402L || !defined(__GLIBCXX__)   || \
        (defined(_GLIBCXX_REGEX_DFS_QUANTIFIERS_LIMIT) || \
         defined(_GLIBCXX_REGEX_STATE_LIMIT)           || \
         (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE > 4)))
#define HAVE_WORKING_REGEX 1
#else
#define HAVE_WORKING_REGEX 0
#endif


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

  data()->addAttribute(PREV_TYPE_ID(), ModelAPI_AttributeInteger::typeId());
  data()->attribute(PREV_TYPE_ID())->setIsArgument(false);
  aValidators->registerNotObligatory(getKind(), PREV_TYPE_ID());
  if (attribute(TYPE_ID())->isInitialized())
    integer(PREV_TYPE_ID())->setValue(integer(TYPE_ID())->value());

  data()->addAttribute(SELECTED_FIRST_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->attribute(SELECTED_FIRST_POINT_ID())->setIsArgument(false);
  aValidators->registerNotObligatory(getKind(), SELECTED_FIRST_POINT_ID());

  data()->addAttribute(SELECTED_SECOND_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->attribute(SELECTED_SECOND_POINT_ID())->setIsArgument(false);
  aValidators->registerNotObligatory(getKind(), SELECTED_SECOND_POINT_ID());

  AttributeIntegerPtr aVerAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(
      data()->addAttribute(VERSION_ID(), ModelAPI_AttributeInteger::typeId()));
  aVerAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERSION_ID());
  if (!aVerAttr->isInitialized()) {
    // this is a newly created feature (not read from file),
    // so, initialize the latest version
    aVerAttr->setValue(THE_VERSION_1);
  }
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

  AttributeIntegerPtr aVersion = integer(VERSION_ID());
  if (!aVersion->isInitialized() || aVersion->value() < THE_VERSION_1)
    updateVersion();

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

// LCOV_EXCL_START
std::string SketchPlugin_ConstraintAngle::processEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::string aFilledAttributeName;

  std::shared_ptr<ModelAPI_EventReentrantMessage> aReentrantMessage =
      std::dynamic_pointer_cast<ModelAPI_EventReentrantMessage>(theMessage);
  if (aReentrantMessage.get()) {
    aFilledAttributeName = ENTITY_A();
    refattr(ENTITY_A())->setObject(aReentrantMessage->selectedObject());
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(SELECTED_FIRST_POINT_ID()))
        ->setValue(aReentrantMessage->clickedPoint());
  }
  return aFilledAttributeName;
}
// LCOV_EXCL_STOP

void SketchPlugin_ConstraintAngle::attributeChanged(const std::string& theID)
{
  if (myFlyoutUpdate)
    return;

  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData)
    return;

  if (theID == TYPE_ID())
    updateAngleValue();

  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_B());
  if (!aLineA || !aLineB)
    return;

  AttributeIntegerPtr aVersion = integer(VERSION_ID());
  if (!aVersion->isInitialized() || aVersion->value() < THE_VERSION_1)
    updateVersion();

  if (theID == ENTITY_A() || theID == ENTITY_B() ||
      theID == TYPE_ID() || theID == ANGLE_VALUE_ID()) {
    calculateAngle();
  } else if (theID == FLYOUT_VALUE_PNT()) {
    compute(theID);
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

  AttributeDoublePtr anAngleValueAttr = real(ANGLE_VALUE_ID());
  if (!anAngleValueAttr->isInitialized()) {
    std::shared_ptr<GeomAPI_Angle2d> anAng(
        new GeomAPI_Angle2d(aLine1, isReversed1, aLine2, isReversed2));
    anAngleValueAttr->setValue(getAngleForType(fabs(anAng->angleDegree())));
  }

  std::shared_ptr<GeomAPI_Angle2d> anAng(new GeomAPI_Angle2d(aLine1, false, aLine2, false));
  double anAngle = anAng->angleDegree();

  anAngle /= fabs(anAngle);
  anAngle *= getAngleForType(anAngleValueAttr->value(), isReversed1, isReversed2);

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

// Convert angle value from the DIRECT to any given type.
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

double SketchPlugin_ConstraintAngle::getAngleForType(double theAngle,
                                                     bool isReversed1,
                                                     bool isReversed2)
{
  double anAngle = angleForType(theAngle, integer(TYPE_ID())->value());
  if (isReversed1 != isReversed2)
    anAngle = 180.0 - anAngle;
  return anAngle;
}

#if !HAVE_WORKING_REGEX
static bool parseString(const std::wstring& theString, std::wostringstream* theResult)
{
  // skip leading spaces
  size_t aLength = theString.size();
  size_t aPos = theString.find_first_not_of(L' ');
  if (aPos == std::wstring::npos)
    return false;
  // first should be a value
  if (theString[aPos] == L'-' || theString[aPos] == L'+')
    theResult[1] << theString[aPos++];
  while (aPos < aLength && theString[aPos] >= L'0' && theString[aPos] <= L'9')
    theResult[1] << theString[aPos++];
  if (theString[aPos] != L' ') {
    if (theString[aPos] != L'.')
      return false;
    theResult[1] << theString[aPos++];
    while (aPos < aLength && theString[aPos] >= L'0' && theString[aPos] <= L'9')
      theResult[1] << theString[aPos++];
  }

  // next, find the sign
  aPos = theString.find_first_not_of(L' ', aPos);
  if (aPos == std::wstring::npos)
    return false;
  if (theString[aPos] == L'-' || theString[aPos] == L'+')
    theResult[2] << theString[aPos++];

  // a variable should be at the end
  aPos = theString.find_first_not_of(L' ', aPos);
  if (aPos == std::wstring::npos)
    return false;
  if (theString[aPos] != L'(' || theString.back() != L')')
    return false;
  theResult[3] << theString.substr(aPos + 1, aLength - aPos - 2);

  return true;
}
#endif

// Convert angle value or a text expression from one angle type to another
static void convertAngle(AttributeDoublePtr theAngle,
                         const int thePrevType, const int theNewType)
{
  if (theAngle->isInitialized()) {
    if (theAngle->text().empty()) {
      // calculate value related to the type twice:
      // the first time - to return to direct angle,
      // the second time - to apply new type
      double aValue = angleForType(theAngle->value(), thePrevType);
      aValue = angleForType(aValue, theNewType);
      theAngle->setValue(aValue);
    }
    else {
      // process the parametric value
      std::wstring anAngleText = theAngle->text();
#if HAVE_WORKING_REGEX
      std::wregex anAngleRegex(L"\\s*([-+]?[0-9]*\\.?[0-9]*)\\s*([-+])\\s*\\((.*)\\)$",
                               std::regex_constants::ECMAScript);
#endif

      double anAnglePrefix = 0.0;
      static const wchar_t aSignPrefix[2] = { L'-', L'+' };
      int aSignInd = 1;

#if HAVE_WORKING_REGEX
      std::wsmatch aResult;
      if (std::regex_search(anAngleText, aResult, anAngleRegex)) {
#else
      // workaround to support old versions of GCC (less than 4.9)
      std::wostringstream aResult[4];
      if (parseString(anAngleText, aResult)) {
#endif
        anAnglePrefix = std::atof(Locale::Convert::toString(aResult[1].str()).c_str());
        aSignInd = aResult[2].str()[0] == aSignPrefix[0] ? 0 : 1;
        anAngleText = aResult[3].str();
      }

      if (thePrevType != SketcherPrs_Tools::ANGLE_DIRECT)
        aSignInd = 1 - aSignInd;
      anAnglePrefix = angleForType(anAnglePrefix, thePrevType);

      if (theNewType != SketcherPrs_Tools::ANGLE_DIRECT)
        aSignInd = 1 - aSignInd;
      anAnglePrefix = angleForType(anAnglePrefix, theNewType);

      std::wostringstream aText;
      bool isPrintSign = true;
      if (fabs(anAnglePrefix) > tolerance)
        aText << anAnglePrefix;
      else
        isPrintSign = aSignInd == 0;
      if (isPrintSign)
        aText << L" " << aSignPrefix[aSignInd] << L" (";
      aText << anAngleText << (isPrintSign ? L")" : L"");
      theAngle->setText(aText.str());
    }
  }
}

void SketchPlugin_ConstraintAngle::updateAngleValue()
{
  AttributeIntegerPtr anAngleType = integer(TYPE_ID());
  AttributeIntegerPtr aPrevAngleType = integer(PREV_TYPE_ID());
  convertAngle(real(ANGLE_VALUE_ID()), aPrevAngleType->value(), anAngleType->value());
  aPrevAngleType->setValue(anAngleType->value());
}

static GeomPnt2dPtr lineBoundary(const FeaturePtr& theLine, const bool theReversed,
                                 const GeomPnt2dPtr& thePointToAvoid)
{
  GeomPnt2dPtr aPoint = SketcherPrs_Tools::getPoint(theLine.get(),
      theReversed ? SketchPlugin_Line::START_ID() : SketchPlugin_Line::END_ID());
  if (aPoint->distance(thePointToAvoid) < tolerance) {
    // extremity is equal to the intersection point,
    // thus recalculate it using another boundary point
    aPoint = SketcherPrs_Tools::getPoint(theLine.get(),
        theReversed ? SketchPlugin_Line::END_ID() : SketchPlugin_Line::START_ID());
    aPoint->setX(thePointToAvoid->x() * 2.0 - aPoint->x());
    aPoint->setY(thePointToAvoid->y() * 2.0 - aPoint->y());
  }
  return aPoint;
}

bool SketchPlugin_ConstraintAngle::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;
  if (!sketch())
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(attribute(theAttributeId));

  DataPtr aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, ENTITY_B());

  if ((aLineA.get() == NULL) || (aLineB.get() == NULL))
    return false;

  // Intersection of lines
  std::shared_ptr<GeomAPI_Pnt2d> anInter = intersect(aLineA, aLineB);
  if (!anInter)
    return false;

  bool isReversed1 = boolean(ANGLE_REVERSED_FIRST_LINE_ID())->value();
  bool isReversed2 = boolean(ANGLE_REVERSED_SECOND_LINE_ID())->value();

  int anAngleType = integer(TYPE_ID())->value();

  bool isSupplementary = anAngleType == (int)SketcherPrs_Tools::ANGLE_COMPLEMENTARY;

  // point on lines to compose an angle
  GeomPnt2dPtr aPointA = lineBoundary(aLineA, isReversed1 ^ isSupplementary, anInter);
  GeomPnt2dPtr aPointB = lineBoundary(aLineB, isReversed2, anInter);

  myFlyoutUpdate = true;
  if (aFlyOutAttr->isInitialized()) {
    std::shared_ptr<GeomAPI_XY> aFlyoutPoint = aFlyOutAttr->pnt()->xy();
    std::shared_ptr<GeomAPI_XY> anInterXY = anInter->xy();
    std::shared_ptr<GeomAPI_XY> aDirIF = aFlyoutPoint->decreased(anInterXY);
    std::shared_ptr<GeomAPI_XY> aDirIA = aPointA->xy()->decreased(anInterXY);
    std::shared_ptr<GeomAPI_XY> aDirIB = aPointB->xy()->decreased(anInterXY);
    double aSign = aDirIA->cross(aDirIB);
    aSign /= fabs(aSign);
    if (anAngleType == (int)SketcherPrs_Tools::ANGLE_BACKWARD)
      aSign *= -1.0;

    double cross1 = aSign * aDirIA->cross(aDirIF);
    if (cross1 < -tolerance)
      boolean(ANGLE_REVERSED_SECOND_LINE_ID())->setValue(!isReversed2);
    double cross2 = aSign * aDirIF->cross(aDirIB);
    if (cross2 < -tolerance)
      boolean(ANGLE_REVERSED_FIRST_LINE_ID())->setValue(!isReversed1);

    // the direction is reversed only once
    if ((cross1 + tolerance) * (cross2 + tolerance) < 0.0) {
      if (anAngleType == (int)SketcherPrs_Tools::ANGLE_BACKWARD) {
        convertAngle(real(ANGLE_VALUE_ID()), (int)SketcherPrs_Tools::ANGLE_BACKWARD,
                     (int)SketcherPrs_Tools::ANGLE_DIRECT);
      }
      convertAngle(real(ANGLE_VALUE_ID()), (int)SketcherPrs_Tools::ANGLE_DIRECT,
                   (int)SketcherPrs_Tools::ANGLE_COMPLEMENTARY);
      if (anAngleType == (int)SketcherPrs_Tools::ANGLE_BACKWARD) {
        convertAngle(real(ANGLE_VALUE_ID()), (int)SketcherPrs_Tools::ANGLE_DIRECT,
                     (int)SketcherPrs_Tools::ANGLE_BACKWARD);
      }
    }

    calculateAngle();
  }
  else {
    // default position of the presentation
    double aX = (aPointA->x() + aPointB->x() + anInter->x()) / 3.;
    double aY = (aPointA->y() + aPointB->y() + anInter->y()) / 3.;
    aFlyOutAttr->setValue(aX, aY);
  }
  myFlyoutUpdate = false;

  return true;
}

void SketchPlugin_ConstraintAngle::updateVersion()
{
  bool aWasBlocked = data()->blockSendAttributeUpdated(true);

  // Calculate angle value by the old algorithm and
  // update the corresponding attribute to meet the new requirements.
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(data(), ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(data(), ENTITY_B());

  GeomPnt2dPtr aStartA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndA = SketcherPrs_Tools::getPoint(aLineA.get(), SketchPlugin_Line::END_ID());
  GeomPnt2dPtr aStartB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::START_ID());
  GeomPnt2dPtr aEndB = SketcherPrs_Tools::getPoint(aLineB.get(), SketchPlugin_Line::END_ID());

  std::shared_ptr<GeomAPI_Angle2d> anAng;

  if (boolean(ANGLE_REVERSED_FIRST_LINE_ID())->isInitialized() &&
      boolean(ANGLE_REVERSED_SECOND_LINE_ID())->isInitialized()) {
    bool isReversed1 = boolean(ANGLE_REVERSED_FIRST_LINE_ID())->value();
    bool isReversed2 = boolean(ANGLE_REVERSED_SECOND_LINE_ID())->value();

    std::shared_ptr<GeomAPI_Lin2d> aLine1(new GeomAPI_Lin2d(aStartA, aEndA));
    std::shared_ptr<GeomAPI_Lin2d> aLine2(new GeomAPI_Lin2d(aStartB, aEndB));
    anAng.reset(new GeomAPI_Angle2d(aLine1, isReversed1, aLine2, isReversed2));
  }
  else {
    anAng.reset(new GeomAPI_Angle2d(aStartA, aEndA, aStartB, aEndB));

    bool isReversed1 = anAng->isReversed(0);
    bool isReversed2 = anAng->isReversed(1);

    boolean(ANGLE_REVERSED_FIRST_LINE_ID())->setValue(isReversed1);
    boolean(ANGLE_REVERSED_SECOND_LINE_ID())->setValue(isReversed2);
  }
  double anAngleValue = anAng->angleDegree();
  double aConstValue = real(ANGLE_VALUE_ID())->value();

  AttributeIntegerPtr aType = integer(TYPE_ID());
  switch ((SketcherPrs_Tools::AngleType)aType->value()) {
  case SketcherPrs_Tools::ANGLE_DIRECT:
    if (anAngleValue < 0.0 && aConstValue > 180.0)
      convertAngle(real(ANGLE_VALUE_ID()), SketcherPrs_Tools::ANGLE_BACKWARD,
                                           SketcherPrs_Tools::ANGLE_DIRECT);
    break;
  case SketcherPrs_Tools::ANGLE_BACKWARD:
    if (anAngleValue < 0.0 && aConstValue < 180.0)
      convertAngle(real(ANGLE_VALUE_ID()), SketcherPrs_Tools::ANGLE_DIRECT,
                                           SketcherPrs_Tools::ANGLE_BACKWARD);
    break;
  default:
    break;
  }
  data()->blockSendAttributeUpdated(aWasBlocked, false);
  integer(VERSION_ID())->setValue(THE_VERSION_1);
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
