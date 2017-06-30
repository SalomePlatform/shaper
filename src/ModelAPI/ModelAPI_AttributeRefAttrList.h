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
  virtual int size() const = 0;

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
