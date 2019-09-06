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

#include <PlaneGCSSolver_FeatureBuilder.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_BooleanWrapper.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

static bool isAttributeApplicable(const std::string& theAttrName,
                                  const std::string& theOwnerName);

static EntityWrapperPtr createLine(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createCircle(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createArc(const AttributeEntityMap&    theAttributes,
                                  PlaneGCSSolver_Storage*      theStorage);
static EntityWrapperPtr createEllipse(const AttributeEntityMap& theAttributes);


PlaneGCSSolver_FeatureBuilder::PlaneGCSSolver_FeatureBuilder(
    PlaneGCSSolver_Storage* theStorage)
  : PlaneGCSSolver_AttributeBuilder(theStorage)
{
}

PlaneGCSSolver_FeatureBuilder::PlaneGCSSolver_FeatureBuilder(const StoragePtr& theStorage)
  : PlaneGCSSolver_AttributeBuilder(theStorage)
{
}

EntityWrapperPtr PlaneGCSSolver_FeatureBuilder::createAttribute(
    AttributePtr theAttribute)
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  EntityWrapperPtr anAttr;
  if (isAttributeApplicable(theAttribute->id(), anOwner->getKind()))
    anAttr = PlaneGCSSolver_AttributeBuilder::createAttribute(theAttribute);
  if (anAttr)
    myAttributes[theAttribute] = anAttr;
  return anAttr;
}

EntityWrapperPtr PlaneGCSSolver_FeatureBuilder::createFeature(FeaturePtr theFeature)
{
  EntityWrapperPtr aResult;
  if (myStorage)
    aResult = myStorage->entity(theFeature);
  if (aResult)
    return aResult;

  // Process SketchPlugin features only
  std::shared_ptr<SketchPlugin_Feature> aFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
  if (!aFeature)
    return aResult;

  // Verify the feature by its kind
  const std::string& aFeatureKind = aFeature->getKind();
  // Line
  if (aFeatureKind == SketchPlugin_Line::ID())
    aResult = createLine(myAttributes);
  // Circle
  else if (aFeatureKind == SketchPlugin_Circle::ID())
    aResult = createCircle(myAttributes);
  // Arc
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    aResult = createArc(myAttributes, myStorage);
  // Ellipse
  else if (aFeatureKind == SketchPlugin_Ellipse::ID())
    aResult = createEllipse(myAttributes);
  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
  else if (aFeatureKind == SketchPlugin_Point::ID() ||
           aFeatureKind == SketchPlugin_IntersectionPoint::ID()) {
    AttributeEntityMap::const_iterator anIt = myAttributes.begin();
    for (; anIt != myAttributes.end(); ++anIt)
      if (anIt->first->id() == SketchPlugin_Point::COORD_ID()) {
        aResult = anIt->second;
        break;
      }
  }

  if (aResult && !myStorage)
    aResult->setExternal(true);

  myAttributes.clear();
  return aResult;
}





// ==============   Auxiliary functions   =====================================
EntityWrapperPtr createLine(const AttributeEntityMap& theAttributes)
{
  std::shared_ptr<GCS::Line> aNewLine(new GCS::Line);

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (!aPoint)
      continue;

    if (anIt->first->id() == SketchPlugin_Line::START_ID())
      aNewLine->p1 = *(aPoint->point());
    else if (anIt->first->id() == SketchPlugin_Line::END_ID())
      aNewLine->p2 = *(aPoint->point());
  }

  return EntityWrapperPtr(new PlaneGCSSolver_EdgeWrapper(aNewLine));
}

EntityWrapperPtr createCircle(const AttributeEntityMap& theAttributes)
{
  std::shared_ptr<GCS::Circle> aNewCircle(new GCS::Circle);

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    if (anIt->first->id() == SketchPlugin_Circle::CENTER_ID()) {
      std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
      aNewCircle->center = *(aPoint->point());
    }
    else if(anIt->first->id() == SketchPlugin_Circle::RADIUS_ID()) {
      ScalarWrapperPtr aScalar =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      aNewCircle->rad = aScalar->scalar();
    }
  }

  return EntityWrapperPtr(new PlaneGCSSolver_EdgeWrapper(aNewCircle));
}

static double* createParameter(PlaneGCSSolver_Storage* theStorage)
{
  return theStorage ? theStorage->createParameter() : (new double(0));
}

EntityWrapperPtr createArc(const AttributeEntityMap&    theAttributes,
                           PlaneGCSSolver_Storage*      theStorage)
{
  std::shared_ptr<GCS::Arc> aNewArc(new GCS::Arc);
  BooleanWrapperPtr isReversed;

  // Base attributes of arc (center, start and end points)
  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (aPoint) {
      if (anIt->first->id() == SketchPlugin_Arc::CENTER_ID())
        aNewArc->center = *(aPoint->point());
      else if (anIt->first->id() == SketchPlugin_Arc::START_ID())
        aNewArc->start = *(aPoint->point());
      else if (anIt->first->id() == SketchPlugin_Arc::END_ID())
        aNewArc->end = *(aPoint->point());
    }
    else {
      // reversed flag
      isReversed = std::dynamic_pointer_cast<PlaneGCSSolver_BooleanWrapper>(anIt->second);
    }
  }

  // Additional atrtributes of arc necessary for PlaneGCS solver
  // (start and end angles, radius)
  aNewArc->startAngle = createParameter(theStorage);
  aNewArc->endAngle   = createParameter(theStorage);
  aNewArc->rad        = createParameter(theStorage);

  static std::shared_ptr<GeomAPI_Dir2d> OX(new GeomAPI_Dir2d(1.0, 0.0));
  std::shared_ptr<GeomAPI_Pnt2d> aCenter(
      new GeomAPI_Pnt2d(*aNewArc->center.x, *aNewArc->center.y));
  std::shared_ptr<GeomAPI_Pnt2d> aStart(
      new GeomAPI_Pnt2d(*aNewArc->start.x, *aNewArc->start.y));

  *aNewArc->rad = aStart->distance(aCenter);

  std::shared_ptr<GeomAPI_Dir2d> aDir(new GeomAPI_Dir2d(aStart->xy()->decreased(aCenter->xy())));
  *aNewArc->startAngle = OX->angle(aDir);

  aDir = std::shared_ptr<GeomAPI_Dir2d>(
      new GeomAPI_Dir2d((*aNewArc->end.x) - aCenter->x(), (*aNewArc->end.y) - aCenter->y()));
  *aNewArc->endAngle = OX->angle(aDir);

  EdgeWrapperPtr anArcWrapper(new PlaneGCSSolver_EdgeWrapper(aNewArc));
  anArcWrapper->setReversed(isReversed);
  return anArcWrapper;
}

EntityWrapperPtr createEllipse(const AttributeEntityMap& theAttributes)
{
  std::shared_ptr<GCS::Ellipse> aNewEllipse(new GCS::Ellipse);

  std::map<std::string, EntityWrapperPtr> anAdditionalAttributes;

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (aPoint) {
      if (anIt->first->id() == SketchPlugin_Ellipse::CENTER_ID())
        aNewEllipse->center = *(aPoint->point());
      else if (anIt->first->id() == SketchPlugin_Ellipse::FIRST_FOCUS_ID())
        aNewEllipse->focus1 = *(aPoint->point());
      else
        anAdditionalAttributes[anIt->first->id()] = anIt->second;
    }
    else if (anIt->first->id() == SketchPlugin_Ellipse::MINOR_RADIUS_ID()) {
      ScalarWrapperPtr aScalar =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      aNewEllipse->radmin = aScalar->scalar();
    }
    else
      anAdditionalAttributes[anIt->first->id()] = anIt->second;
  }

  EntityWrapperPtr anEllipseWrapper(new PlaneGCSSolver_EdgeWrapper(aNewEllipse));
  anEllipseWrapper->setAdditionalAttributes(anAdditionalAttributes);
  return anEllipseWrapper;
}

bool isAttributeApplicable(const std::string& theAttrName, const std::string& theOwnerName)
{
  if (theOwnerName == SketchPlugin_Arc::ID()) {
    return theAttrName == SketchPlugin_Arc::CENTER_ID() ||
           theAttrName == SketchPlugin_Arc::START_ID() ||
           theAttrName == SketchPlugin_Arc::END_ID() ||
           theAttrName == SketchPlugin_Arc::REVERSED_ID();
  }
  else if (theOwnerName == SketchPlugin_Circle::ID()) {
    return theAttrName == SketchPlugin_Circle::CENTER_ID() ||
           theAttrName == SketchPlugin_Circle::RADIUS_ID();
  }
  else if (theOwnerName == SketchPlugin_Line::ID()) {
    return theAttrName == SketchPlugin_Line::START_ID() ||
           theAttrName == SketchPlugin_Line::END_ID();
  }
  else if (theOwnerName == SketchPlugin_Ellipse::ID()) {
    return theAttrName == SketchPlugin_Ellipse::CENTER_ID() ||
           theAttrName == SketchPlugin_Ellipse::FIRST_FOCUS_ID() ||
           theAttrName == SketchPlugin_Ellipse::SECOND_FOCUS_ID() ||
           theAttrName == SketchPlugin_Ellipse::MAJOR_AXIS_START_ID() ||
           theAttrName == SketchPlugin_Ellipse::MAJOR_AXIS_END_ID() ||
           theAttrName == SketchPlugin_Ellipse::MINOR_AXIS_START_ID() ||
           theAttrName == SketchPlugin_Ellipse::MINOR_AXIS_END_ID() ||
           theAttrName == SketchPlugin_Ellipse::MAJOR_RADIUS_ID() ||
           theAttrName == SketchPlugin_Ellipse::MINOR_RADIUS_ID();
  }

  // suppose that all remaining features are points
  return theAttrName == SketchPlugin_Point::COORD_ID();
}
