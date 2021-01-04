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

#ifndef SketchPlugin_MacroRectangle_H_
#define SketchPlugin_MacroRectangle_H_

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <GeomAPI_IPresentable.h>

class GeomAPI_Pnt2d;

/**\class SketchPlugin_MacroRectangle
 * \ingroup Plugins
 * \brief Feature for creation of the new Rectangle in Sketch.
 */
class SketchPlugin_MacroRectangle: public SketchPlugin_SketchEntity,  public GeomAPI_IPresentable
{
public:
  /// Rectangle feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroRectangle");
    return ID;
  }

  inline static const std::string& TYPE_ID()
  {
    static const std::string ID("rectangle_type");
    return ID;
  }

  inline static const std::string& EDIT_TYPE_ID()
  {
    static const std::string ID("edit_rectangle_type");
    return ID;
  }

  inline static const std::string& START_END_POINT_TYPE_ID()
  {
    static const std::string ID("rectangle_type_by_start_and_end_points");
    return ID;
  }

  inline static const std::string& START_CENTER_POINT_TYPE_ID()
  {
    static const std::string ID("rectangle_type_by_start_and_center_points");
    return ID;
  }

  /// 2D point - start point of the Rectangle
  inline static const std::string& START1_ID()
  {
    static const std::string ID("rectangle_start_point1");
    return ID;
  } 
  /// 2D point - end point of the Rectangle
  inline static const std::string& END1_ID()
  {
    static const std::string ID("rectangle_end_point1");
    return ID;
  }
  /// 2D point - start point of the second Rectangle type
  inline static const std::string& START2_ID()
  {
    static const std::string ID("rectangle_start_point2");
    return ID;
  }

   /// 2D point - center point of the second Rectangle type
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID("rectangle_center_point");
    return ID;
  } 

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroRectangle::ID();
    return MY_KIND;
  }

   /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  SketchPlugin_MacroRectangle();

  SKETCHPLUGIN_EXPORT virtual bool isMacro() const
  {return true;}

  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;}

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

private:

  std::shared_ptr<GeomAPI_Pnt2d> myStartPoint;
  std::shared_ptr<GeomAPI_Pnt2d> myEndPoint;

  void startPoint();
  void endPoint();
  FeaturePtr createRectangle();
};

#endif
