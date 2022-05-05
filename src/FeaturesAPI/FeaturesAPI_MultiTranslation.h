// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef FEATURESAPI_MULTITRANSLATION_H_
#define FEATURESAPI_MULTITRANSLATION_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_MultiTranslation.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Selection.h>

class ModelHighAPI_Dumper;

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
FEATURESAPI_EXPORT MultiTranslationPtr addMultiTranslation(
    const std::shared_ptr<ModelAPI_Document>& thePart,
    const std::list<ModelHighAPI_Selection>& theMainObjects,
    const ModelHighAPI_Selection& theFirstAxisObject,
    const ModelHighAPI_Double& theFirstStep,
    const ModelHighAPI_Integer& theFirstNumber,
    const ModelHighAPI_Selection& theSecondAxisObject = ModelHighAPI_Selection(),
    const ModelHighAPI_Double& theSecondStep = ModelHighAPI_Double(),
    const ModelHighAPI_Integer& theSecondNumber = ModelHighAPI_Integer(),
    const bool keepSubResults = false);

#endif // FEATURESAPI_MULTITRANSLATION_H_
