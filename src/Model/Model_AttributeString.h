// File:        Model_AttributeString.h
// Created:     25 august 2014
// Author:      sbh

#ifndef MODEL_ATTRIBUTESTRING_H_
#define MODEL_ATTRIBUTESTRING_H_

#include <Model.h>
#include <ModelAPI_AttributeString.h>

#include <TDF_Label.hxx>
#include <Handle_TDataStd_Name.hxx>

#include <string>

/**\class Model_AttributeString
 * \ingroup DataModel
 * \brief Attribute that contains std (null terminated) string.
 */

class Model_AttributeString : public ModelAPI_AttributeString
{
  Handle_TDataStd_Name myString;
 public:
  /// Defines the std::string value
  MODEL_EXPORT virtual void setValue(const std::string& theValue);

  /// Returns the std::string  value
  MODEL_EXPORT virtual std::string value();

 protected:
  /// Initializes attibutes
  Model_AttributeString(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
