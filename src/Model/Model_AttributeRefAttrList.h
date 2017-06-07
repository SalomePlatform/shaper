// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_AttributeRefAttrList_H_
#define Model_AttributeRefAttrList_H_

#include "Model.h"
#include "ModelAPI_AttributeRefAttrList.h"
#include "ModelAPI_Feature.h"

#include <TDF_Label.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ExtStringList.hxx>

/**\class Model_AttributeRefAttrList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features (located in the same document)
 * or references to attributes of the features (list of AttributeRefAttr)
 */

class Model_AttributeRefAttrList : public ModelAPI_AttributeRefAttrList
{
  TDF_Label myLab; ///< the main label of this attribute
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
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
