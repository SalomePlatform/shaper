// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Translation.h
// Purpose:
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_TRANSLATION_H_
#define SRC_SKETCHAPI_SKETCHAPI_TRANSLATION_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <list>

#include <SketchPlugin_MultiTranslation.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Object;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Translation
 * \ingroup CPPHighAPI
 * \brief Interface for Translation feature
 */
class SketchAPI_Translation : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Translation(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Translation(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                        const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
                        const ModelHighAPI_RefAttr & thePoint1,
                        const ModelHighAPI_RefAttr & thePoint2,
                        const ModelHighAPI_Integer & theNumberOfObjects,
                        bool theFullValue = false);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Translation();

  INTERFACE_7(SketchPlugin_MultiTranslation::ID(),
              translationList, SketchPlugin_MultiTranslation::TRANSLATION_LIST_ID(),
              ModelAPI_AttributeRefList, /** Translation list */,
              startPoint, SketchPlugin_MultiTranslation::START_POINT_ID(),
              ModelAPI_AttributeRefAttr, /** Start point */,
              endPoint, SketchPlugin_MultiTranslation::END_POINT_ID(),
              ModelAPI_AttributeRefAttr, /** Start point */,
              numberOfObjects, SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID(),
              ModelAPI_AttributeInteger, /** Number of objects */,
              valueType, SketchPlugin_MultiTranslation::VALUE_TYPE(),
              ModelAPI_AttributeString, /** Value type */,
              referenceObjects, SketchPlugin_MultiTranslation::ENTITY_A(),
              ModelAPI_AttributeRefList, /** Reference objects */,
              translatedObjects, SketchPlugin_MultiTranslation::ENTITY_B(),
              ModelAPI_AttributeRefList, /** Translationed objects */
  )

  /// List of translated objects
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > translated() const;

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Translation object
typedef std::shared_ptr<SketchAPI_Translation> TranslationPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_TRANSLATION_H_ */
