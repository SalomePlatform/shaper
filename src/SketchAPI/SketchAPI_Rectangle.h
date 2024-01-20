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

#ifndef SRC_SKETCHAPI_SKETCHAPI_RECTANGLE_H_
#define SRC_SKETCHAPI_SKETCHAPI_RECTANGLE_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include "SketchAPI_SketchEntity.h"

//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Rectangle
 * \ingroup CPPHighAPI
 * \brief Interface for Rectangle feature
 */
class SketchAPI_Rectangle : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Rectangle(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Rectangle(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                      double theX1, double theY1, double theX2, double theY2);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Rectangle(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                      const std::shared_ptr<GeomAPI_Pnt2d> & theFirstPoint,
                      const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Rectangle();

  INTERFACE_7("SketchRectangle",
              type, "RectangleType", ModelAPI_AttributeString,
              /** Creation type of rectangle */,
              startPoint, "RectStartPoint", GeomDataAPI_Point2D,
              /** Start point */,
              endPoint, "RectEndPoint", GeomDataAPI_Point2D,
              /** End point */,
              centerPoint, "RectCenterPoint", GeomDataAPI_Point2D,
              /** Center point */,
              centerPointRef, "RectCenterPointRef", ModelAPI_AttributeRefAttr,
              /** Reference to center point */,
              cornerPoint, "RectCornerPoint", GeomDataAPI_Point2D,
              /** Corner point */,
              linesList, "RectangleList", ModelAPI_AttributeRefList,
              /** Lines list */
  )

  /// Set by coordinates
  SKETCHAPI_EXPORT
  void setByCoordinates(double theX1, double theY1, double theX2, double theY2);

  /// Set by points
  SKETCHAPI_EXPORT
  void setByPoints(const std::shared_ptr<GeomAPI_Pnt2d> & theFirstPoint,
                   const std::shared_ptr<GeomAPI_Pnt2d> & theSecondPoint);

  /// List of lines composing rectangle
  SKETCHAPI_EXPORT std::list<std::shared_ptr<SketchAPI_SketchEntity> > lines() const;
};

//! Pointer on Rectangle object
typedef std::shared_ptr<SketchAPI_Rectangle> RectanglePtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_RECTANGLE_H_ */
