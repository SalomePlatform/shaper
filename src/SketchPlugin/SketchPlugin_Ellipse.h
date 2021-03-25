// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_Ellipse_H_
#define SketchPlugin_Ellipse_H_

#include <SketchPlugin.h>
#include <SketchPlugin_SketchEntity.h>

/**\class SketchPlugin_Ellipse
 * \ingroup Plugins
 * \brief Feature for creation of the new ellipse in Sketch.
 */
class SketchPlugin_Ellipse: public SketchPlugin_SketchEntity
{
 public:
  /// Ellipse feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchEllipse");
    return ID;
  }

  /// 2D point - center of the ellipse
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID("ellipse_center");
    return ID;
  }

  /// 2D point - focus of the ellipse
  inline static const std::string& FIRST_FOCUS_ID()
  {
    static const std::string ID("ellipse_first_focus");
    return ID;
  }
  /// 2D point - second focus of the ellipse
  inline static const std::string& SECOND_FOCUS_ID()
  {
    static const std::string ID("ellipse_second_focus");
    return ID;
  }

  /// 2D point - start point of major axis
  inline static const std::string& MAJOR_AXIS_START_ID()
  {
    static const std::string ID("ellipse_major_axis_start_point");
    return ID;
  }
  /// 2D point - end point of major axis
  inline static const std::string& MAJOR_AXIS_END_ID()
  {
    static const std::string ID("ellipse_major_axis_end_point");
    return ID;
  }

  /// 2D point - start point of minor axis
  inline static const std::string& MINOR_AXIS_START_ID()
  {
    static const std::string ID("ellipse_minor_axis_start_point");
    return ID;
  }
  /// 2D point - end point of minor axis
  inline static const std::string& MINOR_AXIS_END_ID()
  {
    static const std::string ID("ellipse_minor_axis_end_point");
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
    static std::string MY_KIND = SketchPlugin_Ellipse::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  SketchPlugin_Ellipse();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  bool fillCharacteristicPoints();

  void createEllipse(SketchPlugin_Sketch* theSketch, const int theResultIndex);
};

#endif
