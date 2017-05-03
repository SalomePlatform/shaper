// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Rectangle.h
// Purpose:
//
// History:
// 17/06/16 - Sergey POKHODENKO - Creation of the file

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
                      const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
                      const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Rectangle();

  INTERFACE_3("SketchRectangle",
              startPoint, "RectStartPoint", GeomDataAPI_Point2D, /** Start point */,
              endPoint, "RectEndPoint", GeomDataAPI_Point2D, /** End point */,
              linesList, "RectangleList", ModelAPI_AttributeRefList, /** Lines list */
  )

  /// Set by coordinates
  SKETCHAPI_EXPORT
  void setByCoordinates(double theX1, double theY1, double theX2, double theY2);

  /// Set by points
  SKETCHAPI_EXPORT
  void setByPoints(const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
                   const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);

  /// List of lines composing rectangle
  SKETCHAPI_EXPORT std::list<std::shared_ptr<SketchAPI_SketchEntity> > lines() const;
};

//! Pointer on Rectangle object
typedef std::shared_ptr<SketchAPI_Rectangle> RectanglePtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_RECTANGLE_H_ */
