// Name   : ModelHighAPI_Integer.h
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_INTEGER_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_INTEGER_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>

#include <boost/variant.hpp>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeInteger;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Integer
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeInteger
 */
class ModelHighAPI_Integer
{
public:
  /// Constructor for int
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Integer(int theValue = 0.);
  /// Constructor for std::string
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Integer(const std::string & theValue);
  /// Constructor for char *
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Integer(const char * theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_Integer();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT
  virtual void fillAttribute(const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute) const;

private:
  boost::variant<int, std::string> myValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_INTEGER_H_ */
