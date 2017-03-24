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
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        bool theInversed)
{
  fillAttribute(theCenter, mycenter);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myreversed);

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
    // arc given by center and start, end points
    theDumper << aBase << " = " << aSketchName << ".addArc(" << center() << ", "
              << startPoint() << ", " << endPoint() << ", " << reversed() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
