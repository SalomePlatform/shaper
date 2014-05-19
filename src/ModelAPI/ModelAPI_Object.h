// File:        ModelAPI_Object.hxx
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Object_HeaderFile
#define ModelAPI_Object_HeaderFile

#include "ModelAPI_Feature.h"

/**\class ModelAPI_Object
 * \ingroup DataModel
 * \brief Represents the result of some feature in the object browser
 * under the specific folder. Just a reference to specific feature-operation
 * with possibility to rename it.
 */
class ModelAPI_Object : public ModelAPI_Feature
{
public:

  /// It is never located in history
  MODELAPI_EXPORT virtual bool isInHistory() {return false;}

  /// Reference to the feature-operation that produces this object
  MODELAPI_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> featureRef() = 0;

  /// Returns the name of this object (by default equal to the name of feature)
  MODELAPI_EXPORT virtual std::string getName() = 0;

  /// Defines the name of the object
  MODELAPI_EXPORT virtual void setName(std::string theName) = 0;
};

#endif
