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
/*
 *
 */
class ModelHighAPI_Double
{
public:
  ModelHighAPI_Double();
  ModelHighAPI_Double(double theValue);
  ModelHighAPI_Double(const std::string & theValue);
  ModelHighAPI_Double(const char * theValue);
  virtual ~ModelHighAPI_Double();

  virtual void fillAttribute(std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const;

private:
  boost::variant<double, std::string> myValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_ */ 
