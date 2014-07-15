// File:        ModelAPI_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Data_HeaderFile
#define ModelAPI_Data_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

class ModelAPI_AttributeDocRef;
class ModelAPI_AttributeDouble;
class ModelAPI_AttributeReference;
class ModelAPI_AttributeRefAttr;
class ModelAPI_AttributeRefList;
class ModelAPI_AttributeBoolean;
class ModelAPI_Document;
class ModelAPI_Attribute;
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
  virtual void setName(std::string theName) = 0;

  /// Returns the attribute that references to another document
  virtual boost::shared_ptr<ModelAPI_AttributeDocRef> docRef(const std::string theID) = 0;
  /// Returns the attribute that contains real value with double precision
  virtual boost::shared_ptr<ModelAPI_AttributeDouble> real(const std::string theID) = 0;
  /// Returns the attribute that contains reference to a feature
  virtual boost::shared_ptr<ModelAPI_AttributeReference> reference(const std::string theID) = 0;
  /// Returns the attribute that contains reference to an attribute of a feature
  virtual boost::shared_ptr<ModelAPI_AttributeRefAttr> refattr(const std::string theID) = 0;
  /// Returns the attribute that contains list of references to features
  virtual boost::shared_ptr<ModelAPI_AttributeRefList> reflist(const std::string theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual boost::shared_ptr<ModelAPI_AttributeBoolean> boolean(const std::string theID) = 0;

  /// Returns the generic attribute by identifier
  /// \param theID identifier of the attribute
  virtual boost::shared_ptr<ModelAPI_Attribute> attribute(const std::string theID) = 0;
  /// Returns all attributes ofthe feature of the given type
  /// or all attributes if "theType" is empty
  virtual std::list<boost::shared_ptr<ModelAPI_Attribute> >
    attributes(const std::string theType) = 0;
  /// Identifier by the id (not fast, iteration by map)
  /// \param theAttr attribute already created in this data
  virtual const std::string& id(const boost::shared_ptr<ModelAPI_Attribute> theAttr) = 0;
  /// Returns true if data belongs to same features
  virtual bool isEqual(const boost::shared_ptr<ModelAPI_Data> theData) = 0;
  /// Returns true if it is correctly connected t othe data model
  virtual bool isValid() = 0;

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  virtual void addAttribute(std::string theID, std::string theAttrType) = 0;

  /// Useful method for "set" methods of the attributes: sends an UPDATE event and
  /// makes attribute initialized
  virtual void sendAttributeUpdated(ModelAPI_Attribute* theAttr) = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Data() {}

protected:
  /// Objects are created for features automatically
  ModelAPI_Data()
  {}
};

typedef boost::shared_ptr<ModelAPI_Data> DataPtr;


#endif
