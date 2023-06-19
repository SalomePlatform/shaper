// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef ModelAPI_Data_H_
#define ModelAPI_Data_H_

#ifdef _WINDOWS // to avoid too long decorated name warning
#pragma warning( disable : 4503 )
#endif

#include "ModelAPI.h"
#include <string>
#include <list>
#include <set>
#include <memory>

class ModelAPI_Attribute;
class ModelAPI_AttributeDocRef;
class ModelAPI_AttributeInteger;
class ModelAPI_AttributeDouble;
class ModelAPI_AttributeDoubleArray;
class ModelAPI_AttributeReference;
class ModelAPI_AttributeRefAttr;
class ModelAPI_AttributeRefList;
class ModelAPI_AttributeRefAttrList;
class ModelAPI_AttributeBoolean;
class ModelAPI_AttributeString;
class ModelAPI_AttributeStringArray;
class ModelAPI_Document;
class ModelAPI_Attribute;
class ModelAPI_Feature;
class ModelAPI_AttributeSelection;
class ModelAPI_AttributeSelectionList;
class ModelAPI_AttributeIntArray;
class ModelAPI_AttributeImage;
class ModelAPI_AttributeTables;
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
  virtual std::wstring name() = 0;
  /// Defines the name of the feature visible by the user in the object browser
  virtual void setName(const std::wstring& theName) = 0;
  /// Return \c true if the object has been renamed by the user
  virtual bool hasUserDefinedName() const = 0;

  /// Returns version of the feature (empty string if not applicable)
  virtual std::string version() = 0;
  /// Initialize the version of the feature
  virtual void setVersion(const std::string& theVersion) = 0;

  /// Returns the attribute that references to another document
  virtual std::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID) = 0;
  /// Returns the attribute that contains real value with double precision
  virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID) = 0;
  /// Returns the attribute that contains double values array
  virtual std::shared_ptr<ModelAPI_AttributeDoubleArray> realArray(const std::string& theID) = 0;
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
  /// Returns the attribute that contains list of references to features or reference to
  /// an attribute of a feature
  virtual std::shared_ptr<ModelAPI_AttributeRefAttrList> refattrlist(const std::string& theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual std::shared_ptr<ModelAPI_AttributeBoolean> boolean(const std::string& theID) = 0;
  /// Returns the attribute that contains boolean value
  virtual std::shared_ptr<ModelAPI_AttributeString> string(const std::string& theID) = 0;
  /// Returns the attribute that contains integer values array
  virtual std::shared_ptr<ModelAPI_AttributeIntArray> intArray(const std::string& theID) = 0;
  /// Returns the attribute that contains string values array
  virtual std::shared_ptr<ModelAPI_AttributeStringArray> stringArray(const std::string& theID) = 0;
  /// Returns the attribute that contains image
  virtual std::shared_ptr<ModelAPI_AttributeImage> image(const std::string& theID) = 0;
  /// Returns the attribute that contains tables
  virtual std::shared_ptr<ModelAPI_AttributeTables> tables(const std::string& theID) = 0;

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
  /// \param theIndex index of the attribute in the internal data structure, for not-floating
  ///                 attributes it is -1 to let it automatically be added
  /// \returns the just created attribute
  virtual std::shared_ptr<ModelAPI_Attribute> addAttribute(
    const std::string& theID, const std::string theAttrType, const int theIndex = -1) = 0;

  /// Adds a floating attribute (that may be added/removed during the data life)
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  /// \param theGroup identifier of the group this attribute belongs to, may be an empty string
  virtual std::shared_ptr<ModelAPI_Attribute>
    addFloatingAttribute(const std::string& theID, const std::string theAttrType,
      const std::string& theGroup) = 0;

  /// Returns all groups of this data (ordered).
  virtual void allGroups(std::list<std::string>& theGroups) = 0;

  /// Returns an ordered list of attributes that belong to the given group
  virtual void attributesOfGroup(const std::string& theGroup,
    std::list<std::shared_ptr<ModelAPI_Attribute> >& theAttrs) = 0;

  /// Remove all attributes of the given group
  virtual void removeAttributes(const std::string& theGroup) = 0;

  /// Useful method for "set" methods of the attributes: sends an UPDATE event and
  /// makes attribute initialized
  virtual void sendAttributeUpdated(ModelAPI_Attribute* theAttr) = 0;
  /// Blocks sending "attribute updated" if theBlock is true
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  /// \returns the previous state of block
  virtual bool blockSendAttributeUpdated(
    const bool theBlock, const bool theSendMessage = true) = 0;

  /// Erases all the data from the data model
  virtual void erase() = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Data();

  /// Stores the state of the object to execute it later accordingly
  virtual void execState(const ModelAPI_ExecState theState) = 0;

  /// Returns the state of the latest execution of the feature
  virtual ModelAPI_ExecState execState() = 0;

  /// Registers error during the execution, causes the ExecutionFailed state
  virtual void setError(const std::string& theError, bool theSend = true) = 0;

  /// Returns error, arose during the execution
  virtual std::string error() const = 0;

  /// Returns the identifier of feature-owner, unique in this document
  virtual int featureId() const = 0;

  /// returns all objects referenced to this
  virtual const std::set<std::shared_ptr<ModelAPI_Attribute> >& refsToMe() = 0;

  /// returns all references by attributes of this data
  /// \param theRefs returned list of pairs: id of referenced attribute and
  ///                list of referenced objects
  virtual void referencesToObjects(
    std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > >& theRefs) =0;

  /// Copies all attributes content into theTarget data
  virtual void copyTo(std::shared_ptr<ModelAPI_Data> theTarget) = 0;

  /// Returns the invalid data pointer (to avoid working with NULL shared pointers in swig)
  virtual std::shared_ptr<ModelAPI_Data> invalidPtr() = 0;

  /// Identifier of the transaction when object (feature or result) was updated last time.
  virtual int updateID() = 0;

  /// Identifier of the transaction when object (feature or result) was updated last time.
  /// This method is called by the updater.
  virtual void setUpdateID(const int theID) = 0;

  /// Returns the owner of this data
  virtual std::shared_ptr<ModelAPI_Object> owner() = 0;

  /// Returns true if the object is deleted, but some data is still kept in memory
  virtual bool isDeleted() = 0;

  /// Sets true if the object is deleted, but some data is still kept in memory
  virtual void setIsDeleted(const bool theFlag) = 0;

  /// Returns \c true if theAttribute1 is going earlier than theAttribute2 in the data
  virtual bool isPrecedingAttribute(const std::string& theAttribute1,
                                    const std::string& theAttribute2) const = 0;

 protected:
  /// Objects are created for features automatically
  ModelAPI_Data();

  /// Returns true if "is in history" custom behaviors is defined for the feature
  virtual bool isInHistory() = 0;

  /// Defines the custom "is in history" behavior
  virtual void setIsInHistory(const bool theFlag) = 0;

  /// Returns true if object must be displayed in the viewer: flag is stored in the
  /// data model, so on undo/redo, open/save or recreation of object by history-playing it keeps
  /// the original state in the current transaction.
  virtual bool isDisplayed() = 0;

  /// Sets the displayed/hidden state of the object. If it is changed, sends the "redisplay"
  /// signal.
  virtual void setDisplayed(const bool theDisplay) = 0;

  friend class ModelAPI_Object;
};

typedef std::shared_ptr<ModelAPI_Data> DataPtr;

#endif
