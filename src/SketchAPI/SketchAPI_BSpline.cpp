// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#include "SketchAPI_BSpline.h"

#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAlgoAPI_EdgeBuilder.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <ModelAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <cmath>


SketchAPI_BSpline::SketchAPI_BSpline(const std::shared_ptr<ModelAPI_Feature> & theFeature)
  : SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_BSpline::SketchAPI_BSpline(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     bool theInitialize)
  : SketchAPI_SketchEntity(theFeature)
{
  if (theInitialize)
    initialize();
}

SketchAPI_BSpline::~SketchAPI_BSpline()
{
}

void SketchAPI_BSpline::setByDegreePolesAndWeights(const ModelHighAPI_Integer& theDegree,
                                                   const std::list<GeomPnt2dPtr>& thePoles,
                                                   const std::list<ModelHighAPI_Double>& theWeights)
{
  std::list<ModelHighAPI_Double> aWeights;
  if (theWeights.size() <= 1) {
    // prepare array of equal weights
    aWeights.assign(thePoles.size(),
        theWeights.empty() ? ModelHighAPI_Double(1.0) : theWeights.front());
  }
  else
    aWeights = theWeights;

  ModelHighAPI_Integer aDegree = theDegree;
  std::list<ModelHighAPI_Double> aKnots;
  std::list<ModelHighAPI_Integer> aMults;
  getDefaultParameters(thePoles, aWeights, aDegree, aKnots, aMults);

  setByParameters(aDegree, thePoles, aWeights, aKnots, aMults);
}

void SketchAPI_BSpline::setByParameters(const ModelHighAPI_Integer& theDegree,
                                        const std::list<GeomPnt2dPtr>& thePoles,
                                        const std::list<ModelHighAPI_Double>& theWeights,
                                        const std::list<ModelHighAPI_Double>& theKnots,
                                        const std::list<ModelHighAPI_Integer>& theMults)
{
  fillAttribute(theDegree, degree());

  fillAttribute(thePoles, poles());
  if (theWeights.size() <= 1) {
    // prepare array of equal weights
    std::list<ModelHighAPI_Double> aWeights(thePoles.size(),
        theWeights.empty() ? ModelHighAPI_Double(1.0) : theWeights.front());
    fillAttribute(aWeights, weights());
  }
  else
    fillAttribute(theWeights, weights());

  fillAttribute(theKnots, knots());
  fillAttribute(theMults, multiplicities());

  if (feature()->getKind() != SketchPlugin_BSplinePeriodic::ID())
    setStartAndEndPoints();
  execute();
}

void SketchAPI_BSpline::setStartAndEndPoints()
{
  fillAttribute(poles()->pnt(0), startPoint());
  fillAttribute(poles()->pnt(poles()->size() - 1), endPoint());
}

void SketchAPI_BSpline::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());
  execute();
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
                                     const AttributePoint2DPtr& thePoint,
                                     const AttributePoint2DArrayPtr& thePoles,
                                     const int thePoleIndex)
{
  FeaturePtr aConstraint = theSketch->addFeature(SketchPlugin_ConstraintCoincidenceInternal::ID());
  aConstraint->refattr(SketchPlugin_Constraint::ENTITY_A())->setAttr(thePoint);
  aConstraint->refattr(SketchPlugin_Constraint::ENTITY_B())->setAttr(thePoles);
  aConstraint->integer(SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_B())
      ->setValue(thePoleIndex);
  aConstraint->execute();
}

static void createPole(const CompositeFeaturePtr& theSketch,
                       const FeaturePtr& theBSpline,
                       const AttributePoint2DArrayPtr& thePoles,
                       const int thePoleIndex,
                       const bool theAuxiliary,
                       std::list<FeaturePtr>& theEntities)
{
  GeomPnt2dPtr aPole = thePoles->pnt(thePoleIndex);

  FeaturePtr aPointFeature = theSketch->addFeature(SketchPlugin_Point::ID());
  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aPointFeature->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(aPole);
  aPointFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theBSpline);
  aPointFeature->execute();

  std::wostringstream aName;
  aName << theBSpline->name() << "_" << ModelAPI_Tools::toWString(thePoles->id()) << "_" << thePoleIndex;
  aPointFeature->data()->setName(aName.str());
  aPointFeature->lastResult()->data()->setName(aName.str());

  aPointFeature->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(theAuxiliary);

  createInternalConstraint(theSketch, aCoord, thePoles, thePoleIndex);

  theEntities.push_back(aPointFeature);
}

static void createSegment(const CompositeFeaturePtr& theSketch,
                          const FeaturePtr& theBSpline,
                          const AttributePoint2DArrayPtr& thePoles,
                          const int theStartPoleIndex,
                          const bool theAuxiliary,
                          std::list<FeaturePtr>& theEntities)
{
  int aEndPoleIndex = (theStartPoleIndex + 1) % thePoles->size();
  GeomPnt2dPtr aStartPoint = thePoles->pnt(theStartPoleIndex);
  GeomPnt2dPtr aEndPoint = thePoles->pnt(aEndPoleIndex);

  FeaturePtr aLineFeature = theSketch->addFeature(SketchPlugin_Line::ID());
  AttributePoint2DPtr aLineStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::START_ID()));
  aLineStart->setValue(aStartPoint);
  AttributePoint2DPtr aLineEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::END_ID()));
  aLineEnd->setValue(aEndPoint);
  aLineFeature->reference(SketchPlugin_Point::PARENT_ID())->setValue(theBSpline);
  aLineFeature->execute();

  std::wostringstream aName;
  aName << theBSpline->name() << "_segment_" << theStartPoleIndex << "_" << aEndPoleIndex;
  aLineFeature->data()->setName(aName.str());
  aLineFeature->lastResult()->data()->setName(aName.str());

  aLineFeature->boolean(SketchPlugin_Line::AUXILIARY_ID())->setValue(theAuxiliary);

  createInternalConstraint(theSketch, aLineStart, thePoles, theStartPoleIndex);
  createInternalConstraint(theSketch, aLineEnd, thePoles, aEndPoleIndex);

  theEntities.push_back(aLineFeature);
}

static void toMapOfAuxIndices(const std::list<int>& theRegular,
                              const std::list<int>& theAuxiliary,
                              std::map<int, bool>& theIndices)
{
  for (auto it = theRegular.begin(); it != theRegular.end(); ++it)
    theIndices[*it] = false;
  for (auto it = theAuxiliary.begin(); it != theAuxiliary.end(); ++it)
    theIndices[*it] = true;
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_BSpline::controlPoles(
    const std::list<int>& regular,
    const std::list<int>& auxiliary) const
{
  std::map<int, bool> anAux;
  toMapOfAuxIndices(regular, auxiliary, anAux);

  std::list<FeaturePtr> anEntities;

  FeaturePtr aBSpline = feature();
  CompositeFeaturePtr aSketch = sketchForFeature(aBSpline);
  AttributePoint2DArrayPtr aPoles = poles();

  for (auto it = anAux.begin(); it != anAux.end(); ++it)
    createPole(aSketch, aBSpline, aPoles, it->first, it->second, anEntities);

  return SketchAPI_SketchEntity::wrap(anEntities);
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_BSpline::controlPolygon(
    const std::list<int>& regular,
    const std::list<int>& auxiliary) const
{
  std::map<int, bool> anAux;
  toMapOfAuxIndices(regular, auxiliary, anAux);

  std::list<FeaturePtr> anEntities;

  FeaturePtr aBSpline = feature();
  CompositeFeaturePtr aSketch = sketchForFeature(aBSpline);
  AttributePoint2DArrayPtr aPoles = poles();

  for (auto it = anAux.begin(); it != anAux.end(); ++it)
    createSegment(aSketch, aBSpline, aPoles, it->first, it->second, anEntities);

  return SketchAPI_SketchEntity::wrap(anEntities);
}


void SketchAPI_BSpline::getDefaultParameters(
    const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
    const std::list<ModelHighAPI_Double>& theWeights,
    ModelHighAPI_Integer& theDegree,
    std::list<ModelHighAPI_Double>& theKnots,
    std::list<ModelHighAPI_Integer>& theMults) const
{
  std::shared_ptr<GeomAPI_BSpline2d> aBSplineCurve;
  try {
    std::list<double> aWeights;
    for (std::list<ModelHighAPI_Double>::const_iterator it = theWeights.begin();
         it != theWeights.end(); ++it)
      aWeights.push_back(it->value());

    bool isPeriodic = feature()->getKind() == SketchPlugin_BSplinePeriodic::ID();
    if (theDegree.intValue() < 0)
      aBSplineCurve.reset(new GeomAPI_BSpline2d(thePoles, aWeights, isPeriodic));
    else {
      aBSplineCurve.reset(new GeomAPI_BSpline2d(theDegree.intValue(), thePoles, aWeights,
                                                std::list<double>(), std::list<int>(), isPeriodic));
    }
  }
  catch (...) {
    // cannot build a B-spline curve
    return;
  }

  theDegree = aBSplineCurve->degree();
  std::list<double> aKnots = aBSplineCurve->knots();
  std::list<int> aMults = aBSplineCurve->mults();
  theKnots.assign(aKnots.begin(), aKnots.end());
  theMults.assign(aMults.begin(), aMults.end());
}

void SketchAPI_BSpline::checkDefaultParameters(bool& isDefaultDegree,
                                               bool& isDefaultWeights,
                                               bool& isDefaultKnotsMults) const
{
  static const double TOLERANCE = 1.e-7;

  AttributePoint2DArrayPtr aPolesAttr = poles();
  AttributeDoubleArrayPtr aWeightsAttr = weights();
  AttributeDoubleArrayPtr aKnotsAttr = knots();
  AttributeIntArrayPtr aMultsAttr = multiplicities();

  std::list<GeomPnt2dPtr> aPoles;
  std::list<ModelHighAPI_Double> aWeights;
  isDefaultWeights = true;
  for (int anIndex = 0; anIndex < aPolesAttr->size(); ++anIndex) {
    aPoles.push_back(aPolesAttr->pnt(anIndex));
    double aCurWeight = aWeightsAttr->value(anIndex);
    isDefaultWeights = isDefaultWeights && fabs(aCurWeight - 1.0) < TOLERANCE;
    aWeights.push_back(aCurWeight);
  }

  ModelHighAPI_Integer aDegree(-1);
  std::list<ModelHighAPI_Double> aKnots;
  std::list<ModelHighAPI_Integer> aMults;
  getDefaultParameters(aPoles, aWeights, aDegree, aKnots, aMults);
  isDefaultDegree = aDegree.intValue() == degree()->value();
  if (!isDefaultDegree) {
    // recalculate knots and multiplicities with the actual degree
    aDegree = degree()->value();
    getDefaultParameters(aPoles, aWeights, aDegree, aKnots, aMults);
  }

  isDefaultKnotsMults = aKnotsAttr->size() == (int)aKnots.size()
                     && aMultsAttr->size() == (int)aMults.size();
  if (isDefaultKnotsMults) {
    std::list<ModelHighAPI_Double>::iterator anIt = aKnots.begin();
    for (int anIndex = 0; isDefaultKnotsMults && anIt != aKnots.end(); ++anIt, ++anIndex)
      isDefaultKnotsMults = fabs(anIt->value() - aKnotsAttr->value(anIndex)) < TOLERANCE;
  }
  if (isDefaultKnotsMults) {
    std::list<ModelHighAPI_Integer>::iterator anIt = aMults.begin();
    for (int anIndex = 0; isDefaultKnotsMults && anIt != aMults.end(); ++anIt, ++anIndex)
      isDefaultKnotsMults = anIt->intValue() == aMultsAttr->value(anIndex);
  }

  isDefaultDegree = isDefaultDegree && isDefaultKnotsMults;
  isDefaultWeights = isDefaultWeights && isDefaultKnotsMults;
}


static void bsplineAuxiliaryFeature(const AttributeRefAttrPtr& theReference,
                                    FeaturePtr& thePoint,
                                    FeaturePtr& theSegment)
{
  ObjectPtr anAuxObject;
  if (theReference->isObject())
    anAuxObject = theReference->object();
  else
    anAuxObject = theReference->attr()->owner();

  FeaturePtr anAuxFeature = ModelAPI_Feature::feature(anAuxObject);
  if (anAuxFeature->getKind() == SketchPlugin_Point::ID())
    thePoint = anAuxFeature;
  else if (anAuxFeature->getKind() == SketchPlugin_Line::ID() &&
           theReference->attr()->id() == SketchPlugin_Line::START_ID()) {
    // process only coincidence with start point
    theSegment = anAuxFeature;
  }
}

static void collectAuxiliaryFeatures(FeaturePtr theBSpline,
                                     std::map<int, FeaturePtr>& thePoints,
                                     std::map<int, FeaturePtr>& theSegments)
{
  const std::set<AttributePtr>& aRefs = theBSpline->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
       aRefIt != aRefs.end(); ++aRefIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
    if (anOwner->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID()) {
      // process internal constraints only
      AttributeRefAttrPtr aRefAttrA = anOwner->refattr(SketchPlugin_Constraint::ENTITY_A());
      AttributeRefAttrPtr aRefAttrB = anOwner->refattr(SketchPlugin_Constraint::ENTITY_B());
      AttributePtr anAttrA = aRefAttrA->attr();
      AttributePtr anAttrB = aRefAttrB->attr();

      AttributeIntegerPtr aPoleIndex;
      FeaturePtr aPoint, aLine;
      if (anAttrA && anAttrA->attributeType() == GeomDataAPI_Point2DArray::typeId()) {
        aPoleIndex = anOwner->integer(SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_A());
        bsplineAuxiliaryFeature(aRefAttrB, aPoint, aLine);
      }
      else if (anAttrB && anAttrB->attributeType() == GeomDataAPI_Point2DArray::typeId()) {
        aPoleIndex = anOwner->integer(SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_B());
        bsplineAuxiliaryFeature(aRefAttrA, aPoint, aLine);
      }

      if (aPoint)
        thePoints[aPoleIndex->value()] = aPoint;
      else if (aLine)
        theSegments[aPoleIndex->value()] = aLine;
    }
  }
}

void SketchAPI_BSpline::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // B-spline is external
    theDumper << aBase << " = " << aSketchName << ".addSpline(" << anExternal << ")" << std::endl;
  } else {
    // check if some B-spline parameters are default and should not be dumped
    bool isDefaultDegree, isDefaultWeights, isDefaultKnotsMults;
    checkDefaultParameters(isDefaultDegree, isDefaultWeights, isDefaultKnotsMults);

    theDumper << aBase << " = " << aSketchName << ".addSpline(";
    if (!isDefaultDegree)
      theDumper << "degree = " << degree() << ", ";
    theDumper << "poles = " << poles();
    if (!isDefaultWeights)
      theDumper << ", weights = " << weights();
    if (!isDefaultKnotsMults)
      theDumper << ", knots = " << knots() << ", multiplicities = " << multiplicities();
    if (aBase->getKind() == SketchPlugin_BSplinePeriodic::ID())
      theDumper << ", periodic = True";
    theDumper << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);

  // dump control polygon
  std::map<int, FeaturePtr> anAuxPoles, anAuxSegments;
  collectAuxiliaryFeatures(aBase, anAuxPoles, anAuxSegments);

  if (!anAuxPoles.empty())
    dumpControlPolygon(theDumper, aBase, "controlPoles", anAuxPoles);
  if (!anAuxSegments.empty())
    dumpControlPolygon(theDumper, aBase, "controlPolygon", anAuxSegments);
}

static void dumpList(ModelHighAPI_Dumper& theDumper,
                     const std::string& theAttrName,
                     const std::set<int>& theIndices)
{
  theDumper << theAttrName << " = [";
  std::set<int>::const_iterator it = theIndices.begin();
  theDumper << *it;
  for (++it; it != theIndices.end(); ++it)
    theDumper << ", " << *it;
  theDumper << "]";
}

void SketchAPI_BSpline::dumpControlPolygon(
    ModelHighAPI_Dumper& theDumper,
    const FeaturePtr& theBSpline,
    const std::string& theMethod,
    const std::map<int, FeaturePtr>& theAuxFeatures) const
{
  theDumper << "[";
  bool isFirst = true;
  // dump features and split them to auxiliary and regular
  std::set<int> aRegular, anAuxiliary;
  for (std::map<int, FeaturePtr>::const_iterator it = theAuxFeatures.begin();
       it != theAuxFeatures.end(); ++it) {
    if (!isFirst)
      theDumper << ", ";
    theDumper << theDumper.name(it->second, false);
    theDumper.doNotDumpFeature(it->second);
    isFirst = false;

    if (it->second->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value())
      anAuxiliary.insert(it->first);
    else
      aRegular.insert(it->first);
  }
  theDumper << "] = " << theDumper.name(theBSpline) << "." << theMethod << "(";
  if (!aRegular.empty()) {
    dumpList(theDumper, "regular", aRegular);
    if (!anAuxiliary.empty())
      theDumper << ", ";
  }
  if (!anAuxiliary.empty())
    dumpList(theDumper, "auxiliary", anAuxiliary);
  theDumper << ")" << std::endl;
}

static void setCoordinates(const FeaturePtr& theFeature,
                           const std::string& theAttrName,
                           const GeomPnt2dPtr& theCoordinates)
{
  AttributePoint2DPtr aPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFeature->attribute(theAttrName));
  aPoint->setValue(theCoordinates);
}

bool SketchAPI_BSpline::insertPole(const int theIndex,
                                   const GeomPnt2dPtr& theCoordinates,
                                   const ModelHighAPI_Double& theWeight)
{
  std::ostringstream anActionName;
  anActionName << SketchPlugin_BSplineBase::ADD_POLE_ACTION_ID() << "#" << theIndex;
  bool isOk = feature()->customAction(anActionName.str());
  if (isOk) {
    int anIndex = theIndex + 1;
    if (feature()->getKind() == SketchPlugin_BSpline::ID() && anIndex + 1 >= poles()->size())
      anIndex = poles()->size() - 2;
    // initialize coordinates and weight of new pole
    poles()->setPnt(anIndex, theCoordinates);
    weights()->setValue(anIndex, theWeight.value());

    // update coordinates of points of control polygon
    std::map<int, FeaturePtr> aPoints, aLines;
    collectAuxiliaryFeatures(feature(), aPoints, aLines);
    std::map<int, FeaturePtr>::iterator aFound = aPoints.find(anIndex);
    if (aFound != aPoints.end())
      setCoordinates(aFound->second, SketchPlugin_Point::COORD_ID(), theCoordinates);
    aFound = aLines.find(anIndex);
    if (aFound != aLines.end())
      setCoordinates(aFound->second, SketchPlugin_Line::START_ID(), theCoordinates);
    aFound = aLines.find(anIndex - 1);
    if (aFound != aLines.end())
      setCoordinates(aFound->second, SketchPlugin_Line::END_ID(), theCoordinates);
  }
  return isOk;
}



// =================================================================================================
SketchAPI_BSplinePeriodic::SketchAPI_BSplinePeriodic(const FeaturePtr& theFeature)
  : SketchAPI_BSpline(theFeature, false)
{
  initialize();
}
