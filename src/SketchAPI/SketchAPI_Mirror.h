// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Mirror.h
// Purpose:
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_MIRROR_H_
#define SRC_SKETCHAPI_SKETCHAPI_MIRROR_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <list>

#include <SketchPlugin_ConstraintMirror.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Object;
class ModelHighAPI_RefAttr;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Mirror
 * \ingroup CPPHighAPI
 * \brief Interface for Mirror feature
 */
class SketchAPI_Mirror : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Mirror(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Mirror(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const ModelHighAPI_RefAttr & theMirrorLine,
                   const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Mirror();

  INTERFACE_4(SketchPlugin_ConstraintMirror::ID(),
              mirrorLine, SketchPlugin_ConstraintMirror::ENTITY_A(),
              ModelAPI_AttributeRefAttr, /** Mirror line */,
              mirrorList, SketchPlugin_ConstraintMirror::MIRROR_LIST_ID(),
              ModelAPI_AttributeRefList, /** Mirror list */,
              referenceObjects, SketchPlugin_ConstraintMirror::ENTITY_B(),
              ModelAPI_AttributeRefList, /** Reference objects */,
              mirroredObjects, SketchPlugin_ConstraintMirror::ENTITY_C(),
              ModelAPI_AttributeRefList, /** Mirrored objects */
  )

  /// List of mirrored objects
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<ModelHighAPI_Interface> > mirrored() const;

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Mirror object
typedef std::shared_ptr<SketchAPI_Mirror> MirrorPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_MIRROR_H_ */
