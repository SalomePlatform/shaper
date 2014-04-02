// File:        Model_AttributeDocRef.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeDocRef_HeaderFile
#define Model_AttributeDocRef_HeaderFile

#include "Model.h"
#include "ModelAPI_AttributeDocRef.h"
#include <TDataStd_Comment.hxx>
#include <TDF_Label.hxx>

/**\class Model_AttributeDocRef
 * \ingroup DataModel
 * \brief Attribute that contains reference to another document.
 */

class MODEL_EXPORT Model_AttributeDocRef : public ModelAPI_AttributeDocRef
{
  Handle_TDataStd_Comment myComment; ///< reference to document is identified as string-id
public:
  /// Defines the document referenced from this attribute
  virtual void setValue(std::shared_ptr<ModelAPI_Document> theDoc);

  /// Returns document referenced from this attribute
  virtual std::shared_ptr<ModelAPI_Document> value();

protected:
  /// Initializes attibutes
  Model_AttributeDocRef(TDF_Label& theLabel);

  friend class Model_Object;
};

#endif
