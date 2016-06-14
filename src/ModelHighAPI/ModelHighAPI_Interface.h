// Name   : ModelHighAPI_Interface.h
// Purpose: 
//
// History:
// 17/05/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_INTERFACE_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_INTERFACE_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <list>
#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class ModelAPI_Feature;
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Interface
 * \ingroup CPPHighAPI
 * \brief Base class for feature interfaces
 */
class ModelHighAPI_Interface
{
public:
  /// Constructor
  MODELHIGHAPI_EXPORT
  explicit ModelHighAPI_Interface(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_Interface();

  /// Return ModelAPI_Feature
  MODELHIGHAPI_EXPORT
  std::shared_ptr<ModelAPI_Feature> feature() const;

  /// Shortcut for feature()->getKind()
  MODELHIGHAPI_EXPORT
  const std::string& getKind() const;

  /// Shortcut for feature()->execute()
  MODELHIGHAPI_EXPORT
  void execute();

  /// Return all objects of the feature
  MODELHIGHAPI_EXPORT
  virtual std::list<ModelHighAPI_Selection> result() const;

  /// Throw exception to event loop
  MODELHIGHAPI_EXPORT
  void throwException(const std::string & theDescription);

protected:
  std::shared_ptr<ModelAPI_Feature> myFeature;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_INTERFACE_H_ */
