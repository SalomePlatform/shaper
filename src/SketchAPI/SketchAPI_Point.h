// Name   : SketchAPI_Point.h
// Purpose: 
//
// History:
// 15/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_POINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_POINT_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_Point.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Point
 * \ingroup CPPHighAPI
 * \brief Interface for Point feature
 */
class SketchAPI_Point : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 double theX, double theY);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const ModelHighAPI_Selection & theExternal);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::string & theExternalName);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Point();

  INTERFACE_2(SketchPlugin_Point::ID(),
              coordinates, SketchPlugin_Point::COORD_ID(), GeomDataAPI_Point2D, /** Point coordinates */,
              external, SketchPlugin_Point::EXTERNAL_ID(), ModelAPI_AttributeSelection, /** External */
  )

  /// Set by coordinates
  SKETCHAPI_EXPORT
  void setCoordinates(double theX, double theY);

  /// Set by points
  SKETCHAPI_EXPORT
  void setCoordinates(const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);

  /// Set by external
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection & theExternal);

  /// Set by external name
  SKETCHAPI_EXPORT
  void setByExternalName(const std::string & theExternalName);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Point object
typedef std::shared_ptr<SketchAPI_Point> PointPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_POINT_H_ */
