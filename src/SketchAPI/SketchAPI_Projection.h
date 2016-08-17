// Name   : SketchAPI_Projection.h
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_PROJECTION_H_
#define SRC_SKETCHAPI_SKETCHAPI_PROJECTION_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_Projection.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Projection
 * \ingroup CPPHighAPI
 * \brief Interface for Projection feature
 */
class SketchAPI_Projection : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Projection(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Projection(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const ModelHighAPI_Selection & theExternalFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Projection(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const std::string & theExternalName);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Projection();

  INTERFACE_3(SketchPlugin_Projection::ID(),
              externalFeature, SketchPlugin_Projection::EXTERNAL_FEATURE_ID(), ModelAPI_AttributeSelection, /** External feature */,
              projectedFeature, SketchPlugin_Projection::PROJECTED_FEATURE_ID(), ModelAPI_AttributeRefAttr, /** Projected feature */,
              external, SketchPlugin_Projection::EXTERNAL_ID(), ModelAPI_AttributeSelection, /** External */
  )

  /// Set external feature
  SKETCHAPI_EXPORT
  void setExternalFeature(const ModelHighAPI_Selection & theExternalLine);

  /// Set by external name
  SKETCHAPI_EXPORT
  void setByExternalName(const std::string & theExternalName);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Projection object
typedef std::shared_ptr<SketchAPI_Projection> ProjectionPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_PROJECTION_H_ */
