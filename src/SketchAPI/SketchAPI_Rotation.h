// Name   : SketchAPI_Rotation.h
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_ROTATION_H_
#define SRC_SKETCHAPI_SKETCHAPI_ROTATION_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <list>

#include <SketchPlugin_MultiRotation.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelAPI_Object;
class ModelHighAPI_Double;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Rotation
 * \ingroup CPPHighAPI
 * \brief Interface for Rotation feature
 */
class SketchAPI_Rotation : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Rotation(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Rotation(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                     const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
                     const ModelHighAPI_RefAttr & theCenter,
                     const ModelHighAPI_Double & theAngle,
                     const ModelHighAPI_Integer & theNumberOfObjects,
                     bool theFullValue = false);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Rotation();

  INTERFACE_7(SketchPlugin_MultiRotation::ID(),
              rotationList, SketchPlugin_MultiRotation::ROTATION_LIST_ID(), ModelAPI_AttributeRefList, /** Rotation list */,
              center, SketchPlugin_MultiRotation::CENTER_ID(), ModelAPI_AttributeRefAttr, /** Center */,
              angle, SketchPlugin_MultiRotation::ANGLE_ID(), ModelAPI_AttributeDouble, /** Angle */,
              numberOfObjects, SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID(), ModelAPI_AttributeInteger, /** Number of objects */,
              valueType, SketchPlugin_MultiRotation::ANGLE_TYPE(), ModelAPI_AttributeString, /** Value type */,
              referenceObjects, SketchPlugin_MultiRotation::ENTITY_A(), ModelAPI_AttributeRefList, /** Reference objects */,
              rotatedObjects, SketchPlugin_MultiRotation::ENTITY_B(), ModelAPI_AttributeRefList, /** Rotated objects */
  )

};

//! Pointer on Rotation object
typedef std::shared_ptr<SketchAPI_Rotation> RotationPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_ROTATION_H_ */
