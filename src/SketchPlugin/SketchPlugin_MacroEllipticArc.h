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

#ifndef SketchPlugin_MacroEllipticArc_H_
#define SketchPlugin_MacroEllipticArc_H_

#include <ModelAPI_IReentrant.h>
#include <SketchPlugin.h>
#include <SketchPlugin_SketchEntity.h>
#include <GeomAPI_IPresentable.h>

class GeomAPI_Pnt2d;
class GeomAPI_Shape;

/**\class SketchPlugin_MacroEllipticArc
 * \ingroup Plugins
 * \brief Feature for creation of the new elliptic arc in Sketch.
 */
class SketchPlugin_MacroEllipticArc: public SketchPlugin_SketchEntity,
                                     public GeomAPI_IPresentable,
                                     public ModelAPI_IReentrant
{
 public:
  /// Elliptic arc feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroEllipticArc");
    return ID;
  }

  /// Attribute for the central point selected during elliptic arc creation.
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID("center");
    return ID;
  }

  /// Reference to the first selected point (center of ellipse).
  inline static const std::string& CENTER_REF_ID()
  {
    static const std::string ID("center_ref");
    return ID;
  }

  /// Attribute for the point on major semi-axis selected during elliptic arc creation.
  inline static const std::string& MAJOR_AXIS_POINT_ID()
  {
    static const std::string ID("major_axis_point");
    return ID;
  }

  /// Reference to the second selected point (major semi-axis of the ellipse).
  inline static const std::string& MAJOR_AXIS_POINT_REF_ID()
  {
    static const std::string ID("major_axis_point_ref");
    return ID;
  }

  /// Attribute for the start point of the elliptic arc selected during creation.
  inline static const std::string& START_POINT_ID()
  {
    static const std::string ID("start_point");
    return ID;
  }

  /// Reference for the start point selection.
  inline static const std::string& START_POINT_REF_ID()
  {
    static const std::string ID("start_point_ref");
    return ID;
  }

  /// Attribute for the end point of the elliptic arc selected during creation.
  inline static const std::string& END_POINT_ID()
  {
    static const std::string ID("end_point");
    return ID;
  }

  /// Reference for the end point selection.
  inline static const std::string& END_POINT_REF_ID()
  {
    static const std::string ID("end_point_ref");
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

  /// Flag the arc is reversed
  inline static const std::string& REVERSED_ID()
  {
    static const std::string ID("reversed");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroEllipticArc::ID();
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
  SketchPlugin_MacroEllipticArc();

private:
  std::shared_ptr<GeomAPI_Shape> getArcShape();

////  void constraintsForEllipseByCenterAxisAndPassed(FeaturePtr theEllipseFeature);
////  void constraintsForEllipseByMajoxAxisAndPassed(FeaturePtr theEllipseFeature);

  FeaturePtr createEllipticArcFeature();

private:
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  std::shared_ptr<GeomAPI_Pnt2d> myMajorAxis;
  std::shared_ptr<GeomAPI_Pnt2d> myStartPnt;
  std::shared_ptr<GeomAPI_Pnt2d> myEndPnt;
  double myMajorRadius;
  double myMinorRadius;
  double myParamDelta;
};

#endif
