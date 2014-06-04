// File:        Model_AttributeReference.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeReference_HeaderFile
#define Model_AttributeReference_HeaderFile

#include "Model.h"
#include "ModelAPI_AttributeReference.h"
#include <ModelAPI_Feature.h>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>

/**\class Model_AttributeReference
 * \ingroup DataModel
 * \brief Attribute that contains reference to feature (located in the same document).
 */

class Model_AttributeReference : public ModelAPI_AttributeReference
{
  Handle_TDF_Reference myRef; ///< references to the feature label
public:
  /// Defines the feature referenced from this attribute
  MODEL_EXPORT virtual void setValue(FeaturePtr theFeature);

  /// Returns feature referenced from this attribute
  MODEL_EXPORT virtual FeaturePtr value();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeReference(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
