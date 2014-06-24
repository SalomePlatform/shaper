// File:        ModelAPI_Attribute.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Attribute_HeaderFile
#define ModelAPI_Attribute_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;

/**\class ModelAPI_Attribute
 * \ingroup DataModel
 * \brief Generic attribute of the Object.
 */
class ModelAPI_Attribute
{
  ///< needed here to emit signal that feature changed on change of the attribute
  boost::shared_ptr<ModelAPI_Feature> myFeature;
protected: // accessible from the attributes
  bool myIsInitialized;
public:
  
  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType() = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Attribute() {}

  /// Sets the owner of this attribute
  MODELAPI_EXPORT void setFeature(const boost::shared_ptr<ModelAPI_Feature>& theFeature)
    {myFeature = theFeature;}

  /// Returns the owner of this attribute
  MODELAPI_EXPORT const boost::shared_ptr<ModelAPI_Feature>& owner()
  {return myFeature;}

  /// Returns true if attribute was  initialized by some value
  MODELAPI_EXPORT bool isInitialized() {return myIsInitialized;}

  /// Makes attribute initialized
  MODELAPI_EXPORT void setInitialized() {myIsInitialized = true;}

protected:
  /// Objects are created for features automatically
  ModelAPI_Attribute() {myIsInitialized = false;}

};

#endif
