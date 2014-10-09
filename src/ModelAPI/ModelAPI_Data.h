// File:        ModelAPI_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Data_H_
#define ModelAPI_Data_H_

#include "ModelAPI.h"
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

class ModelAPI_AttributeDocRef;
class ModelAPI_AttributeInteger;
class ModelAPI_AttributeDouble;
class ModelAPI_AttributeReference;
class ModelAPI_AttributeRefAttr;
class ModelAPI_AttributeRefList;
class ModelAPI_AttributeBoolean;
class ModelAPI_AttributeString;
class ModelAPI_Document;
class ModelAPI_Attribute;
class ModelAPI_Feature;
class ModelAPI_AttributeSelection;
class GeomAPI_Shape;

/**\class ModelAPI_Data
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class MODELAPI_EXPORT ModelAPI_Data
{
 public:

  /// Returns the name of the feature visible by the user in the object browser
  virtual std::string name() = 0;

  /// Defines the name of the feature visible by the user in the object browser
  virtual void setName(const std::string& theName) = 0;

  /// Returns the attribute that references to another document
  virtual boost::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID) = 0;
  /// Returns the attribute that contains real value with double precision
  virtual boost::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID) = 0;
  /// Returns the attribute that contains integer value
  virtual boost::shared_ptr<ModelAPI_AttributeInteger> integer(const std::string& theID) = 0;
  /// Returns the attribute that contains reference to a feature
  virtual boost::shared_ptr<ModelAPI_AttributeReference> reference(const std::string& theID) = 0;
  /// Returns the attribute that contains selection to a shape
  virtual boost::shared_ptr<ModelAPI_AttributeSelection> selection(const std::string& theID) = 0;
  /// Returns the attribute that contains reference to an attribute of a feature
  virtual boost::shared_ptr<ModelAPI_AttributeRefAttr> refattr(const std::string& theID) = 0;
  /// Returns the attribute that contains list of references to features
  virtual boost::shared_ptr<ModelAPI_AttributeRefList> reflist(const std::string& theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual boost::shared_ptr<ModelAPI_AttributeBoolean> boolean(const std::string& theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual boost::shared_ptr<ModelAPI_AttributeString> string(const std::string& theID) = 0;

  /// Returns the generic attribute by identifier
  /// \param theID identifier of the attribute
  virtual boost::shared_ptr<ModelAPI_Attribute> attribute(const std::string& theID) = 0;
  /// Returns all attributes of the feature of the given type
  /// or all attributes if "theType" is empty
  virtual std::list<boost::shared_ptr<ModelAPI_Attribute> >
  attributes(const std::string& theType) = 0;
  /// Returns all attributes ids of the feature of the given type
  /// or all attributes if "theType" is empty
  virtual std::list<std::string> attributesIDs(const std::string& theType) = 0;
  /// Identifier by the id (not fast, iteration by map)
  /// \param theAttr attribute already created in this data
  virtual const std::string& id(const boost::shared_ptr<ModelAPI_Attribute>& theAttr) = 0;
  /// Returns true if data belongs to same features
  virtual bool isEqual(const boost::shared_ptr<ModelAPI_Data>& theData) = 0;
  /// Returns true if it is correctly connected to the data model
  virtual bool isValid() = 0;

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  virtual void addAttribute(const std::string& theID, const std::string theAttrType) = 0;

  /// Useful method for "set" methods of the attributes: sends an UPDATE event and
  /// makes attribute initialized
  virtual void sendAttributeUpdated(ModelAPI_Attribute* theAttr) = 0;

  /// Erases all the data from the data model
  virtual void erase() = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Data()
  {
  }

  /// Makes feature must be updated later (on rebuild). Normally the Updater must call it
  /// in case of not-automatic update to true
  virtual void mustBeUpdated(const bool theFlag) = 0;

  /// Returns true if feature must be updated (re-executed) on rebuild
  virtual bool mustBeUpdated() = 0;

  /// Returns true if this data attributes are referenced to the given feature or its results
  virtual bool referencesTo(const boost::shared_ptr<ModelAPI_Feature>& theFeature) = 0;

 protected:
  /// Objects are created for features automatically
  ModelAPI_Data()
  {
  }
};

typedef boost::shared_ptr<ModelAPI_Data> DataPtr;

#endif
