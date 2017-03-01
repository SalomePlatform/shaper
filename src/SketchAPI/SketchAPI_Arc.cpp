// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_Arc.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "SketchAPI_Arc.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>

/// Obtain constraints prepared by tangent arc
static std::list<FeaturePtr> tangentArcConstraints(const FeaturePtr& theArc);

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theCenterX, double theCenterY,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenterX, theCenterY, theStartX,
                        theStartY, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenter, theStart, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             double thePassedX, double thePassedY)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStartX, theStartY, theEndX, theEndY, thePassedX, thePassedY);
  }
}

//===============================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStart, theEnd, thePassed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_RefAttr& theTangentPoint,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_RefAttr& theTangentPoint,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Selection& theExternal)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

//================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::string& theExternalName)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

//================================================================================================
SketchAPI_Arc::~SketchAPI_Arc()
{

}

//================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(double theCenterX, double theCenterY,
                                        double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_CENTER_START_END(), myarcType);
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myinversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_CENTER_START_END(), myarcType);
  fillAttribute(theCenter, mycenter);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myinversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByStartEndPassed(double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        double thePassedX, double thePassedY)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_THREE_POINTS(), myarcType);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(passedPoint(), thePassedX, thePassedY);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByStartEndPassed(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_THREE_POINTS(), myarcType);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(thePassed, mypassedPoint);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                 double theEndX, double theEndY,
                                 bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_TANGENT(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myinversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                 bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_TANGENT(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myinversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

//===============================================================================================
void SketchAPI_Arc::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//===============================================================================================
void SketchAPI_Arc::setRadius(double theRadius)
{
  fillAttribute(ModelHighAPI_Double(theRadius), myradius);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setAngle(double theAngle)
{
  fillAttribute(ModelHighAPI_Double(theAngle), myangle);

  execute();
}

//================================================================================================
void SketchAPI_Arc::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // arc is external
    theDumper << aBase << " = " << aSketchName << ".addArc(" << anExternal << ")" << std::endl;
  } else {
    AttributeStringPtr aType = arcType();
    if (aType->value() == SketchPlugin_Arc::ARC_TYPE_CENTER_START_END()) {
      // arc given by center and start, end points
      theDumper << aBase << " = " << aSketchName << ".addArc(" << center() << ", "
                << startPoint() << ", " << endPoint() << ", " << inversed() << ")" << std::endl;
    } else if (aType->value() == SketchPlugin_Arc::ARC_TYPE_THREE_POINTS()) {
      // arc given by three points
      theDumper << aBase << " = " << aSketchName << ".addArc(" << startPoint() << ", "
                << endPoint() << ", " << passedPoint() << ")" << std::endl;
    } else {
      // do not dump coincidence and tangency constraint built by tangent arc
      std::list<FeaturePtr> aConstraintsToSkip = tangentArcConstraints(aBase);
      std::list<FeaturePtr>::iterator anIt = aConstraintsToSkip.begin();
      for (; anIt != aConstraintsToSkip.end(); ++anIt)
        theDumper.doNotDumpFeature(*anIt);

      // tangent arc
      AttributeRefAttrPtr aTangentPoint = tangentPoint();
      theDumper << aBase << " = " << aSketchName << ".addArc("
                << aTangentPoint << ", " << endPoint() << ", " << inversed() << ")" << std::endl;
    }
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}


// ====================   Auxiliary functions   ===============================
std::list<FeaturePtr> tangentArcConstraints(const FeaturePtr& theArc)
{
  std::list<FeaturePtr> aConstraints;

  std::set<FeaturePtr> aCoincidences;
  std::set<FeaturePtr> aTangencies;

  const std::set<AttributePtr>& aBaseRefs = theArc->data()->refsToMe();
  std::set<AttributePtr>::const_iterator anIt = aBaseRefs.begin();
  for (; anIt != aBaseRefs.end(); ++anIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature((*anIt)->owner());
    if (anOwner->getKind() == SketchPlugin_ConstraintCoincidence::ID())
      aCoincidences.insert(anOwner);
  }
  const std::set<AttributePtr>& aBaseResultRefs = theArc->lastResult()->data()->refsToMe();
  for (anIt = aBaseResultRefs.begin(); anIt != aBaseResultRefs.end(); ++anIt) {
    FeaturePtr anOwner = ModelAPI_Feature::feature((*anIt)->owner());
    if (anOwner->getKind() == SketchPlugin_ConstraintTangent::ID())
      aTangencies.insert(anOwner);
  }

  AttributePtr aTangentPoint = theArc->refattr(SketchPlugin_Arc::TANGENT_POINT_ID())->attr();
  if (aTangentPoint) {
    FeaturePtr aTangentFeature = ModelAPI_Feature::feature(aTangentPoint->owner());

    const std::set<AttributePtr>& aTgRefs = aTangentFeature->data()->refsToMe();
    for (anIt = aTgRefs.begin(); anIt != aTgRefs.end(); ++anIt) {
      FeaturePtr anOwner = ModelAPI_Feature::feature((*anIt)->owner());
      if (aCoincidences.find(anOwner) != aCoincidences.end()) {
        // check the coincidence is correct
        AttributePtr aConstrained1 = anOwner->refattr(SketchPlugin_Constraint::ENTITY_A())->attr();
        AttributePtr aConstrained2 = anOwner->refattr(SketchPlugin_Constraint::ENTITY_B())->attr();
        AttributePtr anArcStartPoint = theArc->attribute(SketchPlugin_Arc::START_ID());
        if ((aConstrained1 == anArcStartPoint && aConstrained2 == aTangentPoint) ||
            (aConstrained1 == aTangentPoint && aConstrained2 == anArcStartPoint)) {
          aConstraints.push_back(anOwner);
          break; // search first applicable coincidence only
        }
      }
    }

    const std::set<AttributePtr>& aTgResultRefs =
        aTangentFeature->lastResult()->data()->refsToMe();
    for (anIt = aTgResultRefs.begin(); anIt != aTgResultRefs.end(); ++anIt) {
      FeaturePtr anOwner = ModelAPI_Feature::feature((*anIt)->owner());
      if (aTangencies.find(anOwner) != aTangencies.end()) {
        aConstraints.push_back(anOwner);
        break; // search first tangency only
      }
    }
  }

  return aConstraints;
}
