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
   ///< ID of the referenced attirbute (empty if this is a reference to a feature)
  Handle_TDataStd_Comment myID;
public:
  /// Returns true if this attribute references to a feature (not to the attribute)
  MODEL_EXPORT virtual bool isFeature();

  /// Defines the reference to the attribute
  MODEL_EXPORT virtual void setAttr(boost::shared_ptr<ModelAPI_Attribute> theAttr);

  /// Returns attribute referenced from this attribute
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Attribute> attr();

  /// Defines the reference to the feature
  MODEL_EXPORT virtual void setFeature(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Returns feature referenced from this attribute
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> feature();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefAttr(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
