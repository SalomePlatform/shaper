// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Folder.h>
#include <ModelAPI_Object.h>

#include <TDF_Label.hxx>
#include <TDataStd_BooleanArray.hxx>

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
  typedef std::map<std::string, std::pair<std::shared_ptr<ModelAPI_Attribute>, int> > AttributeMap;

  TDF_Label myLab;  ///< label of the feature in the document
  /// All attributes of the object identified by the attribute ID
  /// (the attribute is stored together with its index in the feature)
  AttributeMap myAttrs;
  /// Array of flags of this data
  Handle(TDataStd_BooleanArray) myFlags;

  /// needed here to emit signal that object changed on change of the attribute
  ObjectPtr myObject;

  /// List of attributes referenced to owner (updated only during the transaction change)
  std::set<AttributePtr> myRefsToMe;
  /// flag that may block the "attribute updated" sending
  bool mySendAttributeUpdated;
  /// if some attribute was changed, but mySendAttributeUpdated was false, this stores this
  std::list<ModelAPI_Attribute*> myWasChangedButBlocked;

  /// Returns label of this feature
  TDF_Label label()
  {
    return myLab;
  }

  friend class Model_Document;
  friend class Model_Objects;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
  friend class Model_AttributeRefAttrList;
  friend class Model_AttributeSelection;
  friend class Model_AttributeSelectionList;
  friend class Model_ValidatorsFactory;
  friend class Model_SelectionNaming;
  friend class Model_ResultConstruction;
  friend class Model_ResultBody;
  friend class Model_Tools;

 public:
  /// The simplest constructor. "setLabel" must be called just after to initialize correctly.
  Model_Data();
  /// Returns the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual std::string name();
  /// Defines the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual void setName(const std::string& theName);
  /// Return \c true if the object has been renamed by the user
  MODEL_EXPORT virtual bool hasUserDefinedName() const;
  /// Returns the attribute that references to another document
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID);
  /// Returns the attribute that contains double values array
  MODEL_EXPORT virtual
    std::shared_ptr<ModelAPI_AttributeDoubleArray> realArray(const std::string& theID);
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
  /// Returns the attribute that contains list of references to features
  /// or reference to an attribute of a feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeRefAttrList>
    refattrlist(const std::string& theID);
  /// Returns the attribute that contains boolean value
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeBoolean>
    boolean(const std::string& theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeString>
    string(const std::string& theID);
  /// Returns the attribute that contains integer values array
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeIntArray>
    intArray(const std::string& theID);
  /// Returns the attribute that contains string values array
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeStringArray>
    stringArray(const std::string& theID);
  /// Returns the attribute that contains string values array
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeTables>
    tables(const std::string& theID);

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
  /// Returns true if it is correctly connected to the data model
  MODEL_EXPORT virtual bool isValid();

  /// Returns the label where the shape must be stored (used in ResultBody)
  TDF_Label shapeLab() const
  {
    return myLab.IsNull() ? myLab : myLab.Father().FindChild(2);
  }

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  /// \param theIndex index of the attribute in the internal data structure, for not-floating
  ///                 attributes it is -1 to let it automatically be added
  /// \returns the just created attribute
  MODEL_EXPORT virtual AttributePtr
    addAttribute(const std::string& theID, const std::string theAttrType, const int theIndex = -1);

  /// Adds a floating attribute (that may be added/removed during the data life)
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  /// \param theGroup identifier of the group this attribute belongs to, may be an empty string
  MODEL_EXPORT virtual AttributePtr
    addFloatingAttribute(const std::string& theID, const std::string theAttrType,
      const std::string& theGroup);

  /// Returns all groups of this data (ordered).
  MODEL_EXPORT virtual void allGroups(std::list<std::string>& theGroups);

  /// Returns an ordered list of attributes that belong to the given group
  MODEL_EXPORT virtual void attributesOfGroup(const std::string& theGroup,
    std::list<std::shared_ptr<ModelAPI_Attribute> >& theAttrs);

  /// Remove all attributes of the given group
  MODEL_EXPORT virtual void removeAttributes(const std::string& theGroup);

  /// Useful method for "set" methods of the attributes: sends an UPDATE event and
  /// makes attribute initialized
  MODEL_EXPORT virtual void sendAttributeUpdated(ModelAPI_Attribute* theAttr);
  /// Blocks sending "attribute updated" if theBlock is true
  /// \param theBlock allows switching on/off the blocking state
  /// \param theSendMessage if false, it does not send the update message
  ///            even if something is changed
  ///            (normally is it used in attributeChanged because this message will be sent anyway)
  /// \returns the previous state of block
  MODEL_EXPORT virtual bool blockSendAttributeUpdated(
    const bool theBlock, const bool theSendMessage = true);

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
  MODEL_EXPORT virtual void setError(const std::string& theError, bool theSend = true);

  /// Erases the error string if it is not empty
  void eraseErrorString();

  /// Registers error during the execution, causes the ExecutionFailed state
  MODEL_EXPORT virtual std::string error() const;

  /// Returns the identifier of feature-owner, unique in this document
  MODEL_EXPORT virtual int featureId() const;

  /// returns all objects referenced to this
  MODEL_EXPORT virtual const std::set<AttributePtr>& refsToMe() {return myRefsToMe;}

  /// returns all references by attributes of this data
  /// \param theRefs returned list of pairs:
  ///                id of referenced attribute and list of referenced objects
  MODEL_EXPORT virtual void referencesToObjects(
    std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs);

  /// Copies all attributes content into theTarget data
  MODEL_EXPORT virtual void copyTo(std::shared_ptr<ModelAPI_Data> theTarget);

  /// Returns the invalid data pointer (to avoid working with NULL shared pointers in swig)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Data> invalidPtr();

  /// Returns the invalid data pointer: static method
  static std::shared_ptr<ModelAPI_Data> invalidData();

  /// Identifier of the transaction when object (feature or result) was updated last time.
  MODEL_EXPORT virtual int updateID();

  /// Identifier of the transaction when object (feature or result) was updated last time.
  /// This method is called by the updater.
  MODEL_EXPORT virtual void setUpdateID(const int theID);

  /// Returns true if the given object is owner of this data (needed for correct erase of object
  /// with duplicated data)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Object> owner();

protected:
  /// Returns true if "is in history" custom behaviors is defined for the feature
  MODEL_EXPORT virtual bool isInHistory();

  /// Defines the custom "is in history" behavior
  MODEL_EXPORT virtual void setIsInHistory(const bool theFlag);

  /// Returns true if the object is deleted, but some data is still kept in memory
  MODEL_EXPORT virtual bool isDeleted();

  /// Sets true if the object is deleted, but some data is still kept in memory
  MODEL_EXPORT virtual void setIsDeleted(const bool theFlag);

  /// Erases all attributes from myAttrs, but keeping them in the data structure
  void clearAttributes();

private:
  /// Removes a back reference (with identifier which attribute references to this object)
  /// \param theFeature feature referenced to this
  /// \param theAttrID identifier of the attribute that is references from theFeature to this
  void removeBackReference(ObjectPtr theObject, std::string theAttrID);
  /// Removes a back reference (by the attribute)
  /// \param theAttr the referenced attribute
  void removeBackReference(AttributePtr theAttr);
  /// Adds a back reference (with identifier which attribute references to this object
  /// \param theFeature feature referenced to this
  /// \param theAttrID identifier of the attribute that is references from theFeature to this
  /// \param theApplyConcealment applies concealment flag changes
  void addBackReference(FeaturePtr theFeature, std::string theAttrID,
    const bool theApplyConcealment = true);
  /// Adds a back reference to an object
  /// \param theObject object referenced to this
  /// \param theAttrID identifier of the attribute that is references from theFolder to this
  void addBackReference(ObjectPtr theObject, std::string theAttrID);

  /// Makes the concealment flag up to date for this object-owner.
  MODEL_EXPORT virtual void updateConcealmentFlag();

  /// Returns true if object must be displayed in the viewer: flag is stored in the
  /// data model, so on undo/redo, open/save or recreation of object by history-playing it keeps
  /// the original state in the current transaction.
  MODEL_EXPORT virtual bool isDisplayed();

  /// Sets the displayed/hidden state of the object. If it is changed, sends the "redisplay"
  /// signal.
  MODEL_EXPORT virtual void setDisplayed(const bool theDisplay);

  /// Returns \c true if theAttribute1 is going earlier than theAttribute2 in the data
  MODEL_EXPORT virtual bool isPrecedingAttribute(const std::string& theAttribute1,
                                                 const std::string& theAttribute2) const;

};

/// Generic method to register back reference, used in referencing attributes.
/// Without concealment change, it will be done later, on synchronization.
#define ADD_BACK_REF(TARGET) \
  if (TARGET.get() != NULL) { \
    std::shared_ptr<Model_Data> aTargetData = \
        std::dynamic_pointer_cast<Model_Data>((TARGET)->data()); \
    FeaturePtr anAttributeOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner()); \
    if (anAttributeOwnerFeature.get()) \
      aTargetData->addBackReference(anAttributeOwnerFeature, id(), false); \
    else { \
      FolderPtr anAttributeOwnerFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(owner()); \
      if (anAttributeOwnerFolder.get()) \
        aTargetData->addBackReference(ObjectPtr(anAttributeOwnerFolder), id()); \
    } \
  }

/// Generic method to unregister back reference, used in referencing attributes.
/// Without concealment change, it will be done later, on synchronization.
#define REMOVE_BACK_REF(TARGET) \
  if (TARGET.get() != NULL) { \
    std::shared_ptr<Model_Data> aTargetData = \
        std::dynamic_pointer_cast<Model_Data>((TARGET)->data()); \
    FeaturePtr anAttOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner()); \
    if (anAttOwnerFeature.get()) \
      aTargetData->removeBackReference(anAttOwnerFeature, id()); \
    else { \
      FolderPtr anAttributeOwnerFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(owner()); \
      if (anAttributeOwnerFolder.get()) \
        aTargetData->removeBackReference(ObjectPtr(anAttributeOwnerFolder), id()); \
    } \
  }

#endif
