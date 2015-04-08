// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Attribute.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Attribute_H_
#define ModelAPI_Attribute_H_

#include "ModelAPI.h"
#include <string>
#include <memory>

class ModelAPI_Object;

/**\class ModelAPI_Attribute
 * \ingroup DataModel
 * \brief Generic attribute of the Object.
 */
class ModelAPI_Attribute
{
  ///< needed here to emit signal that feature changed on change of the attribute
  std::shared_ptr<ModelAPI_Object> myObject;
  std::string myID; ///< identifier of this attribute in Data class
 protected:
  // accessible from the attributes
  bool myIsInitialized; ///< is some value assigned to this attribute
  bool mySetInitializedBlocked; ///< is initialized blocked
  bool myIsArgument;    ///< is this attribute used as an argument for execution
  bool myIsImmutable;   ///< is this attribute can be changed programmatically (e.g. by constraint)

 public:

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType() = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Attribute();

  /// Sets the owner of this attribute
  MODELAPI_EXPORT virtual void setObject(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Returns the owner of this attribute
  MODELAPI_EXPORT const std::shared_ptr<ModelAPI_Object>& owner() const;

  /// Returns true if attribute was  initialized by some value
  MODELAPI_EXPORT bool isInitialized();

  /// Makes attribute initialized
  MODELAPI_EXPORT void setInitialized();

  /// Blocks sending "attribute updated" if theBlock is true
  /// \param theBlock a block value
  /// \return the previous block value
  MODELAPI_EXPORT bool blockSetInitialized(const bool theBlock);

  /// Set this attribute is argument for result (change of this attribute requires update of result).
  /// By default it is true.
  MODELAPI_EXPORT void setIsArgument(const bool theFlag);

  /// Returns true if attribute causes the result change
  MODELAPI_EXPORT bool isArgument();

  /// Immutable argument can not be changed programaticaly (e.g. by constraint)
  /// By default it is false.
  /// Returns the previous state of the attribute's immutability.
  MODELAPI_EXPORT bool setImmutable(const bool theFlag);

  /// Returns true if can not be changed programaticaly
  MODELAPI_EXPORT bool isImmutable();

  /// ID of the attribute in Data
  MODELAPI_EXPORT const std::string& id() const;

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_Attribute();

  /// Sets the ID of the attribute in Data (called from Data)
  MODELAPI_EXPORT virtual void setID(const std::string theID);

  friend class Model_Data;
};

//! Pointer on attribute object
typedef std::shared_ptr<ModelAPI_Attribute> AttributePtr;

#endif
