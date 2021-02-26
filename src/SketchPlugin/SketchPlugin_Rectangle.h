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

#ifndef SketchPlugin_Rectangle_H_
#define SketchPlugin_Rectangle_H_

#include "SketchPlugin.h"
#include "SketchPlugin_SketchEntity.h"

/**\class SketchPlugin_Rectangle
 * \ingroup Plugins
 * \brief Feature for creation of the new Rectangle in Sketch.
 */
class SketchPlugin_Rectangle: public SketchPlugin_SketchEntity
{
 public:
  /// Rectangle feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchRectangle");
    return ID;
  }

  /// 2D point - start point of the Rectangle
  inline static const std::string& START_ID()
  {
    static const std::string ID("rectangle_start_point");
    return ID;
  }

  /// 2D point - end point of the Rectangle
  inline static const std::string& END_ID()
  {
    static const std::string ID("rectangle_end_point");
    return ID;
  }

  /// 2D point - center point of the Rectangle
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID("rectangle_center_point");
    return ID;
  }

  /// 2D point - center point of the Rectangle
  inline static const std::string& CENTER_REF_ID()
  {
    static const std::string ID("rectangle_center_point_ref");
    return ID;
  }

  /// 2D point - list of Rectangle lines
  inline static const std::string& LINES_LIST_ID()
  {
    static const std::string ID("RectangleLinesList");
    return ID;
  }

  /// 2D point - list of Diagonal lines
  inline static const std::string& DIAGONAL_LIST_ID()
  {
    static const std::string ID("RectangleDiagonalLinesList");
    return ID;
  }

  inline static const std::string& ISHV_LIST_ID()
  {
    static const std::string ID("IsHVList");
    return ID;
  }

  inline static const std::string& NOT_TO_DUMP_LIST_ID()
  {
    static const std::string ID("NotToDumpList");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Rectangle::ID();
    return MY_KIND;
  }

  SKETCHPLUGIN_EXPORT virtual bool isMacro() const
  { return true;}

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation
  SketchPlugin_Rectangle();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// \brief updateLines crates lines from start and en points
  void updateLines();
  void updateStartPoint();
};


#endif
