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
class AIS_Dimension;

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

  /// Returns result object referenced by feature
  /// \param theFeature a feature
  /// \param theAttrName an attribute name
  SKETCHERPRS_EXPORT ObjectPtr getResult(ModelAPI_Feature* theFeature,
                                         const std::string& theAttrName);

  /// Returns shape from result object (or NULL)
  /// \param theObject a result object
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject);


  /// Returns point from a line feature
  /// \param theFeature a line feature
  /// \param theAttrName an attribute of the point
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getPoint(ModelAPI_Feature* theFeature,
                                          const std::string& theAttrName);


  /// Returns point projected on a line
  /// \param theLine  a line
  /// \param thePoint a projecting point
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(
                        const FeaturePtr theLine,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  SKETCHERPRS_EXPORT FeaturePtr getFeatureLine(DataPtr theData,
                                               const std::string& theAttribute);

  /// Obtain the point object from specified constraint parameter
  /// \param theData a data object
  /// \param theAttribute an attribute name
  /// \param thePlane a projection plane (sketcher plane)
  SKETCHERPRS_EXPORT std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute,
                                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Returns value of dimension arrows size
  SKETCHERPRS_EXPORT double getArrowSize();

  /// Set value of dimension arrows size
  /// \param theSize a size value
  SKETCHERPRS_EXPORT void setArrowSize(double theSize);

  /// Returns default value of dimension arrows size
  SKETCHERPRS_EXPORT int getDefaultArrowSize();

  /// Set value of dimension text height
  /// \param theHeight a height value
  SKETCHERPRS_EXPORT void setTextHeight(double theHeight);

  /// Returns value of dimension text height
  SKETCHERPRS_EXPORT double getTextHeight();

  /// Returns default value of dimension text height
  SKETCHERPRS_EXPORT double getDefaultTextHeight();

  /// Get flayout distance of the dimension constraint
  /// \param theConstraint a dimension constraint object
  SKETCHERPRS_EXPORT double getFlyoutDistance(const ModelAPI_Feature* theConstraint);

  /// Compute anchor pint for radius dimension
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt> getAnchorPoint(
                                              const ModelAPI_Feature* theConstraint,
                                              const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Display/hide sigma symbol in the dimension presentation
  /// \param theDimension a dimension constraint
  /// \param theToDisplay a boolean value
  SKETCHERPRS_EXPORT void setDisplaySpecialSymbol(AIS_Dimension* theDimension,
                                                  const bool& theToDisplay);
};

#endif
