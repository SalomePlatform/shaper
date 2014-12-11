// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeRefAttr.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeRefAttr_H_
#define Model_AttributeRefAttr_H_

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
  Handle_TDF_Reference myRef;  ///< reference to the feature label
  ///< ID of the referenced attirbute (empty if this is a reference to a feature)
  Handle_TDataStd_Comment myID;
 public:
  /// Returns true if this attribute references to a object (not to the attribute)
  MODEL_EXPORT virtual bool isObject();

  /// Defines the reference to the attribute
  MODEL_EXPORT virtual void setAttr(std::shared_ptr<ModelAPI_Attribute> theAttr);

  /// Returns attribute referenced from this attribute
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Attribute> attr();

  /// Defines the reference to the object
  MODEL_EXPORT virtual void setObject(ObjectPtr theFeature);

  /// Returns object referenced from this attribute
  MODEL_EXPORT virtual ObjectPtr object();

 protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefAttr(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
