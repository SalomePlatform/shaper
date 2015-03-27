// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.h
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Tools_H
#define SketcherPrs_Tools_H

#include "SketcherPrs.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt2d.h>
#include <ModelAPI_Object.h>
#include <string>

class SketchPlugin_Constraint;
class SketchPlugin_Line;
class GeomDataAPI_Point2D;

namespace SketcherPrs_Tools {

  SKETCHERPRS_EXPORT ObjectPtr getResult(SketchPlugin_Constraint* theFeature,
                      const std::string& theAttrName);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getPoint(SketchPlugin_Constraint* theFeature,
                                          const std::string& theAttrName);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(
                        const std::shared_ptr<SketchPlugin_Line>& theLine,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  SKETCHERPRS_EXPORT std::shared_ptr<SketchPlugin_Line> getFeatureLine(DataPtr theData,
                                                    const std::string& theAttribute);

  /// Obtain the point object from specified constraint parameter
  SKETCHERPRS_EXPORT std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute);

};

#endif