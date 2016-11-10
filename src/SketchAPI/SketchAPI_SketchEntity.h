// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_SketchEntity.h
// Purpose:
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_SKETCHAPI_SKETCHAPI_SKETCHENTITY_H_
#define SRC_SKETCHAPI_SKETCHAPI_SKETCHENTITY_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_SketchEntity.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelAPI_Feature;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_SketchEntity
 * \ingroup CPPHighAPI
 * \brief Base class for Sketch feature interfaces
 */
class SketchAPI_SketchEntity : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_SketchEntity(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_SketchEntity();

  /// Auxiliary
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelAPI_AttributeBoolean> auxiliary() const;

  /// Set auxiliary
  SKETCHAPI_EXPORT
  void setAuxiliary(bool theAuxiliary);

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

  /// Convert list of features to list of appropriate wrappers
  SKETCHAPI_EXPORT
  static std::list<std::shared_ptr<ModelHighAPI_Interface> >
  wrap(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures);

protected:
  std::shared_ptr<ModelAPI_AttributeBoolean> myAuxiliary;

  bool initialize();

  /// Check the entity is a copy of another feature
  bool isCopy() const;
};

//! Pointer on SketchEntity object
typedef std::shared_ptr<SketchAPI_SketchEntity> SketchEntityPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_SKETCHENTITY_H_ */
