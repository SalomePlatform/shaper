// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#ifndef SRC_SKETCHAPI_SKETCHAPI_MACRO_MIDDLEPOINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_MACRO_MIDDLEPOINT_H_

#include "SketchAPI_SketchEntity.h"

#include <SketchAPI_Point.h>

class ModelHighAPI_RefAttr;


/**\class SketchAPI_MacroMiddlePoint
 * \ingroup CPPHighAPI
 * \brief Interface for Middle Point feature
 */
class SketchAPI_MacroMiddlePoint : public SketchAPI_Point
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroMiddlePoint(const std::shared_ptr<ModelAPI_Feature> & theFeature);

  /// Constructor with line and middle point coordinates
  SKETCHAPI_EXPORT
    SketchAPI_MacroMiddlePoint(const std::shared_ptr<ModelAPI_Feature>& theFeature,
      const ModelHighAPI_RefAttr& theLine,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  static std::string ID()
  {
    static const std::string MY_SKETCH_CONSTRAINT_ID = "MacroConstraintMiddle";
    return MY_SKETCH_CONSTRAINT_ID;
  }
  virtual std::string getID() { return ID(); }

protected:
  void createConstraint(const ModelHighAPI_RefAttr& theLine);
};

#endif
