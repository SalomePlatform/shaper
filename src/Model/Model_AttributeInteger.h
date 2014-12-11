// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeInteger.h
// Created:     03 sep 2014
// Author:      sbh

#ifndef MODEL_ATTRIBUTEINTEGER_H_
#define MODEL_ATTRIBUTEINTEGER_H_

#include <Model.h>
#include <ModelAPI_AttributeInteger.h>

#include <TDF_Label.hxx>
#include <TDataStd_Integer.hxx>

/**\class Model_AttributeInteger
 * \ingroup DataModel
 * \brief Attribute that contains integer (int).
 */

class Model_AttributeInteger : public ModelAPI_AttributeInteger
{
  Handle_TDataStd_Integer myInteger;
 public:
  /// Defines the int value
  MODEL_EXPORT virtual void setValue(const int theValue);

  /// Returns the int value
  MODEL_EXPORT virtual int value();

 protected:
  /// Initializes attibutes
  Model_AttributeInteger(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
