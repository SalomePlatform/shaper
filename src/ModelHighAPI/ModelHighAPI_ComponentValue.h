// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_ComponentValue.h
// Purpose:
//
// History:
// 16/11/16 - Mikhail Ponikarov - Creation of the file

#ifndef SRC_MODELHIGHAPI_ModelHighAPI_ComponentValue_H_
#define SRC_MODELHIGHAPI_ModelHighAPI_ComponentValue_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <ModelAPI_AttributeTables.h>

//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_ComponentValue
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeTable elements
 */
class ModelHighAPI_ComponentValue
{
public:
  /// Constructor for Boolean
  MODELHIGHAPI_EXPORT ModelHighAPI_ComponentValue(const bool theValue = false);
  /// Constructor for int
  MODELHIGHAPI_EXPORT ModelHighAPI_ComponentValue(const int theValue);
  /// Constructor for double
  MODELHIGHAPI_EXPORT ModelHighAPI_ComponentValue(const double theValue);
  /// Constructor for std::string
  MODELHIGHAPI_EXPORT ModelHighAPI_ComponentValue(const std::string & theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT virtual ~ModelHighAPI_ComponentValue();

  /// Sets value to the table
  MODELHIGHAPI_EXPORT virtual void fill(const std::shared_ptr<ModelAPI_AttributeTables>& theAttr,
    const int theTable, const int theColumn, const int theRow) const;

private:
  ModelAPI_AttributeTables::ValueType myType; ///< type of the value set
  ModelAPI_AttributeTables::Value myValue; ///< value itself
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_ModelHighAPI_ComponentValue_H_ */
