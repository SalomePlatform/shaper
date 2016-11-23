// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionAPI_Field.h
// Created:     16 Nov 2016
// Author:      Mikhail Ponikarov

#ifndef CollectionAPI_Field_H_
#define CollectionAPI_Field_H_

#include "CollectionAPI.h"

#include <CollectionPlugin_Field.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;
class ModelHighAPI_Integer;
class ModelHighAPI_ComponentValue;

/// \class CollectionAPI_Field
/// \inField CPPHighAPI
/// \brief Interface for Field feature.
class CollectionAPI_Field: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  COLLECTIONAPI_EXPORT
  explicit CollectionAPI_Field(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Destructor.
  COLLECTIONAPI_EXPORT
  virtual ~CollectionAPI_Field();

  INTERFACE_4(CollectionPlugin_Field::ID(),
    selection, CollectionPlugin_Field::SELECTED_ID(),
    ModelAPI_AttributeSelectionList, /** Field selection list*/,
    componentsNames, CollectionPlugin_Field::COMPONENTS_NAMES_ID(),
    ModelAPI_AttributeStringArray, /** Names of components list of strings */,
    stamps, CollectionPlugin_Field::STAMPS_ID(),
    ModelAPI_AttributeIntArray, /** Identifiers of stamps */,
    values, CollectionPlugin_Field::VALUES_ID(), ModelAPI_AttributeTables /** Table of values */,
    )

  /// Set selected objects.
  COLLECTIONAPI_EXPORT
  void setSelection(const std::list<ModelHighAPI_Selection>& theFieldList);
  /// Set names of components
  COLLECTIONAPI_EXPORT
  void setComponentsNames(const std::list<std::string>& theNames);
  /// Set type of values
  COLLECTIONAPI_EXPORT
  void setValuesType(const std::string& theType);
  /// Set number of steps
  COLLECTIONAPI_EXPORT
  void setStepsNum(const ModelHighAPI_Integer& theSteps);
  /// Set stamps identifiers
  COLLECTIONAPI_EXPORT
  void setStamps(const std::list<ModelHighAPI_Integer>& theStamps);
  /// Sets the double values of specific step
  COLLECTIONAPI_EXPORT
  void addStep(const ModelHighAPI_Integer& theStepNum, const ModelHighAPI_Integer& theStamp,
    const std::list<std::list<double> >& theComponents);
  /// Sets the integer values of specific step
  COLLECTIONAPI_EXPORT
  void addStep(const ModelHighAPI_Integer& theStepNum, const ModelHighAPI_Integer& theStamp,
    const std::list<std::list<int> >& theComponents);
  /// Sets the string values of specific step
  COLLECTIONAPI_EXPORT
  void addStep(const ModelHighAPI_Integer& theStepNum, const ModelHighAPI_Integer& theStamp,
    const std::list<std::list<std::string> >& theComponents);
  /// Sets the boolean values of specific step
  COLLECTIONAPI_EXPORT
  void addStep(const ModelHighAPI_Integer& theStepNum, const ModelHighAPI_Integer& theStamp,
    const std::list<std::list<bool> >& theComponents);

  /// Dump wrapped feature
  COLLECTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

  /// Returns the internal values tables
  COLLECTIONAPI_EXPORT
  std::shared_ptr<ModelAPI_AttributeTables> tableValues();

};

/// Pointer on Field object.
typedef std::shared_ptr<CollectionAPI_Field> FieldPtr;

/// \inField CPPHighAPI
/// \brief Create Field feature.
COLLECTIONAPI_EXPORT
FieldPtr addField(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Integer& theStepsNum,
                  const std::string& theComponentType,
                  const int theComponentsNum,
                  const std::list<std::string>& theComponentNames,
                  const std::list<ModelHighAPI_Selection>& theSelectionList);

#endif // CollectionAPI_Field_H_
