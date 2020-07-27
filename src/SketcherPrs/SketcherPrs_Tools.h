// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SketcherPrs_Tools_H
#define SketcherPrs_Tools_H

#include "SketcherPrs.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Ax3.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>
#include <string>

#include <Events_Loop.h>
#include <Events_Message.h>

class GeomDataAPI_Point2D;

namespace SketcherPrs_Tools {

  /// \enum ParameterStyle lists styles of parameter
  enum ParameterStyle {
    ParameterValue, ///< using symbol with the parameter value
    ParameterText ///< using parameter text
  };

  /// Set dimensions parameters style
  /// \param theStyle new style
  SKETCHERPRS_EXPORT void setParameterStyle(ParameterStyle theStyle);

  /// Return dimensions parameters style
  SKETCHERPRS_EXPORT ParameterStyle parameterStyle();

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

    /// Selection mode for text of dimension
    Sel_Dimension_Text,

    /// Selectiom mode for faces selection on sketch
    Sel_Sketch_Face,

    /// Selectiom mode for wires selection on sketch
    Sel_Sketch_Wire
  };

  /// Type of angle
  enum AngleType{
    ANGLE_DIRECT,   ///< Angle from the first line to the second line
    ANGLE_COMPLEMENTARY,  ///< Additional angle to the angle from first to second line
    ANGLE_BACKWARD ///< Angle from the second line to the first line
  };

  /// Type of dimension location
  enum LocationType{
    LOCATION_RIGHT,   ///< Position is marked by right arrow placed on the left
    LOCATION_AUTOMATIC,  ///< Position is marked by two arrow placed on the both sides
    LOCATION_LEFT ///< Position is marked by left arrow placed on the left
  };

  /// Event ID about expression visual state has been changed, the symbol with a digit
  /// or parameter text is shown

  /// Returns attribute object referenced by feature
  /// \param theFeature a feature
  /// \param theAttrName an attribute name
  SKETCHERPRS_EXPORT AttributePtr getAttribute(ModelAPI_Feature* theFeature,
                                               const std::string& theAttrName);

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

  /// Collect all sketch points which are not connected with other entities.
  /// \param theSketch sketch feature
  /// \return list of results of SketchPlugin_Point features
  SKETCHERPRS_EXPORT std::list<ResultPtr> getFreePoints(const CompositeFeaturePtr& theSketch);

  /// Returns value of dimension arrows size
  SKETCHERPRS_EXPORT double getArrowSize();

  /// Set value of dimension arrows size
  /// \param theSize a size value
  SKETCHERPRS_EXPORT void setArrowSize(double theSize);

  /// Returns default value of dimension arrows size
  SKETCHERPRS_EXPORT int getDefaultArrowSize();

  /// Returns value of dimension arrows size set in preferences
  SKETCHERPRS_EXPORT int getConfigArrowSize();

  /// Set value of dimension text height
  /// \param theHeight a height value
  SKETCHERPRS_EXPORT void setTextHeight(double theHeight);

  /// Returns value of dimension text height
  SKETCHERPRS_EXPORT double getTextHeight();

  /// Returns default value of dimension text height
  SKETCHERPRS_EXPORT double getDefaultTextHeight();

  /// Returns value of dimension text height set in preferences
  SKETCHERPRS_EXPORT double getConfigTextHeight();

  /// Get flayout distance of the dimension constraint
  /// \param theConstraint a dimension constraint object
  SKETCHERPRS_EXPORT double getFlyoutDistance(const ModelAPI_Feature* theConstraint);

  /// Compute anchor pint for radius dimension
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt> getAnchorPoint(
                                              const ModelAPI_Feature* theConstraint,
                                              const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Throws an exception(in debug mode) and sends a signal about necessity to hide the object
  /// \param theFeature a feature where AIS presentation is empty
  /// \param theError a debug error information
  SKETCHERPRS_EXPORT void sendEmptyPresentationError(ModelAPI_Feature* theFeature,
                                                     const std::string theError);

  SKETCHERPRS_EXPORT void setPixelRatio(int theRatio);

  SKETCHERPRS_EXPORT int pixelRatio();
};

#endif
