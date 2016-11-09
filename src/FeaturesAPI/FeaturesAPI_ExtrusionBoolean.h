// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_ExtrusionBoolean.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_ExtrusionBoolean_H_
#define FeaturesAPI_ExtrusionBoolean_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_ExtrusionCut.h>
#include <FeaturesPlugin_ExtrusionFuse.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Reference;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_ExtrusionBoolean
/// \ingroup CPPHighAPI
/// \brief Interface for ExtrusionBoolean feature.
class FeaturesAPI_ExtrusionBoolean: public ModelHighAPI_Interface
{
public:
  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_ExtrusionBoolean();

  INTERFACE_11("",
               sketch, FeaturesPlugin_Extrusion::SKETCH_ID(),
               ModelAPI_AttributeReference, /** Sketch launcher */,
               baseObjects, FeaturesPlugin_Extrusion::BASE_OBJECTS_ID(),
               ModelAPI_AttributeSelectionList, /** Base objects */,
               direction, FeaturesPlugin_Extrusion::DIRECTION_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** Direction */,
               creationMethod, FeaturesPlugin_Extrusion::CREATION_METHOD(),
               ModelAPI_AttributeString, /** Creation method */,
               toSize, FeaturesPlugin_Extrusion::TO_SIZE_ID(),
               ModelAPI_AttributeDouble, /** To size */,
               fromSize, FeaturesPlugin_Extrusion::FROM_SIZE_ID(),
               ModelAPI_AttributeDouble, /** From size */,
               toObject, FeaturesPlugin_Extrusion::TO_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** To object */,
               toOffset, FeaturesPlugin_Extrusion::TO_OFFSET_ID(),
               ModelAPI_AttributeDouble, /** To offset */,
               fromObject, FeaturesPlugin_Extrusion::FROM_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** From object */,
               fromOffset, FeaturesPlugin_Extrusion::FROM_OFFSET_ID(),
               ModelAPI_AttributeDouble, /** From offset */,
               booleanObjects, FeaturesPlugin_CompositeBoolean::OBJECTS_ID(),
               ModelAPI_AttributeSelectionList, /** Boolean objects */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setNestedSketch(const ModelHighAPI_Reference& theSketch);

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify direction_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setDirection(const ModelHighAPI_Selection& theDirection);

  /// Modify CreationMethod, to_size, from_size attributes of the feature.
  FEATURESAPI_EXPORT
  void setSizes(const ModelHighAPI_Double& theToSize, const ModelHighAPI_Double& theFromSize);

  /// Modify creation_method, to_size, from_size attributes of the feature.
  FEATURESAPI_EXPORT
  void setSize(const ModelHighAPI_Double& theSize);

  /// Modify creation_method, to_object, to_offset, from_object,
  /// from_offset attributes of the feature.
  FEATURESAPI_EXPORT
  void setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                           const ModelHighAPI_Double& theToOffset,
                           const ModelHighAPI_Selection& theFromObject,
                           const ModelHighAPI_Double& theFromOffset);

  /// Modiyfy main_objects attribute of the feature.
  FEATURESAPI_EXPORT
  void setBooleanObjects(const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

protected:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionBoolean(const std::shared_ptr<ModelAPI_Feature>& theFeature);

private:
  void execIfBaseNotEmpty();
};

/// \class FeaturesAPI_ExtrusionCut
/// \ingroup CPPHighAPI
/// \brief Interface for ExtrusionCut feature.
class FeaturesAPI_ExtrusionCut: public FeaturesAPI_ExtrusionBoolean
{
public:

  static std::string ID() { return FeaturesPlugin_ExtrusionCut::ID(); }
  virtual std::string getID() { return ID(); }

  //FEATURESAPI_EXPORT
  //virtual std::string getID() {
  //  return FeaturesPlugin_ExtrusionCut::ID();
  //}

  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Double& theSize,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theDirection,
                                    const ModelHighAPI_Double& theSize,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Double& theToSize,
                                    const ModelHighAPI_Double& theFromSize,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theDirection,
                                    const ModelHighAPI_Double& theToSize,
                                    const ModelHighAPI_Double& theFromSize,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theToObject,
                                    const ModelHighAPI_Double& theToOffset,
                                    const ModelHighAPI_Selection& theFromObject,
                                    const ModelHighAPI_Double& theFromOffset,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theDirection,
                                    const ModelHighAPI_Selection& theToObject,
                                    const ModelHighAPI_Double& theToOffset,
                                    const ModelHighAPI_Selection& theFromObject,
                                    const ModelHighAPI_Double& theFromOffset,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);
};

/// Pointer on ExtrusionCut object.
typedef std::shared_ptr<FeaturesAPI_ExtrusionCut> ExtrusionCutPtr;

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionCut feature.
FEATURESAPI_EXPORT
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionCut feature.
FEATURESAPI_EXPORT
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Double& theSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionCut feature.
FEATURESAPI_EXPORT
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theToSize,
                                const ModelHighAPI_Double& theFromSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionCut feature.
FEATURESAPI_EXPORT
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Double& theToSize,
                                const ModelHighAPI_Double& theFromSize,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionCut feature.
FEATURESAPI_EXPORT
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theToObject,
                                const ModelHighAPI_Double& theToOffset,
                                const ModelHighAPI_Selection& theFromObject,
                                const ModelHighAPI_Double& theFromOffset,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionCut feature.
FEATURESAPI_EXPORT
ExtrusionCutPtr addExtrusionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Selection& theDirection,
                                const ModelHighAPI_Selection& theToObject,
                                const ModelHighAPI_Double& theToOffset,
                                const ModelHighAPI_Selection& theFromObject,
                                const ModelHighAPI_Double& theFromOffset,
                                const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \class FeaturesAPI_ExtrusionFuse
/// \ingroup CPPHighAPI
/// \brief Interface for ExtrusionFuse feature.
class FeaturesAPI_ExtrusionFuse: public FeaturesAPI_ExtrusionBoolean
{
public:
  static std::string ID() { return FeaturesPlugin_ExtrusionFuse::ID(); }
  virtual std::string getID() { return ID(); }

  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Double& theSize,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theDirection,
                                     const ModelHighAPI_Double& theSize,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Double& theToSize,
                                     const ModelHighAPI_Double& theFromSize,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theDirection,
                                     const ModelHighAPI_Double& theToSize,
                                     const ModelHighAPI_Double& theFromSize,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theToObject,
                                     const ModelHighAPI_Double& theToOffset,
                                     const ModelHighAPI_Selection& theFromObject,
                                     const ModelHighAPI_Double& theFromOffset,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ExtrusionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theDirection,
                                     const ModelHighAPI_Selection& theToObject,
                                     const ModelHighAPI_Double& theToOffset,
                                     const ModelHighAPI_Selection& theFromObject,
                                     const ModelHighAPI_Double& theFromOffset,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);
};

/// Pointer on ExtrusionFuse object.
typedef std::shared_ptr<FeaturesAPI_ExtrusionFuse> ExtrusionFusePtr;

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionFuse feature.
FEATURESAPI_EXPORT
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionFuse feature.
FEATURESAPI_EXPORT
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Double& theSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionFuse feature.
FEATURESAPI_EXPORT
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Double& theToSize,
                                  const ModelHighAPI_Double& theFromSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionFuse feature.
FEATURESAPI_EXPORT
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Double& theToSize,
                                  const ModelHighAPI_Double& theFromSize,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionFuse feature.
FEATURESAPI_EXPORT
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create ExtrusionFuse feature.
FEATURESAPI_EXPORT
ExtrusionFusePtr addExtrusionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theDirection,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

#endif // FeaturesAPI_ExtrusionBoolean_H_
