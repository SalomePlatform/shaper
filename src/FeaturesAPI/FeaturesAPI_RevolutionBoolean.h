// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_RevolutionBoolean.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_RevolutionBoolean_H_
#define FeaturesAPI_RevolutionBoolean_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_CompositeBoolean.h>
#include <FeaturesPlugin_RevolutionCut.h>
#include <FeaturesPlugin_RevolutionFuse.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_RevolutionBoolean
/// \ingroup CPPHighAPI
/// \brief Interface for RevolutionBoolean feature.
class FeaturesAPI_RevolutionBoolean: public ModelHighAPI_Interface
{
public:
  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_RevolutionBoolean();

  INTERFACE_11("",
               sketchLauncher, FeaturesPlugin_Revolution::SKETCH_ID(), ModelAPI_AttributeReference, /** Sketch launcher */,
               baseObjects, FeaturesPlugin_Revolution::BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Base objects */,
               axis, FeaturesPlugin_Revolution::AXIS_OBJECT_ID(), ModelAPI_AttributeSelection, /** Axis */,
               creationMethod, FeaturesPlugin_Revolution::CREATION_METHOD(), ModelAPI_AttributeString, /** Creation method */,
               toAngle, FeaturesPlugin_Revolution::TO_ANGLE_ID(), ModelAPI_AttributeDouble, /** To angle */,
               fromAngle, FeaturesPlugin_Revolution::FROM_ANGLE_ID(), ModelAPI_AttributeDouble, /** From angle */,
               toObject, FeaturesPlugin_Revolution::TO_OBJECT_ID(), ModelAPI_AttributeSelection, /** To object */,
               toOffset, FeaturesPlugin_Revolution::TO_OFFSET_ID(), ModelAPI_AttributeDouble, /** To offset */,
               fromObject, FeaturesPlugin_Revolution::FROM_OBJECT_ID(), ModelAPI_AttributeSelection, /** From object */,
               fromOffset, FeaturesPlugin_Revolution::FROM_OFFSET_ID(), ModelAPI_AttributeDouble, /** From offset */,
               booleanObjects, FeaturesPlugin_CompositeBoolean::OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Boolean objects */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify axis_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setAxis(const ModelHighAPI_Selection& theAxis);

  /// Modify CreationMethod, to_angle, from_angle attributes of the feature.
  FEATURESAPI_EXPORT
  void setAngles(const ModelHighAPI_Double& theToAngle, const ModelHighAPI_Double& theFromAngle);

  /// Modify creation_method, to_angle, from_angle attributes of the feature.
  FEATURESAPI_EXPORT
  void setAngle(const ModelHighAPI_Double& theAngle);

  /// Modify creation_method, to_object, to_offset, from_object, from_offset attributes of the feature.
  FEATURESAPI_EXPORT
  void setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                           const ModelHighAPI_Double& theToOffset,
                           const ModelHighAPI_Selection& theFromObject,
                           const ModelHighAPI_Double& theFromOffset);

  /// Modiyfy main_objects attribute of the feature.
  FEATURESAPI_EXPORT
  void setBooleanObjects(const std::list<ModelHighAPI_Selection>& theBooleanObjects);

protected:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionBoolean(const std::shared_ptr<ModelAPI_Feature>& theFeature);
};

class FeaturesAPI_RevolutionCut: public FeaturesAPI_RevolutionBoolean
{
public:
  FEATURESAPI_EXPORT
  virtual std::string getID() {
    return FeaturesPlugin_RevolutionCut::ID();
  }

  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theAxis,
                                     const ModelHighAPI_Double& theAngle,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theAxis,
                                     const ModelHighAPI_Double& theToAngle,
                                     const ModelHighAPI_Double& theFromAngle,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionCut(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Selection& theAxis,
                                     const ModelHighAPI_Selection& theToObject,
                                     const ModelHighAPI_Double& theToOffset,
                                     const ModelHighAPI_Selection& theFromObject,
                                     const ModelHighAPI_Double& theFromOffset,
                                     const std::list<ModelHighAPI_Selection>& theBooleanObjects);
};

/// Pointer on RevolutionCut object.
typedef std::shared_ptr<FeaturesAPI_RevolutionCut> RevolutionCutPtr;

/// \ingroup CPPHighAPI
/// \brief Create RevolutionCut feature.
FEATURESAPI_EXPORT
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theAngle,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create RevolutionCut feature.
FEATURESAPI_EXPORT
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theToAngle,
                                  const ModelHighAPI_Double& theFromAngle,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create RevolutionCut feature.
FEATURESAPI_EXPORT
RevolutionCutPtr addRevolutionCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset,
                                  const std::list<ModelHighAPI_Selection>& theBooleanObjects);

class FeaturesAPI_RevolutionFuse: public FeaturesAPI_RevolutionBoolean
{
public:
  FEATURESAPI_EXPORT
  virtual std::string getID() {
    return FeaturesPlugin_RevolutionFuse::ID();
  }

  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                      const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                      const ModelHighAPI_Selection& theAxis,
                                      const ModelHighAPI_Double& theAngle,
                                      const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                      const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                      const ModelHighAPI_Selection& theAxis,
                                      const ModelHighAPI_Double& theToAngle,
                                      const ModelHighAPI_Double& theFromAngle,
                                      const std::list<ModelHighAPI_Selection>& theBooleanObjects);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RevolutionFuse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                      const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                      const ModelHighAPI_Selection& theAxis,
                                      const ModelHighAPI_Selection& theToObject,
                                      const ModelHighAPI_Double& theToOffset,
                                      const ModelHighAPI_Selection& theFromObject,
                                      const ModelHighAPI_Double& theFromOffset,
                                      const std::list<ModelHighAPI_Selection>& theBooleanObjects);
};

/// Pointer on RevolutionFuse object.
typedef std::shared_ptr<FeaturesAPI_RevolutionFuse> RevolutionFusePtr;

/// \ingroup CPPHighAPI
/// \brief Create RevolutionFuse feature.
FEATURESAPI_EXPORT
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Double& theAngle,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create RevolutionFuse feature.
FEATURESAPI_EXPORT
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Double& theToAngle,
                                    const ModelHighAPI_Double& theFromAngle,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

/// \ingroup CPPHighAPI
/// \brief Create RevolutionFuse feature.
FEATURESAPI_EXPORT
RevolutionFusePtr addRevolutionFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theAxis,
                                    const ModelHighAPI_Selection& theToObject,
                                    const ModelHighAPI_Double& theToOffset,
                                    const ModelHighAPI_Selection& theFromObject,
                                    const ModelHighAPI_Double& theFromOffset,
                                    const std::list<ModelHighAPI_Selection>& theBooleanObjects);

#endif // FeaturesAPI_RevolutionBoolean_H_
