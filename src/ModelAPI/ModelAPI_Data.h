// File:        ModelAPI_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Data_HeaderFile
#define ModelAPI_Data_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

class ModelAPI_AttributeDocRef;
class ModelAPI_AttributeDouble;
class ModelAPI_Document;
class ModelAPI_Attribute;

/**\class ModelAPI_Data
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class MODELAPI_EXPORT ModelAPI_Data
{
public:

  /// Returns the name of the feature visible by the user in the object browser
  virtual std::string getName() = 0;

  /// Defines the name of the feature visible by the user in the object browser
  virtual void setName(std::string theName) = 0;

  /// Returns the attribute that references to another document
  virtual boost::shared_ptr<ModelAPI_AttributeDocRef> docRef(const std::string theID) = 0;
  /// Returns the attribute that contains real value with double precision
  virtual boost::shared_ptr<ModelAPI_AttributeDouble> real(const std::string theID) = 0;

  /// Returns the generic attribute by identifier
  /// \param theID identifier of the attribute
  virtual boost::shared_ptr<ModelAPI_Attribute> attribute(const std::string theID) = 0;

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  virtual void addAttribute(std::string theID, std::string theAttrType) = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Data() {}

protected:
  /// Objects are created for features automatically
  ModelAPI_Data()
  {}
};

#endif
