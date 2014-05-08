// File:        Model_AttributeRefAttr.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeRefAttr_HeaderFile
#define Model_AttributeRefAttr_HeaderFile

#include "Model.h"
#include "ModelAPI_AttributeRefAttr.h"
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDataStd_Comment.hxx>

/**\class Model_AttributeRefAttr
 * \ingroup DataModel
 * \brief Attribute that contains reference to an attribute of a feature 
 * (located in the same document).
 */

class Model_AttributeRefAttr : public ModelAPI_AttributeRefAttr
{
  Handle_TDF_Reference myRef; ///< reference to the feature label
  Handle_TDataStd_Comment myID; ///< ID of the referenced attirbute
public:
  /// Defines the attribute referenced from this attribute
  MODEL_EXPORT virtual void setValue(boost::shared_ptr<ModelAPI_Attribute> theAttr);

  /// Returns attribute referenced from this attribute
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Attribute> value();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefAttr(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
