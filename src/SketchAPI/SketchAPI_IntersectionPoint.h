// Name   : SketchAPI_IntersectionPoint.h
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_INTERSECTIONPOINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_INTERSECTIONPOINT_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_IntersectionPoint.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_IntersectionPoint
 * \ingroup CPPHighAPI
 * \brief Interface for IntersectionPoint feature
 */
class SketchAPI_IntersectionPoint : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_IntersectionPoint(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_IntersectionPoint(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const ModelHighAPI_Selection & theExternal);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_IntersectionPoint(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::string & theExternalName);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_IntersectionPoint();

  INTERFACE_2(SketchPlugin_IntersectionPoint::ID(),
              coordinates, SketchPlugin_IntersectionPoint::COORD_ID(), GeomDataAPI_Point2D, /** IntersectionPoint coordinates */,
              externalLine, SketchPlugin_IntersectionPoint::EXTERNAL_LINE_ID(), ModelAPI_AttributeSelection, /** External line */
  )

  /// Set by external
  SKETCHAPI_EXPORT
  void setByExternalLine(const ModelHighAPI_Selection & theExternalLine);

  /// Set by external name
  SKETCHAPI_EXPORT
  void setByExternalLineName(const std::string & theExternalLineName);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on IntersectionPoint object
typedef std::shared_ptr<SketchAPI_IntersectionPoint> IntersectionPointPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_INTERSECTIONPOINT_H_ */
