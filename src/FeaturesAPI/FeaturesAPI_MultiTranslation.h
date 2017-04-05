// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_MultiTranslation.h
// Created:     09 Feb 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESAPI_MULTITRANSLATION_H_
#define FEATURESAPI_MULTITRANSLATION_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_MultiTranslation.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Integer;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_MultiTranslation
/// \ingroup CPPHighAPI
/// \brief Interface for Translation feature.
class FeaturesAPI_MultiTranslation: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_MultiTranslation(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_MultiTranslation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theAxisObject,
                                        const ModelHighAPI_Double& theStep,
                                        const ModelHighAPI_Integer& theNumber);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_MultiTranslation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theFirstAxisObject,
                                        const ModelHighAPI_Double& theFirstStep,
                                        const ModelHighAPI_Integer& theFirstNumber,
                                        const ModelHighAPI_Selection& theSecondAxisObject,
                                        const ModelHighAPI_Double& theSecondStep,
                                        const ModelHighAPI_Integer& theSecondNumber);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_MultiTranslation();

  INTERFACE_8(FeaturesPlugin_MultiTranslation::ID(),
              mainObjects, FeaturesPlugin_MultiTranslation::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              firstAxisObject, FeaturesPlugin_MultiTranslation::AXIS_FIRST_DIR_ID(),
              ModelAPI_AttributeSelection, /** First axis object */,
              firstStep, FeaturesPlugin_MultiTranslation::STEP_FIRST_DIR_ID(),
              ModelAPI_AttributeDouble, /** First step */,
              firstNumber, FeaturesPlugin_MultiTranslation::NB_COPIES_FIRST_DIR_ID(),
              ModelAPI_AttributeInteger, /** First number of copies */,
              useSecondDir, FeaturesPlugin_MultiTranslation::USE_SECOND_DIR_ID(),
              ModelAPI_AttributeString, /** Use the second dir */,
              secondAxisObject, FeaturesPlugin_MultiTranslation::AXIS_SECOND_DIR_ID(),
              ModelAPI_AttributeSelection, /** Second axis object */,
              secondStep, FeaturesPlugin_MultiTranslation::STEP_SECOND_DIR_ID(),
              ModelAPI_AttributeDouble, /** Second step */,
              secondNumber, FeaturesPlugin_MultiTranslation::NB_COPIES_SECOND_DIR_ID(),
              ModelAPI_AttributeInteger, /** Second number of copies */
             )

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify CreationMethod, axis_first_dir, step_first_dir attributes of the feature.
  FEATURESAPI_EXPORT
  void setFirstAxisAndDistance(const ModelHighAPI_Selection& theAxisObject,
                               const ModelHighAPI_Double& theDistance);

  /// Modify CreationMethod, axis_second_dir, step_second_dir attributes of the feature.
  FEATURESAPI_EXPORT
  void setSecondAxisAndDistance(const ModelHighAPI_Selection& theAxisObject,
                                const ModelHighAPI_Double& theDistance);

  /// Modify CreationMethod, nb_first_dir attribute of the feature.
  FEATURESAPI_EXPORT
  void setFirstNumber(const ModelHighAPI_Integer& theFirstNumber);

  /// Modify CreationMethod, nb_second_dir attribute of the feature.
  FEATURESAPI_EXPORT
  void setSecondNumber(const ModelHighAPI_Integer& theSecondNumber);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on MultiTranslation object.
typedef std::shared_ptr<FeaturesAPI_MultiTranslation> MultiTranslationPtr;

/// \ingroup CPPHighAPI
/// \brief Create MultiTranslation feature.
FEATURESAPI_EXPORT
MultiTranslationPtr addMultiTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theAxisObject,
                                        const ModelHighAPI_Double& theStep,
                                        const ModelHighAPI_Integer& theNumber);

/// \ingroup CPPHighAPI
/// \brief Create MultiTranslation feature.
FEATURESAPI_EXPORT
MultiTranslationPtr addMultiTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                                        const ModelHighAPI_Selection& theFirstAxisObject,
                                        const ModelHighAPI_Double& theFirstStep,
                                        const ModelHighAPI_Integer& theFirstNumber,
                                        const ModelHighAPI_Selection& theSecondAxisObject,
                                        const ModelHighAPI_Double& theSecondStep,
                                        const ModelHighAPI_Integer& theSecondNumber);

#endif // FEATURESAPI_MULTITRANSLATION_H_
