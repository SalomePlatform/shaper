// Name   : ModelHighAPI_Double.h
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_

//--------------------------------------------------------------------------------------
#include <memory>
#include <string>

#include <boost/variant.hpp>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeDouble;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Double
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeDouble
 */
class ModelHighAPI_Double
{
public:
  /// Constructor for double
  ModelHighAPI_Double(double theValue = 0.);
  /// Constructor for std::string
  ModelHighAPI_Double(const std::string & theValue);
  /// Constructor for char *
  ModelHighAPI_Double(const char * theValue);
  /// Destructor
  virtual ~ModelHighAPI_Double();

  /// Fill attribute values
  virtual void fillAttribute(std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const;

private:
  boost::variant<double, std::string> myValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_ */ 
