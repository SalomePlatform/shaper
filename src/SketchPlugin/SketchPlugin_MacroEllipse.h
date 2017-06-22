// Copyright (C) 2017  CEA/DEN, EDF R&D
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

// File:        SketchPlugin_MacroEllipse.h
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

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

  /// 2D point - center of the ellipse.
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

  /// 2D point - major axis point of the ellipse.
  inline static const std::string& MAJOR_AXIS_POINT_ID()
  {
    static const std::string ID("major_axis_point");
    return ID;
  }

  /// Reference for major axis point selection.
  inline static const std::string& MAJOR_AXIS_POINT_REF_ID()
  {
    static const std::string ID("major_axis_point_ref");
    return ID;
  }

  /// 2D point - passed point of the ellipse
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
    static const std::string ID("ellipse_major_radius");
    return ID;
  }

  /// Minor radius of the ellipse
  inline static const std::string& MINOR_RADIUS_ID()
  {
    static const std::string ID("ellipse_minor_radius");
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

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY)
  {}

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
  void constraintsForEllipse(FeaturePtr theEllipseFeature);

  FeaturePtr createEllipseFeature();

private:
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  std::shared_ptr<GeomAPI_Pnt2d> myFocus;
  double                         myMajorRadius;
  double                         myMinorRadius;
};

#endif
