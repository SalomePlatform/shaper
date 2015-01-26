// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Data_H_
#define ModelAPI_Data_H_

#include "ModelAPI.h"
#include <string>
#include <list>
#include <set>
#include <memory>

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
class ModelAPI_AttributeSelectionList;
class ModelAPI_Object;
class GeomAPI_Shape;

/// Enumeration that contains the execution status of the Object
enum ModelAPI_ExecState {
  ModelAPI_StateDone, ///< execution was performed and result is up to date
  ModelAPI_StateMustBeUpdated, ///< execution must be performed to obtain the up to date result
  ModelAPI_StateExecFailed, ///< execution was failed (results are deleted in this case)
  ModelAPI_StateInvalidArgument, ///< execution was not performed (results are deleted in this case)
  ModelAPI_StateNothing ///< internal state that actually means that nothing must be changed
};

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
  virtual std::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID) = 0;
  /// Returns the attribute that contains real value with double precision
  virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID) = 0;
  /// Returns the attribute that contains integer value
  virtual std::shared_ptr<ModelAPI_AttributeInteger> integer(const std::string& theID) = 0;
  /// Returns the attribute that contains reference to a feature
  virtual std::shared_ptr<ModelAPI_AttributeReference> reference(const std::string& theID) = 0;
  /// Returns the attribute that contains selection to a shape
  virtual std::shared_ptr<ModelAPI_AttributeSelection> selection(const std::string& theID) = 0;
  /// Returns the attribute that contains selection to a shape
  virtual std::shared_ptr<ModelAPI_AttributeSelectionList> 
    selectionList(const std::string& theID) = 0;
  /// Returns the attribute that contains reference to an attribute of a feature
  virtual std::shared_ptr<ModelAPI_AttributeRefAttr> refattr(const std::string& theID) = 0;
  /// Returns the attribute that contains list of references to features
  virtual std::shared_ptr<ModelAPI_AttributeRefList> reflist(const std::string& theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual std::shared_ptr<ModelAPI_AttributeBoolean> boolean(const std::string& theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual std::shared_ptr<ModelAPI_AttributeString> string(const std::string& theID) = 0;

  /// Returns the generic attribute by identifier
  /// \param theID identifier of the attribute
  virtual std::shared_ptr<ModelAPI_Attribute> attribute(const std::string& theID) = 0;
  /// Returns all attributes of the feature of the given type
  /// or all attributes if "theType" is empty
  virtual std::list<std::shared_ptr<ModelAPI_Attribute> >
  attributes(const std::string& theType) = 0;
  /// Returns all attributes ids of the feature of the given type
  /// or all attributes if "theType" is empty
  virtual std::list<std::string> attributesIDs(const std::string& theType) = 0;
  /// Identifier by the id (not fast, iteration by map)
  /// \param theAttr attribute already created in this data
  virtual const std::string& id(const std::shared_ptr<ModelAPI_Attribute>& theAttr) = 0;
  /// Returns true if data belongs to same features
  virtual bool isEqual(const std::shared_ptr<ModelAPI_Data>& theData) = 0;
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
  /// Blocks sending "attribute updated" if theBlock is true
  virtual void blockSendAttributeUpdated(const bool theBlock) = 0;

  /// Erases all the data from the data model
  virtual void erase() = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Data()
  {
  }

  /// Stores the state of the object to execute it later accordingly
  virtual void execState(const ModelAPI_ExecState theState) = 0;

  /// Returns the state of the latest execution of the feature
  virtual ModelAPI_ExecState execState() = 0;

  /// Registers error during the execution, causes the ExecutionFailed state
  virtual void setError(const std::string& theError) = 0;

  /// Returns the identifier of feature-owner, unique in this document
  virtual int featureId() const = 0;

 // returns all objects referenced to this
  virtual const std::set<std::shared_ptr<ModelAPI_Attribute> >& refsToMe() = 0;

  // returns all references by attributes of this data
  // \param theRefs returned list of pairs: id of referenced attribute and list of referenced objects
  virtual void referencesToObjects(
    std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > >& theRefs) = 0;
 protected:
  /// Objects are created for features automatically
  ModelAPI_Data()
  {
  }
};

typedef std::shared_ptr<ModelAPI_Data> DataPtr;

#endif
