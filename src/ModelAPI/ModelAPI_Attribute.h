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
  bool myIsArgument;    ///< is this attribute used as an argument for execution
  bool myIsImmutable;   ///< is this attribute can be changed programmatically (e.g. by constraint)

 public:

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType() = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Attribute()
  {
  }

  /// Sets the owner of this attribute
  MODELAPI_EXPORT virtual void setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
  {
    myObject = theObject;
  }

  /// Returns the owner of this attribute
  MODELAPI_EXPORT const std::shared_ptr<ModelAPI_Object>& owner() const
  {
    return myObject;
  }

  /// Returns true if attribute was  initialized by some value
  MODELAPI_EXPORT bool isInitialized()
  {
    return myIsInitialized;
  }

  /// Makes attribute initialized
  MODELAPI_EXPORT void setInitialized()
  {
    myIsInitialized = true;
  }

  /// Set this attribute is argument for result (change of this attribute requires update of result).
  /// By default it is true.
  MODELAPI_EXPORT void setIsArgument(const bool theFlag)
  {
    myIsArgument = theFlag;
  }

  /// Returns true if attribute causes the result change
  MODELAPI_EXPORT bool isArgument()
  {
    return myIsArgument;
  }

  /// Immutable argument can not be changed programaticaly (e.g. by constraint)
  /// By default it is false.
  /// Returns the previous state of the attribute's immutability.
  MODELAPI_EXPORT bool setImmutable(const bool theFlag)
  {
    bool aResult = myIsImmutable;
    myIsImmutable = theFlag;
    return aResult;
  }

  /// Returns true if can not be changed programaticaly
  MODELAPI_EXPORT bool isImmutable()
  {
    return myIsImmutable;
  }

  /// ID of the attribute in Data
  MODELAPI_EXPORT const std::string& id() const
  {
    return myID;
  }

 protected:
  /// Objects are created for features automatically
  ModelAPI_Attribute()
  {
    myIsInitialized = false;
    myIsArgument = true;
    myIsImmutable = false;
  }

  /// Sets the ID of the attribute in Data (called from Data)
  MODELAPI_EXPORT void setID(const std::string theID)
  {
    myID = theID;
  }

  friend class Model_Data;
};

//! Pointer on attribute object
typedef std::shared_ptr<ModelAPI_Attribute> AttributePtr;

#endif
