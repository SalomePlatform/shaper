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

#include "SketchAPI_Ellipse.h"

#include <GeomAPI_Pnt2d.h>

#include <Locale_Convert.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <ModelAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

static const std::wstring AUXILIARY_VALUE = L"aux";
static const std::string MAJOR_AXIS_ID = "majorAxis";
static const std::string MINOR_AXIS_ID = "minorAxis";

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature> & theFeature)
  : SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     double theCenterX, double theCenterY,
                                     double theFocusX, double theFocusY,
                                     double theMinorRadius)
  : SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterFocusAndRadius(theCenterX, theCenterY, theFocusX, theFocusY, theMinorRadius);
  }
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                     double theMinorRadius)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterFocusAndRadius(theCenter, theFocus, theMinorRadius);
  }
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const ModelHighAPI_Selection& theExternal)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

SketchAPI_Ellipse::SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::wstring& theExternalName)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_Ellipse::~SketchAPI_Ellipse()
{
}

void SketchAPI_Ellipse::setByCenterFocusAndRadius(double theCenterX, double theCenterY,
                                                  double theFocusX, double theFocusY,
                                                  double theMinorRadius)
{
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(firstFocus(), theFocusX, theFocusY);
  fillAttribute(theMinorRadius, myminorRadius);

  execute();
}

void SketchAPI_Ellipse::setByCenterFocusAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                                  const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                                  double theMinorRadius)
{
  fillAttribute(theCenter, mycenter);
  fillAttribute(theFocus, myfirstFocus);
  fillAttribute(theMinorRadius, myminorRadius);

  execute();
}

void SketchAPI_Ellipse::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());
  execute();
}

void SketchAPI_Ellipse::setByExternalName(const std::wstring & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());
  execute();
}

void SketchAPI_Ellipse::setCenter(double theX, double theY)
{
  fillAttribute(center(), theX, theY);
  execute();
}

void SketchAPI_Ellipse::setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter)
{
  fillAttribute(theCenter, mycenter);
  execute();
}

void SketchAPI_Ellipse::setFocus(double theX, double theY)
{
  fillAttribute(firstFocus(), theX, theY);
  execute();
}

void SketchAPI_Ellipse::setFocus(const std::shared_ptr<GeomAPI_Pnt2d> & theFocus)
{
  fillAttribute(theFocus, myfirstFocus);
  execute();
}

void SketchAPI_Ellipse::setMinorRadius(double theMinorRadius)
{
  fillAttribute(theMinorRadius, myminorRadius);
  execute();
}

static const std::list<PairOfStrings>& ellipseAttrAndDumpNames()
{
  static std::list<PairOfStrings> anAttributes;
  if (anAttributes.empty()) {
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::CENTER_ID(), "center"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::FIRST_FOCUS_ID(), "firstFocus"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::SECOND_FOCUS_ID(), "secondFocus"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID(), "majorAxisStart"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID(), "majorAxisEnd"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::MINOR_AXIS_START_ID(), "minorAxisStart"));
    anAttributes.push_back(
        PairOfStrings(SketchPlugin_Ellipse::MINOR_AXIS_END_ID(), "minorAxisEnd"));
  }
  return anAttributes;
}

static CompositeFeaturePtr sketchForFeature(FeaturePtr theFeature)
{
  const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt)
    if ((*anIt)->id() == SketchPlugin_Sketch::FEATURES_ID())
      return std::dynamic_pointer_cast<ModelAPI_CompositeFeature>((*anIt)->owner());
  return CompositeFeaturePtr();
}

static void createInternalConstraint(const CompositeFeaturePtr& theSketch,
                                     const AttributePoint2DPtr& thePoint1,
                                     const AttributePoint2DPtr& thePoint2)
{
  FeaturePtr aConstraint = theSketch->addFeature(SketchPlugin_ConstraintCoincidenceInternal::ID());
  aConstraint->refattr(SketchPlugin_Constraint::ENTITY_A())->setAttr(thePoint1);
  aConstraint->refattr(SketchPlugin_Constraint::ENTITY_B())->setAttr(thePoint2);
  aConstraint->execute();
}

static void createPoint(const CompositeFeaturePtr& theSketch,
                        const FeaturePtr& theEllipse,
                        const std::string& theCoincident,
                        const std::wstring& theAuxOrName,
                        std::list<FeaturePtr>& theEntities)
{
  if (theAuxOrName.empty())
    return;

  AttributePoint2DPtr anElPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theEllipse->attribute(theCoincident));

  FeaturePtr aPointFeature = theSketch->addFeature(SketchPlugin_Point::ID());
  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(anElPoint->x(), anElPoint->y());
  aPointFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theEllipse);
  aPointFeature->execute();

  std::wstring aName = theEllipse->name() + L"_" + Locale::Convert::toWString(theCoincident);
  aPointFeature->data()->setName(aName);
  aPointFeature->lastResult()->data()->setName(aName);

  if (theAuxOrName == AUXILIARY_VALUE)
    aPointFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  else if (!theAuxOrName.empty()) {
    aPointFeature->data()->setName(theAuxOrName);
    aPointFeature->lastResult()->data()->setName(theAuxOrName);
  }

  createInternalConstraint(theSketch, anElPoint, aCoord);

  theEntities.push_back(aPointFeature);
}

static void createAxis(const CompositeFeaturePtr& theSketch,
                       const FeaturePtr& theEllipse,
                       const std::string& theCoincidentStart,
                       const std::string& theCoincidentEnd,
                       const std::wstring& theAuxOrName,
                       std::list<FeaturePtr>& theEntities)
{
  if (theAuxOrName.empty())
    return;

  AttributePoint2DPtr aStartPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEllipse->attribute(theCoincidentStart));
  AttributePoint2DPtr aEndPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEllipse->attribute(theCoincidentEnd));

  FeaturePtr aLineFeature = theSketch->addFeature(SketchPlugin_Line::ID());
  AttributePoint2DPtr aLineStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::START_ID()));
  aLineStart->setValue(aStartPoint->x(), aStartPoint->y());
  AttributePoint2DPtr aLineEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::END_ID()));
  aLineEnd->setValue(aEndPoint->x(), aEndPoint->y());
  aLineFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theEllipse);
  aLineFeature->execute();

  std::wstring aName = theEllipse->name() + L"_" +
      (theCoincidentStart == SketchPlugin_Ellipse::MAJOR_AXIS_START_ID() ?
       L"major_axis" : L"minor_axis");
  aLineFeature->data()->setName(aName);
  aLineFeature->lastResult()->data()->setName(aName);

  if (theAuxOrName == AUXILIARY_VALUE)
    aLineFeature->boolean(SketchPlugin_Line::AUXILIARY_ID())->setValue(true);
  else if (!theAuxOrName.empty()) {
    aLineFeature->data()->setName(theAuxOrName);
    aLineFeature->lastResult()->data()->setName(theAuxOrName);
  }

  createInternalConstraint(theSketch, aStartPoint, aLineStart);
  createInternalConstraint(theSketch, aEndPoint, aLineEnd);

  theEntities.push_back(aLineFeature);
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Ellipse::construction(
    const std::wstring& center,
    const std::wstring& firstFocus,
    const std::wstring& secondFocus,
    const std::wstring& majorAxisStart,
    const std::wstring& majorAxisEnd,
    const std::wstring& minorAxisStart,
    const std::wstring& minorAxisEnd,
    const std::wstring& majorAxis,
    const std::wstring& minorAxis) const
{
  FeaturePtr anEllipse = feature();

  std::list<PairOfStrings> anAttributes = ellipseAttrAndDumpNames();
  // append start and end attributes for axes
  anAttributes.push_back(PairOfStrings(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID(),
                                       SketchPlugin_Ellipse::MAJOR_AXIS_END_ID()));
  anAttributes.push_back(PairOfStrings(SketchPlugin_Ellipse::MINOR_AXIS_START_ID(),
                                       SketchPlugin_Ellipse::MINOR_AXIS_END_ID()));

  return buildConstructionEntities(anEllipse, anAttributes, center, firstFocus, secondFocus,
            majorAxisStart, majorAxisEnd, minorAxisStart, minorAxisEnd, majorAxis, minorAxis);
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Ellipse::buildConstructionEntities(
      const FeaturePtr& theEllipse,
      const std::list<PairOfStrings>& theAttributes,
      const std::wstring& theCenter,
      const std::wstring& theFirstFocus,
      const std::wstring& theSecondFocus,
      const std::wstring& theMajorAxisStart,
      const std::wstring& theMajorAxisEnd,
      const std::wstring& theMinorAxisStart,
      const std::wstring& theMinorAxisEnd,
      const std::wstring& theMajorAxis,
      const std::wstring& theMinorAxis)
{
  CompositeFeaturePtr aSketch = sketchForFeature(theEllipse);

  std::list<FeaturePtr> anEntities;
  std::list<PairOfStrings>::const_iterator anAttrIt = theAttributes.begin();
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theCenter, anEntities);
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theFirstFocus, anEntities);
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theSecondFocus, anEntities);
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theMajorAxisStart, anEntities);
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theMajorAxisEnd, anEntities);
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theMinorAxisStart, anEntities);
  createPoint(aSketch, theEllipse, (anAttrIt++)->first, theMinorAxisEnd, anEntities);

  createAxis(aSketch, theEllipse, anAttrIt->first, anAttrIt->second, theMajorAxis, anEntities);
  ++anAttrIt;
  createAxis(aSketch, theEllipse, anAttrIt->first, anAttrIt->second, theMinorAxis, anEntities);

  return SketchAPI_SketchEntity::wrap(anEntities);
}

static void ellipseAttributeAndAuxiliaryFeature(
    const FeaturePtr& theInternalConstraint,
    const std::pair<std::string, std::string>& theMajorAxisStartEnd,
    const std::pair<std::string, std::string>& theMinorAxisStartEnd,
    std::map<std::string, FeaturePtr>& theAttrToFeature)
{
  AttributeRefAttrPtr aRefAttrA =
      theInternalConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  AttributeRefAttrPtr aRefAttrB =
      theInternalConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());
  // the first point is usually an ellipse attribute
  // and the second point is an attribute of the auxiliary feature
  ObjectPtr anAuxObject;
  if (aRefAttrB->isObject())
    anAuxObject = aRefAttrB->object();
  else
    anAuxObject = aRefAttrB->attr()->owner();

  FeaturePtr anAuxFeature = ModelAPI_Feature::feature(anAuxObject);
  if (anAuxFeature->getKind() == SketchPlugin_Point::ID())
    theAttrToFeature[aRefAttrA->attr()->id()] = anAuxFeature;
  else {
    const std::string& anAttrID = aRefAttrA->attr()->id();
    if (anAttrID == theMajorAxisStartEnd.first || anAttrID == theMajorAxisStartEnd.second)
      theAttrToFeature[MAJOR_AXIS_ID] = anAuxFeature;
    else if (anAttrID == theMinorAxisStartEnd.first || anAttrID == theMinorAxisStartEnd.second)
      theAttrToFeature[MINOR_AXIS_ID] = anAuxFeature;
  }
}

void SketchAPI_Ellipse::collectAuxiliaryFeatures(
    FeaturePtr theEllipse,
    const std::pair<std::string, std::string>& theMajorAxis,
    const std::pair<std::string, std::string>& theMinorAxis,
    std::map<std::string, FeaturePtr>& theAttrToFeature)
{
  const std::set<AttributePtr>& aRefs = theEllipse->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
       aRefIt != aRefs.end(); ++aRefIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
    if (anOwner->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID()) {
      // process internal constraints only
      ellipseAttributeAndAuxiliaryFeature(anOwner, theMajorAxis, theMinorAxis, theAttrToFeature);
    }
  }
}

void SketchAPI_Ellipse::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // circle is external
    theDumper << aBase << " = " << aSketchName << ".addEllipse(" << anExternal << ")" << std::endl;
  } else {
    // ellipse given by center, focus and radius
    theDumper << aBase << " = " << aSketchName << ".addEllipse("
              << center() << ", " << firstFocus() << ", " << minorRadius() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);

  // dump auxiliary features produced by ellipse
  std::map<std::string, FeaturePtr> anAuxFeatures;
  static const std::pair<std::string, std::string> aMajorAxis(
      SketchPlugin_Ellipse::MAJOR_AXIS_START_ID(),
      SketchPlugin_Ellipse::MAJOR_AXIS_END_ID());
  static const std::pair<std::string, std::string> aMinorAxis(
      SketchPlugin_Ellipse::MINOR_AXIS_START_ID(),
      SketchPlugin_Ellipse::MINOR_AXIS_END_ID());
  collectAuxiliaryFeatures(aBase, aMajorAxis, aMinorAxis, anAuxFeatures);

  if (!anAuxFeatures.empty()) {
    // a list of attributes to write features in special order
    const std::list<PairOfStrings>& anAttributes = ellipseAttrAndDumpNames();
    dumpConstructionEntities(theDumper, aBase, anAttributes, anAuxFeatures);
  }
}

void SketchAPI_Ellipse::dumpConstructionEntities(
    ModelHighAPI_Dumper& theDumper,
    const FeaturePtr& theEllipse,
    const std::list<PairOfStrings>& theAttributes,
    const std::map<std::string, FeaturePtr>& theAuxFeatures)
{
  std::list<PairOfStrings> anAttributes = theAttributes;
  // append axes
  anAttributes.push_back(PairOfStrings(MAJOR_AXIS_ID, MAJOR_AXIS_ID));
  anAttributes.push_back(PairOfStrings(MINOR_AXIS_ID, MINOR_AXIS_ID));

  theDumper << "[";
  bool isFirst = true;
  for (std::list<PairOfStrings>::iterator anIt = anAttributes.begin();
        anIt != anAttributes.end(); ++anIt) {
    std::map<std::string, FeaturePtr>::const_iterator aFound = theAuxFeatures.find(anIt->first);
    if (aFound == theAuxFeatures.end())
      continue;
    if (!isFirst)
      theDumper << ", ";
    theDumper << theDumper.name(aFound->second, false);
    theDumper.doNotDumpFeature(aFound->second);
    isFirst = false;
  }
  theDumper << "] = " << theDumper.name(theEllipse) << ".construction(";
  isFirst = true;
  for (std::list<PairOfStrings>::iterator anIt = anAttributes.begin();
        anIt != anAttributes.end(); ++anIt) {
    std::map<std::string, FeaturePtr>::const_iterator aFound = theAuxFeatures.find(anIt->first);
    if (aFound == theAuxFeatures.end())
      continue;
    if (!isFirst)
      theDumper << ", ";
    isFirst = false;
    theDumper << anIt->second << " = \"";
    if (aFound->second->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value())
      theDumper << AUXILIARY_VALUE;
    else
      theDumper << aFound->second->name();
    theDumper << "\"";
  }
  theDumper << ")" << std::endl;
}
