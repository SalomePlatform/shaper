// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeRefList.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeRefList_H_
#define Model_AttributeRefList_H_

#include "Model.h"
#include "ModelAPI_AttributeRefList.h"
#include "ModelAPI_Feature.h"

#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ExtStringList.hxx>

/**\class Model_AttributeRefList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features, may be located in different documents.
 */

class Model_AttributeRefList : public ModelAPI_AttributeRefList
{
  Handle_TDataStd_ReferenceList myRef;  ///< references to the features labels
  /// pairs of doc ID and entries if reference is to external object, appends some in this list if
  /// something in myRef is empty
  Handle_TDataStd_ExtStringList myExtDocRef;
 public:
  /// Appends the feature to the end of a list
  MODEL_EXPORT virtual void append(ObjectPtr theObject);

  /// Erases the first meet of the feature in the list
  MODEL_EXPORT virtual void remove(ObjectPtr theObject);

  /// Returns number of features in the list
  ///\param theWithEmpty if it is false, returns the number of not-empty referenced objects
  MODEL_EXPORT virtual int size(const bool theWithEmpty = true) const;

  /// Removes all references from the list
  MODEL_EXPORT virtual void clear();

  /// Returns the list of features
  MODEL_EXPORT virtual std::list<ObjectPtr> list();

  /// Returns true if the object is in list
  MODEL_EXPORT virtual bool isInList(const ObjectPtr& theObj);

  /// Returns the list of features
  ///\param theIndex zero-based index in the list
  ///\param theWithEmpty if it is false, counts the not-empty referenced objects only
  MODEL_EXPORT virtual ObjectPtr object(const int theIndex, const bool theWithEmpty = true) const;

  /// Substitutes the feature by another one. Does nothing if such object is not found.
  /// Does not support the external documents objects yet.
  MODEL_EXPORT virtual void substitute(const ObjectPtr& theCurrent, const ObjectPtr& theNew);

  /// Substitutes the object by another one and back. So, features will become exchanged in the list
  /// Does not support the external documents objects yet.
  MODEL_EXPORT virtual void exchange(const ObjectPtr& theObject1, const ObjectPtr& theObject2);

  /// Removes the last element in the list.
  /// Does not support the external documents objects yet.
  MODEL_EXPORT virtual void removeLast();

  /// Removes the elements from the list.
  /// Does not support the external documents objects yet.
  /// \param theIndices a list of indices of elements to be removed
  MODEL_EXPORT virtual void remove(const std::set<int>& theIndices);

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();
 protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefList(TDF_Label& theLabel);
  /// Returns the object by iterators (theExtIter is iterated if necessary)
  ObjectPtr iteratedObject(TDF_ListIteratorOfLabelList& theLIter,
    TDataStd_ListIteratorOfListOfExtendedString& theExtIter,
    std::shared_ptr<Model_Document> theDoc) const;

  friend class Model_Data;
};

#endif
