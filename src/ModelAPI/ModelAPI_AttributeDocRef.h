// File:        ModelAPI_AttributeDocRef.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeDocRef_H_
#define ModelAPI_AttributeDocRef_H_

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Document.h"

/**\class ModelAPI_AttributeDocRef
 * \ingroup DataModel
 * \brief Attribute that contains reference to another document.
 */

class ModelAPI_AttributeDocRef : public ModelAPI_Attribute
{
 public:
  /// Defines the document referenced from this attribute
  MODELAPI_EXPORT virtual void setValue(boost::shared_ptr<ModelAPI_Document> theDoc) = 0;

  /// Returns document referenced from this attribute
  MODELAPI_EXPORT virtual boost::shared_ptr<ModelAPI_Document> value() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type()
  {
    return "DocRef";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType()
  {
    return type();
  }

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeDocRef()
  {
  }

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeDocRef()
  {
  }
};

typedef boost::shared_ptr<ModelAPI_AttributeDocRef> AttributeDocRefPtr;

#endif
