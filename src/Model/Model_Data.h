// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Data_H_
#define Model_Data_H_

#include <Model.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>

#include <TDF_Label.hxx>

#include <memory>

#include <map>
#include <list>
#include <string>
#include <set>

class ModelAPI_Attribute;

/**\class Model_Data
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class Model_Data : public ModelAPI_Data
{
  TDF_Label myLab;  ///< label of the feature in the document
  /// All attributes of the object identified by the attribute ID
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> > myAttrs;

  /// needed here to emit signal that object changed on change of the attribute
  ObjectPtr myObject;

  /// List of attributes referenced to owner (updated only during the transaction change)
  std::set<AttributePtr> myRefsToMe;
  /// flag that may block the "attribute updated" sending
  bool mySendAttributeUpdated;

  Model_Data();

  /// Returns label of this feature
  TDF_Label label()
  {
    return myLab;
  }

  friend class Model_Document;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
  friend class Model_AttributeSelection;

 public:
  /// Returns the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual std::string name();
  /// Defines the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual void setName(const std::string& theName);
  /// Returns the attribute that references to another document
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID);
  /// Returns the attribute that contains integer value
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeInteger>
    integer(const std::string& theID);
  /// Returns the attribute that contains reference to a feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeReference>
    reference(const std::string& theID);
  /// Returns the attribute that contains selection to a shape
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeSelection>
    selection(const std::string& theID);
  /// Returns the attribute that contains selection to a shape
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeSelectionList> 
    selectionList(const std::string& theID);
  /// Returns the attribute that contains reference to an attribute of a feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeRefAttr>
    refattr(const std::string& theID);
  /// Returns the attribute that contains list of references to features
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeRefList>
    reflist(const std::string& theID);
  /// Returns the attribute that contains boolean value
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeBoolean>
    boolean(const std::string& theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeString>
    string(const std::string& theID);
  /// Returns the generic attribute by identifier
  /// \param theID identifier of the attribute
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Attribute> attribute(const std::string& theID);
  /// Returns all attributes of the feature of the given type
  /// or all attributes if "theType" is empty
  MODEL_EXPORT virtual std::list<std::shared_ptr<ModelAPI_Attribute> >
    attributes(const std::string& theType);
  /// Returns all attributes ids of the feature of the given type
  /// or all attributes if "theType" is empty
  MODEL_EXPORT virtual std::list<std::string> attributesIDs(const std::string& theType);

  /// Identifier by the id (not fast, iteration by map)
  /// \param theAttr attribute already created in this data
  MODEL_EXPORT virtual const std::string& id(const std::shared_ptr<ModelAPI_Attribute>& theAttr);
  /// Returns true if data belongs to same features
  MODEL_EXPORT virtual bool isEqual(const std::shared_ptr<ModelAPI_Data>& theData);
  /// Returns true if it is correctly connected t othe data model
  MODEL_EXPORT virtual bool isValid();

  /// Returns the label where the shape must be stored (used in ResultBody)
  TDF_Label& shapeLab()
  {
    return myLab;
  }

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  MODEL_EXPORT virtual void addAttribute(const std::string& theID, const std::string theAttrType);

  /// Useful method for "set" methods of the attributes: sends an UPDATE event and
  /// makes attribute initialized
  MODEL_EXPORT virtual void sendAttributeUpdated(ModelAPI_Attribute* theAttr);
  /// Blocks sending "attribute updated" if theBlock is true
  MODEL_EXPORT virtual void blockSendAttributeUpdated(const bool theBlock);

  /// Puts feature to the document data sub-structure
  MODEL_EXPORT void setLabel(TDF_Label theLab);

  /// Sets the object of this data
  MODEL_EXPORT virtual void setObject(ObjectPtr theObject)
  {
    myObject = theObject;
  }

  /// Erases all the data from the data model
  MODEL_EXPORT virtual void erase();

  /// Stores the state of the object to execute it later accordingly
  MODEL_EXPORT virtual void execState(const ModelAPI_ExecState theState);

  /// Returns the state of the latest execution of the feature
  MODEL_EXPORT virtual ModelAPI_ExecState execState();

  /// Registers error during the execution, causes the ExecutionFailed state
  MODEL_EXPORT virtual void setError(const std::string& theError);

  /// Returns the identifier of feature-owner, unique in this document
  MODEL_EXPORT virtual int featureId() const;

  /// returns all objects referenced to this
  MODEL_EXPORT virtual const std::set<AttributePtr>& refsToMe() {return myRefsToMe;}

  /// returns all references by attributes of this data
  /// \param theRefs returned list of pairs: id of referenced attribute and list of referenced objects
  MODEL_EXPORT virtual void referencesToObjects(
    std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs);

private:
  /// removes all information about back references
  void eraseBackReferences();
  // adds a back reference (with identifier which attribute references to this object
  void addBackReference(FeaturePtr theFeature, std::string theAttrID);
};

#endif
