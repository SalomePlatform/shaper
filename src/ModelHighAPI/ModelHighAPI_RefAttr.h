// Name   : ModelHighAPI_RefAttr.h
// Purpose: 
//
// History:
// 08/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>

#include <boost/variant.hpp>
//--------------------------------------------------------------------------------------
class ModelAPI_Attribute;
class ModelAPI_AttributeRefAttr;
class ModelAPI_Object;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_RefAttr
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeRefAttr
 */
class ModelHighAPI_RefAttr
{
public:
  /// Default constructor
  MODELHIGHAPI_EXPORT
  ModelHighAPI_RefAttr();
  /// Constructor for attribute
  MODELHIGHAPI_EXPORT
  explicit ModelHighAPI_RefAttr(const std::shared_ptr<ModelAPI_Attribute> & theValue);
  /// Constructor for object
  MODELHIGHAPI_EXPORT
  explicit ModelHighAPI_RefAttr(const std::shared_ptr<ModelAPI_Object> & theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_RefAttr();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT
  virtual void fillAttribute(const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute) const;

private:
  boost::variant<
    std::shared_ptr<ModelAPI_Attribute>,
    std::shared_ptr<ModelAPI_Object>
  > myValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_ */
