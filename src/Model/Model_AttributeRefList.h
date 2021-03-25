// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef Model_AttributeRefList_H_
#define Model_AttributeRefList_H_

#include "Model.h"
#include "ModelAPI_AttributeRefList.h"
#include "ModelAPI_Feature.h"
#include "Model_Document.h"

#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ExtStringList.hxx>

/**\class Model_AttributeRefList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features, may be located in different documents.
 */

class Model_AttributeRefList : public ModelAPI_AttributeRefList
{
  TDF_Label myLab; ///< the main label of this attribute

  Handle_TDataStd_ReferenceList myRef;  ///< references to the features labels
  /// pairs of doc ID and entries if reference is to external object, appends some in this list if
  /// something in myRef is empty
  Handle_TDataStd_ExtStringList myExtDocRef;
  bool myHashUsed; ///< true if the hash stored is valid
  std::set<ObjectPtr> myHashObjects; ///< hashed information: objects existing in the list
  std::map<int, ObjectPtr> myHashIndex; ///< index to object in the list
  std::map<int, ObjectPtr> myHashIndexNoEmpty; ///< index to not empty object in the list
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
  MODEL_EXPORT virtual ObjectPtr object(const int theIndex, const bool theWithEmpty = true);

  /// Substitutes the feature by another one. Does nothing if such object is not found.
  /// Does not support the external documents objects yet.
  MODEL_EXPORT virtual void substitute(const ObjectPtr& theCurrent, const ObjectPtr& theNew);

  /// Removes the last element in the list.
  /// Does not support the external documents objects yet.
  MODEL_EXPORT virtual void removeLast();

  /// Removes the elements from the list.
  /// Does not support the external documents objects yet.
  /// \param theIndices a list of indices of elements to be removed
  MODEL_EXPORT virtual void remove(const std::set<int>& theIndices);

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();

  /// Erases the hashed objects caused by complicated modifications in the list
  void eraseHash();

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefList(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();
  /// Returns the object by iterators (theExtIter is iterated if necessary)
  ObjectPtr iteratedObject(TDF_ListIteratorOfLabelList& theLIter,
    TDataStd_ListIteratorOfListOfExtendedString& theExtIter,
    std::shared_ptr<Model_Document> theDoc) const;
  /// Creates the hash-objects containers (does nothing if hash is already correct)
  void createHash();

  friend class Model_Data;
};

#endif
