// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef SketchPlugin_Circle_H_
#define SketchPlugin_Circle_H_

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>

/**\class SketchPlugin_Circle
 * \ingroup Plugins
 * \brief Feature for creation of the new circle in Sketch.
 */
class SketchPlugin_Circle: public SketchPlugin_SketchEntity
{
 public:
  /// Circle feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchCircle");
    return ID;
  }

  /// 2D point - center of the circle
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID("circle_center");
    return ID;
  }

  /// Is to create contruction point or not
  inline static const std::string& ADD_CONSTRUCTION_POINT_ID()
  {
    static const std::string ID("add_construction_point");
    return ID;
  }

  /// Contain created point as feature
  inline static const std::string& CONSTRUCTION_POINT_REF_ID()
  {
    static const std::string ID("circle_construction_point_ref");
    return ID;
  }

  /// Radius of the circle
  inline static const std::string& RADIUS_ID()
  {
    static const std::string ID("circle_radius");
    return ID;
  }

  /// Angle of rotation sewing point of the circle
  inline static const std::string& ANGLE_ID()
  {
    static const std::string ID("circle_angle");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Circle::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  SketchPlugin_Circle();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

  // Methods for construction point creation
  void computeAngle();
  void setConstructionPoint();
  void removeConstructionPoint();
  std::shared_ptr<GeomAPI_Edge> getCircleShape();
};

#endif
