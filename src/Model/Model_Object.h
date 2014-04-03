// File:        Model_Object.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Object_HeaderFile
#define Model_Object_HeaderFile

#include "Model.h"
#include <ModelAPI_Object.h>
#include <TDF_Label.hxx>

#include <map>

class ModelAPI_Attribute;

/**\class Model_Object
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class Model_Object: public ModelAPI_Object
{
  TDF_Label myLab; ///< label of the feature in the document
  /// All attributes of the object identified by the attribute ID
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> > myAttrs;

  Model_Object();

  friend class Model_Document;

public:
  /// Returns the name of the feature visible by the user in the object browser
  virtual std::string getName();
  /// Defines the name of the feature visible by the user in the object browser
  virtual void setName(std::string theName);
  /// Returns the attribute that references to another document
  virtual std::shared_ptr<ModelAPI_AttributeDocRef> docRef(const std::string theID);
  /// Returns the attribute that contains real value with double precision
  virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string theID);

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  virtual void addAttribute(std::string theID, std::string theAttrType);

  /// Puts feature to the document data sub-structure
  void setLabel(TDF_Label& theLab);
};

#endif
