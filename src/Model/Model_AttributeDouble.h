// File:        Model_AttributeDouble.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeDouble_HeaderFile
#define Model_AttributeDouble_HeaderFile

#include "Model.h"
#include "ModelAPI_AttributeDouble.h"
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>

/**\class Model_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class MODEL_EXPORT Model_AttributeDouble : public ModelAPI_AttributeDouble
{
  Handle_TDataStd_Real myReal; ///< double is Real attribute
public:
  /// Defines the double value
  virtual void setValue(const double theValue);

  /// Returns the double value
  virtual double value();

protected:
  /// Initializes attibutes
  Model_AttributeDouble(TDF_Label& theLabel);

  friend class Model_Object;
};

#endif
