// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchAPI_EllipticArc.h"
#include "SketchAPI_Ellipse.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>


SketchAPI_EllipticArc::SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature> & theFeature)
  : SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_EllipticArc::SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             double theCenterX, double theCenterY,
                                             double theFocusX, double theFocusY,
                                             double theStartX, double theStartY,
                                             double theEndX, double theEndY,
                                             bool theInversed)
  : SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterFocusAndPoints(theCenterX, theCenterY, theFocusX, theFocusY,
                              theStartX, theStartY, theEndX, theEndY, theInversed);
  }
}

SketchAPI_EllipticArc::SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                             bool theInversed)
  : SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterFocusAndPoints(theCenter, theFocus, theStart, theEnd, theInversed);
  }
}

SketchAPI_EllipticArc::SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Selection& theExternal)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

SketchAPI_EllipticArc::SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::string& theExternalName)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_EllipticArc::~SketchAPI_EllipticArc()
{
}

void SketchAPI_EllipticArc::setByCenterFocusAndPoints(double theCenterX, double theCenterY,
                                                      double theFocusX, double theFocusY,
                                                      double theStartX, double theStartY,
                                                      double theEndX, double theEndY,
                                                      bool theInversed)
{
  // the order of attribute initialization is reversed to avoid odd recalculation of an elliptic arc
  fillAttribute(theInversed, reversed());
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(firstFocus(), theFocusX, theFocusY);
  fillAttribute(center(), theCenterX, theCenterY);

  execute();
}

void SketchAPI_EllipticArc::setByCenterFocusAndPoints(
    const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
    const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
    const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
    const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
    bool theInversed)
{
  // the order of attribute initialization is reversed to avoid odd recalculation of an elliptic arc
  fillAttribute(theInversed, reversed());
  fillAttribute(theEnd, endPoint());
  fillAttribute(theStart, startPoint());
  fillAttribute(theFocus, firstFocus());
  fillAttribute(theCenter, center());

  execute();
}

void SketchAPI_EllipticArc::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());
  execute();
}

void SketchAPI_EllipticArc::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());
  execute();
}

void SketchAPI_EllipticArc::setCenter(double theX, double theY)
{
  fillAttribute(center(), theX, theY);
  execute();
}

void SketchAPI_EllipticArc::setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter)
{
  fillAttribute(theCenter, mycenter);
  execute();
}

void SketchAPI_EllipticArc::setFocus(double theX, double theY)
{
  fillAttribute(firstFocus(), theX, theY);
  execute();
}

void SketchAPI_EllipticArc::setFocus(const std::shared_ptr<GeomAPI_Pnt2d> & theFocus)
{
  fillAttribute(theFocus, myfirstFocus);
  execute();
}

static const std::list<PairOfStrings>& ellipticArcAttrAndDumpNames()
{
  static std::list<PairOfStrings> anAttributes;
  if (anAttributes.empty()) {
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::CENTER_ID(), "center"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::FIRST_FOCUS_ID(), "firstFocus"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::SECOND_FOCUS_ID(), "secondFocus"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID(), "majorAxisStart"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID(), "majorAxisEnd"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::MINOR_AXIS_START_ID(), "minorAxisStart"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_EllipticArc::MINOR_AXIS_END_ID(), "minorAxisEnd"));
  }
  return anAttributes;
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_EllipticArc::construction(
    const std::string& center,
    const std::string& firstFocus,
    const std::string& secondFocus,
    const std::string& majorAxisStart,
    const std::string& majorAxisEnd,
    const std::string& minorAxisStart,
    const std::string& minorAxisEnd,
    const std::string& majorAxis,
    const std::string& minorAxis) const
{
  FeaturePtr anEllipse = feature();

  std::list<PairOfStrings> anAttributes = ellipticArcAttrAndDumpNames();
  // append start and end attributes for axes
  anAttributes.push_back(PairOfStrings(SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID(),
                                       SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID()));
  anAttributes.push_back(PairOfStrings(SketchPlugin_EllipticArc::MINOR_AXIS_START_ID(),
                                       SketchPlugin_EllipticArc::MINOR_AXIS_END_ID()));

  return SketchAPI_Ellipse::buildConstructionEntities(anEllipse, anAttributes,
                                                      center, firstFocus, secondFocus,
                                                      majorAxisStart, majorAxisEnd,
                                                      minorAxisStart, minorAxisEnd,
                                                      majorAxis, minorAxis);
}

void SketchAPI_EllipticArc::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // circle is external
    theDumper << aBase << " = " << aSketchName << ".addEllipticArc("
              << anExternal << ")" << std::endl;
  } else {
    // ellipse given by center, focus and radius
    theDumper << aBase << " = " << aSketchName << ".addEllipticArc("
              << center() << ", " << firstFocus() << ", "
              << startPoint() << ", " << endPoint() << ", "
              << reversed() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);

  // dump auxiliary features produced by elliptic arc
  std::map<std::string, FeaturePtr> anAuxFeatures;
  static const std::pair<std::string, std::string> aMajorAxis(
      SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID(),
      SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID());
  static const std::pair<std::string, std::string> aMinorAxis(
      SketchPlugin_EllipticArc::MINOR_AXIS_START_ID(),
      SketchPlugin_EllipticArc::MINOR_AXIS_END_ID());
  SketchAPI_Ellipse::collectAuxiliaryFeatures(aBase, aMajorAxis, aMinorAxis, anAuxFeatures);

  if (!anAuxFeatures.empty()) {
    // a list of attributes to write features in special order
    static std::list<PairOfStrings> anAttributes = ellipticArcAttrAndDumpNames();
    SketchAPI_Ellipse::dumpConstructionEntities(theDumper, aBase, anAttributes, anAuxFeatures);
  }
}
