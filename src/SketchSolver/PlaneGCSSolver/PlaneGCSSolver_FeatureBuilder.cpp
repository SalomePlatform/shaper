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
#include <PlaneGCSSolver_BooleanWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_GeoExtensions.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_PointArrayWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ScalarArrayWrapper.h>
#include <PlaneGCSSolver_Tools.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>


static EntityWrapperPtr createLine(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createCircle(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createArc(const AttributeEntityMap&    theAttributes,
                                  PlaneGCSSolver_Storage*      theStorage);
static EntityWrapperPtr createEllipse(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createEllipticArc(const AttributeEntityMap& theAttributes,
                                          PlaneGCSSolver_Storage*   theStorage);
template <typename TYPE>
static EntityWrapperPtr createBSpline(const AttributeEntityMap& theAttributes);


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
  if (PlaneGCSSolver_Tools::isAttributeApplicable(theAttribute->id(), anOwner->getKind()))
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
  // Arc of ellipse
  else if (aFeatureKind == SketchPlugin_EllipticArc::ID())
    aResult = createEllipticArc(myAttributes, myStorage);
  // B-spline curve
  else if (aFeatureKind == SketchPlugin_BSpline::ID())
    aResult = createBSpline<SketchPlugin_BSpline>(myAttributes);
  else if (aFeatureKind == SketchPlugin_BSplinePeriodic::ID())
    aResult = createBSpline<SketchPlugin_BSplinePeriodic>(myAttributes);
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

  // Additional attributes of arc necessary for PlaneGCS solver
  // (start and end angles, radius)
  aNewArc->startAngle = createParameter(theStorage);
  aNewArc->endAngle   = createParameter(theStorage);
  aNewArc->rad        = createParameter(theStorage);

  EdgeWrapperPtr anArcWrapper(new PlaneGCSSolver_EdgeWrapper(aNewArc));
  anArcWrapper->setReversed(isReversed);
  PlaneGCSSolver_Tools::recalculateArcParameters(anArcWrapper);

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

EntityWrapperPtr createEllipticArc(const AttributeEntityMap& theAttributes,
                                   PlaneGCSSolver_Storage*   theStorage)
{
  std::shared_ptr<GCS::ArcOfEllipse> aNewArc(new GCS::ArcOfEllipse);

  BooleanWrapperPtr isReversed;
  std::map<std::string, EntityWrapperPtr> anAdditionalAttributes;

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (aPoint) {
      if (anIt->first->id() == SketchPlugin_EllipticArc::CENTER_ID())
        aNewArc->center = *(aPoint->point());
      else if (anIt->first->id() == SketchPlugin_EllipticArc::FIRST_FOCUS_ID())
        aNewArc->focus1 = *(aPoint->point());
      else if (anIt->first->id() == SketchPlugin_EllipticArc::START_POINT_ID())
        aNewArc->start = *(aPoint->point());
      else if (anIt->first->id() == SketchPlugin_EllipticArc::END_POINT_ID())
        aNewArc->end = *(aPoint->point());
      else
        anAdditionalAttributes[anIt->first->id()] = anIt->second;
    }
    else if (anIt->first->id() == SketchPlugin_EllipticArc::MINOR_RADIUS_ID()) {
      ScalarWrapperPtr aScalar =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      aNewArc->radmin = aScalar->scalar();
    }
    else if (anIt->first->id() == SketchPlugin_EllipticArc::REVERSED_ID())
      isReversed = std::dynamic_pointer_cast<PlaneGCSSolver_BooleanWrapper>(anIt->second);
    else
      anAdditionalAttributes[anIt->first->id()] = anIt->second;
  }

  // Additional attributes of elliptic arc necessary for PlaneGCS solver (start and end angles)
  aNewArc->startAngle = createParameter(theStorage);
  aNewArc->endAngle = createParameter(theStorage);

  EdgeWrapperPtr anEllipseWrapper(new PlaneGCSSolver_EdgeWrapper(aNewArc));
  anEllipseWrapper->setReversed(isReversed);
  anEllipseWrapper->setAdditionalAttributes(anAdditionalAttributes);
  PlaneGCSSolver_Tools::recalculateArcParameters(anEllipseWrapper);

  return anEllipseWrapper;
}

template <typename TYPE>
EntityWrapperPtr createBSpline(const AttributeEntityMap& theAttributes)
{
  std::shared_ptr<GCS::BSplineImpl> aNewSpline(new GCS::BSplineImpl);

  aNewSpline->degree = 3;
  aNewSpline->periodic = std::is_same<TYPE, SketchPlugin_BSplinePeriodic>();

  std::map<std::string, EntityWrapperPtr> anAdditionalAttributes;

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    const std::string& anAttrID = anIt->first->id();
    if (anAttrID == TYPE::POLES_ID()) {
      PointArrayWrapperPtr anArray =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointArrayWrapper>(anIt->second);

      int aSize = anArray->size();
      aNewSpline->poles.reserve(aSize);
      for (int anIndex = 0; anIndex < aSize; ++anIndex)
        aNewSpline->poles.push_back(*anArray->value(anIndex)->point());

      aNewSpline->start = aNewSpline->poles.front();
      aNewSpline->end = aNewSpline->poles.back();
    }
    else if (anAttrID == TYPE::DEGREE_ID()) {
      ScalarWrapperPtr aScalar =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      aNewSpline->degree = (int)aScalar->value();
    }
    else if (anAttrID == SketchPlugin_BSpline::START_ID() ||
             anAttrID == SketchPlugin_BSpline::END_ID()) {
      anAdditionalAttributes[anAttrID] = anIt->second;
    }
    else {
      ScalarArrayWrapperPtr anArray =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarArrayWrapper>(anIt->second);
      if (anAttrID == TYPE::WEIGHTS_ID())
        aNewSpline->weights = anArray->array();
      else if (anAttrID == TYPE::KNOTS_ID())
        aNewSpline->knots = anArray->array();
      else if (anAttrID == TYPE::MULTS_ID()) {
        const GCS::VEC_pD& aValues = anArray->array();
        aNewSpline->mult.reserve(aValues.size());
        for (GCS::VEC_pD::const_iterator anIt = aValues.begin(); anIt != aValues.end(); ++anIt)
          aNewSpline->mult.push_back((int)(**anIt));
      }
    }
  }

  EdgeWrapperPtr aWrapper(new PlaneGCSSolver_EdgeWrapper(aNewSpline));
  aWrapper->setAdditionalAttributes(anAdditionalAttributes);
  return aWrapper;
}
