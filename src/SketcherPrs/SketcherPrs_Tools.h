// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.h
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Tools_H
#define SketcherPrs_Tools_H

#include "SketcherPrs.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Ax3.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <string>

class GeomDataAPI_Point2D;

//#define MyTextHeight 20

namespace SketcherPrs_Tools {

/// Enumeration with modes for activation of selection custom presentations
enum SelectionModes {
  /// Start of enumeration
  Sel_Mode_First = 100, 
  
  /// Selection mode for all constraints exclude dimensions
  Sel_Constraint,
  
  /// Selection mode for whole dimension
  Sel_Dimension_All,
  
  /// Selection mode for line of dimension
  Sel_Dimension_Line,

  /// Selection mode foe text of dimension
  Sel_Dimension_Text
};

  SKETCHERPRS_EXPORT ObjectPtr getResult(ModelAPI_Feature* theFeature,
                                         const std::string& theAttrName);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getPoint(ModelAPI_Feature* theFeature,
                                          const std::string& theAttrName);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(
                        const FeaturePtr theLine,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  SKETCHERPRS_EXPORT FeaturePtr getFeatureLine(DataPtr theData,
                                               const std::string& theAttribute);

  /// Obtain the point object from specified constraint parameter
  SKETCHERPRS_EXPORT std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute,
                                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  SKETCHERPRS_EXPORT double getArrowSize();

  SKETCHERPRS_EXPORT void setArrowSize(double theSize);

  SKETCHERPRS_EXPORT void setTextHeight(double theHeight);

  SKETCHERPRS_EXPORT double getTextHeight();

  SKETCHERPRS_EXPORT double getDefaultTextHeight();

  SKETCHERPRS_EXPORT double getFlyoutDistance(const ModelAPI_Feature* theConstraint);

  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt> getAnchorPoint(
                                              const ModelAPI_Feature* theConstraint,
                                              const std::shared_ptr<GeomAPI_Ax3>& thePlane);
};

#endif
