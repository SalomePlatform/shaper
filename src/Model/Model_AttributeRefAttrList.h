// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeRefAttrList.h
// Created:     20 Jan 2016
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeRefAttrList_H_
#define Model_AttributeRefAttrList_H_

#include "Model.h"
#include "ModelAPI_AttributeRefAttrList.h"
#include "ModelAPI_Feature.h"

#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ExtStringList.hxx>

/**\class Model_AttributeRefAttrList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features (located in the same document)
 * or references to attributes of the features (list of AttributeRefAttr)
 */

class Model_AttributeRefAttrList : public ModelAPI_AttributeRefAttrList
{
  Handle_TDataStd_ReferenceList myRef;  ///< references to the features labels
  Handle_TDataStd_ExtStringList myIDs;  ///< the referenced attributes IDs (empty for just object)
 public:
  /// Appends the feature to the end of a list
  MODEL_EXPORT virtual void append(ObjectPtr theObject);
  /// Appends the attribute to the end of a list
  MODEL_EXPORT virtual void append(AttributePtr theAttr);

  /// Erases the first meet of the feature in the list
  MODEL_EXPORT virtual void remove(ObjectPtr theObject);
  /// Erases the first meet of the attribute in the list
  MODEL_EXPORT virtual void remove(AttributePtr theAttr);

  /// Removes all references from the list
  MODEL_EXPORT virtual void clear();

  /// Returns number of features in the list
  MODEL_EXPORT virtual int size() const;

  /// Returns the list of features and attributes (if it is reference to the attribute)
  MODEL_EXPORT virtual std::list<std::pair<ObjectPtr, AttributePtr> > list();

  /// Returns true if the object is in list
  MODEL_EXPORT virtual bool isInList(const ObjectPtr& theObj);
  /// Returns true if the attribute is in list
  MODEL_EXPORT virtual bool isInList(const AttributePtr& theObj);

  /// Returns true if this is reference to an attribute, not just object
  MODEL_EXPORT virtual bool isAttribute(const int theIndex) const;

  /// Returns the referenced object by the zero-based index
  ///\param theIndex zero-based index in the list
  MODEL_EXPORT virtual ObjectPtr object(const int theIndex) const;
  /// Returns the referenced attribute by the zero-based index
  ///\param theIndex zero-based index in the list
  MODEL_EXPORT virtual AttributePtr attribute(const int theIndex) const;

  /// Removes the last element in the list.
  MODEL_EXPORT virtual void removeLast();

  /// Removes the elements from the list.
  /// \param theIndices a list of indices of elements to be removed
  MODEL_EXPORT virtual void remove(const std::set<int>& theIndices);

  /// Returns true if attribute was initialized by some value
  MODEL_EXPORT virtual bool isInitialized();
 protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefAttrList(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
