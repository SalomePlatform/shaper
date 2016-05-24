// Name   : ModelHighAPI_Interface.h
// Purpose: 
//
// History:
// 17/05/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_INTERFACE_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_INTERFACE_H_

//--------------------------------------------------------------------------------------
#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class ModelAPI_Feature;
//--------------------------------------------------------------------------------------
/*
 *
 */
class ModelHighAPI_Interface
{
public:
  explicit ModelHighAPI_Interface(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  virtual ~ModelHighAPI_Interface();

  /// Return ModelAPI_Feature
  std::shared_ptr<ModelAPI_Feature> feature() const;

  /// Shortcut for feature()->execute()
  void execute();

  /// Throw exception to event loop.
  void throwException(const std::string & theDescription);

protected:
  std::shared_ptr<ModelAPI_Feature> myFeature;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_INTERFACE_H_ */ 
