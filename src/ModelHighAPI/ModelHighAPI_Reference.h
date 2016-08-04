// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Reference.h
// Created:     04 August 2016
// Author:      Mikhail Ponikarov

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_REFERENCE_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_REFERENCE_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class ModelAPI_Attribute;
class ModelAPI_AttributeReference;
class ModelAPI_AttributeRefList;
class ModelAPI_Object;
class ModelHighAPI_Interface;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Reference
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeReference
 */
class ModelHighAPI_Reference
{
public:
  /// Default constructor
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Reference();
  /// Constructor for object
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Reference(const std::shared_ptr<ModelAPI_Object> & theValue);
  /// Constructor for Interface (use result() for object)
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Reference(const std::shared_ptr<ModelHighAPI_Interface> & theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_Reference();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT
  virtual void fillAttribute(const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute) const;

  /// Append to list attribute
  MODELHIGHAPI_EXPORT
  virtual void appendToList(const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute) const;

private:
  std::shared_ptr<ModelAPI_Object> myObject;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_ */
