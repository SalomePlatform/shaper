// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_MacroEllipse_H_
#define SketchPlugin_MacroEllipse_H_

#include <ModelAPI_IReentrant.h>
#include <SketchPlugin.h>
#include <SketchPlugin_SketchEntity.h>
#include <GeomAPI_IPresentable.h>

////class GeomAPI_Circ2d;
class GeomAPI_Pnt2d;

/**\class SketchPlugin_MacroEllipse
 * \ingroup Plugins
 * \brief Feature for creation of the new ellipse in Sketch.
 */
class SketchPlugin_MacroEllipse: public SketchPlugin_SketchEntity,
                                 public GeomAPI_IPresentable,
                                 public ModelAPI_IReentrant
{
 public:
  /// Ellipse feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroEllipse");
    return ID;
  }

  static const std::string& ELLIPSE_TYPE()
  {
    static const std::string ID("ellipse_type");
    return ID;
  }

  static const std::string& ELLIPSE_TYPE_BY_CENTER_AXIS_POINT()
  {
    static const std::string ID("by_center_axis_point");
    return ID;
  }
  static const std::string& ELLIPSE_TYPE_BY_AXIS_AND_POINT()
  {
    static const std::string ID("by_major_axis_and_point");
    return ID;
  }

  static const std::string& EDIT_ELLIPSE_TYPE()
  {
    static const std::string ID("edit_ellipse_type");
    return ID;
  }

  /// Attribute for the first point selected during ellipse creation.
  inline static const std::string& FIRST_POINT_ID()
  {
    static const std::string ID("first_point");
    return ID;
  }

  /// Reference to the first selected point.
  inline static const std::string& FIRST_POINT_REF_ID()
  {
    static const std::string ID("first_point_ref");
    return ID;
  }

  /// Attribute for the second point selected during ellipse creation.
  inline static const std::string& SECOND_POINT_ID()
  {
    static const std::string ID("second_point");
    return ID;
  }

  /// Reference to the second selected point.
  inline static const std::string& SECOND_POINT_REF_ID()
  {
    static const std::string ID("second_point_ref");
    return ID;
  }

  /// Attribute for the third point selected during ellipse creation.
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

  /// Major radius of the ellipse
  inline static const std::string& MAJOR_RADIUS_ID()
  {
    static const std::string ID("major_radius");
    return ID;
  }

  /// Minor radius of the ellipse
  inline static const std::string& MINOR_RADIUS_ID()
  {
    static const std::string ID("minor_radius");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroEllipse::ID();
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
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const
  {return true;}

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const
  {return false;}

  /// Apply information of the message to current object. It fills reference object,
  /// tangent type and tangent point refence in case of tangent arc
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Use plugin manager for features creation
  SketchPlugin_MacroEllipse();

private:
  void constraintsForEllipseByCenterAxisAndPassed(FeaturePtr theEllipseFeature);
  void constraintsForEllipseByMajoxAxisAndPassed(FeaturePtr theEllipseFeature);

  FeaturePtr createEllipseFeature();

private:
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  std::shared_ptr<GeomAPI_Pnt2d> myFocus;
  double                         myMajorRadius;
  double                         myMinorRadius;
};

#endif
