// File:        ModelAPI_AttributeDocRef.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeDocRef_HeaderFile
#define ModelAPI_AttributeDocRef_HeaderFile

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Document.h"

/**\class ModelAPI_AttributeDocRef
 * \ingroup DataModel
 * \brief Attribute that contains reference to another document.
 */

class MODELAPI_EXPORT ModelAPI_AttributeDocRef : public ModelAPI_Attribute
{
public:
  /// Defines the document referenced from this attribute
  virtual void setValue(std::shared_ptr<ModelAPI_Document> theDoc) = 0;

  /// Returns document referenced from this attribute
  virtual std::shared_ptr<ModelAPI_Document> value() = 0;

  /// Returns the type of this class of attributes
  static std::string type() {return "DocRef";}

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType() {return type();}

protected:
  /// Objects are created for features automatically
  ModelAPI_AttributeDocRef()
  {}
};

#endif
