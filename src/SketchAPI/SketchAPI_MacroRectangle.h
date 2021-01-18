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

#ifndef SketchAPI_MacroRectangle_H_
#define SketchAPI_MacroRectangle_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"
#include <SketchPlugin_MacroRectangle.h>

class ModelHighAPI_Selection;

/// \class SketchAPI_MacroRectangle
/// \ingroup CPPHighAPI
/// \brief Interface for Rectangle feature.
class SketchAPI_MacroRectangle: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroRectangle(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroRectangle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        double theStartX,
                        double theStartY,
                        double theSecondX,
                        double theSecondY, bool isFirstPointCenter = false);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroRectangle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theStartPoint,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint, bool isFirstPointCenter = false);


  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_MacroRectangle();

  INTERFACE_5(SketchPlugin_MacroRectangle::ID(),
              rectangleType, SketchPlugin_MacroRectangle::RECTANGLE_TYPE_ID(),
              ModelAPI_AttributeString, /** Rectangle type */,
              startPoint1, SketchPlugin_MacroRectangle::START1_ID(),
              GeomDataAPI_Point2D, /** Start point 1 */,
              endPoint1, SketchPlugin_MacroRectangle::END1_ID(),
              GeomDataAPI_Point2D, /** End point 1 */,
              endPoint2, SketchPlugin_MacroRectangle::END2_ID(),
              GeomDataAPI_Point2D, /** First point 2 */,
              centerPoint, SketchPlugin_MacroRectangle::CENTER_ID(),
              GeomDataAPI_Point2D, /** Center point */)

  /*SKETCHAPI_EXPORT
  void dump(ModelHighAPI_Dumper& theDumper) const;
*/
private:
  /// Set by start  and end points.
  void setByStartAndEndPoints(double theStartX, double theStartY,
                                  double theEndX, double theEndY);

  /// Set by start and end points.
  void setByStartAndEndPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theStartPoint,
                                  const std::shared_ptr<GeomAPI_Pnt2d>& theEndPoint);

  /// Set by start  and center points.
  void setByCenterAndEndPoints(double theCenterX, double theCenterY,
                                  double theEndX, double theEndY);

  /// Set by start and center points.
  void setByCenterAndEndPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
                                  const std::shared_ptr<GeomAPI_Pnt2d>& theEndPoint);
};

/// Pointer on Rectangle object.
typedef std::shared_ptr<SketchAPI_MacroRectangle> MacroRectanglePtr;

#endif // SketchAPI_MacroRectangle_H_
