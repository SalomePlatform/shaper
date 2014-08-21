// File:        Model_AttributeDocRef.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeDocRef_H_
#define Model_AttributeDocRef_H_

#include "Model.h"
#include "ModelAPI_AttributeDocRef.h"
#include <TDataStd_Comment.hxx>
#include <TDF_Label.hxx>

/**\class Model_AttributeDocRef
 * \ingroup DataModel
 * \brief Attribute that contains reference to another document.
 */

class Model_AttributeDocRef : public ModelAPI_AttributeDocRef
{
  Handle_TDataStd_Comment myComment;  ///< reference to document is identified as string-id
 public:
  /// Defines the document referenced from this attribute
  MODEL_EXPORT virtual void setValue(boost::shared_ptr<ModelAPI_Document> theDoc);

  /// Returns document referenced from this attribute
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> value();

 protected:
  /// Initializes attibutes
  Model_AttributeDocRef(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
