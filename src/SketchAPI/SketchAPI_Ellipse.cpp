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

#include "SketchAPI_Ellipse.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

static const std::string AUXILIARY_VALUE = "aux";
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
                                     const std::string& theExternalName)
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

void SketchAPI_Ellipse::setByExternalName(const std::string & theExternalName)
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

ModelHighAPI_Selection SketchAPI_Ellipse::majorAxis() const
{
  return ModelHighAPI_Selection();
}

ModelHighAPI_Selection SketchAPI_Ellipse::minorAxis() const
{
  return ModelHighAPI_Selection();
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

static FeaturePtr createPoint(const CompositeFeaturePtr& theSketch,
                              const AttributePtr& theCoincident,
                              const std::string& theAuxOrName)
{
  AttributePoint2DPtr anElPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theCoincident);

  FeaturePtr aPointFeature = theSketch->addFeature(SketchPlugin_Point::ID());
  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(anElPoint->x(), anElPoint->y());
  aPointFeature->execute();

  if (theAuxOrName == AUXILIARY_VALUE)
    aPointFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  else if (!theAuxOrName.empty()) {
    aPointFeature->data()->setName(theAuxOrName);
    aPointFeature->lastResult()->data()->setName(theAuxOrName);
  }

  createInternalConstraint(theSketch, anElPoint, aCoord);

  return aPointFeature;
}

static FeaturePtr createAxis(const CompositeFeaturePtr& theSketch,
                             const AttributePtr& theCoincidentStart,
                             const AttributePtr& theCoincidentEnd,
                             const std::string& theAuxOrName)
{
  AttributePoint2DPtr aStartPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theCoincidentStart);
  AttributePoint2DPtr aEndPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theCoincidentEnd);

  FeaturePtr aLineFeature = theSketch->addFeature(SketchPlugin_Line::ID());
  AttributePoint2DPtr aLineStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::START_ID()));
  aLineStart->setValue(aStartPoint->x(), aStartPoint->y());
  AttributePoint2DPtr aLineEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::END_ID()));
  aLineEnd->setValue(aEndPoint->x(), aEndPoint->y());
  aLineFeature->execute();

  if (theAuxOrName == AUXILIARY_VALUE)
    aLineFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);
  else if (!theAuxOrName.empty()) {
    aLineFeature->data()->setName(theAuxOrName);
    aLineFeature->lastResult()->data()->setName(theAuxOrName);
  }

  createInternalConstraint(theSketch, aStartPoint, aLineStart);
  createInternalConstraint(theSketch, aEndPoint, aLineEnd);

  return aLineFeature;
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Ellipse::construction(
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
  CompositeFeaturePtr aSketch = sketchForFeature(anEllipse);

  std::list<FeaturePtr> anEntities;
  if (!center.empty()) {
    AttributePtr aCenterAttr = anEllipse->attribute(SketchPlugin_Ellipse::CENTER_ID());
    anEntities.push_back(createPoint(aSketch, aCenterAttr, center));
  }
  if (!firstFocus.empty()) {
    AttributePtr aFocusAttr = anEllipse->attribute(SketchPlugin_Ellipse::FIRST_FOCUS_ID());
    anEntities.push_back(createPoint(aSketch, aFocusAttr, firstFocus));
  }
  if (!secondFocus.empty()) {
    AttributePtr aFocusAttr = anEllipse->attribute(SketchPlugin_Ellipse::SECOND_FOCUS_ID());
    anEntities.push_back(createPoint(aSketch, aFocusAttr, secondFocus));
  }
  if (!majorAxisStart.empty()) {
    AttributePtr aStartAttr = anEllipse->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID());
    anEntities.push_back(createPoint(aSketch, aStartAttr, majorAxisStart));
  }
  if (!majorAxisEnd.empty()) {
    AttributePtr aEndAttr = anEllipse->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID());
    anEntities.push_back(createPoint(aSketch, aEndAttr, majorAxisEnd));
  }
  if (!minorAxisStart.empty()) {
    AttributePtr aStartAttr = anEllipse->attribute(SketchPlugin_Ellipse::MINOR_AXIS_START_ID());
    anEntities.push_back(createPoint(aSketch, aStartAttr, minorAxisStart));
  }
  if (!minorAxisEnd.empty()) {
    AttributePtr aEndAttr = anEllipse->attribute(SketchPlugin_Ellipse::MINOR_AXIS_END_ID());
    anEntities.push_back(createPoint(aSketch, aEndAttr, minorAxisEnd));
  }
  if (!majorAxis.empty()) {
    AttributePtr aStartAttr = anEllipse->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID());
    AttributePtr aEndAttr = anEllipse->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID());
    anEntities.push_back(createAxis(aSketch, aStartAttr, aEndAttr, majorAxis));
  }
  if (!minorAxis.empty()) {
    AttributePtr aStartAttr = anEllipse->attribute(SketchPlugin_Ellipse::MINOR_AXIS_START_ID());
    AttributePtr aEndAttr = anEllipse->attribute(SketchPlugin_Ellipse::MINOR_AXIS_END_ID());
    anEntities.push_back(createAxis(aSketch, aStartAttr, aEndAttr, minorAxis));
  }

  return SketchAPI_SketchEntity::wrap(anEntities);
}

static void ellipseAttributeAndAuxiliaryFeature(
    const FeaturePtr& theInternalConstraint,
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
    if (anAttrID == SketchPlugin_Ellipse::MAJOR_AXIS_START_ID() ||
        anAttrID == SketchPlugin_Ellipse::MAJOR_AXIS_END_ID())
      theAttrToFeature[MAJOR_AXIS_ID] = anAuxFeature;
    else if (anAttrID == SketchPlugin_Ellipse::MINOR_AXIS_START_ID() ||
             anAttrID == SketchPlugin_Ellipse::MINOR_AXIS_END_ID())
      theAttrToFeature[MINOR_AXIS_ID] = anAuxFeature;
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
  const std::set<AttributePtr>& aRefs = aBase->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
       aRefIt != aRefs.end(); ++aRefIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
    if (anOwner->getKind() != SketchPlugin_ConstraintCoincidenceInternal::ID())
      continue; // process internal constraints only
    ellipseAttributeAndAuxiliaryFeature(anOwner, anAuxFeatures);
  }
  if (!anAuxFeatures.empty()) {
    typedef std::pair<std::string, std::string> PairOfStrings;
    static PairOfStrings anAttributes[] = {
        PairOfStrings(SketchPlugin_Ellipse::CENTER_ID(), "center"),
        PairOfStrings(SketchPlugin_Ellipse::FIRST_FOCUS_ID(), "firstFocus"),
        PairOfStrings(SketchPlugin_Ellipse::SECOND_FOCUS_ID(), "secondFocus"),
        PairOfStrings(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID(), "majorAxisStart"),
        PairOfStrings(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID(), "majorAxisEnd"),
        PairOfStrings(SketchPlugin_Ellipse::MINOR_AXIS_START_ID(), "minorAxisStart"),
        PairOfStrings(SketchPlugin_Ellipse::MINOR_AXIS_END_ID(), "minorAxisEnd"),
        PairOfStrings(MAJOR_AXIS_ID, MAJOR_AXIS_ID),
        PairOfStrings(MINOR_AXIS_ID, MINOR_AXIS_ID)
    };

    theDumper << "[";
    bool isFirst = true;
    for (PairOfStrings* anIt = std::begin(anAttributes);
         anIt != std::end(anAttributes); ++anIt) {
      std::map<std::string, FeaturePtr>::iterator aFound = anAuxFeatures.find(anIt->first);
      if (aFound == anAuxFeatures.end())
        continue;
      if (!isFirst)
        theDumper << ", ";
      theDumper << theDumper.name(aFound->second, false);
      theDumper.doNotDumpFeature(aFound->second);
      isFirst = false;
    }
    theDumper << "] = " << theDumper.name(aBase) << ".construction(";
    isFirst = true;
    for (PairOfStrings* anIt = std::begin(anAttributes);
         anIt != std::end(anAttributes); ++anIt) {
      std::map<std::string, FeaturePtr>::iterator aFound = anAuxFeatures.find(anIt->first);
      if (aFound == anAuxFeatures.end())
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
}
