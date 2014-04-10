// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Data_HeaderFile
#define Model_Data_HeaderFile

#include "Model.h"
#include <ModelAPI_Data.h>
#include <TDF_Label.hxx>

#include <map>

class ModelAPI_Attribute;

/**\class Model_Data
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class Model_Data: public ModelAPI_Data
{
  TDF_Label myLab; ///< label of the feature in the document
  /// All attributes of the object identified by the attribute ID
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> > myAttrs;

  std::shared_ptr<ModelAPI_Document> myDoc; ///< document this feature belongs to

  Model_Data();

  TDF_Label label() {return myLab;}

  friend class Model_Document;
  friend class Model_Iterator;

public:
  /// Returns the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual std::string getName();
  /// Defines the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual void setName(std::string theName);
  /// Returns the attribute that references to another document
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDocRef> docRef(const std::string theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string theID);

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  MODEL_EXPORT virtual void addAttribute(std::string theID, std::string theAttrType);

  /// Returns the document of this data
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> document() {return myDoc;}

  /// Puts feature to the document data sub-structure
  MODEL_EXPORT void setLabel(TDF_Label& theLab);

  /// Sets the document of this data
  MODEL_EXPORT virtual void setDocument(const std::shared_ptr<ModelAPI_Document>& theDoc) {myDoc = theDoc;}
};

#endif
