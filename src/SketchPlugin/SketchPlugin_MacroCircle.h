// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_MacroCircle_H_
#define SketchPlugin_MacroCircle_H_

#include <ModelAPI_IReentrant.h>

#include "SketchPlugin.h"

#include "SketchPlugin_SketchEntity.h"

#include <GeomAPI_IPresentable.h>

class GeomAPI_Circ2d;
class GeomAPI_Pnt2d;

/**\class SketchPlugin_MacroCircle
 * \ingroup Plugins
 * \brief Feature for creation of the new circle in Sketch.
 */
class SketchPlugin_MacroCircle: public SketchPlugin_SketchEntity,
                                public GeomAPI_IPresentable,
                                public ModelAPI_IReentrant
{
 public:
  /// Circle feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroCircle");
    return ID;
  }

  inline static const std::string& CIRCLE_TYPE()
  {
    static const std::string ID("circle_type");
    return ID;
  }

  inline static const std::string& EDIT_CIRCLE_TYPE()
  {
    static const std::string ID("edit_circle_type");
    return ID;
  }

  /// Creation method by center and passed point.
  inline static const std::string& CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS()
  {
    static const std::string ID("circle_type_by_center_and_passed_points");
    return ID;
  }

  /// Creation method by three points.
  inline static const std::string& CIRCLE_TYPE_BY_THREE_POINTS()
  {
    static const std::string ID("circle_type_by_three_points");
    return ID;
  }

  /// 2D point - center of the circle.
  inline static const std::string& CENTER_POINT_ID()
  {
    static const std::string ID("center_point");
    return ID;
  }

  /// Reference for center point selection.
  inline static const std::string& CENTER_POINT_REF_ID()
  {
    static const std::string ID("center_point_ref");
    return ID;
  }

  /// 2D point - passed point of the circle
  inline static const std::string& PASSED_POINT_ID()
  {
    static const std::string ID("passed_point");
    return ID;
  }

  /// Reference for passed point selection.
  inline static const std::string& PASSED_POINT_REF_ID()
  {
    static const std::string ID("passed_point_ref");
    return ID;
  }

  /// First point id.
  inline static const std::string& FIRST_POINT_ID()
  {
    static const std::string ID("first_point");
    return ID;
  }

  /// Reference for first point selection.
  inline static const std::string& FIRST_POINT_REF_ID()
  {
    static const std::string ID("first_point_ref");
    return ID;
  }

  /// Second point id.
  inline static const std::string& SECOND_POINT_ID()
  {
    static const std::string ID("second_point");
    return ID;
  }

  /// Reference for second point selection.
  inline static const std::string& SECOND_POINT_REF_ID()
  {
    static const std::string ID("second_point_ref");
    return ID;
  }

  /// Third point id.
  inline static const std::string& THIRD_POINT_ID()
  {
    static const std::string ID("third_point");
    return ID;
  }

  /// Reference for third point selection.
  inline static const std::string& THIRD_POINT_REF_ID()
  {
    static const std::string ID("third_point_ref");
    return ID;
  }

  /// Radius of the circle
  inline static const std::string& CIRCLE_RADIUS_ID()
  {
    static const std::string ID("circle_radius");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroCircle::ID();
    return MY_KIND;
  }

  /// \brief Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Apply information of the message to current object. It fills reference object,
  /// tangent type and tangent point refence in case of tangent arc
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Use plugin manager for features creation
  SketchPlugin_MacroCircle();

private:
  void fillByCenterAndPassed();
  void fillByThreePoints();
  /// set fields if only two of three points is initialized
  void fillByTwoPassedPoints();

  void constraintsForCircleByCenterAndPassed(FeaturePtr theCircleFeature);
  void constraintsForCircleByThreePoints(FeaturePtr theCircleFeature);

  FeaturePtr createCircleFeature();

private:
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  double                         myRadius;
};

#endif
