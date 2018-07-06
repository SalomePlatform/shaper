// Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef FeaturesPlugin_Measurement_H_
#define FeaturesPlugin_Measurement_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_Measurement
/// \ingroup Plugins
/// \brief Feature for calculation metrics.
///
/// Supported following metrics:
/// * length of edge,
/// * distance between shapes,
/// * radius of arc or cylindrical faces,
/// * angle between edges.
class FeaturesPlugin_Measurement : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Measurement");
    return MY_ID;
  }

  /// \return the kind of a feature.
  virtual const std::string& getKind()
  {
    return ID();
  }

  /// Attribute name for measurement method.
  inline static const std::string& MEASURE_KIND()
  {
    static const std::string MY_MEASURE_KIND_ID("MeasureKind");
    return MY_MEASURE_KIND_ID;
  }

  /// Attribute name for length measure.
  inline static const std::string& MEASURE_LENGTH()
  {
    static const std::string MY_MEASURE_ID("Length");
    return MY_MEASURE_ID;
  }

  /// Attribute name for distance measure.
  inline static const std::string& MEASURE_DISTANCE()
  {
    static const std::string MY_MEASURE_ID("Distance");
    return MY_MEASURE_ID;
  }

  /// Attribute name for radius measure.
  inline static const std::string& MEASURE_RADIUS()
  {
    static const std::string MY_MEASURE_ID("Radius");
    return MY_MEASURE_ID;
  }

  /// Attribute name for angle measure.
  inline static const std::string& MEASURE_ANGLE()
  {
    static const std::string MY_MEASURE_ID("Angle");
    return MY_MEASURE_ID;
  }

  /// Attribute name for angle measurement by 3 points.
  inline static const std::string& MEASURE_ANGLE_POINTS()
  {
    static const std::string MY_MEASURE_ID("AngleBy3Points");
    return MY_MEASURE_ID;
  }


  /// Attribute name of edge selected for length calculation.
  inline static const std::string& EDGE_FOR_LENGTH_ID()
  {
    static const std::string MY_EDGE_FOR_LENGTH_ID("edge_for_length");
    return MY_EDGE_FOR_LENGTH_ID;
  }

  /// Attribute name of first shape selected for distance calculation.
  inline static const std::string& DISTANCE_FROM_OBJECT_ID()
  {
    static const std::string MY_DISTANCE_FROM_OBJECT_ID("distance_from");
    return MY_DISTANCE_FROM_OBJECT_ID;
  }

  /// Attribute name of second shape selected for distance calculation.
  inline static const std::string& DISTANCE_TO_OBJECT_ID()
  {
    static const std::string MY_DISTANCE_TO_OBJECT_ID("distance_to");
    return MY_DISTANCE_TO_OBJECT_ID;
  }

  // Attribute name of edge or face selected to calculate radius.
  inline static const std::string& CIRCULAR_OBJECT_ID()
  {
    static const std::string MY_CIRCULAR_OBJECT_ID("circular");
    return MY_CIRCULAR_OBJECT_ID;
  }

  /// Attribute name of first edge selected for angle calculation.
  inline static const std::string& ANGLE_FROM_EDGE_ID()
  {
    static const std::string MY_ANGLE_FROM_EDGE_ID("angle_from");
    return MY_ANGLE_FROM_EDGE_ID;
  }

  /// Attribute name of second shape selected for angle calculation.
  inline static const std::string& ANGLE_TO_EDGE_ID()
  {
    static const std::string MY_ANGLE_TO_EDGE_ID("angle_to");
    return MY_ANGLE_TO_EDGE_ID;
  }

  /// Attribute name of first point selected for angle calculation.
  inline static const std::string& ANGLE_POINT1_ID()
  {
    static const std::string MY_ANGLE_POINT1_ID("angle_point_1");
    return MY_ANGLE_POINT1_ID;
  }

  /// Attribute name of second point (apex) selected for angle calculation.
  inline static const std::string& ANGLE_POINT2_ID()
  {
    static const std::string MY_ANGLE_POINT2_ID("angle_point_2");
    return MY_ANGLE_POINT2_ID;
  }

  /// Attribute name of third point selected for angle calculation.
  inline static const std::string& ANGLE_POINT3_ID()
  {
    static const std::string MY_ANGLE_POINT3_ID("angle_point_3");
    return MY_ANGLE_POINT3_ID;
  }

  /// Attribute name for result.
  inline static const std::string& RESULT_ID()
  {
    static const std::string MY_RESULT_ID("result");
    return MY_RESULT_ID;
  }

  /// Attribute name for values of result.
  inline static const std::string& RESULT_VALUES_ID()
  {
    static const std::string MY_RESULT_VALUES_ID("result_values");
    return MY_RESULT_VALUES_ID;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  virtual bool isMacro() const { return true; }

  /// Use plugin manager for features creation
  FeaturesPlugin_Measurement();

private:
  /// Compute length of the edge
  void computeLength();
  /// Compute minimal distance between pair of shapes
  void computeDistance();
  /// Compute radius of circular edge or cylindrical face
  void computeRadius();
  /// Compute angle(s) between pair of edges if they are intersected
  void computeAngle();
  /// Compute angle by three points
  void computeAngleByPoints();
};

#endif
