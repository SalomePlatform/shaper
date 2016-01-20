// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefAttrList.h
// Created:     20 Jan 2016
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeRefAttrList_H_
#define ModelAPI_AttributeRefAttrList_H_

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Feature.h"
#include <list>

/**\class ModelAPI_AttributeRefAttrList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features (located in the same document)
 * or references to attributes of the features (list of AttributeRefAttr)
 */

class ModelAPI_AttributeRefAttrList : public ModelAPI_Attribute
{
 public:
  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "RefAttrList";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// Appends the feature to the end of a list
  virtual void append(ObjectPtr theObject) = 0;
  /// Appends the attribute to the end of a list
  virtual void append(AttributePtr theAttr) = 0;

  /// Erases the first meet of the feature in the list
  virtual void remove(ObjectPtr theObject) = 0;
  /// Erases the first meet of the attribute in the list
  virtual void remove(AttributePtr theAttr) = 0;

  /// Removes all references from the list
  virtual void clear() = 0;

  /// Returns number of features in the list
  ///\param theWithEmpty if it is false, returns the number of not-empty referenced objects
  virtual int size(const bool theWithEmpty = true) const = 0;

  /// Returns the list of features and attributes (if it is reference to the attribute)
  virtual std::list<std::pair<ObjectPtr, AttributePtr> > list() = 0;

  /// Returns true if the object is in list
  virtual bool isInList(const ObjectPtr& theObj) = 0;
  /// Returns true if the attribute is in list
  virtual bool isInList(const AttributePtr& theObj) = 0;

  /// Returns true if this is reference to an attribute, not just object
  virtual bool isAttribute(const int theIndex) const = 0;

  /// Returns the referenced object by the zero-based index
  ///\param theIndex zero-based index in the list
  virtual ObjectPtr object(const int theIndex) const = 0;
  /// Returns the referenced attribute by the zero-based index
  ///\param theIndex zero-based index in the list
  virtual AttributePtr attribute(const int theIndex) const = 0;

  /// Removes the last element in the list.
  virtual void removeLast() = 0;

  /// Removes the elements from the list.
  /// \param theIndices a list of indices of elements to be removed
  virtual void remove(const std::set<int>& theIndices) = 0;

  MODELAPI_EXPORT virtual ~ModelAPI_AttributeRefAttrList();
 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeRefAttrList();

};

typedef std::shared_ptr<ModelAPI_AttributeRefAttrList> AttributeRefAttrListPtr;

#endif
