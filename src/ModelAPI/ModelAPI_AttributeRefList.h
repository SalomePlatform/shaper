// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefList.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeRefList_H_
#define ModelAPI_AttributeRefList_H_

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Feature.h"
#include <list>

/**\class ModelAPI_AttributeRefList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features (located in the same document).
 */

class ModelAPI_AttributeRefList : public ModelAPI_Attribute
{
 public:
  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "RefList";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// Appends the feature to the end of a list
  virtual void append(ObjectPtr theObject) = 0;

  /// Erases the first meet of the feature in the list
  virtual void remove(ObjectPtr theObject) = 0;

  /// Removes all references from the list
  virtual void clear() = 0;

  /// Returns number of features in the list
  ///\param theWithEmpty if it is false, returns the number of not-empty referenced objects
  virtual int size(const bool theWithEmpty = true) const = 0;

  /// Returns the list of features
  virtual std::list<ObjectPtr> list() = 0;

  /// Returns true if the object is in list
  virtual bool isInList(const ObjectPtr& theObj) = 0;

  /// Returns the referenced object by the zero-based index
  ///\param theIndex zero-based index in the list
  ///\param theWithEmpty if it is false, counts the not-empty referenced objects only
  virtual ObjectPtr object(const int theIndex, const bool theWithEmpty = true) const = 0;

  /// Substitutes the object by another one. Does nothing if such object is not found.
  virtual void substitute(const ObjectPtr& theCurrent, const ObjectPtr& theNew) = 0;

  /// Substitutes the object by another one and back. So, features wil become exchanged in the list
  virtual void exchange(const ObjectPtr& theObject1, const ObjectPtr& theObject2) = 0;

  /// Removes the last element in the list.
  virtual void removeLast() = 0;

  /// Removes the elements from the list.
  /// \param theIndices a list of indices of elements to be removed
  virtual void remove(const std::set<int>& theIndices) = 0;

  MODELAPI_EXPORT virtual ~ModelAPI_AttributeRefList();
 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeRefList();

};

typedef std::shared_ptr<ModelAPI_AttributeRefList> AttributeRefListPtr;

#endif
