// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeBoolean.h
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#ifndef Model_AttributeBoolean_H_
#define Model_AttributeBoolean_H_

#include "Model.h"
#include "ModelAPI_AttributeBoolean.h"
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>

/**\class Model_AttributeBoolean
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class Model_AttributeBoolean : public ModelAPI_AttributeBoolean
{
  Handle_TDataStd_Integer myBool;  ///< double is Real attribute
 public:
  /// Defines the double value
  MODEL_EXPORT virtual void setValue(bool theValue);

  /// Returns the double value
  MODEL_EXPORT virtual bool value();

 protected:
  /// Initializes attibutes
  Model_AttributeBoolean(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
