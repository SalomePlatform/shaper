// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeDouble.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeDouble_H_
#define Model_AttributeDouble_H_

#include "Model.h"
#include "ModelAPI_AttributeDouble.h"
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>

#include <TDataStd_Name.hxx>

/**\class Model_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class Model_AttributeDouble : public ModelAPI_AttributeDouble
{
  Handle_TDataStd_Real myReal;  ///< double is Real attribute
  Handle_TDataStd_Name myText;  ///< Text representation of the attribute (may differ for parametres)
 public:
  /// Defines the double value
  MODEL_EXPORT virtual void setValue(const double theValue);

  /// Returns the double value
  MODEL_EXPORT virtual double value();

  /// Defines the double value
  MODEL_EXPORT virtual void setText(const std::string& theText);

  /// Returns the double value
  MODEL_EXPORT virtual std::string text();

 protected:
  /// Initializes attibutes
  Model_AttributeDouble(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
